// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "SimModeSIPCharacter.h"
#include "UObject/ConstructorHelpers.h"
#include "Engine/World.h"

#include "AirBlueprintLib.h"
#include "common/AirSimSettings.hpp"
#include "PawnSimApi.h"
#include "common/Common.hpp"
#include "common/EarthUtils.hpp"
#include "api/VehicleSimApiBase.hpp"
#include "SIPCharacterPawnSimApi.h"
#include "common/AirSimSettings.hpp"
#include "physics/Kinematics.hpp"
#include "api/RpcLibServerBase.hpp"

std::unique_ptr<msr::airlib::ApiServerBase> ASimModeSIPCharacter::createApiServer() const
{
#ifdef AIRLIB_NO_RPC
    return ASimModeBase::createApiServer();
#else
    return std::unique_ptr<msr::airlib::ApiServerBase>(new msr::airlib::RpcLibServerBase(
        getApiProvider(), getSettings().api_server_address, getSettings().api_port));
#endif
}

void ASimModeSIPCharacter::setupClockSpeed()
{
    Super::setupClockSpeed();

    current_clockspeed_ = getSettings().clock_speed;

    //setup clock in PhysX
    UAirBlueprintLib::setUnrealClockSpeed(this, current_clockspeed_);
    UAirBlueprintLib::LogMessageString("Clock Speed: ", std::to_string(current_clockspeed_), LogDebugLevel::Informational);
}

void ASimModeSIPCharacter::getExistingVehiclePawns(TArray<AActor*>& pawns) const
{
    UAirBlueprintLib::FindAllActor<TVehiclePawn>(this, pawns);
}

bool ASimModeSIPCharacter::isVehicleTypeSupported(const std::string& vehicle_type) const
{
    return vehicle_type == msr::airlib::AirSimSettings::kVehicleTypeSIPCharacter;
}

std::string ASimModeSIPCharacter::getVehiclePawnPathName(const AirSimSettings::VehicleSetting& vehicle_setting) const
{
    //decide which derived BP to use
    std::string pawn_path = vehicle_setting.pawn_path;
    if (pawn_path == "")
        pawn_path = "DefaultSIPCharacter";

    return pawn_path;
}

PawnEvents* ASimModeSIPCharacter::getVehiclePawnEvents(APawn* pawn) const
{
    return static_cast<TVehiclePawn*>(pawn)->getPawnEvents();
}
const common_utils::UniqueValueMap<std::string, APIPCamera*> ASimModeSIPCharacter::getVehiclePawnCameras(
    APawn* pawn) const
{
    return static_cast<const TVehiclePawn*>(pawn)->getCameras();
}
void ASimModeSIPCharacter::initializeVehiclePawn(APawn* pawn)
{
    static_cast<TVehiclePawn*>(pawn)->initializeForBeginPlay(getSettings().character_defaults);
}
std::unique_ptr<PawnSimApi> ASimModeSIPCharacter::createVehicleSimApi(
    const PawnSimApi::Params& pawn_sim_api_params) const
{
    auto vehicle_sim_api = std::unique_ptr<PawnSimApi>(new SIPCharacterPawnSimApi(pawn_sim_api_params));
    vehicle_sim_api->initialize();
    vehicle_sim_api->reset();
    return vehicle_sim_api;
}

msr::airlib::VehicleApiBase* ASimModeSIPCharacter::getVehicleApi(const PawnSimApi::Params& pawn_sim_api_params,
                                                                 const PawnSimApi* sim_api) const
{
    //we don't have real vehicle so no vehicle API
    return nullptr;
}

void ASimModeSIPCharacter::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    this->spawned_actors_.Empty();
    this->vehicle_sim_apis_.clear();
    Super::EndPlay(EndPlayReason);
}

