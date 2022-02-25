// The Gateway of Realities: Planes of Existence.


#include "TGOR_Avatar.h"

#include "Realities/Components/System/Actions/TGOR_EquipmentComponent.h"
#include "Realities/Components/System/Actions/TGOR_AbilityComponent.h"

#include "Realities/Mod/Customisations/TGOR_Customisation.h"
#include "Realities/Mod/Skins/TGOR_Skin.h"
#include "Realities/Mod/Colours/TGOR_Colour.h"
#include "Realities/Mod/Bodyparts/TGOR_Bodypart.h"
#include "Realities/Mod/Movements/TGOR_Movement.h"
#include "Realities/Mod/Actions/TGOR_Action.h"

#include "Realities/Components/Combat/TGOR_AimComponent.h"
#include "Realities/Base/Controller/TGOR_PlayerController.h"
#include "Realities/Components/System/Actions/TGOR_ActionComponent.h"
#include "Realities/Components/System/Actions/TGOR_AbilityComponent.h"
#include "Realities/Components/System/Actions/TGOR_EquipmentComponent.h"
#include "Realities/Components/Inventory/TGOR_InventoryComponent.h"
#include "Realities/Components/Creature/TGOR_SimulationComponent.h"
#include "Realities/Components/Creature/TGOR_ModularSkeletalMeshComponent.h"
#include "Realities/Components/Movement/TGOR_MovementComponent.h"
#include "Realities/Components/Combat/TGOR_HitVolumeComponent.h"
#include "Realities/Components/TGOR_SceneComponent.h"

#include "Realities/Mod/Trackers/TGOR_AvatarTracker.h"
#include "Realities/Mod/Trackers/TGOR_Tracker.h"

#include "Realities/Animation/TGOR_AnimInstance.h"
#include "Realities/Base/Inventory/TGOR_Storage.h"
#include "Realities/Utility/Storage/TGOR_Package.h"

#include "Components/CapsuleComponent.h"
#include "Realities/Base/TGOR_Singleton.h"
#include "Realities/Base/Inventory/TGOR_ItemStorage.h"
#include "Realities/Base/System/Data/TGOR_DimensionData.h"
#include "Realities/Actors/Dimension/TGOR_MainRegionActor.h"

#include "Net/UnrealNetwork.h"

// Sets default values
ATGOR_Avatar::ATGOR_Avatar(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer),
	PhysicalRootBone("Pelvis_C")
{
	PrimaryActorTick.bCanEverTick = true;

	UCapsuleComponent* Capsule = GetCapsule();

	EquipmentComponent = ObjectInitializer.CreateDefaultSubobject<UTGOR_EquipmentComponent>(this, FName(TEXT("EquipmentComponent")));
	EquipmentComponent->SetNetAddressable();
	EquipmentComponent->SetIsReplicated(true);

	AbilityComponent = ObjectInitializer.CreateDefaultSubobject<UTGOR_AbilityComponent>(this, FName(TEXT("AbilityComponent")));
	AbilityComponent->SetNetAddressable();
	AbilityComponent->SetIsReplicated(true);

	InventoryComponent = ObjectInitializer.CreateDefaultSubobject<UTGOR_InventoryComponent>(this, FName(TEXT("InventoryComponent")));
	InventoryComponent->SetNetAddressable();
	InventoryComponent->SetIsReplicated(true);

	CombatComponent = ObjectInitializer.CreateDefaultSubobject<UTGOR_HitVolumeComponent>(this, FName(TEXT("CombatComponent")));
	CombatComponent->SetupAttachment(Capsule);
	CombatComponent->SetNetAddressable();
	CombatComponent->SetIsReplicated(true);

	VisualsComponent = ObjectInitializer.CreateDefaultSubobject<UTGOR_SceneComponent>(this, FName(TEXT("VisualsComponent")));
	VisualsComponent->SetupAttachment(Capsule);

	SkeletalMeshComponent = ObjectInitializer.CreateDefaultSubobject<UTGOR_ModularSkeletalMeshComponent>(this, FName(TEXT("SkeletalMeshComponent")));
	SkeletalMeshComponent->SetupAttachment(VisualsComponent);
	GetSelfie()->SetupAttachment(VisualsComponent);

	BoundingComponent = ObjectInitializer.CreateDefaultSubobject<UBoxComponent>(this, FName(TEXT("BoundingComponent")));
	BoundingComponent->SetupAttachment(Capsule);
	BoundingComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	BoundingComponent->SetCollisionProfileName("NoCollision");

	SimulationComponent = ObjectInitializer.CreateDefaultSubobject<UTGOR_SimulationComponent>(this, FName(TEXT("SimulationComponent")));
	SimulationComponent->SetNetAddressable();
	SimulationComponent->SetIsReplicated(true);

	MovementComponent = ObjectInitializer.CreateDefaultSubobject<UTGOR_MovementComponent>(this, FName(TEXT("MovementComponent")));
	MovementComponent->SetNetAddressable();
	MovementComponent->SetIsReplicated(true);

	PhysicsAnimationComponent = ObjectInitializer.CreateDefaultSubobject<UPhysicalAnimationComponent>(this, FName(TEXT("PhysicsAnimationComponent")));

	PhysicalAnimationStrength = 0.0f;
}

