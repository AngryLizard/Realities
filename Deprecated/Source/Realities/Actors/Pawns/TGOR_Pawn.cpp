///////////////////////////////////////////////////
// The Gateway of Realities: Planes of Existence //
// Copyright 2016-2017                           //
///////////////////////////////////////////////////


#include "TGOR_Pawn.h"

#include "Realities/Mod/Trackers/TGOR_Tracker.h"
#include "Realities/Base/Inventory/TGOR_Storage.h"
#include "Realities/Mod/Dimensions/TGOR_Dimension.h"
#include "Realities/Base/System/Data/TGOR_UserData.h"
#include "Realities/Base/Controller/TGOR_PlayerController.h"
#include "Realities/Components/Camera/TGOR_CameraComponent.h"
#include "Realities/Components/Dimension/TGOR_IdentityComponent.h"
#include "Realities/Components/TGOR_SceneComponent.h"
#include "Realities/Base/System/Data/TGOR_DimensionData.h"
#include "Realities/Actors/Dimension/TGOR_MainRegionActor.h"
#include "Realities/Actors/Dimension/TGOR_GateActor.h"
#include "Realities/Components/Dimension/Portals/TGOR_DimensionPortalComponent.h"
#include "Realities/Components/Dimension/Portals/TGOR_DimensionGateComponent.h"

#include "Components/SkeletalMeshComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/GameMode.h"

#include "Realities/Base/TGOR_Singleton.h"
#include "Realities/Base/Content/TGOR_ContentManager.h"

#include "Net/UnrealNetwork.h"


ATGOR_Pawn::ATGOR_Pawn(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = true;
	bFindCameraComponentWhenViewTarget = true;
	
	SetReplicatingMovement(false);

	bReplicates = true;


	CapsuleComponent = ObjectInitializer.CreateDefaultSubobject<UCapsuleComponent>(this, FName(TEXT("CapsuleComponent")));
	CapsuleComponent->CanCharacterStepUpOn = ECB_No;
	//CapsuleComponent->bCheckAsyncSceneOnMove = false;
	CapsuleComponent->SetCanEverAffectNavigation(false);
	CapsuleComponent->bDynamicObstacle = true;
	SetRootComponent(CapsuleComponent);

	SelfieStick = ObjectInitializer.CreateDefaultSubobject<USpringArmComponent>(this, FName(TEXT("SelfieStick")));
	SelfieStick->SetupAttachment(CapsuleComponent);
	SelfieStick->TargetArmLength = 500.0f;
	SelfieStick->TargetOffset.Z = 150.0f;

	CameraComponent = ObjectInitializer.CreateDefaultSubobject<UTGOR_CameraComponent>(this, FName(TEXT("CameraComponent")));
	CameraComponent->SetupAttachment(SelfieStick);

	PlayerComponent = ObjectInitializer.CreateDefaultSubobject<UTGOR_PlayerComponent>(this, FName(TEXT("PlayerComponent")));
	PlayerComponent->SetNetAddressable();
	PlayerComponent->SetIsReplicated(true);

	IdentityComponent = ObjectInitializer.CreateDefaultSubobject<UTGOR_IdentityComponent>(this, FName(TEXT("IdentityComponent")));
	PlayerComponent->SetIsReplicated(true);
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
		CameraComponent->RotateCameraUpside(GetUpVector(), FMath::Min(0.1f, DeltaTime) * 10.0f);
		CameraComponent->TickCamera(DeltaTime);
	}
}

bool ATGOR_Pawn::Assemble(UTGOR_DimensionData* Dimension)
{
	// automatically call user setup if available
	ATGOR_PlayerController* PlayerController = Cast<ATGOR_PlayerController>(GetController());
	if (IsValid(PlayerController))
	{
		// Set own dimension to swap cosmetics
		PlayerController->SetDimensionOwner(Dimension->GetIdentifier());
	}

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

UTGOR_DimensionPortalComponent* ATGOR_Pawn::GetClosestPortal(TSubclassOf<UTGOR_DimensionPortalComponent> Type) const
{
	DIMENSIONBASE_RETCHK(false);

	// There is at least one portal if state was found
	ETGOR_FetchEnumeration State;
	TArray<UTGOR_DimensionPortalComponent*> Portals = DimensionData->GetPortalsOfType(Type, State);
	if (State == ETGOR_FetchEnumeration::Found)
	{
		// TODO: Could use faster custom sort to only compute each distance once
		const FVector Ours = GetActorLocation();
		Portals.Sort(
			[Ours](const UTGOR_DimensionPortalComponent& A, const UTGOR_DimensionPortalComponent& B) -> bool 
			{
				const float DistA = (A.GetComponentLocation() - Ours).SizeSquared();
				const float DistB = (B.GetComponentLocation() - Ours).SizeSquared();
				return(DistA < DistB);
			});
		return(Portals[0]);
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

UTGOR_Spawner* ATGOR_Pawn::GetSpawnerContent() const
{
	if (IsValid(IdentityComponent))
	{
		return IdentityComponent->GetActorSpawner();
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

FVector ATGOR_Pawn::GetBoundingDimensions() const
{
	USceneComponent* Root = GetRootComponent();
	if (IsValid(Root))
	{
		return Root->Bounds.BoxExtent * 2.0f;
	}
	return FVector::ZeroVector;
}

float ATGOR_Pawn::GetCameraHeight(float Ratio) const
{
	// Get capsule half height
	UCapsuleComponent* Capsule = GetCapsule();
	float Half = Capsule->GetScaledCapsuleHalfHeight();

	// Set camera height
	return(2 * Half * Ratio);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

float ATGOR_Pawn::GetLegLength() const
{
	// Get capsule half height
	UCapsuleComponent* Capsule = GetCapsule();
	return Capsule->GetScaledCapsuleHalfHeight();
}

FVector ATGOR_Pawn::GetFeetLocation() const
{
	// Get capsule half height
	UCapsuleComponent* Capsule = GetCapsule();
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
	return FVector::UpVector;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void ATGOR_Pawn::AnimationChange(UTGOR_Animation* Old, UTGOR_Animation* New)
{
	OnAnimationChange(Old, New);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

TSet<UTGOR_Content*> ATGOR_Pawn::GetContext() const
{
	TSet<UTGOR_Content*> ContentContext = IdentityComponent->GetActiveContent_Implementation();
	return ContentContext;
}

void ATGOR_Pawn::PutTracker(TSubclassOf<UTGOR_Tracker> Type, const TSet<UTGOR_Content*>& Appendum, float Delta)
{
	SINGLETON_CHK;
	UTGOR_ContentManager* ContentManager = Singleton->GetContentManager();
	UTGOR_Tracker* Tracker = ContentManager->GetTFromType<UTGOR_Tracker>(Type);
	if (IsValid(Tracker))
	{
		FTGOR_TrackerInstance& Instance = Trackers.Data.FindOrAdd(Tracker);
		TSet<UTGOR_Content*> ContentContext = Appendum.Union(GetContext());
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


