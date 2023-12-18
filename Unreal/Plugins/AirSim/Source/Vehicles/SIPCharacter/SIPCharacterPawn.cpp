#include "SIPCharacterPawn.h"
#include "Engine/World.h"
#include "ManualPoseController.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/SpringArmComponent.h"
#include "AirBlueprintLib.h"
#include "Engine/SkeletalMeshSocket.h"
#include "AirBlueprintLib.h"

ASIPCharacterPawn::ASIPCharacterPawn()
{
    static ConstructorHelpers::FClassFinder<APIPCamera> pip_camera_class(TEXT("Blueprint'/AirSim/Blueprints/BP_PIPCamera'"));
    pip_camera_class_ = pip_camera_class.Succeeded() ? pip_camera_class.Class : nullptr;

    RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));

    MyCharacter = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("MyCharacter"));
    MyCharacter->SetupAttachment(RootComponent);

    camera_socket_1_base_ = CreateDefaultSubobject<USceneComponent>(TEXT("camera_socket_1_base_"));
    camera_socket_1_base_->SetRelativeLocation(FVector(0, 0, 0)); //1
    camera_socket_2_base_ = CreateDefaultSubobject<USceneComponent>(TEXT("camera_socket_2_base_"));
    camera_socket_2_base_->SetRelativeLocation(FVector(0, 0, 0)); //2
    camera_socket_3_base_ = CreateDefaultSubobject<USceneComponent>(TEXT("camera_socket_3_base_"));
    camera_socket_3_base_->SetRelativeLocation(FVector(0, 0, 0)); //3
    camera_socket_4_base_ = CreateDefaultSubobject<USceneComponent>(TEXT("camera_socket_4_base_"));
    camera_socket_4_base_->SetRelativeLocation(FVector(0, 0, 0)); //4
    camera_socket_5_base_ = CreateDefaultSubobject<USceneComponent>(TEXT("camera_socket_5_base_"));
    camera_socket_5_base_->SetRelativeLocation(FVector(0, 0, 0)); //5
    camera_socket_6_base_ = CreateDefaultSubobject<USceneComponent>(TEXT("camera_socket_6_base_"));
    camera_socket_6_base_->SetRelativeLocation(FVector(0, 0, 0)); //6
    camera_view_character_base_ = CreateDefaultSubobject<USceneComponent>(TEXT("camera_view_character_base_"));
    camera_view_character_base_->SetRelativeLocation(FVector(0, 0, 0)); //view

    SpringSocket1 = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringSocket1"));
    SpringSocket2 = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringSocket2"));
    SpringSocket3 = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringSocket3"));
    SpringSocket4 = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringSocket4"));
    SpringSocket5 = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringSocket5"));
    SpringSocket6 = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringSocket6"));

    //define 6 springs
    SpringMesh = Cast<USkeletalMesh>(UAirBlueprintLib::LoadObject("/AirSim/Blueprints/Spring/SecondTrial"));
    MySpring1 = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("MySpring1"));
    MySpring1->SetSkeletalMesh(SpringMesh);

    MySpring2 = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("MySpring2"));
    MySpring2->SetSkeletalMesh(SpringMesh);

    MySpring3 = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("MySpring3"));
    MySpring3->SetSkeletalMesh(SpringMesh);

    MySpring4 = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("MySpring4"));
    MySpring4->SetSkeletalMesh(SpringMesh);

    MySpring5 = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("MySpring5"));
    MySpring5->SetSkeletalMesh(SpringMesh);

    MySpring6 = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("MySpring6"));
    MySpring6->SetSkeletalMesh(SpringMesh);
    
    //camera_view_character_base_->SetupAttachment(RootComponent);
}

void ASIPCharacterPawn::NotifyHit(class UPrimitiveComponent* MyComp, class AActor* Other, class UPrimitiveComponent* OtherComp, bool bSelfMoved, FVector HitLocation,
                                  FVector HitNormal, FVector NormalImpulse, const FHitResult& Hit)
{
    pawn_events_.getCollisionSignal().emit(MyComp, Other, OtherComp, bSelfMoved, HitLocation, HitNormal, NormalImpulse, Hit);
}

