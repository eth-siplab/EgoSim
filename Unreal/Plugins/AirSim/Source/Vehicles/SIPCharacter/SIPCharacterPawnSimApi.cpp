#include "SIPCharacterPawnSimApi.h"
#include "AirBlueprintLib.h"
#include <exception>
#include "SIPCharacterPawn.h"

using namespace msr::airlib;

SIPCharacterPawnSimApi::SIPCharacterPawnSimApi(const Params& params)
    : PawnSimApi(params)
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



        auto my_header = "VehicleName\tTimeStamp\tPOS_X\tPOS_Y\tPOS_Z\tQ_X\tQ_Y\tQ_Z\tQ_W\t"
            "POS_X2\tPOS_Y2\tPOS_Z2\tQ_X2\tQ_Y2\tQ_Z2\tQ_W2"
            "\tPOS_X3\tPOS_Y3\tPOS_Z3\tQ_X3\tQ_Y3\tQ_Z3\tQ_W3"
            "\tPOS_X4\tPOS_Y4\tPOS_Z4\tQ_X4\tQ_Y4\tQ_Z4\tQ_W4"
            "\tPOS_X5\tPOS_Y5\tPOS_Z5\tQ_X5\tQ_Y5\tQ_Z5\tQ_W5"
            "\tPOS_X6\tPOS_Y6\tPOS_Z6\tQ_X6\tQ_Y6\tQ_Z6\tQ_W6"
            "\tPelvis_X\tPelvis_Y\tPelvis_Z\tPelvis_QX\tPelvis_QY\tPelvis_QZ\tPelvis_QW"
            "\timu1_accx\timu1_accy\timu1_accz\timu1_velx\timu1_vely\timu1_velz\t";

        return my_header;
    }

    const uint64_t timestamp_millis = static_cast<uint64_t>(std::chrono::high_resolution_clock::now().time_since_epoch().count() / 1.0E6);//static_cast<uint64_t>(clock()->nowNanos() / 1.0E6);
    auto cam_rec = getCamera("1");
    auto CamTransform = cam_rec->GetActorLocation();
    auto CamRot = cam_rec->GetActorRotation().Quaternion();

    //cam 2 to 6
    auto cam_rec2 = getCamera("2");
    auto CamTransform2 = cam_rec2->GetActorLocation();
    auto CamRot2 = cam_rec2->GetActorRotation().Quaternion();

    auto cam_rec3 = getCamera("3");
    auto CamTransform3 = cam_rec3->GetActorLocation();
    auto CamRot3 = cam_rec3->GetActorRotation().Quaternion();

    auto cam_rec4 = getCamera("4");
    auto CamTransform4 = cam_rec4->GetActorLocation();
    auto CamRot4 = cam_rec4->GetActorRotation().Quaternion();

    auto cam_rec5 = getCamera("5");
    auto CamTransform5 = cam_rec5->GetActorLocation();
    auto CamRot5 = cam_rec5->GetActorRotation().Quaternion();

    auto cam_rec6 = getCamera("6");
    auto CamTransform6 = cam_rec6->GetActorLocation();
    auto CamRot6 = cam_rec6->GetActorRotation().Quaternion();
    ///////////////////////////
    FName BoneName1 = TEXT("pelvis");
    auto pelvispos = sip_pawn_->MyCharacter->GetBoneLocation(BoneName1, EBoneSpaces::WorldSpace);
    auto pelvisrot = sip_pawn_->MyCharacter->GetBoneQuaternion(BoneName1, EBoneSpaces::WorldSpace);
    //FName BoneName2 = TEXT("root");
    //auto rootpos = sip_pawn_->MyCharacter->GetBoneLocation(BoneName2, EBoneSpaces::WorldSpace);
    //auto rootrot = sip_pawn_->MyCharacter->GetBoneQuaternion(BoneName2, EBoneSpaces::WorldSpace);

    auto imu1_linearacc = sip_pawn_->LinearAcceleration1;
    auto imu1_angularvel = sip_pawn_->AngularVelocity1;

    std::ostringstream ss;
    ss << getVehicleName() << "\t";
    ss << timestamp_millis << "\t";
    ss << CamTransform.X << "\t" << CamTransform.Y << "\t" << CamTransform.Z << "\t";
    ss << CamRot.X << "\t" << CamRot.Y << "\t"
        << CamRot.Z << "\t" << CamRot.W << "\t";
    ///////cam 2 to 6
    ss << CamTransform2.X << "\t" << CamTransform2.Y << "\t" << CamTransform2.Z << "\t";
    ss << CamRot2.X << "\t" << CamRot2.Y << "\t"
        << CamRot2.Z << "\t" << CamRot2.W << "\t";

    ss << CamTransform3.X << "\t" << CamTransform3.Y << "\t" << CamTransform3.Z << "\t";
    ss << CamRot3.X << "\t" << CamRot3.Y << "\t"
        << CamRot3.Z << "\t" << CamRot3.W << "\t";

    ss << CamTransform4.X << "\t" << CamTransform4.Y << "\t" << CamTransform4.Z << "\t";
    ss << CamRot4.X << "\t" << CamRot4.Y << "\t"
        << CamRot4.Z << "\t" << CamRot4.W << "\t";

    ss << CamTransform5.X << "\t" << CamTransform5.Y << "\t" << CamTransform5.Z << "\t";
    ss << CamRot5.X << "\t" << CamRot5.Y << "\t"
        << CamRot5.Z << "\t" << CamRot5.W << "\t";

    ss << CamTransform6.X << "\t" << CamTransform6.Y << "\t" << CamTransform6.Z << "\t";
    ss << CamRot6.X << "\t" << CamRot6.Y << "\t"
        << CamRot6.Z << "\t" << CamRot6.W << "\t";
    // 
    //ss << rootpose.position<< "\t" << rootpose.orientation << "\t";;
    //ss << rootpos.X << "\t" << rootpos.Y << "\t" << rootpos.Z << "\t";
    //ss << rootrot.X << "\t" << rootrot.Y << "\t" << rootrot.Z << "\t" << rootrot.W << "\t";
    ss << pelvispos.X << "\t" << pelvispos.Y << "\t" << pelvispos.Z << "\t";
    ss<< pelvisrot.X << "\t"<< pelvisrot.Y << "\t" << pelvisrot.Z << "\t"<< pelvisrot.W<<"\t";

    ss << imu1_linearacc.X << "\t" << imu1_linearacc.Y << "\t" << imu1_linearacc.Z << "\t";
    ss << imu1_angularvel.X << "\t" << imu1_angularvel.Y << "\t" << imu1_angularvel.Z << "\t";

    return ss.str();
}
