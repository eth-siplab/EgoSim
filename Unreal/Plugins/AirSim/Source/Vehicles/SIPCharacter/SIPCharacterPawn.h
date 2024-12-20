#pragma once

#include "CoreMinimal.h"
#include "UObject/ConstructorHelpers.h"

#include "physics/Kinematics.hpp"
#include "common/AirSimSettings.hpp"
#include "AirBlueprintLib.h"
#include "api/VehicleSimApiBase.hpp"
#include "common/common_utils/UniqueValueMap.hpp"
#include "PawnEvents.h"
#include "PIPCamera.h"
#include "ManualPoseController.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/Pawn.h"
#include "AirBlueprintLib.h"
#include "Engine/SkeletalMeshSocket.h"

#include "SIPCharacterPawn.generated.h"

UCLASS()
class ASIPCharacterPawn : public APawn
{
    GENERATED_BODY()

public:
    UPROPERTY()
    USkeletalMesh* CharacterMesh;
    UPROPERTY()
    USkeletalMeshComponent* MyCharacter;
    UPROPERTY()
    UAnimBlueprintGeneratedClass* AnimBp;

    UPROPERTY()
    USkeletalMeshComponent* MySpring1;
    UPROPERTY()
    USkeletalMeshComponent* MySpring2;
    UPROPERTY()
    USkeletalMeshComponent* MySpring3;
    UPROPERTY()
    USkeletalMeshComponent* MySpring4;
    UPROPERTY()
    USkeletalMeshComponent* MySpring5;
    UPROPERTY()
    USkeletalMeshComponent* MySpring6;

    UPROPERTY()
    FString SocName1;
    UPROPERTY()
    FString SocName2;
    UPROPERTY()
    FString SocName3;
    UPROPERTY()
    FString SocName4;
    UPROPERTY()
    FString SocName5;
    UPROPERTY()
    FString SocName6;

    UPROPERTY()
    FName IMU1;
    UPROPERTY()
    FName IMU2;
    UPROPERTY()
    FName IMU3;
    UPROPERTY()
    FName IMU4;
    UPROPERTY()
    FName IMU5;
    UPROPERTY()
    FName IMU6;

    UPROPERTY()
    FVector Gravity = FVector(0, 0, 9.81);

    //noise
    std::default_random_engine generator;

    double mean_vel1 = 0.0;
    double mean_acc1 = 0.0;
    double stddev_vel1;
    double stddev_acc1;

    double mean_vel2 = 0.0;
    double mean_acc2 = 0.0;
    double stddev_vel2;
    double stddev_acc2;

    double mean_vel3 = 0.0;
    double mean_acc3 = 0.0;
    double stddev_vel3;
    double stddev_acc3;

    double mean_vel4 = 0.0;
    double mean_acc4 = 0.0;
    double stddev_vel4;
    double stddev_acc4;

    double mean_vel5 = 0.0;
    double mean_acc5 = 0.0;
    double stddev_vel5;
    double stddev_acc5;

    double mean_vel6 = 0.0;
    double mean_acc6 = 0.0;
    double stddev_vel6;
    double stddev_acc6;


    std::normal_distribution<double>  dist_vel1;
    std::normal_distribution<double> dist_acc1;
    std::normal_distribution<double>  dist_vel2;
    std::normal_distribution<double> dist_acc2;
    std::normal_distribution<double>  dist_vel3;
    std::normal_distribution<double> dist_acc3;
    std::normal_distribution<double>  dist_vel4;
    std::normal_distribution<double> dist_acc4;
    std::normal_distribution<double>  dist_vel5;
    std::normal_distribution<double> dist_acc5;
    std::normal_distribution<double>  dist_vel6;
    std::normal_distribution<double> dist_acc6;

    UPROPERTY()
    FQuat DeltaRotation1;
    UPROPERTY()
    FVector Axis1;
    float Angle1;
    UPROPERTY()
    FVector AngularVelocity1;
    UPROPERTY()
    FVector LinearAcceleration1;