////////////////////////////////////////////////////////////////////////////////////////////////////


void ATGOR_Avatar::BeginPlay()
{
	Super::BeginPlay();

	PhysicsAnimationComponent->SetSkeletalMeshComponent(SkeletalMeshComponent);
	
	if (IsValid(CurrentSetup.Creature))
	{
		ApplySetup(CurrentSetup);
	}
}

void ATGOR_Avatar::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// Replicate to everyone
	DOREPLIFETIME_CONDITION(ATGOR_Avatar, CurrentSetup, COND_None);
}

void ATGOR_Avatar::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	/*
	// TODO
	if (HasPhysicalAnimationLerp)
	{
		PhysicalAnimationStrength -= DeltaTime;
		if (PhysicalAnimationStrength <= 0.0f)
		{
			ApplyCurrentPhysicalAnimation();
			HasPhysicalAnimationLerp = false;
		}
		else
		{
			ApplyCurrentPhysicalBlending();
		}
	}
	*/

	if (!Controller) return;
	Controller->SetControlRotation(GetCameraRotation().Rotator());

	ATGOR_PlayerController* PlayerController = Cast<ATGOR_PlayerController>(GetController());
	if (IsValid(PlayerController) && PlayerController->IsLocalController())
	{
		// Rotate with movement base
		FTGOR_MovementSpace ParentSpace = MovementComponent->GetBase().ComputeParentSpace();
		const float Angle = ParentSpace.AngularVelocity.Size();
		if (Angle > SMALL_NUMBER)
		{
			GetCamera()->RotateCamera(ParentSpace.AngularVelocity / Angle, Angle * DeltaTime, false);
		}

		// Update aim
		const FVector Location = PlayerController->PlayerCameraManager->GetCameraLocation();
		const FRotator Rotation = PlayerController->PlayerCameraManager->GetCameraRotation();
		CombatComponent->UpdateAimFromCameraNearby(Location, Rotation.Vector());
	}
}

bool ATGOR_Avatar::Assemble(UTGOR_DimensionData* Dimension)
{
	return(ATGOR_Pawn::Assemble(Dimension));
}


FVector ATGOR_Avatar::GetUpVector() const
{
	return MovementComponent->ComputePhysicsUpVector();
}

TSet<UTGOR_Content*> ATGOR_Avatar::GetContext() const
{
	TSet<UTGOR_Content*> ContentContext = Super::GetContext();

	ContentContext.Append(MovementComponent->GetActiveContent_Implementation());
	ContentContext.Append(EquipmentComponent->GetActiveContent_Implementation());
	ContentContext.Append(AbilityComponent->GetActiveContent_Implementation());
	//ContentContext.Emplace(GetCreature()); Already handled with Spawner content in ATGOR_Pawn

	return ContentContext;
}

UTGOR_ItemStorage* ATGOR_Avatar::PutStorage(UTGOR_ItemStorage* Storage)
{
	Storage = Super::PutStorage(Storage);
	InventoryComponent->PutItem(Storage);
	return nullptr;
}

UTGOR_AnimInstance* ATGOR_Avatar::GetAnimation() const
{
	return Cast<UTGOR_AnimInstance>(SkeletalMeshComponent->AnimScriptInstance);
}

FVector ATGOR_Avatar::GetBoundingDimensions() const
{
	return(BoundingComponent->GetScaledBoxExtent() * 2.0f); // Times two for actual dimensions
}

USkeletalMeshComponent* ATGOR_Avatar::GetSkeletalMesh() const
{
	return GetMesh();
}

void ATGOR_Avatar::Write_Implementation(FTGOR_GroupWritePackage& Package, UTGOR_Singleton* Context) const
{
	Package.WriteEntry("Setup", CurrentSetup);
}