void ASIPCharacterPawn::initializeForBeginPlay(msr::airlib::AirSimSettings::CharacterDefaults Character_info)
{

    dist_vel1 = std::normal_distribution<double>(mean_vel1, Character_info.vel_std_dev1);
    dist_acc1 = std::normal_distribution<double>(mean_acc1, Character_info.acc_std_dev1);

    dist_vel2 = std::normal_distribution<double>(mean_vel2, Character_info.vel_std_dev2);
    dist_acc2 = std::normal_distribution<double>(mean_acc2, Character_info.acc_std_dev2);

    dist_vel3 = std::normal_distribution<double>(mean_vel3, Character_info.vel_std_dev3);
    dist_acc3 = std::normal_distribution<double>(mean_acc3, Character_info.acc_std_dev3);

    dist_vel4 = std::normal_distribution<double>(mean_vel4, Character_info.vel_std_dev4);
    dist_acc4 = std::normal_distribution<double>(mean_acc4, Character_info.acc_std_dev4);

    dist_vel5 = std::normal_distribution<double>(mean_vel5, Character_info.vel_std_dev5);
    dist_acc5 = std::normal_distribution<double>(mean_acc5, Character_info.acc_std_dev5);

    dist_vel6 = std::normal_distribution<double>(mean_vel6, Character_info.vel_std_dev6);
    dist_acc6 = std::normal_distribution<double>(mean_acc6, Character_info.acc_std_dev6);



    std::string CharacterMeshPath = Character_info.pawn_skeletal_mesh;
    if (CharacterMeshPath == "") {
        UAirBlueprintLib::LogMessageString("Failed to load character mesh path", "", LogDebugLevel::Failure);
    }
    else {
        CharacterMesh = Cast<USkeletalMesh>(UAirBlueprintLib::LoadObject(CharacterMeshPath));
        MyCharacter->SetSkeletalMesh(CharacterMesh);
    }

    std::string CharacterAnimPath = Character_info.pawn_animation;

    if (CharacterAnimPath == "") {
        UAirBlueprintLib::LogMessageString("Failed to load character animation path", "", LogDebugLevel::Failure);
    }
    else {
        AnimBp = Cast<UAnimBlueprintGeneratedClass>(UAirBlueprintLib::LoadObject(CharacterAnimPath));
        MyCharacter->SetAnimationMode(EAnimationMode::AnimationBlueprint);
        MyCharacter->SetAnimInstanceClass(AnimBp);
    }

    //put camera little bit above vehicle
    FTransform camera_transform(FVector::ZeroVector);
    FActorSpawnParameters camera_spawn_params;
    camera_spawn_params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

    //load spring animation
    AnimBpSpring = Cast<UAnimBlueprintGeneratedClass>(UAirBlueprintLib::LoadObject("/AirSim/Blueprints/Spring/NewAnimBlueprint.NewAnimBlueprint_C"));

    //Sockets
    Soc1 = Character_info.socket_1;

    if (Soc1 != "") {
        SocName1 = UTF8_TO_TCHAR(Soc1.c_str());
        Socket1 = MyCharacter->GetSocketByName(FName(*SocName1));
        Soc1Spring = Character_info.socket_with_spring1;


        //load if spring is true or false

        if (Soc1Spring) {
           
            MySpring1->SetAnimationMode(EAnimationMode::AnimationBlueprint);
            MySpring1->SetAnimInstanceClass(AnimBpSpring);

            MySpring1->AttachToComponent(MyCharacter, FAttachmentTransformRules::KeepRelativeTransform, Socket1->SocketName);
            const USkeletalMeshSocket* SpringSocketName1= MySpring1->GetSocketByName(FName("Bone_002Socket"));

            SpringSocket1->AttachToComponent(MySpring1, FAttachmentTransformRules::KeepRelativeTransform, SpringSocketName1->SocketName);
            SpringSocket1->SetRelativeLocationAndRotation(FVector(Character_info.cam1x, Character_info.cam1y, Character_info.cam1z), FRotator(Character_info.cam1rotx, Character_info.cam1roty, Character_info.cam1rotz)); //FVector(0.0f, 0.0f, 5.0f),FRotator(90.0f, 90.0f, 0.0f)
            SpringSocket1->TargetArmLength = 0.f;


            camera_socket_1_base_->AttachToComponent(SpringSocket1, FAttachmentTransformRules::KeepRelativeTransform, USpringArmComponent::SocketName);
            camera_view_character_base_->AttachToComponent(SpringSocket1, FAttachmentTransformRules::KeepRelativeTransform, USpringArmComponent::SocketName);
        }
        else {
 
            SpringSocket1->AttachToComponent(MyCharacter, FAttachmentTransformRules::KeepRelativeTransform, Socket1->SocketName);
            SpringSocket1->SetRelativeLocationAndRotation(FVector(Character_info.cam1x, Character_info.cam1y, Character_info.cam1z), FRotator(Character_info.cam1rotx, Character_info.cam1roty, Character_info.cam1rotz));
            SpringSocket1->TargetArmLength = 0.f;

            camera_socket_1_base_->AttachToComponent(SpringSocket1, FAttachmentTransformRules::KeepRelativeTransform, USpringArmComponent::SocketName);
            camera_view_character_base_->AttachToComponent(SpringSocket1, FAttachmentTransformRules::KeepRelativeTransform, USpringArmComponent::SocketName);

        }
        

    }
    else {
        
        camera_socket_1_base_->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
        camera_view_character_base_->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
    }

    Soc2 = Character_info.socket_2;

    if (Soc2 != "") {
        SocName2 = UTF8_TO_TCHAR(Soc2.c_str());
        Socket2 = MyCharacter->GetSocketByName(FName(*SocName2));
        Soc2Spring = Character_info.socket_with_spring2;


        if (Soc2Spring) {

            MySpring2->SetAnimationMode(EAnimationMode::AnimationBlueprint);
            MySpring2->SetAnimInstanceClass(AnimBpSpring);

            MySpring2->AttachToComponent(MyCharacter, FAttachmentTransformRules::KeepRelativeTransform, Socket2->SocketName);
            const USkeletalMeshSocket* SpringSocketName2= MySpring2->GetSocketByName(FName("Bone_002Socket"));

            SpringSocket2->AttachToComponent(MySpring2, FAttachmentTransformRules::KeepRelativeTransform, SpringSocketName2->SocketName);
            SpringSocket2->SetRelativeLocationAndRotation(FVector(Character_info.cam2x, Character_info.cam2y, Character_info.cam2z), FRotator(Character_info.cam2rotx, Character_info.cam2roty, Character_info.cam2rotz)); //FRotator(90.0f, 90.0f, 0.0f)
            SpringSocket2->TargetArmLength = 0.f;


            camera_socket_2_base_->AttachToComponent(SpringSocket2, FAttachmentTransformRules::KeepRelativeTransform, USpringArmComponent::SocketName);
            
        }
        else {

            SpringSocket2->AttachToComponent(MyCharacter, FAttachmentTransformRules::KeepRelativeTransform, Socket2->SocketName);
            SpringSocket2->SetRelativeLocationAndRotation(FVector(Character_info.cam2x, Character_info.cam2y, Character_info.cam2z), FRotator(Character_info.cam2rotx, Character_info.cam2roty, Character_info.cam2rotz));
            SpringSocket2->TargetArmLength = 0.f;

            camera_socket_2_base_->AttachToComponent(SpringSocket2, FAttachmentTransformRules::KeepRelativeTransform, USpringArmComponent::SocketName);


        }
    }
    else {
        camera_socket_2_base_->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
    }

    Soc3 = Character_info.socket_3;

    if (Soc3 != "") {
        SocName3= UTF8_TO_TCHAR(Soc3.c_str());
        Socket3 = MyCharacter->GetSocketByName(FName(*SocName3));
        Soc3Spring = Character_info.socket_with_spring3;

        if (Soc3Spring) {

            MySpring3->SetAnimationMode(EAnimationMode::AnimationBlueprint);
            MySpring3->SetAnimInstanceClass(AnimBpSpring);

            MySpring3->AttachToComponent(MyCharacter, FAttachmentTransformRules::KeepRelativeTransform, Socket3->SocketName);
            const USkeletalMeshSocket* SpringSocketName3= MySpring3->GetSocketByName(FName("Bone_002Socket"));

            SpringSocket3->AttachToComponent(MySpring3, FAttachmentTransformRules::KeepRelativeTransform, SpringSocketName3->SocketName);
            SpringSocket3->SetRelativeLocationAndRotation(FVector(Character_info.cam3x, Character_info.cam3y, Character_info.cam3z), FRotator(Character_info.cam3rotx, Character_info.cam3roty, Character_info.cam3rotz)); //FRotator(90.0f, 90.0f, 0.0f)
            SpringSocket3->TargetArmLength = 0.f;


            camera_socket_3_base_->AttachToComponent(SpringSocket3, FAttachmentTransformRules::KeepRelativeTransform, USpringArmComponent::SocketName);

        }
        else {

            SpringSocket3->AttachToComponent(MyCharacter, FAttachmentTransformRules::KeepRelativeTransform, Socket3->SocketName);
            SpringSocket3->SetRelativeLocationAndRotation(FVector(Character_info.cam3x, Character_info.cam3y, Character_info.cam3z), FRotator(Character_info.cam3rotx, Character_info.cam3roty, Character_info.cam3rotz));
            SpringSocket3->TargetArmLength = 0.f;

            camera_socket_3_base_->AttachToComponent(SpringSocket3, FAttachmentTransformRules::KeepRelativeTransform, USpringArmComponent::SocketName);


        }
    }
    else {
        camera_socket_3_base_->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
    }

    Soc4 = Character_info.socket_4;

    if (Soc4 != "") {
        SocName4 = UTF8_TO_TCHAR(Soc4.c_str());
        Socket4 = MyCharacter->GetSocketByName(FName(*SocName4));
        Soc4Spring = Character_info.socket_with_spring4;

        if (Soc4Spring) {

            MySpring4->SetAnimationMode(EAnimationMode::AnimationBlueprint);
            MySpring4->SetAnimInstanceClass(AnimBpSpring);

            MySpring4->AttachToComponent(MyCharacter, FAttachmentTransformRules::KeepRelativeTransform, Socket4->SocketName);
            const USkeletalMeshSocket* SpringSocketName4= MySpring4->GetSocketByName(FName("Bone_002Socket"));

            SpringSocket4->AttachToComponent(MySpring4, FAttachmentTransformRules::KeepRelativeTransform, SpringSocketName4->SocketName);
            SpringSocket4->SetRelativeLocationAndRotation(FVector(Character_info.cam4x, Character_info.cam4y, Character_info.cam4z), FRotator(Character_info.cam4rotx, Character_info.cam4roty, Character_info.cam4rotz)); //FRotator(90.0f, 90.0f, 0.0f)
            SpringSocket4->TargetArmLength = 0.f;


            camera_socket_4_base_->AttachToComponent(SpringSocket4, FAttachmentTransformRules::KeepRelativeTransform, USpringArmComponent::SocketName);

        }
        else {

            SpringSocket4->AttachToComponent(MyCharacter, FAttachmentTransformRules::KeepRelativeTransform, Socket4->SocketName);
            SpringSocket4->SetRelativeLocationAndRotation(FVector(Character_info.cam4x, Character_info.cam4y, Character_info.cam4z), FRotator(Character_info.cam4rotx, Character_info.cam4roty, Character_info.cam4rotz));
            SpringSocket4->TargetArmLength = 0.f;

            camera_socket_4_base_->AttachToComponent(SpringSocket4, FAttachmentTransformRules::KeepRelativeTransform, USpringArmComponent::SocketName);


        }
    }
    else {
        camera_socket_4_base_->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
    }

    Soc5 = Character_info.socket_5;

    if (Soc5 != "") {
        SocName5= UTF8_TO_TCHAR(Soc5.c_str());
        Socket5 = MyCharacter->GetSocketByName(FName(*SocName5));
        Soc5Spring = Character_info.socket_with_spring5;

        if (Soc5Spring) {

            MySpring5->SetAnimationMode(EAnimationMode::AnimationBlueprint);
            MySpring5->SetAnimInstanceClass(AnimBpSpring);

            MySpring5->AttachToComponent(MyCharacter, FAttachmentTransformRules::KeepRelativeTransform, Socket5->SocketName);
            const USkeletalMeshSocket* SpringSocketName5= MySpring5->GetSocketByName(FName("Bone_002Socket"));

            SpringSocket5->AttachToComponent(MySpring5, FAttachmentTransformRules::KeepRelativeTransform, SpringSocketName5->SocketName);
            SpringSocket5->SetRelativeLocationAndRotation(FVector(Character_info.cam5x, Character_info.cam5y, Character_info.cam5z), FRotator(Character_info.cam5rotx, Character_info.cam5roty, Character_info.cam5rotz)); //FRotator(90.0f, 90.0f, 0.0f)
            SpringSocket5->TargetArmLength = 0.f;


            camera_socket_5_base_->AttachToComponent(SpringSocket5, FAttachmentTransformRules::KeepRelativeTransform, USpringArmComponent::SocketName);

        }
        else {

            SpringSocket5->AttachToComponent(MyCharacter, FAttachmentTransformRules::KeepRelativeTransform, Socket5->SocketName);
            SpringSocket5->SetRelativeLocationAndRotation(FVector(Character_info.cam5x, Character_info.cam5y, Character_info.cam5z), FRotator(Character_info.cam5rotx, Character_info.cam5roty, Character_info.cam5rotz));
            SpringSocket5->TargetArmLength = 0.f;

            camera_socket_5_base_->AttachToComponent(SpringSocket5, FAttachmentTransformRules::KeepRelativeTransform, USpringArmComponent::SocketName);


        }
    }
    else {
        camera_socket_5_base_->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
    }

    Soc6 = Character_info.socket_6;

    if (Soc6 != "") {
        SocName6= UTF8_TO_TCHAR(Soc6.c_str());
        Socket6 = MyCharacter->GetSocketByName(FName(*SocName6));
        Soc6Spring = Character_info.socket_with_spring6;

        if (Soc6Spring) {

            MySpring6->SetAnimationMode(EAnimationMode::AnimationBlueprint);
            MySpring6->SetAnimInstanceClass(AnimBpSpring);

            MySpring6->AttachToComponent(MyCharacter, FAttachmentTransformRules::KeepRelativeTransform, Socket6->SocketName);
            const USkeletalMeshSocket* SpringSocketName6= MySpring6->GetSocketByName(FName("Bone_002Socket"));

            SpringSocket6->AttachToComponent(MySpring6, FAttachmentTransformRules::KeepRelativeTransform, SpringSocketName6->SocketName);
            SpringSocket6->SetRelativeLocationAndRotation(FVector(Character_info.cam6x, Character_info.cam6y, Character_info.cam6z), FRotator(Character_info.cam6rotx, Character_info.cam6roty, Character_info.cam6rotz)); //FRotator(90.0f, 90.0f, 0.0f)
            SpringSocket6->TargetArmLength = 0.f;


            camera_socket_6_base_->AttachToComponent(SpringSocket6, FAttachmentTransformRules::KeepRelativeTransform, USpringArmComponent::SocketName);

        }
        else {

            SpringSocket6->AttachToComponent(MyCharacter, FAttachmentTransformRules::KeepRelativeTransform, Socket6->SocketName);
            SpringSocket6->SetRelativeLocationAndRotation(FVector(Character_info.cam6x, Character_info.cam6y, Character_info.cam6z), FRotator(Character_info.cam6rotx, Character_info.cam6roty, Character_info.cam6rotz));
            SpringSocket6->TargetArmLength = 0.f;

            camera_socket_6_base_->AttachToComponent(SpringSocket6, FAttachmentTransformRules::KeepRelativeTransform, USpringArmComponent::SocketName);


        }
    }
    else {
        camera_socket_6_base_->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
    }

    camera_spawn_params.Name = FName(*(this->GetName() + "_camera_view_character"));
    camera_view_character_ = this->GetWorld()->SpawnActor<APIPCamera>(pip_camera_class_, FTransform(FRotator(0, 180, 0), FVector(200, 0, 100)), camera_spawn_params);
    camera_view_character_->AttachToComponent(camera_view_character_base_, FAttachmentTransformRules::KeepRelativeTransform);

    camera_spawn_params.Name = FName(*(this->GetName() + "_camera_socket_1"));
    camera_socket_1_ = this->GetWorld()->SpawnActor<APIPCamera>(pip_camera_class_, camera_transform, camera_spawn_params);
    camera_socket_1_->AttachToComponent(camera_socket_1_base_, FAttachmentTransformRules::KeepRelativeTransform);

    camera_spawn_params.Name = FName(*(this->GetName() + "_camera_socket_2"));
    camera_socket_2_ = this->GetWorld()->SpawnActor<APIPCamera>(pip_camera_class_, camera_transform, camera_spawn_params);
    camera_socket_2_->AttachToComponent(camera_socket_2_base_, FAttachmentTransformRules::KeepRelativeTransform);

    camera_spawn_params.Name = FName(*(this->GetName() + "_camera_socket_3"));
    camera_socket_3_ = this->GetWorld()->SpawnActor<APIPCamera>(pip_camera_class_, camera_transform, camera_spawn_params);
    camera_socket_3_->AttachToComponent(camera_socket_3_base_, FAttachmentTransformRules::KeepRelativeTransform);

    camera_spawn_params.Name = FName(*(this->GetName() + "_camera_socket_4"));
    camera_socket_4_ = this->GetWorld()->SpawnActor<APIPCamera>(pip_camera_class_, camera_transform, camera_spawn_params);
    camera_socket_4_->AttachToComponent(camera_socket_4_base_, FAttachmentTransformRules::KeepRelativeTransform);

    camera_spawn_params.Name = FName(*(this->GetName() + "_camera_socket_5"));
    camera_socket_5_ = this->GetWorld()->SpawnActor<APIPCamera>(pip_camera_class_, camera_transform, camera_spawn_params);
    camera_socket_5_->AttachToComponent(camera_socket_5_base_, FAttachmentTransformRules::KeepRelativeTransform);

    camera_spawn_params.Name = FName(*(this->GetName() + "_camera_socket_6"));
    camera_socket_6_ = this->GetWorld()->SpawnActor<APIPCamera>(pip_camera_class_, camera_transform, camera_spawn_params);
    camera_socket_6_->AttachToComponent(camera_socket_6_base_, FAttachmentTransformRules::KeepRelativeTransform);

    manual_pose_controller_ = NewObject<UManualPoseController>(this, "SIPCharacter_ManualPoseController");
    manual_pose_controller_->initializeForPlay();
    manual_pose_controller_->setActor(this);
}

