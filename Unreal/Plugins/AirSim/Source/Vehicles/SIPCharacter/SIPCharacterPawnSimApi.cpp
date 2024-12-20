#include "SIPCharacterPawnSimApi.h"
#include "AirBlueprintLib.h"
#include <exception>
#include "SIPCharacterPawn.h"
#include "Animation/AnimSingleNodeInstance.h"

using namespace msr::airlib;

SIPCharacterPawnSimApi::SIPCharacterPawnSimApi(const Params& params)
    : PawnSimApi(params), create_time(std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count())
{
}
void SIPCharacterPawnSimApi::initialize()
{
    PawnSimApi::initialize();
    sip_pawn_ = static_cast<ASIPCharacterPawn*>(getPawn());
}
std::string SIPCharacterPawnSimApi::getRecordFileLine(bool is_header_line) const
{
    std::string common_line = PawnSimApi::getRecordFileLine(is_header_line);
    if (is_header_line) {



        std::string my_header = "VehicleName\tTimeStampMs\tTimeStampAnimationS";
        for (int i = 1; i <= 6; i++) {
            my_header += "\t" + std::string("POS_X_CAM") + std::to_string(i);
            my_header += "\t" + std::string("POS_Y_CAM") + std::to_string(i);
            my_header += "\t" + std::string("POS_Z_CAM") + std::to_string(i);
            my_header += "\t" + std::string("Q_X_CAM") + std::to_string(i);
            my_header += "\t" + std::string("Q_Y_CAM") + std::to_string(i);
            my_header += "\t" + std::string("Q_Z_CAM") + std::to_string(i);
            my_header += "\t" + std::string("Q_W_CAM") + std::to_string(i);
        }

        for (int i = 1; i <= 6; i++) {
            my_header += "\t" + std::string("IMU_CAM") + std::to_string(i) + std::string("_accx");
            my_header += "\t" + std::string("IMU_CAM") + std::to_string(i) + std::string("_accy");
            my_header += "\t" + std::string("IMU_CAM") + std::to_string(i) + std::string("_accz");
            my_header += "\t" + std::string("IMU_CAM") + std::to_string(i) + std::string("_velx");
            my_header += "\t" + std::string("IMU_CAM") + std::to_string(i) + std::string("_vely");
            my_header += "\t" + std::string("IMU_CAM") + std::to_string(i) + std::string("_velz");
        }

        // append the header for all the joints of the skeleton
        TArray<FName> BoneNames;
        sip_pawn_->MyCharacter->GetBoneNames(BoneNames);
        for (auto& BoneName : BoneNames) {
            my_header += "\t" + std::string("POS_X_") + std::string(TCHAR_TO_UTF8(*BoneName.ToString()));
            my_header += "\t" + std::string("POS_Y_") + std::string(TCHAR_TO_UTF8(*BoneName.ToString()));
            my_header += "\t" + std::string("POS_Z_") + std::string(TCHAR_TO_UTF8(*BoneName.ToString()));
            my_header += "\t" + std::string("Q_X_") + std::string(TCHAR_TO_UTF8(*BoneName.ToString()));
            my_header += "\t" + std::string("Q_Y_") + std::string(TCHAR_TO_UTF8(*BoneName.ToString()));
            my_header += "\t" + std::string("Q_Z_") + std::string(TCHAR_TO_UTF8(*BoneName.ToString()));
            my_header += "\t" + std::string("Q_W_") + std::string(TCHAR_TO_UTF8(*BoneName.ToString()));
        }
        my_header.append("\t");
        
        return my_header;
    }

    uint64_t timestamp_millis = std::chrono::duration_cast<std::chrono::milliseconds>(
                                                                    std::chrono::high_resolution_clock::now().time_since_epoch())
                                                                    .count();
    timestamp_millis = timestamp_millis - create_time;

    std::ostringstream ss;
    ss << getVehicleName() << "\t";
    ss << timestamp_millis << "\t";
    ss << sip_pawn_->GetLastAnimTimeS() << "\t";

    for (int i = 1; i <= 6; ++i) {
        auto cam_rec = getCamera(std::to_string(i));
        auto CamTransform = cam_rec->GetActorLocation();
        auto CamRot = cam_rec->GetActorRotation().Quaternion();

        ss << CamTransform.X << "\t" << CamTransform.Y << "\t" << CamTransform.Z << "\t";
        ss << CamRot.X << "\t" << CamRot.Y << "\t"
            << CamRot.Z << "\t" << CamRot.W << "\t";
    }


    std::vector< FVector> imu_linearaccs = { sip_pawn_->LinearAcceleration1, sip_pawn_->LinearAcceleration2, sip_pawn_->LinearAcceleration3, sip_pawn_->LinearAcceleration4, sip_pawn_->LinearAcceleration5, sip_pawn_->LinearAcceleration6 };
    std::vector< FVector> imu_angularvels = { sip_pawn_->AngularVelocity1, sip_pawn_->AngularVelocity2, sip_pawn_->AngularVelocity3, sip_pawn_->AngularVelocity4, sip_pawn_->AngularVelocity5, sip_pawn_->AngularVelocity6 };
    for (int i = 0; i < 6; ++i) {
        auto imu_linearacc = imu_linearaccs[i];
        auto imu_angularvel = imu_angularvels[i];

        ss << imu_linearacc.X << "\t" << imu_linearacc.Y << "\t" << imu_linearacc.Z << "\t";
        ss << imu_angularvel.X << "\t" << imu_angularvel.Y << "\t" << imu_angularvel.Z << "\t";
    }

    // log the joints
    TArray<FName> BoneNames;
    sip_pawn_->MyCharacter->GetBoneNames(BoneNames);
    for (auto& BoneName : BoneNames) {
		auto BonePos = sip_pawn_->MyCharacter->GetBoneLocation(BoneName, EBoneSpaces::WorldSpace);
		auto BoneRot = sip_pawn_->MyCharacter->GetBoneQuaternion(BoneName, EBoneSpaces::WorldSpace);

		ss << BonePos.X << "\t" << BonePos.Y << "\t" << BonePos.Z << "\t";
		ss << BoneRot.X << "\t" << BoneRot.Y << "\t" << BoneRot.Z << "\t" << BoneRot.W << "\t";
	}

    return ss.str();
}