bool ATGOR_Avatar::Read_Implementation(FTGOR_GroupReadPackage& Package, UTGOR_Singleton* Context)
{
	if (Package.ReadEntry("Setup", CurrentSetup))
	{
		ApplySetup(CurrentSetup);
		return true;
	}
	return false;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

UTGOR_ModularSkeletalMeshComponent* ATGOR_Avatar::GetMesh() const
{
	return(SkeletalMeshComponent);
}


////////////////////////////////////////////////////////////////////////////////////////////////////

void ATGOR_Avatar::ApplyCurrentPhysicalAnimation()
{
	// Reset whole tree
	PhysicsAnimationComponent->ApplyPhysicalAnimationSettingsBelow(PhysicalRootBone, FPhysicalAnimationData(), true);
	SkeletalMeshComponent->SetAllBodiesBelowSimulatePhysics(PhysicalRootBone, false, true);

	const FTGOR_MovementContent& Content = MovementComponent->GetContent();
	if (IsValid(Content.Animation))
	{
		// Set new profiles
		for (const TPair<FName, FName>& Pair : Content.Animation->PhysicsProfiles)
		{
			PhysicsAnimationComponent->ApplyPhysicalAnimationProfileBelow(Pair.Key, Pair.Value, true, true);
			SkeletalMeshComponent->SetAllBodiesBelowSimulatePhysics(Pair.Key, true, true);
		}
		PhysicalAnimationStrength = 1.0f;
	}
	else
	{
		PhysicalAnimationStrength = 0.0f;
	}
}

void ATGOR_Avatar::ApplyCurrentPhysicalBlending()
{
	// Blend everything
	SkeletalMeshComponent->SetAllBodiesBelowPhysicsBlendWeight(PhysicalRootBone, PhysicalAnimationStrength, false, true);

	// Exclude unchanged bone blends
	for (const FName& UnchangedBone : PhysicalAnimationUnchangedBones)
	{
		SkeletalMeshComponent->SetAllBodiesBelowPhysicsBlendWeight(UnchangedBone, 1.0f, false, true);
	}
}

void ATGOR_Avatar::AnimationChange(UTGOR_Animation* Old, UTGOR_Animation* New)
{
	/*
	// TODO
	HasPhysicalAnimationLerp = false;
	if (IsValid(Old))
	{
		for (const TPair<FName, FName> OldPair : Old->PhysicsProfiles)
		{
			// Check whether the profile directly above us overrides us
			bool GetsOverridden = true;
			for (const TPair<FName, FName> NewPair : New->PhysicsProfiles)
			{
				if (SkeletalMeshComponent->BoneIsChildOf(OldPair.Key, NewPair.Key) || NewPair.Key.IsEqual(OldPair.Key))
				{
					GetsOverridden = !NewPair.Value.IsEqual(OldPair.Value);
				}
			}

			// If we get overridden, lerp out
			if (GetsOverridden)
			{
				HasPhysicalAnimationLerp = true;
			}
		}

		PhysicalAnimationUnchangedBones.Empty();
		for (const TPair<FName, FName> NewPair : New->PhysicsProfiles)
		{
			// Check whether the profile directly above us is the same as us
			bool HasMatchingParent = false;
			for (const TPair<FName, FName> OldPair : Old->PhysicsProfiles)
			{
				if (SkeletalMeshComponent->BoneIsChildOf(NewPair.Key, OldPair.Key) || OldPair.Key.IsEqual(NewPair.Key))
				{
					HasMatchingParent = OldPair.Value.IsEqual(NewPair.Value);
				}
			}

			// If we get overridden, lerp out
			if (HasMatchingParent)
			{
				PhysicalAnimationUnchangedBones.Emplace(NewPair.Key);
			}
		}
	}

	// If not lerping go right ahead
	if (!HasPhysicalAnimationLerp)
	{
		ApplyCurrentPhysicalAnimation();
	}
	*/
	Super::AnimationChange(Old, New);
}


////////////////////////////////////////////////////////////////////////////////////////////////////


void ATGOR_Avatar::SetScale(float Scale)
{
	SetActorScale3D(FVector(Scale, Scale, Scale));

	SkeletalMeshComponent->ResetAllBodiesSimulatePhysics();
	SkeletalMeshComponent->UpdateBoneBodyMapping();
	//SkeletalMeshComponent->InitSkelControls();
	if (IsValid(CurrentSetup.Creature))
	{
		MovementComponent->BuildMovementFrame(CurrentSetup.Creature);
	}
}

void ATGOR_Avatar::ApplySetup(const FTGOR_CreatureSetupInstance& Setup)
{
	SINGLETON_CHK;

	CurrentSetup = Setup;

	// Build skeletal mesh component
	ApplyAppearance(CurrentSetup.Appearance);
	
	// Build movement component
	MovementComponent->BuildMovementQueues(CurrentSetup.Creature);

	UTGOR_CameraComponent* Camera = GetCamera();
	Camera->BuildModifiers(CurrentSetup.Creature);

	OnSetupChange(Setup.Creature);
}

void ATGOR_Avatar::ApplyAppearance(const FTGOR_CreatureAppearanceInstance& Appearance)
{
	// Apply appearance
	SkeletalMeshComponent->ApplyAppearance(Appearance, CurrentSetup.Creature);

	// In case appearance changed due to validation, reconstruct proper appearance
	CurrentSetup.Appearance = SkeletalMeshComponent->ConstructCurrentAppearance();
}

void ATGOR_Avatar::OnReplicateSetup()
{
	if (HasActorBegunPlay())
	{
		ApplySetup(CurrentSetup);
	}
}

UTGOR_Creature* ATGOR_Avatar::GetCreature() const
{
	return(CurrentSetup.Creature);
}


void ATGOR_Avatar::RequestAppearance_Implementation(FTGOR_CreatureAppearanceInstance Appearance)
{
	SINGLETON_CHK;
	UTGOR_WorldData* WorldData = Singleton->GetWorldData();

	// Appearance changes are only allowed in persistent level
	ETGOR_FetchEnumeration State;
	if (WorldData->GetPersistentIdentifier() == GetDimensionIdentifier(State))
	{
		ApplyAppearance(Appearance);
	}
}

bool ATGOR_Avatar::RequestAppearance_Validate(FTGOR_CreatureAppearanceInstance Appearance)
{
	return true;
}
