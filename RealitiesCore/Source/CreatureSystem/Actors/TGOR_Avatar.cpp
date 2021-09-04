// The Gateway of Realities: Planes of Existence.


#include "TGOR_Avatar.h"

#include "ActionSystem/Components/TGOR_ActionComponent.h"

#include "TargetSystem/Components/TGOR_AimComponent.h"
#include "InventorySystem/Components/TGOR_InventoryComponent.h"
#include "InventorySystem/Components/TGOR_MatterComponent.h"
#include "SimulationSystem/Components/TGOR_SimulationComponent.h"
#include "CustomisationSystem/Components/TGOR_ModularSkeletalMeshComponent.h"
#include "MovementSystem/Components/TGOR_MovementComponent.h"
#include "PhysicsSystem/Components/TGOR_ArmatureComponent.h"
#include "ActionSystem/HitVolumes/TGOR_HitVolumeComponent.h"
#include "CoreSystem/Components/TGOR_SceneComponent.h"

#include "AnimationSystem/Content/TGOR_Animation.h"
#include "AnimationSystem/Instances/TGOR_AnimInstance.h"

#include "CoreSystem/Storage/TGOR_Package.h"

#include "Components/CapsuleComponent.h"
#include "CoreSystem/TGOR_Singleton.h"
#include "InventorySystem/Storage/TGOR_ItemStorage.h"
#include "DimensionSystem/Data/TGOR_DimensionData.h"

#include "Net/UnrealNetwork.h"

// Sets default values
ATGOR_Avatar::ATGOR_Avatar(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer),
	PhysicalRootBone("Pelvis_C")
{
	PrimaryActorTick.bCanEverTick = true;

	ActionComponent = ObjectInitializer.CreateDefaultSubobject<UTGOR_ActionComponent>(this, FName(TEXT("ActionComponent")));
	ActionComponent->SetNetAddressable();
	ActionComponent->SetIsReplicated(true);

	InventoryComponent = ObjectInitializer.CreateDefaultSubobject<UTGOR_InventoryComponent>(this, FName(TEXT("InventoryComponent")));
	InventoryComponent->SetNetAddressable();
	InventoryComponent->SetIsReplicated(true);

	UTGOR_ArmatureComponent* Armature = GetArmature();

	CombatComponent = ObjectInitializer.CreateDefaultSubobject<UTGOR_HitVolumeComponent>(this, FName(TEXT("CombatComponent")));
	CombatComponent->SetupAttachment(Armature);
	CombatComponent->SetNetAddressable();
	CombatComponent->SetIsReplicated(true);

	MatterComponent = ObjectInitializer.CreateDefaultSubobject<UTGOR_MatterComponent>(this, FName(TEXT("MatterComponent")));
	MatterComponent->SetNetAddressable();
	MatterComponent->SetIsReplicated(true);

	VisualsComponent = ObjectInitializer.CreateDefaultSubobject<UTGOR_SceneComponent>(this, FName(TEXT("VisualsComponent")));
	VisualsComponent->SetupAttachment(Armature);

	SkeletalMeshComponent = ObjectInitializer.CreateDefaultSubobject<UTGOR_ModularSkeletalMeshComponent>(this, FName(TEXT("SkeletalMeshComponent")));
	SkeletalMeshComponent->SetupAttachment(VisualsComponent);
	GetSelfie()->SetupAttachment(VisualsComponent);

	SimulationComponent = ObjectInitializer.CreateDefaultSubobject<UTGOR_SimulationComponent>(this, FName(TEXT("SimulationComponent")));
	SimulationComponent->SetNetAddressable();
	SimulationComponent->SetIsReplicated(true);

	PhysicsAnimationComponent = ObjectInitializer.CreateDefaultSubobject<UPhysicalAnimationComponent>(this, FName(TEXT("PhysicsAnimationComponent")));

	PhysicalAnimationStrength = 0.0f;
}

////////////////////////////////////////////////////////////////////////////////////////////////////


void ATGOR_Avatar::BeginPlay()
{
	Super::BeginPlay();

	PhysicsAnimationComponent->SetSkeletalMeshComponent(SkeletalMeshComponent);
}

void ATGOR_Avatar::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

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

	APlayerController* PlayerController = Cast<APlayerController>(GetController());
	if (IsValid(PlayerController) && PlayerController->IsLocalController())
	{
		// Rotate with movement base
		UTGOR_ArmatureComponent* Armature = GetArmature();
		const FTGOR_MovementDynamic ParentSpace = Armature->ComputeBase();
		const float Angle = ParentSpace.AngularVelocity.Size();
		if (Angle > SMALL_NUMBER)
		{
			UTGOR_CameraComponent* Camera = GetCamera();
			Camera->RotateCamera(ParentSpace.AngularVelocity / Angle, Angle * DeltaTime, false);
		}

		// Update aim
		if (CombatComponent->UpdateCandidatesNearby())
		{
			const FVector Location = PlayerController->PlayerCameraManager->GetCameraLocation();
			const FRotator Rotation = PlayerController->PlayerCameraManager->GetCameraRotation();
			CombatComponent->UpdateAimFromCamera(Location, Rotation.Vector());
		}
	}
}

bool ATGOR_Avatar::Assemble(UTGOR_DimensionData* Dimension)
{
	return ATGOR_Pawn::Assemble(Dimension);
}


TSet<UTGOR_CoreContent*> ATGOR_Avatar::GetContext() const
{
	TSet<UTGOR_CoreContent*> ContentContext = Super::GetContext();

	ContentContext.Append(ActionComponent->GetActiveContent_Implementation());
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

USkeletalMeshComponent* ATGOR_Avatar::GetSkeletalMesh() const
{
	return GetMesh();
}

void ATGOR_Avatar::Write_Implementation(FTGOR_GroupWritePackage& Package, UTGOR_Singleton* Context) const
{
}

bool ATGOR_Avatar::Read_Implementation(FTGOR_GroupReadPackage& Package, UTGOR_Singleton* Context)
{
	return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

UTGOR_ModularSkeletalMeshComponent* ATGOR_Avatar::GetMesh() const
{
	return(SkeletalMeshComponent);
}


////////////////////////////////////////////////////////////////////////////////////////////////////

void ATGOR_Avatar::ApplyCurrentPhysicalAnimation()
{
	/*
	// Reset whole tree
	PhysicsAnimationComponent->ApplyPhysicalAnimationSettingsBelow(PhysicalRootBone, FPhysicalAnimationData(), true);
	SkeletalMeshComponent->SetAllBodiesBelowSimulatePhysics(PhysicalRootBone, false, true);

	UTGOR_MovementComponent* Movement = GetMovement();
	const FTGOR_MovementContent& Content = Movement->GetContent();
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
	*/
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

	UTGOR_MovementComponent* Movement = GetMovement();
	Movement->BuildMovementFrame();
}