const common_utils::UniqueValueMap<std::string, APIPCamera*> ASIPCharacterPawn::getCameras() const
{
    common_utils::UniqueValueMap<std::string, APIPCamera*> cameras;
    cameras.insert_or_assign("view_character", camera_view_character_);
    cameras.insert_or_assign("socket1", camera_socket_1_);
    cameras.insert_or_assign("socket2", camera_socket_2_);
    cameras.insert_or_assign("socket3", camera_socket_3_);
    cameras.insert_or_assign("socket4", camera_socket_4_);
    cameras.insert_or_assign("socket5", camera_socket_5_);
    cameras.insert_or_assign("socket6", camera_socket_6_);

    cameras.insert_or_assign("0", camera_view_character_);
    cameras.insert_or_assign("1", camera_socket_1_);
    cameras.insert_or_assign("2", camera_socket_2_);
    cameras.insert_or_assign("3", camera_socket_3_);
    cameras.insert_or_assign("4", camera_socket_4_);
    cameras.insert_or_assign("5", camera_socket_5_);
    cameras.insert_or_assign("6", camera_socket_6_);

    cameras.insert_or_assign("fpv", camera_view_character_);
    cameras.insert_or_assign("", camera_view_character_);

    return cameras;
}

void ASIPCharacterPawn::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    camera_socket_1_ = nullptr;
    camera_socket_2_ = nullptr;
    camera_socket_3_ = nullptr;
    camera_socket_4_ = nullptr;
    camera_socket_5_ = nullptr;
    camera_socket_6_ = nullptr;
    camera_view_character_ = nullptr;

    camera_socket_1_base_ = nullptr;
    camera_socket_2_base_ = nullptr;
    camera_socket_3_base_ = nullptr;
    camera_socket_4_base_ = nullptr;
    camera_socket_5_base_ = nullptr;
    camera_socket_6_base_ = nullptr;
    camera_view_character_base_ = nullptr;

    manual_pose_controller_ = nullptr;
}

