#include "SIPCharacterPawn.h"
#include "Engine/World.h"
#include "ManualPoseController.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/SpringArmComponent.h"
#include "AirBlueprintLib.h"
#include "Engine/SkeletalMeshSocket.h"
#include "AirBlueprintLib.h"
#include "Animation/AnimSingleNodeInstance.h"
#include "Components/BillboardComponent.h"
#include "DrawDebugHelpers.h" 


FString getAnimationForSkeletalMesh(std::string path) {
    // Find the last occurrence of '/' 
    size_t lastSlash = path.rfind('/');

    if (lastSlash == std::string::npos) {
        // No '/' found, return the original path
        return FString(path.c_str());
    }

    // Find the first occurrence of '.' after the last '/'
    size_t dot = path.find('.', lastSlash);

    if (dot == std::string::npos) {
        // No '.' found after last '/', return the original path
        return FString(path.c_str());
    }

    // Insert "_Anim" at the end of filename and after the dot
    std::string newPath = path;
    newPath.insert(dot, "_Anim");
    newPath.append("_Anim");

    return FString(newPath.c_str());
    //return FString("AnimSequence'/Game/chars/anim001/anim001_Anim.anim001_Anim'");
}

UTexture* LoadRandomTextureFromFolder(const FString& FolderPath)
{
    // Access the Asset Registry
    FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry");
    TArray<FAssetData> AssetData;

    // Define the filter for assets
    FARFilter Filter;
    Filter.ClassNames.Add(UTexture::StaticClass()->GetFName());
    Filter.PackagePaths.Add(*FolderPath);
    Filter.bRecursivePaths = true;
    Filter.bRecursiveClasses = true;

    // Query the Asset Registry for assets matching the filter
    AssetRegistryModule.Get().GetAssets(Filter, AssetData);

    if (AssetData.Num() == 0) {
        UE_LOG(LogTemp, Warning, TEXT("No textures found in the specified folder: %s"), *FolderPath);

        return nullptr;
    }

    // Select a random texture
    int32 RandomIndex = FMath::RandRange(0, AssetData.Num() - 1);
    FAssetData RandomTextureData = AssetData[RandomIndex];

    // Load and return the texture
    return Cast<UTexture>(StaticLoadObject(UTexture::StaticClass(), nullptr, *RandomTextureData.ObjectPath.ToString()));
}