    UPROPERTY()
    FQuat DeltaRotation2;
    UPROPERTY()
    FVector Axis2;
    float Angle2;
    UPROPERTY()
    FVector AngularVelocity2;
    UPROPERTY()
    FVector LinearAcceleration2;

    UPROPERTY()
    FQuat DeltaRotation3;
    UPROPERTY()
    FVector Axis3;
    float Angle3;
    UPROPERTY()
    FVector AngularVelocity3;
    UPROPERTY()
    FVector LinearAcceleration3;

    UPROPERTY()
    FQuat DeltaRotation4;
    UPROPERTY()
    FVector Axis4;
    float Angle4;
    UPROPERTY()
    FVector AngularVelocity4;
    UPROPERTY()
    FVector LinearAcceleration4;

    UPROPERTY()
    FQuat DeltaRotation5;
    UPROPERTY()
    FVector Axis5;
    float Angle5;
    UPROPERTY()
    FVector AngularVelocity5;
    UPROPERTY()
    FVector LinearAcceleration5;

    UPROPERTY()
    FQuat DeltaRotation6;
    UPROPERTY()
    FVector Axis6;
    float Angle6;
    UPROPERTY()
    FVector AngularVelocity6;
    UPROPERTY()
    FVector LinearAcceleration6;


 
    std::string joint_1;
    std::string joint_2;
    std::string joint_3;
    std::string joint_4;
    std::string joint_5;
    std::string joint_6;


    UPROPERTY()
    bool Soc1Spring = false;
    UPROPERTY()
    bool Soc2Spring = false;
    UPROPERTY()
    bool Soc3Spring = false;
    UPROPERTY()
    bool Soc4Spring = false;
    UPROPERTY()
    bool Soc5Spring = false;
    UPROPERTY()
    bool Soc6Spring = false;

    UPROPERTY()
    USkeletalMesh* SpringMesh;
    UPROPERTY()
    UAnimBlueprintGeneratedClass* AnimBpSpring;

    ASIPCharacterPawn();

    virtual void BeginPlay() override;
    virtual void Tick(float Delta) override;
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
    float GetLastAnimTimeS() const;
    virtual void NotifyHit(class UPrimitiveComponent* MyComp, class AActor* Other, class UPrimitiveComponent* OtherComp, bool bSelfMoved, FVector HitLocation,
                           FVector HitNormal, FVector NormalImpulse, const FHitResult& Hit) override;

    void SetupCameraBase(USkeletalMeshComponent* spring, const std::string& joint_name, USpringArmComponent* SpringSocket, USceneComponent* camera_base, const bool use_spring, const float lag_speed, const float trans_x, const float trans_y, const float trans_z, const float rot_y, const float rot_z, const float rot_x);

    //interface
    void initializeForBeginPlay(msr::airlib::AirSimSettings::CharacterDefaults Character_info);
    const common_utils::UniqueValueMap<std::string, APIPCamera*> getCameras() const;
    PawnEvents* getPawnEvents()
    {
        return &pawn_events_;
    }

private:
    UPROPERTY()
    UClass* pip_camera_class_;


    float GetAnimTimeS() const; // Get the time of the animation. Loops around

    // timestamp of the animation in second. Update on tick
    float animationTS_s;

    PawnEvents pawn_events_;

    UPROPERTY()
    USceneComponent* camera_view_character_base_;
    UPROPERTY()
    USceneComponent* camera_socket_1_base_;
    UPROPERTY()
    USceneComponent* camera_socket_2_base_;
    UPROPERTY()
    USceneComponent* camera_socket_3_base_;
    UPROPERTY()
    USceneComponent* camera_socket_4_base_;
    UPROPERTY()
    USceneComponent* camera_socket_5_base_;
    UPROPERTY()
    USceneComponent* camera_socket_6_base_;

