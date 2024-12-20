#pragma once

#include "CoreMinimal.h"

#include "SIPCharacterPawn.h"
#include "common/Common.hpp"
#include "api/VehicleSimApiBase.hpp"
#include "SimMode/SimModeBase.h"
#include "SIPCharacterPawnSimApi.h"
#include "SimModeSIPCharacter.generated.h"

UCLASS()
class AIRSIM_API ASimModeSIPCharacter : public ASimModeBase
{
    GENERATED_BODY()
public:
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
    virtual void Tick(float DeltaSeconds) override;

private:
    typedef ASIPCharacterPawn TVehiclePawn;
    TArray<AActor*> spawned_actors_;
    std::vector<std::unique_ptr<msr::airlib::VehicleSimApiBase>> vehicle_sim_apis_;
    std::atomic<float> current_clockspeed_;
    float test2 = 0;

protected:
    virtual void setupClockSpeed() override;
    virtual void setupVehiclesAndCamera() override;
    virtual std::unique_ptr<msr::airlib::ApiServerBase> createApiServer() const override;
    virtual void getExistingVehiclePawns(TArray<AActor*>& pawns) const override;
    virtual bool isVehicleTypeSupported(const std::string& vehicle_type) const override;
    virtual std::string getVehiclePawnPathName(const AirSimSettings::VehicleSetting& vehicle_setting) const override;
    virtual PawnEvents* getVehiclePawnEvents(APawn* pawn) const override;
    virtual const common_utils::UniqueValueMap<std::string, APIPCamera*> getVehiclePawnCameras(APawn* pawn) const override;
    virtual void initializeVehiclePawn(APawn* pawn) override;
    virtual std::unique_ptr<PawnSimApi> createVehicleSimApi(
        const PawnSimApi::Params& pawn_sim_api_params) const override;
    virtual msr::airlib::VehicleApiBase* getVehicleApi(const PawnSimApi::Params& pawn_sim_api_params,
                                                       const PawnSimApi* sim_api) const override;
};