void ASimModeSIPCharacter::setupVehiclesAndCamera()
{
    //get UU origin of global NED frame
    const FTransform uu_origin = getGlobalNedTransform().getGlobalTransform();

    //determine camera director camera default pose and spawn it
    const auto& camera_director_setting = getSettings().camera_director;
    FVector camera_director_position_uu = uu_origin.GetLocation() +
                                          getGlobalNedTransform().fromLocalNed(camera_director_setting.position);
    FTransform camera_transform(toFRotator(camera_director_setting.rotation, FRotator::ZeroRotator),
                                camera_director_position_uu);
    initializeCameraDirector(camera_transform, camera_director_setting.follow_distance);

    //find all vehicle pawns
    {
        TArray<AActor*> pawns;
        getExistingVehiclePawns(pawns);

        APawn* fpv_pawn = nullptr;

        //add vehicles from settings
        for (auto const& vehicle_setting_pair : getSettings().vehicles) {
            //if vehicle is of type for derived SimMode and auto creatable
            const auto& vehicle_setting = *vehicle_setting_pair.second;
            if (vehicle_setting.auto_create &&
                isVehicleTypeSupported(vehicle_setting.vehicle_type)) {

                //compute initial pose
                FVector spawn_position = uu_origin.GetLocation();
                msr::airlib::Vector3r settings_position = vehicle_setting.position;
                if (!msr::airlib::VectorMath::hasNan(settings_position))
                    spawn_position = getGlobalNedTransform().fromGlobalNed(settings_position);
                FRotator spawn_rotation = toFRotator(vehicle_setting.rotation, uu_origin.Rotator());

                //spawn vehicle pawn
                FActorSpawnParameters pawn_spawn_params;
                pawn_spawn_params.Name = FName(vehicle_setting.vehicle_name.c_str());
                pawn_spawn_params.SpawnCollisionHandlingOverride =
                    ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

                ASIPCharacterPawn* spawned_pawn = this->GetWorld()->SpawnActor<ASIPCharacterPawn>(spawn_position, spawn_rotation, pawn_spawn_params);

                spawned_actors_.Add(spawned_pawn);
                pawns.Add(spawned_pawn);

                if (vehicle_setting.is_fpv_vehicle)
                    fpv_pawn = spawned_pawn;

                ASIPCharacterPawn* vehicle_pawn = static_cast<ASIPCharacterPawn*>(spawned_pawn);
                //initializeVehiclePawn(vehicle_pawn, vehicle_setting.vehicle_character_defaults.pawn_skeletal_mesh);
                static_cast<TVehiclePawn*>(vehicle_pawn)->initializeForBeginPlay(vehicle_setting.vehicle_character_defaults);

                //create vehicle sim api
                const auto& ned_transform = getGlobalNedTransform();
                const auto& pawn_ned_pos = ned_transform.toLocalNed(vehicle_pawn->GetActorLocation());
                const auto& home_geopoint = msr::airlib::EarthUtils::nedToGeodetic(pawn_ned_pos, getSettings().origin_geopoint);
                const std::string vehicle_name = std::string(TCHAR_TO_UTF8(*(vehicle_pawn->GetName())));

                SIPCharacterPawnSimApi::Params pawn_sim_api_params(vehicle_pawn, &getGlobalNedTransform(), getVehiclePawnEvents(vehicle_pawn), getVehiclePawnCameras(vehicle_pawn), pip_camera_class, collision_display_template, home_geopoint, vehicle_name);

                auto vehicle_sim_api = createVehicleSimApi(pawn_sim_api_params);
                auto vehicle_sim_api_p = vehicle_sim_api.get();
                auto vehicle_Api = getVehicleApi(pawn_sim_api_params, vehicle_sim_api_p);
                getApiProvider()->insert_or_assign(vehicle_name, vehicle_Api, vehicle_sim_api_p);
                if ((fpv_pawn == vehicle_pawn || !getApiProvider()->hasDefaultVehicle()) && vehicle_name != "")
                    getApiProvider()->makeDefaultVehicle(vehicle_name);

                vehicle_sim_apis_.push_back(std::move(vehicle_sim_api));
            }
        }
    }

    if (getApiProvider()->hasDefaultVehicle()) {
        //TODO: better handle no FPV vehicles scenario
        getVehicleSimApi()->possess();
        CameraDirector->initializeForBeginPlay(getInitialViewMode(), getVehicleSimApi()->getPawn(), getVehicleSimApi()->getCamera("fpv"), getVehicleSimApi()->getCamera("back_center"), nullptr);
    }
    else
        CameraDirector->initializeForBeginPlay(getInitialViewMode(), nullptr, nullptr, nullptr, nullptr);

    checkVehicleReady();
}

void ASimModeSIPCharacter::Tick(float DeltaSecond)
{
    Super::Tick(DeltaSecond);
    test2 += 1;
}

