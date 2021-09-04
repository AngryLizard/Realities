///////////////////////////////////////////////////
// The Gateway of Realities: Planes of Existence //
// Copyright 2016-2017                           //
///////////////////////////////////////////////////


#include "TGOR_Pawn.h"

#include "KnowledgeSystem/Content/TGOR_Tracker.h"
#include "CreatureSystem/Content/TGOR_Creature.h"
#include "DimensionSystem/Content/TGOR_Dimension.h"
#include "CreatureSystem/Components/TGOR_CameraComponent.h"
#include "SimulationSystem/Components/TGOR_StatComponent.h"
#include "PhysicsSystem/Components/TGOR_ArmatureComponent.h"
#include "MovementSystem/Components/TGOR_MovementComponent.h"
#include "AttributeSystem/Components/TGOR_AttributeComponent.h"
#include "CoreSystem/Components/TGOR_SceneComponent.h"
#include "DimensionSystem/Data/TGOR_DimensionData.h"
#include "DimensionSystem/Regions/TGOR_MainRegionActor.h"
#include "DimensionSystem/Components/TGOR_DimensionReceiverComponent.h"
#include "DimensionSystem/Components/TGOR_TrackedComponent.h"

#include "Components/SkeletalMeshComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/GameMode.h"

#include "CoreSystem/TGOR_Singleton.h"
#include "RealitiesUGC/Mod/TGOR_ContentManager.h"

#include "Net/UnrealNetwork.h"


ATGOR_Pawn::ATGOR_Pawn(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = true;
	bFindCameraComponentWhenViewTarget = true;
	
	SetReplicatingMovement(false);
	bReplicates = true;

	IdentityComponent = ObjectInitializer.CreateDefaultSubobject<UTGOR_TrackedComponent>(this, FName(TEXT("IdentityComponent")));
	IdentityComponent->SetIsReplicated(true);

	ArmatureComponent = ObjectInitializer.CreateDefaultSubobject<UTGOR_ArmatureComponent>(this, FName(TEXT("ArmatureComponent")));
	ArmatureComponent->CanCharacterStepUpOn = ECB_No;
	//MovementComponent->bCheckAsyncSceneOnMove = false;
	ArmatureComponent->SetCanEverAffectNavigation(false);
	ArmatureComponent->bDynamicObstacle = true;
	SetRootComponent(ArmatureComponent);

	SelfieStick = ObjectInitializer.CreateDefaultSubobject<USpringArmComponent>(this, FName(TEXT("SelfieStick")));
	SelfieStick->SetupAttachment(ArmatureComponent);
	SelfieStick->TargetArmLength = 500.0f;
	SelfieStick->TargetOffset.Z = 150.0f;

	MovementComponent = ObjectInitializer.CreateDefaultSubobject<UTGOR_MovementComponent>(this, FName(TEXT("MovementComponent")));
	IdentityComponent->SetIsReplicated(true);

	CameraComponent = ObjectInitializer.CreateDefaultSubobject<UTGOR_CameraComponent>(this, FName(TEXT("CameraComponent")));
	CameraComponent->SetupAttachment(SelfieStick);


	StatComponent = ObjectInitializer.CreateDefaultSubobject<UTGOR_StatComponent>(this, FName(TEXT("StatComponent")));
	AttributeComponent = ObjectInitializer.CreateDefaultSubobject<UTGOR_AttributeComponent>(this, FName(TEXT("AttributeComponent")));

	AutoPossessAI = EAutoPossessAI::Disabled;
}

void ATGOR_Pawn::BeginPlay()
{
	Super::BeginPlay();
}

void ATGOR_Pawn::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// Replicate to everyone
}

void ATGOR_Pawn::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

}


void ATGOR_Pawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	DeltaTime = FMath::Min(DeltaTime, 0.1f);

	if (IsLocallyControlled())
	{
		const FVector UpVector = GetUpVector();
		const float Timestep = FMath::Min(0.1f, DeltaTime) * 10.0f;
		CameraComponent->RotateCameraUpside(UpVector, Timestep);
	}
}