ASIPCharacterPawn::ASIPCharacterPawn()
{
    PrimaryActorTick.bCanEverTick = true;
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

//if (use_spring) {
//    spring->SetAnimationMode(EAnimationMode::AnimationBlueprint);
//    spring->SetAnimInstanceClass(AnimBpSpring);

//    spring->AttachToComponent(MyCharacter, FAttachmentTransformRules::KeepRelativeTransform, joint_name.c_str());
//    const USkeletalMeshSocket* SpringSocketName1 = spring->GetSocketByName(FName("Bone_002Socket"));

//    SpringSocket->AttachToComponent(spring, FAttachmentTransformRules::KeepRelativeTransform, SpringSocketName1->SocketName);
//    SpringSocket->SetRelativeLocationAndRotation(FVector(trans_x, trans_y, trans_z), FRotator(rot_y, rot_z, rot_x));
//    SpringSocket->TargetArmLength = 0.f;

//    camera_base->AttachToComponent(SpringSocket, FAttachmentTransformRules::KeepRelativeTransform, USpringArmComponent::SocketName);
//    //camera_view_character_base_->AttachToComponent(SpringSocket, FAttachmentTransformRules::KeepRelativeTransform, USpringArmComponent::SocketName);
//}
void ASIPCharacterPawn::SetupCameraBase(USkeletalMeshComponent* spring, const std::string& joint_name, USpringArmComponent* SpringSocket, USceneComponent* camera_base, const bool use_spring, const float lag_speed,
    const float trans_x, const float trans_y, const float trans_z, const float rot_y, const float rot_z, const float rot_x) {

    if (use_spring) {
        SpringSocket->AttachToComponent(MyCharacter, FAttachmentTransformRules::KeepRelativeTransform, joint_name.c_str());
        SpringSocket->SetRelativeLocationAndRotation(FVector(trans_x, trans_y, trans_z), FRotator(rot_y, rot_z, rot_x));
        SpringSocket->TargetArmLength = 0.f;
        SpringSocket->bEnableCameraLag = true;
        SpringSocket->bEnableCameraRotationLag = true;
        SpringSocket->CameraLagSpeed = lag_speed;
        SpringSocket->CameraLagMaxDistance = 5.f;

        camera_base->AttachToComponent(SpringSocket, FAttachmentTransformRules::KeepRelativeTransform, USpringArmComponent::SocketName);
        //camera_base->AttachToComponent(MyCharacter, FAttachmentTransformRules::KeepRelativeTransform, joint_name.c_str());
        //camera_base->SetRelativeLocationAndRotation(FVector(trans_x, trans_y, trans_z), FRotator(rot_y, rot_z, rot_x));
    }
    else {
        /* SpringSocket->AttachToComponent(MyCharacter, FAttachmentTransformRules::KeepRelativeTransform, joint_name.c_str());
         SpringSocket->SetRelativeLocationAndRotation(FVector(trans_x, trans_y, trans_z), FRotator(rot_y, rot_z, rot_x));
         SpringSocket->TargetArmLength = 0.f;*/

         //camera_base->AttachToComponent(SpringSocket, FAttachmentTransformRules::KeepRelativeTransform, USpringArmComponent::SocketName);
        camera_base->AttachToComponent(MyCharacter, FAttachmentTransformRules::KeepRelativeTransform, joint_name.c_str());
        camera_base->SetRelativeLocationAndRotation(FVector(trans_x, trans_y, trans_z), FRotator(rot_y, rot_z, rot_x));
        //camera_view_character_base_->AttachToComponent(SpringSocket, FAttachmentTransformRules::KeepRelativeTransform, USpringArmComponent::SocketName);
    }

    // spawn tiny visual marker to show the camera
#if WITH_EDITOR
  //// Ensure the camera_base is valid before proceeding
  //  if (GEngine && camera_base) {
  //      FVector DebugLocation = camera_base->GetComponentLocation();
  //      DrawDebugSphere(GetWorld(), DebugLocation, 10.0f, 12, FColor::Red, true, -1.0f, (uint8)'\000', 1.0f);
  //  }
#endif
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
        //CharacterMesh = Cast<USkeletalMesh>(UAirBlueprintLib::LoadObject(CharacterMeshPath));
        //MyCharacter->SetSkeletalMesh(CharacterMesh);
        // Load the skeletal mesh synchronously
        CharacterMesh = Cast<USkeletalMesh>(StaticLoadObject(USkeletalMesh::StaticClass(), nullptr, *FString(CharacterMeshPath.c_str())));

        if (CharacterMesh) {
            MyCharacter->SetSkeletalMesh(CharacterMesh);
        }
        else {
            UAirBlueprintLib::LogMessageString("Failed to load character mesh from path", CharacterMeshPath, LogDebugLevel::Failure);
        }
    }


    // set the animation automatically based on the skeleton mesh
    std::string CharacterSkeletonPath = Character_info.pawn_skeletal_mesh;
    FString configAnimationPath = getAnimationForSkeletalMesh(CharacterSkeletonPath); // Path from your config file
    UAnimSequence* AnimSequence = Cast<UAnimSequence>(StaticLoadObject(UAnimSequence::StaticClass(), nullptr, *configAnimationPath));
    MyCharacter->SetAnimationMode(EAnimationMode::AnimationSingleNode);
    MyCharacter->PlayAnimation(AnimSequence, true);

    // Set the material from the settings
    //FString materialPath(Character_info.pawn_material.c_str());
    FString materialPath(Character_info.pawn_material_path.c_str());
    UMaterialInterface* material = Cast<UMaterialInterface>(StaticLoadObject(UMaterialInterface::StaticClass(), nullptr, *materialPath));
    UMaterialInstanceDynamic* dynamicMaterial = UMaterialInstanceDynamic::Create(material, this);


    // Example code for setting the textures of a dynamic material for material randomization
    //UTexture* clothTexture = LoadRandomTextureFromFolder(Character_info.pawn_cloth_texture_path.c_str());
    //UTexture* skinTexture = LoadRandomTextureFromFolder(Character_info.pawn_skin_texture_path.c_str());
    //dynamicMaterial->SetTextureParameterValue(FName("ClothTexture"), clothTexture);
    //dynamicMaterial->SetTextureParameterValue(FName("SkinTexture"), skinTexture);

    MyCharacter->SetMaterial(0, dynamicMaterial);

    //put camera little bit above vehicle
    FTransform camera_transform(FVector::ZeroVector);
    FActorSpawnParameters camera_spawn_params;
    camera_spawn_params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

    //load spring animation
    AnimBpSpring = Cast<UAnimBlueprintGeneratedClass>(UAirBlueprintLib::LoadObject("/AirSim/Blueprints/Spring/NewAnimBlueprint.NewAnimBlueprint_C"));

    joint_1 = Character_info.joint_1;

    if (joint_1 != "") {
        Soc1Spring = Character_info.socket_with_spring1;
        SetupCameraBase(MySpring1, joint_1, SpringSocket1, camera_socket_1_base_, Soc1Spring, Character_info.lag_speed1,
                        Character_info.cam1x, Character_info.cam1y, Character_info.cam1z, Character_info.cam1roty, Character_info.cam1rotz, Character_info.cam1rotx);
    }
    else {
        
        camera_socket_1_base_->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
        camera_view_character_base_->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
    }

    joint_2 = Character_info.joint_2;

    if (joint_2 != "") {
        Soc2Spring = Character_info.socket_with_spring2;
		SetupCameraBase(MySpring2, joint_2, SpringSocket2, camera_socket_2_base_, Soc2Spring, Character_info.lag_speed2,
            			Character_info.cam2x, Character_info.cam2y, Character_info.cam2z, Character_info.cam2roty, Character_info.cam2rotz, Character_info.cam2rotx);
    }
    else {
        camera_socket_2_base_->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
    }

    joint_3 = Character_info.joint_3;

    if (joint_3 != "") {
        Soc3Spring = Character_info.socket_with_spring3;
        SetupCameraBase(MySpring3, joint_3, SpringSocket3, camera_socket_3_base_, Soc3Spring, Character_info.lag_speed3,
            			Character_info.cam3x, Character_info.cam3y, Character_info.cam3z, Character_info.cam3roty, Character_info.cam3rotz, Character_info.cam3rotx);
    }
    else {
        camera_socket_3_base_->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
    }

    joint_4 = Character_info.joint_4;

    if (joint_4 != "") {
        Soc4Spring = Character_info.socket_with_spring4;
        SetupCameraBase(MySpring4, joint_4, SpringSocket4, camera_socket_4_base_, Soc4Spring, Character_info.lag_speed4,
                        Character_info.cam4x, Character_info.cam4y, Character_info.cam4z, Character_info.cam4roty, Character_info.cam4rotz, Character_info.cam4rotx);
    }
    else {
        camera_socket_4_base_->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
    }

    joint_5 = Character_info.joint_5;

    if (joint_5 != "") {
        Soc5Spring = Character_info.socket_with_spring5;
        SetupCameraBase(MySpring5, joint_5, SpringSocket5, camera_socket_5_base_, Soc5Spring, Character_info.lag_speed5,
            Character_info.cam5x, Character_info.cam5y, Character_info.cam5z, Character_info.cam5roty, Character_info.cam5rotz, Character_info.cam5rotx);
    }
    else {
        camera_socket_5_base_->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
    }

    joint_6 = Character_info.joint_6;

    if (joint_6 != "") {
        Soc6Spring = Character_info.socket_with_spring6;
		SetupCameraBase(MySpring6, joint_6, SpringSocket6, camera_socket_6_base_, Soc6Spring, Character_info.lag_speed6,
            						Character_info.cam6x, Character_info.cam6y, Character_info.cam6z, Character_info.cam6roty, Character_info.cam6rotz, Character_info.cam6rotx);
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

    //manual_pose_controller_ = NewObject<UManualPoseController>(this, "SIPCharacter_ManualPoseController");
    //manual_pose_controller_->initializeForPlay();
    //manual_pose_controller_->setActor(this);
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

float ASIPCharacterPawn::GetAnimTimeS() const {
    // only works from insde game thread
    if (MyCharacter != nullptr) {
        UAnimSingleNodeInstance* anim_instance = MyCharacter->GetSingleNodeInstance();
        bool pointer_valid = IsValid(anim_instance);
        if (anim_instance != nullptr && anim_instance->IsValidLowLevel() && anim_instance->IsPlaying()) {
            float time = anim_instance->GetCurrentTime();
            return time;
        }
        else {
            UE_LOG(LogTemp, Warning, TEXT("Animation instance is null."));
        }
    }
    else {
        UE_LOG(LogTemp, Warning, TEXT("MyCharacter is null."));
    }
    return 0.0f; // Default return if checks fail
}

float ASIPCharacterPawn::GetLastAnimTimeS() const {
    return animationTS_s;
}

void ASIPCharacterPawn::Tick(float Delta)
{
    Super::Tick(Delta);
    pawn_events_.getPawnTickSignal().emit(Delta);

    animationTS_s = GetAnimTimeS();

    //update ground level
    //if (manual_pose_controller_->getActor() == this) {
    //    manual_pose_controller_->updateActorPose(Delta);
    //}

#if WITH_EDITOR || UE_BUILD_DEVELOPMENT
    if (GEngine && camera_socket_1_base_) {

        // Draw the debug sphere at the new location
        //DrawDebugSphere(GetWorld(), DebugLocation, 10.0f, 12, FColor::Blue, true, 2.0f, (uint8)'\000', 1.0f);
       /* DrawDebugSphere(GetWorld(), camera_socket_1_base_->GetComponentLocation(), 5.0f, 12, FColor::Blue, false, -1.0f, (uint8)'\000', 1.0f);
        DrawDebugSphere(GetWorld(), camera_socket_2_base_->GetComponentLocation(), 5.0f, 12, FColor::Green, false, -1.0f, (uint8)'\000', 1.0f);
        DrawDebugSphere(GetWorld(), camera_socket_3_base_->GetComponentLocation(), 5.0f, 12, FColor::Orange, false, -1.0f, (uint8)'\000', 1.0f);
        DrawDebugSphere(GetWorld(), camera_socket_4_base_->GetComponentLocation(), 5.0f, 12, FColor::Orange, false, -1.0f, (uint8)'\000', 1.0f);
        DrawDebugSphere(GetWorld(), camera_socket_5_base_->GetComponentLocation(), 5.0f, 12, FColor::Purple, false, -1.0f, (uint8)'\000', 1.0f);
        DrawDebugSphere(GetWorld(), camera_socket_6_base_->GetComponentLocation(), 5.0f, 12, FColor::Purple, false, -1.0f, (uint8)'\000', 1.0f);*/
    }
#endif

    auto current_clockspeed_ = msr::airlib::AirSimSettings::singleton().clock_speed;
    auto sample_interval = msr::airlib::AirSimSettings::singleton().recording_setting.record_interval;
    delta_since_last_sample += Delta;

    if (delta_since_last_sample / current_clockspeed_ >= sample_interval){

        auto delta_current_scaled = delta_since_last_sample / current_clockspeed_;
        auto delta_last_sample_scaled = delta_last_sample / current_clockspeed_;

        if (joint_1 != "") {
            RotLast1 = RotCurrent1;
            RotCurrent1= camera_socket_1_base_->GetComponentQuat();

            DeltaRotation1 = RotCurrent1 * RotLast1.Inverse();
            DeltaRotation1= DeltaRotation1.GetNormalized();
            if (DeltaRotation1.W < 0.0) {
                DeltaRotation1.W = -1 * DeltaRotation1.W;
                DeltaRotation1.X = -1 * DeltaRotation1.X;
                DeltaRotation1.Y = -1 * DeltaRotation1.Y;
                DeltaRotation1.Z = -1 * DeltaRotation1.Z;
            }
            // Transform Rotation in WF to Rotation in LF
            DeltaRotation1 = RotCurrent1.Inverse() * DeltaRotation1 * RotCurrent1;

            DeltaRotation1.ToAxisAndAngle(Axis1, Angle1);
            AngularVelocity1 = Axis1 * Angle1 / delta_current_scaled + FVector(this->dist_vel1(generator), dist_vel1(generator), dist_vel1(generator));

            LocSecondLast1= LocLast1;
            LocLast1= LocCurrent1;
            // convert location to meters
            LocCurrent1 = camera_socket_1_base_->GetComponentLocation();
            
            auto LinearAcceleration1_wf = (LocCurrent1 - LocLast1)/ (delta_current_scaled*delta_current_scaled) - (LocLast1 - LocSecondLast1) / (delta_current_scaled * delta_last_sample_scaled) - Gravity + FVector(dist_acc1(generator), dist_acc1(generator), dist_acc1(generator));
            // convert acceleration to local frame of IMU
            LinearAcceleration1 = RotCurrent1.Inverse().RotateVector(LinearAcceleration1_wf);

            //UAirBlueprintLib::LogMessage(TEXT("Current Position:"), LocCurrent1.ToString(), LogDebugLevel::Informational);
            //UAirBlueprintLib::LogMessage(TEXT("LinearAcceleration:"), LinearAcceleration1.ToString(), LogDebugLevel::Informational);
            //UAirBlueprintLib::LogMessage(TEXT("AngularVelocity:"), AngularVelocity1.ToString(), LogDebugLevel::Informational);
        }
        if (joint_2 != "") {
            RotLast2 = RotCurrent2;
            RotCurrent2 = camera_socket_2_base_->GetComponentQuat();

            DeltaRotation2 = RotCurrent2 * RotLast2.Inverse();
            DeltaRotation2 = DeltaRotation2.GetNormalized();
            if (DeltaRotation2.W < 0.0) {
                DeltaRotation2.W = -1 * DeltaRotation2.W;
                DeltaRotation2.X = -1 * DeltaRotation2.X;
                DeltaRotation2.Y = -1 * DeltaRotation2.Y;
                DeltaRotation2.Z = -1 * DeltaRotation2.Z;
            }
            // Transform Rotation in WF to Rotation in LF
            DeltaRotation2 = RotCurrent2.Inverse() * DeltaRotation2 * RotCurrent2;

            DeltaRotation2.ToAxisAndAngle(Axis2, Angle2);
            AngularVelocity2 = Axis2 * Angle2 / delta_current_scaled + FVector(this->dist_vel1(generator), dist_vel1(generator), dist_vel1(generator));

            LocSecondLast2 = LocLast2;
            LocLast2 = LocCurrent2;
            // convert location to meters
            LocCurrent2 = camera_socket_2_base_->GetComponentLocation();


            auto LinearAcceleration2_wf = (LocCurrent2 - LocLast2) / (delta_current_scaled * delta_current_scaled) - (LocLast2 - LocSecondLast2) / (delta_current_scaled * delta_last_sample_scaled) - Gravity + FVector(dist_acc2(generator), dist_acc2(generator), dist_acc2(generator));

            // convert acceleration to local frame of IMU
            LinearAcceleration2 = RotCurrent2.Inverse().RotateVector(LinearAcceleration2_wf);
        }
        if (joint_3 != "") {
            RotLast3 = RotCurrent3;
            RotCurrent3 = camera_socket_3_base_->GetComponentQuat();

            DeltaRotation3 = RotCurrent3 * RotLast3.Inverse();
            DeltaRotation3 = DeltaRotation3.GetNormalized();
            if (DeltaRotation3.W < 0.0) {
                DeltaRotation3.W = -1 * DeltaRotation3.W;
                DeltaRotation3.X = -1 * DeltaRotation3.X;
                DeltaRotation3.Y = -1 * DeltaRotation3.Y;
                DeltaRotation3.Z = -1 * DeltaRotation3.Z;
            }
            // Transform Rotation in WF to Rotation in LF
            DeltaRotation3 = RotCurrent3.Inverse() * DeltaRotation3 * RotCurrent3;

            DeltaRotation3.ToAxisAndAngle(Axis3, Angle3);
            AngularVelocity3 = Axis3 * Angle3 / delta_current_scaled; // +FVector(this->dist_vel1(generator), dist_vel1(generator), dist_vel1(generator));

            LocSecondLast3 = LocLast3;
            LocLast3 = LocCurrent3;
            // convert location to meters
            LocCurrent3 = camera_socket_3_base_->GetComponentLocation();


            auto LinearAcceleration3_wf = (LocCurrent3 - LocLast3) / (delta_current_scaled * delta_current_scaled) - (LocLast3 - LocSecondLast3) / (delta_current_scaled * delta_last_sample_scaled) - Gravity + FVector(dist_acc3(generator), dist_acc3(generator), dist_acc3(generator));


            // convert acceleration to local frame of IMU
            LinearAcceleration3 = RotCurrent3.Inverse().RotateVector(LinearAcceleration3_wf);
        }
        if (joint_4 != "") {
            RotLast4 = RotCurrent4;
            RotCurrent4 = camera_socket_4_base_->GetComponentQuat();

            DeltaRotation4 = RotCurrent4 * RotLast4.Inverse();
            DeltaRotation4 = DeltaRotation4.GetNormalized();
            if (DeltaRotation4.W < 0.0) {
                DeltaRotation4.W = -1 * DeltaRotation4.W;
                DeltaRotation4.X = -1 * DeltaRotation4.X;
                DeltaRotation4.Y = -1 * DeltaRotation4.Y;
                DeltaRotation4.Z = -1 * DeltaRotation4.Z;
            }
            // Transform Rotation in WF to Rotation in LF
            DeltaRotation4 = RotCurrent4.Inverse() * DeltaRotation4 * RotCurrent4;

            DeltaRotation4.ToAxisAndAngle(Axis4, Angle4);
            AngularVelocity4 = Axis4 * Angle4 / delta_current_scaled; // +FVector(this->dist_vel1(generator), dist_vel1(generator), dist_vel1(generator));

            LocSecondLast4 = LocLast4;
            LocLast4 = LocCurrent4;
            // convert location to meters
            LocCurrent4 = camera_socket_4_base_->GetComponentLocation();


            auto LinearAcceleration4_wf = (LocCurrent4 - LocLast4) / (delta_current_scaled * delta_current_scaled) - (LocLast4 - LocSecondLast4) / (delta_current_scaled * delta_last_sample_scaled) - Gravity + FVector(dist_acc4(generator), dist_acc4(generator), dist_acc4(generator));


            // convert acceleration to local frame of IMU
            LinearAcceleration4 = RotCurrent4.Inverse().RotateVector(LinearAcceleration4_wf);
        }
        if (joint_5 != "") {
            RotLast5 = RotCurrent5;
            RotCurrent5 = camera_socket_5_base_->GetComponentQuat();

            DeltaRotation5 = RotCurrent5 * RotLast5.Inverse();
            DeltaRotation5 = DeltaRotation5.GetNormalized();
            if (DeltaRotation5.W < 0.0) {
                DeltaRotation5.W = -1 * DeltaRotation5.W;
                DeltaRotation5.X = -1 * DeltaRotation5.X;
                DeltaRotation5.Y = -1 * DeltaRotation5.Y;
                DeltaRotation5.Z = -1 * DeltaRotation5.Z;
            }
            // Transform Rotation in WF to Rotation in LF
            DeltaRotation5 = RotCurrent5.Inverse() * DeltaRotation5 * RotCurrent5;

            DeltaRotation5.ToAxisAndAngle(Axis5, Angle5);
            AngularVelocity5 = Axis5 * Angle5 / delta_current_scaled; // +FVector(this->dist_vel1(generator), dist_vel1(generator), dist_vel1(generator));

            LocSecondLast5 = LocLast5;
            LocLast5 = LocCurrent5;
            // convert location to meters
            LocCurrent5 = camera_socket_5_base_->GetComponentLocation();

            auto LinearAcceleration5_wf = (LocCurrent5 - LocLast5) / (delta_current_scaled * delta_current_scaled) - (LocLast5 - LocSecondLast5) / (delta_current_scaled * delta_last_sample_scaled) - Gravity + FVector(dist_acc5(generator), dist_acc5(generator), dist_acc5(generator));

            // convert acceleration to local frame of IMU
            LinearAcceleration5 = RotCurrent5.Inverse().RotateVector(LinearAcceleration5_wf);
        }
        if (joint_6 != "") {
            RotLast6 = RotCurrent6;
            RotCurrent6 = camera_socket_6_base_->GetComponentQuat();

            DeltaRotation6 = RotCurrent6 * RotLast6.Inverse();
            DeltaRotation6 = DeltaRotation6.GetNormalized();
            if (DeltaRotation6.W < 0.0) {
                DeltaRotation6.W = -1 * DeltaRotation6.W;
                DeltaRotation6.X = -1 * DeltaRotation6.X;
                DeltaRotation6.Y = -1 * DeltaRotation6.Y;
                DeltaRotation6.Z = -1 * DeltaRotation6.Z;
            }
            // Transform Rotation in WF to Rotation in LF
            DeltaRotation6 = RotCurrent6.Inverse() * DeltaRotation6 * RotCurrent6;

            DeltaRotation6.ToAxisAndAngle(Axis6, Angle6);
            AngularVelocity6 = Axis6 * Angle6 / delta_current_scaled; // +FVector(this->dist_vel1(generator), dist_vel1(generator), dist_vel1(generator));

            LocSecondLast6 = LocLast6;
            LocLast6 = LocCurrent6;
            // convert location to meters
            LocCurrent6 = camera_socket_6_base_->GetComponentLocation();

            auto LinearAcceleration6_wf = (LocCurrent6 - LocLast6) / (delta_current_scaled * delta_current_scaled) - (LocLast6 - LocSecondLast6) / (delta_current_scaled * delta_last_sample_scaled) - Gravity + FVector(dist_acc6(generator), dist_acc6(generator), dist_acc6(generator));

            // convert acceleration to local frame of IMU
            LinearAcceleration6 = RotCurrent6.Inverse().RotateVector(LinearAcceleration6_wf);
        }

        delta_last_sample = delta_since_last_sample;
        delta_since_last_sample = 0;
    }

}

void ASIPCharacterPawn::BeginPlay()
{
    Super::BeginPlay();
}