void ASIPCharacterPawn::Tick(float Delta)
{
    Super::Tick(Delta);
    pawn_events_.getPawnTickSignal().emit(Delta);

    //update ground level
    if (manual_pose_controller_->getActor() == this) {
        manual_pose_controller_->updateActorPose(Delta);
    }

    auto current_clockspeed_ = msr::airlib::AirSimSettings::singleton().clock_speed;
    auto freq = msr::airlib::AirSimSettings::singleton().recording_setting.record_interval;
    counter += 1;

    if (counter * current_clockspeed_ >= freq){

        if (Soc1 != "") {
            IMU1 = FName(*SocName1);
            RotLast1 = RotCurrent1;
            RotCurrent1= MyCharacter->GetSocketQuaternion(IMU1);

            DeltaRotation1= RotCurrent1* RotLast1.Inverse();
            DeltaRotation1= DeltaRotation1.GetNormalized();
            if (DeltaRotation1.W < 0.0) {
                DeltaRotation1.W = -1 * DeltaRotation1.W;
                DeltaRotation1.X = -1 * DeltaRotation1.X;
                DeltaRotation1.Y = -1 * DeltaRotation1.Y;
                DeltaRotation1.Z = -1 * DeltaRotation1.Z;
            }


            DeltaRotation1.ToAxisAndAngle(Axis1, Angle1);
            AngularVelocity1= Axis1 * Angle1 / freq + FVector(this->dist_vel1(generator), dist_vel1(generator), dist_vel1(generator));

            LocSecondLast1= LocLast1;
            LocLast1= LocCurrent1;
            LocCurrent1= MyCharacter->GetSocketLocation(IMU1);

            LinearAcceleration1 = ((LocCurrent1 - 2 * LocLast1 + LocSecondLast1) / freq) / freq - Gravity + FVector(dist_acc1(generator), dist_acc1(generator), dist_acc1(generator));

            UAirBlueprintLib::LogMessage(TEXT("LinearAcceleration:"), LinearAcceleration1.ToString(), LogDebugLevel::Informational);
            UAirBlueprintLib::LogMessage(TEXT("AngularVelocity:"), AngularVelocity1.ToString(), LogDebugLevel::Informational);

        }
        if (Soc2 != "") {
            IMU2 = FName(*SocName2);
            RotLast2 = RotCurrent2;
            RotCurrent2 = MyCharacter->GetSocketQuaternion(IMU2);

            DeltaRotation2 = RotCurrent2 * RotLast2.Inverse();
            DeltaRotation2 = DeltaRotation2.GetNormalized();
            if (DeltaRotation2.W < 0.0) {
                DeltaRotation2.W = -1 * DeltaRotation2.W;
                DeltaRotation2.X = -1 * DeltaRotation2.X;
                DeltaRotation2.Y = -1 * DeltaRotation2.Y;
                DeltaRotation2.Z = -1 * DeltaRotation2.Z;
            }


            DeltaRotation2.ToAxisAndAngle(Axis2, Angle2);
            AngularVelocity2 = Axis2 * Angle2 / freq + FVector(this->dist_vel2(generator), dist_vel2(generator), dist_vel2(generator));

            LocSecondLast2 = LocLast2;
            LocLast2 = LocCurrent2;
            LocCurrent2 = MyCharacter->GetSocketLocation(IMU2);

            LinearAcceleration2 = ((LocCurrent2 - 2 * LocLast2 + LocSecondLast2) / freq) / freq - Gravity + FVector(dist_acc2(generator), dist_acc2(generator), dist_acc2(generator));

        }
        if (Soc3 != "") {
            IMU3 = FName(*SocName3);
            RotLast3 = RotCurrent3;
            RotCurrent3 = MyCharacter->GetSocketQuaternion(IMU3);

            DeltaRotation3 = RotCurrent3 * RotLast3.Inverse();
            DeltaRotation3 = DeltaRotation3.GetNormalized();
            if (DeltaRotation3.W < 0.0) {
                DeltaRotation3.W = -1 * DeltaRotation3.W;
                DeltaRotation3.X = -1 * DeltaRotation3.X;
                DeltaRotation3.Y = -1 * DeltaRotation3.Y;
                DeltaRotation3.Z = -1 * DeltaRotation3.Z;
            }


            DeltaRotation3.ToAxisAndAngle(Axis3, Angle3);
            AngularVelocity3 = Axis3 * Angle3 / freq + FVector(this->dist_vel3(generator), dist_vel3(generator), dist_vel3(generator));

            LocSecondLast3 = LocLast3;
            LocLast3 = LocCurrent3;
            LocCurrent3 = MyCharacter->GetSocketLocation(IMU3);

            LinearAcceleration3 = ((LocCurrent3 - 2 * LocLast3 + LocSecondLast3) / freq) / freq - Gravity + FVector(dist_acc3(generator), dist_acc3(generator), dist_acc3(generator));


        }
        if (Soc4 != "") {
            IMU4 = FName(*SocName4);
            RotLast4 = RotCurrent4;
            RotCurrent4 = MyCharacter->GetSocketQuaternion(IMU4);

            DeltaRotation4 = RotCurrent4 * RotLast4.Inverse();
            DeltaRotation4 = DeltaRotation4.GetNormalized();
            if (DeltaRotation4.W < 0.0) {
                DeltaRotation4.W = -1 * DeltaRotation4.W;
                DeltaRotation4.X = -1 * DeltaRotation4.X;
                DeltaRotation4.Y = -1 * DeltaRotation4.Y;
                DeltaRotation4.Z = -1 * DeltaRotation4.Z;
            }


            DeltaRotation4.ToAxisAndAngle(Axis4, Angle4);
            AngularVelocity4 = Axis4 * Angle4 / freq + FVector(this->dist_vel4(generator), dist_vel4(generator), dist_vel4(generator));

            LocSecondLast4 = LocLast4;
            LocLast4 = LocCurrent4;
            LocCurrent4 = MyCharacter->GetSocketLocation(IMU4);

            LinearAcceleration4 = ((LocCurrent4 - 2 * LocLast4 + LocSecondLast4) / freq) / freq - Gravity + FVector(dist_acc4(generator), dist_acc4(generator), dist_acc4(generator));


        }
        if (Soc5 != "") {
            IMU5 = FName(*SocName5);
            RotLast5 = RotCurrent5;
            RotCurrent5 = MyCharacter->GetSocketQuaternion(IMU5);

            DeltaRotation5 = RotCurrent5 * RotLast5.Inverse();
            DeltaRotation5 = DeltaRotation5.GetNormalized();
            if (DeltaRotation5.W < 0.0) {
                DeltaRotation5.W = -1 * DeltaRotation5.W;
                DeltaRotation5.X = -1 * DeltaRotation5.X;
                DeltaRotation5.Y = -1 * DeltaRotation5.Y;
                DeltaRotation5.Z = -1 * DeltaRotation5.Z;
            }


            DeltaRotation5.ToAxisAndAngle(Axis5, Angle5);
            AngularVelocity5 = Axis5 * Angle5 / freq + FVector(this->dist_vel5(generator), dist_vel5(generator), dist_vel5(generator));

            LocSecondLast5 = LocLast5;
            LocLast5 = LocCurrent5;
            LocCurrent5 = MyCharacter->GetSocketLocation(IMU5);

            LinearAcceleration5 = ((LocCurrent5 - 2 * LocLast5 + LocSecondLast5) / freq) / freq - Gravity + FVector(dist_acc5(generator), dist_acc5(generator), dist_acc5(generator));

        }
        if (Soc6 != "") {
            IMU6 = FName(*SocName6);
            RotLast6 = RotCurrent6;
            RotCurrent6 = MyCharacter->GetSocketQuaternion(IMU6);

            DeltaRotation6 = RotCurrent6 * RotLast6.Inverse();
            DeltaRotation6 = DeltaRotation6.GetNormalized();
            if (DeltaRotation6.W < 0.0) {
                DeltaRotation6.W = -1 * DeltaRotation6.W;
                DeltaRotation6.X = -1 * DeltaRotation6.X;
                DeltaRotation6.Y = -1 * DeltaRotation6.Y;
                DeltaRotation6.Z = -1 * DeltaRotation6.Z;
            }


            DeltaRotation6.ToAxisAndAngle(Axis6, Angle6);
            AngularVelocity6 = Axis6 * Angle6 / freq + FVector(this->dist_vel6(generator), dist_vel6(generator), dist_vel6(generator)) + FVector(dist_acc6(generator), dist_acc6(generator), dist_acc6(generator));

            LocSecondLast6 = LocLast6;
            LocLast6 = LocCurrent6;
            LocCurrent6 = MyCharacter->GetSocketLocation(IMU6);

            LinearAcceleration6 = ((LocCurrent6 - 2 * LocLast6 + LocSecondLast6) / freq) / freq - Gravity;

        }




        counter = 0;
    }

}

void ASIPCharacterPawn::BeginPlay()
{
    Super::BeginPlay();
}