bool ATGOR_Pawn::Assemble(UTGOR_DimensionData* Dimension)
{
	if (ITGOR_DimensionInterface::Assemble(Dimension))
	{
		DimensionEntered();
		return true;
	}
	return false;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

USkeletalMeshComponent* ATGOR_Pawn::GetSkeletalMesh() const
{
	UActorComponent* Component = GetComponentByClass(USkeletalMeshComponent::StaticClass());
	return Cast<USkeletalMeshComponent>(Component);
}

UTGOR_DimensionReceiverComponent* ATGOR_Pawn::GetClosestPortal(TSubclassOf<UTGOR_DimensionReceiverComponent> Type) const
{
	DIMENSIONBASE_RETCHK(false);

	// There is at least one portal if state was found
	TArray<UTGOR_DimensionReceiverComponent*> Connections = DimensionData->GetCListOfType<UTGOR_DimensionReceiverComponent>(Type);
	if (Connections.Num() > 0)
	{
		// TODO: Could use faster custom sort to only compute each distance once
		const FVector Ours = GetActorLocation();
		Connections.Sort(
			[Ours](const UTGOR_DimensionReceiverComponent& A, const UTGOR_DimensionReceiverComponent& B) -> bool
			{
				const float DistA = (A.GetComponentLocation() - Ours).SizeSquared();
				const float DistB = (B.GetComponentLocation() - Ours).SizeSquared();
				return(DistA < DistB);
			});
		return(Connections[0]);
	}
	return(nullptr);
}

UTGOR_Dimension* ATGOR_Pawn::GetDimensionContent() const
{
	if (IsValid(IdentityComponent))
	{
		return IdentityComponent->GetActorDimension();
	}
	return nullptr;
}

UTGOR_Creature* ATGOR_Pawn::GetSpawnerContent() const
{
	if (IsValid(IdentityComponent))
	{
		return Cast<UTGOR_Creature>(IdentityComponent->GetActorSpawner());
	}
	return nullptr;
}

void ATGOR_Pawn::SwapDimension(UTGOR_DimensionData* Other)
{
	ETGOR_FetchEnumeration State;
	UTGOR_DimensionData* Current = GetDimension(State);
	if (Current != Other)
	{
		if (IsValid(Current))
		{
			// Make sure we are registered correctly to begin with
			const int32 ActorIdentifier = IdentityComponent->GetActorIdentifier();
			UTGOR_IdentityComponent* Identity = Current->GetDimensionObject(ActorIdentifier, State);
			if (!IsValid(Identity) || Identity != IdentityComponent)
			{
				Other->RegisterDimensionObject(IdentityComponent);
			}
			else
			{
				// Swap dimension
				Current->SwapDimensionObject(ActorIdentifier, Other);
			}
		}
		else
		{
			Other->RegisterDimensionObject(IdentityComponent);
		}
	}
}

UTGOR_ItemStorage* ATGOR_Pawn::PutStorage(UTGOR_ItemStorage* Storage)
{
	OnItemGiven(Storage);
	return Storage;
}

UTGOR_AnimInstance* ATGOR_Pawn::GetAnimation() const
{
	return nullptr;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void ATGOR_Pawn::RotateCameraHorizontally(float Amount)
{
	CameraComponent->RotateCamera(GetUpVector(), Amount, true);
}

void ATGOR_Pawn::RotateCameraVertically(float Amount)
{
	const FQuat CameraRotation = GetCameraRotation();
	CameraComponent->RotateCamera(CameraRotation.GetAxisY(), Amount, true);
}

FQuat ATGOR_Pawn::GetCameraRotation() const
{
	return CameraComponent->GetControlRotation();
}

FQuat ATGOR_Pawn::ToLocalCameraRotation(const FTGOR_MovementPosition& Position) const
{
	const FQuat CameraRotation = GetCameraRotation();
	return Position.Angular.Inverse() * CameraRotation;
}

void ATGOR_Pawn::FromLocalCameraRotation(const FTGOR_MovementPosition& Position, const FQuat& Quat)
{
	CameraComponent->SetViewRotation(Position.Angular.Inverse() * Quat);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

float ATGOR_Pawn::GetCarryWeight() const
{
	return 0.0f;
}

float ATGOR_Pawn::GetCameraHeight(float Ratio) const
{
	// Get capsule half height
	float Half = ArmatureComponent->GetScaledCapsuleHalfHeight();

	// Set camera height
	return(2 * Half * Ratio);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

float ATGOR_Pawn::GetLegLength() const
{
	return ArmatureComponent->GetScaledCapsuleHalfHeight();
}

FVector ATGOR_Pawn::GetFeetLocation() const
{
	// Get capsule half height
	const float Half = GetLegLength();

	// Get feet location
	return(GetActorLocation() - FVector(0.0f, 0.0f, Half));
}

void ATGOR_Pawn::OutOfLevel()
{
	OnOutOfLevel();
}

FVector ATGOR_Pawn::GetUpVector() const
{
	return ArmatureComponent->ComputePhysicsUpVector();
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void ATGOR_Pawn::AnimationChange(UTGOR_Animation* Old, UTGOR_Animation* New)
{
	OnAnimationChange(Old, New);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

TSet<UTGOR_CoreContent*> ATGOR_Pawn::GetContext() const
{
	TSet<UTGOR_CoreContent*> ContentContext = IdentityComponent->GetActiveContent_Implementation();
	ContentContext.Append(MovementComponent->GetActiveContent_Implementation());
	return ContentContext;
}

void ATGOR_Pawn::PutTracker(TSubclassOf<UTGOR_Tracker> Type, const TSet<UTGOR_CoreContent*>& Appendum, float Delta)
{
	SINGLETON_CHK;
	UTGOR_ContentManager* ContentManager = Singleton->GetContentManager();
	UTGOR_Tracker* Tracker = ContentManager->GetTFromType<UTGOR_Tracker>(Type);
	if (IsValid(Tracker))
	{
		FTGOR_TrackerInstance& Instance = Trackers.Data.FindOrAdd(Tracker);
		TSet<UTGOR_CoreContent*> ContentContext = Appendum.Union(GetContext());
		if (ContentContext.Remove(nullptr) > 0)
		{
			RPORT(Tracker->GetName() + " inserted null context!");
		}
		Instance.Put(ContentContext, Delta);
	}
}

void ATGOR_Pawn::FlushTracker(FTGOR_TrackerData& Target)
{
	Target.Merge(Trackers);
	Trackers.Data.Reset();
}

////////////////////////////////////////////////////////////////////////////////////////////////////

bool ATGOR_Pawn::HasCursor_Implementation()
{
	return false;
}

bool ATGOR_Pawn::DespawnsOnLogout_Implementation()
{
	return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////