    UPROPERTY()
    USpringArmComponent* SpringSocket1;
    UPROPERTY()
    USpringArmComponent* SpringSocket2;
    UPROPERTY()
    USpringArmComponent* SpringSocket3;
    UPROPERTY()
    USpringArmComponent* SpringSocket4;
    UPROPERTY()
    USpringArmComponent* SpringSocket5;
    UPROPERTY()
    USpringArmComponent* SpringSocket6;

    //IMUs
    UPROPERTY()
    FQuat RotLast1= FQuat(0, 0, 0, 0);
    UPROPERTY()
    FQuat RotCurrent1= FQuat(0, 0, 0, 0);
    UPROPERTY()
    FVector LocSecondLast1= FVector::ZeroVector;
    UPROPERTY()
    FVector LocLast1= FVector::ZeroVector;
    UPROPERTY()
    FVector LocCurrent1 = FVector::ZeroVector;

    UPROPERTY()
    FQuat RotLast2 = FQuat(0, 0, 0, 0);
    UPROPERTY()
    FQuat RotCurrent2 = FQuat(0, 0, 0, 0);
    UPROPERTY()
    FVector LocSecondLast2 = FVector::ZeroVector;
    UPROPERTY()
    FVector LocLast2 = FVector::ZeroVector;
    UPROPERTY()
    FVector LocCurrent2 = FVector::ZeroVector;

    UPROPERTY()
    FQuat RotLast3 = FQuat(0, 0, 0, 0);
    UPROPERTY()
    FQuat RotCurrent3 = FQuat(0, 0, 0, 0);
    UPROPERTY()
    FVector LocSecondLast3 = FVector::ZeroVector;
    UPROPERTY()
    FVector LocLast3 = FVector::ZeroVector;
    UPROPERTY()
    FVector LocCurrent3 = FVector::ZeroVector;

    UPROPERTY()
    FQuat RotLast4 = FQuat(0, 0, 0, 0);
    UPROPERTY()
    FQuat RotCurrent4 = FQuat(0, 0, 0, 0);
    UPROPERTY()
    FVector LocSecondLast4 = FVector::ZeroVector;
    UPROPERTY()
    FVector LocLast4 = FVector::ZeroVector;
    UPROPERTY()
    FVector LocCurrent4 = FVector::ZeroVector;

    UPROPERTY()
    FQuat RotLast5 = FQuat(0, 0, 0, 0);
    UPROPERTY()
    FQuat RotCurrent5 = FQuat(0, 0, 0, 0);
    UPROPERTY()
    FVector LocSecondLast5 = FVector::ZeroVector;
    UPROPERTY()
    FVector LocLast5 = FVector::ZeroVector;
    UPROPERTY()
    FVector LocCurrent5 = FVector::ZeroVector;

    UPROPERTY()
    FQuat RotLast6 = FQuat(0, 0, 0, 0);
    UPROPERTY()
    FQuat RotCurrent6 = FQuat(0, 0, 0, 0);
    UPROPERTY()
    FVector LocSecondLast6 = FVector::ZeroVector;
    UPROPERTY()
    FVector LocLast6 = FVector::ZeroVector;
    UPROPERTY()
    FVector LocCurrent6 = FVector::ZeroVector;
    //
    UPROPERTY()
    APIPCamera* camera_view_character_;
    UPROPERTY()
    APIPCamera* camera_socket_1_;
    UPROPERTY()
    APIPCamera* camera_socket_2_;
    UPROPERTY()
    APIPCamera* camera_socket_3_;
    UPROPERTY()
    APIPCamera* camera_socket_4_;
    UPROPERTY()
    APIPCamera* camera_socket_5_;
    UPROPERTY()
    APIPCamera* camera_socket_6_;

    UPROPERTY()
    UManualPoseController* manual_pose_controller_;
    float delta_since_last_sample = 0;
    float delta_last_sample = 0;
    std::chrono::time_point<std::chrono::system_clock> last_ts;
};