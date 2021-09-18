// The Gateway of Realities: Planes of Existence.

#include "TGOR_ArmatureComponent.h"

#include "CoreSystem/TGOR_Singleton.h"
#include "DimensionSystem/Components/TGOR_MobilityComponent.h"
#include "DimensionSystem/Volumes/TGOR_LevelVolume.h"
#include "DimensionSystem/Data/TGOR_DimensionData.h"
#include "DimensionSystem/Content/TGOR_Spawner.h"
#include "RealitiesUtility/Utility/TGOR_Error.h"
#include "RealitiesUtility/Utility/TGOR_Math.h"

#include "DrawDebugHelpers.h"
#include "Net/UnrealNetwork.h"

TAutoConsoleVariable<int32> CVarArmatureOverlapDebugDrawEnable(TEXT("a.Armature.DebugOverlapDraw"), 0, TEXT("Toggle whether armature overlap are drawn."));
TAutoConsoleVariable<int32> CVarArmatureSurfaceDebugDrawEnable(TEXT("a.Armature.DebugSurfaceDraw"), 0, TEXT("Toggle whether armature surfaces are drawn."));

////////////////////////////////////////////////////////////////////////////////////////////////////

UTGOR_ArmatureComponent::UTGOR_ArmatureComponent()
:	Super()
{
}

void UTGOR_ArmatureComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction)
{
	/*
	// Testing attachment points
	FTGOR_MovementSpace Space = GetBase().ComputeParentSpace();
	DrawDebugPoint(GetWorld(), Space.Linear, 25.0f, FColor::Blue, false, -1.0f, 0);
	*/

	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UTGOR_ArmatureComponent::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}

bool UTGOR_ArmatureComponent::UpdatesBase() const
{
	APawn* Pawn = Cast<APawn>(GetOwner());
	return Pawn && (Pawn->IsLocallyControlled());
}

int32 UTGOR_ArmatureComponent::GetIndexFromName(const FName& Name) const
{
	if (!Name.IsNone())
	{
		USkeletalMeshComponent* Mesh = GetSkeletalMesh();
		if (IsValid(Mesh))
		{
			const int32 Index = Mesh->GetBoneIndex(Name);
			if (Index != INDEX_NONE)
			{
				return Index;
			}
		}
	}
	return Super::GetIndexFromName(Name);
}

FName UTGOR_ArmatureComponent::GetNameFromIndex(int32 Index) const
{
	if (Index >= 0)
	{
		USkeletalMeshComponent* Mesh = GetSkeletalMesh();
		if (IsValid(Mesh))
		{
			const int32 Num = Mesh->GetNumBones();
			if (Index < Num)
			{
				return Mesh->GetBoneName(Index);
			}
		}
	}
	return Super::GetNameFromIndex(Index);
}

FTGOR_MovementDynamic UTGOR_ArmatureComponent::GetIndexTransform(int32 Index) const
{
 	if (Index != INDEX_NONE)
	{
		// Get bone transform
		USkeletalMeshComponent* Mesh = GetSkeletalMesh();
		if (IsValid(Mesh) && Index < Mesh->GetNumBones())
		{
			const FTransform ParentTransform = GetComponentTransform();

			const FTransform MeshTransform = Mesh->GetComponentTransform();
			const FTransform& BoneTransform = Mesh->GetBoneTransform(Index, MeshTransform);

			FTGOR_MovementDynamic Dynamic;
			Dynamic.Linear = ParentTransform.InverseTransformPositionNoScale(BoneTransform.GetLocation());
			Dynamic.Angular = ParentTransform.InverseTransformRotation(BoneTransform.GetRotation());
			return Dynamic;
		}
	}
	return Super::GetIndexTransform(Index);
}

bool UTGOR_ArmatureComponent::IsValidIndex(int32 Index) const
{
	USkeletalMeshComponent* Mesh = GetSkeletalMesh();
	if (IsValid(Mesh) && 0 <= Index && Index < Mesh->GetNumBones())
	{
		return true;
	}
	return Super::IsValidIndex(Index);
}


void UTGOR_ArmatureComponent::AttachToMovement(USceneComponent* Component, int32 Index)
{
	if (Index == INDEX_NONE)
	{
		Super::AttachToMovement(Component, Index);
		return;
	}

	const FName Name = GetNameFromIndex(Index);
	USkeletalMeshComponent* Mesh = GetSkeletalMesh();

	check(!Mesh->IsAttachedTo(Component) && "Attachment loop detected");
	Component->AttachToComponent(Mesh, FAttachmentTransformRules::KeepWorldTransform, Name);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

USkeletalMeshComponent* UTGOR_ArmatureComponent::GetSkeletalMesh() const
{
	return GetOwnerComponent<USkeletalMeshComponent>();
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void UTGOR_ArmatureComponent::VisualTrace(const FVector& Offset, const FVector& Direction, float Distance, float Radius, FTGOR_VisualContact& Contact) const
{
	UWorld* World = GetWorld();
	USceneComponent* VisualComponent = GetSkeletalMesh();
	if (IsValid(VisualComponent))
	{
		FHitResult HitOut;
		const FTransform Transform = GetComponentTransform();
		const FTransform VisualTransform = VisualComponent->GetComponentTransform();

		const FVector Location = Transform.TransformPosition(Offset);
		const FVector Normal = UTGOR_Math::Normalize(Transform.TransformVectorNoScale(Direction));
		const FVector Translation = Normal * Distance;
		if (MovementSphereTraceSweep(Radius, Location, Translation, HitOut))
		{
			Contact.Location = VisualTransform.InverseTransformPosition(HitOut.ImpactPoint);
			Contact.Normal = VisualTransform.InverseTransformVectorNoScale(HitOut.ImpactNormal);
			Contact.Blocking = HitOut.bBlockingHit;
		}
		else
		{
			Contact.Location = VisualTransform.InverseTransformPosition(Location + Translation);
			Contact.Normal = VisualTransform.InverseTransformVectorNoScale(Normal);
			Contact.Blocking = false;
		}
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void UTGOR_ArmatureComponent::PreComputePhysics(const FTGOR_MovementTick& Tick)
{
	/*
	// Grab input
	if (UpdatesBase())
	{
		GetMovementInput(MovementInput.Input, MovementInput.View);
	}
	*/
	Super::PreComputePhysics(Tick);
}

void UTGOR_ArmatureComponent::ComputePhysics(FTGOR_MovementSpace& Space, const FTGOR_MovementExternal& External, const FTGOR_MovementTick& Tick, FTGOR_MovementOutput& Output)
{
	/*
	UTGOR_MovementTask* CurrentTask = GetMovementTask();
	if (GetOwnerRole() != ENetRole::ROLE_SimulatedProxy)
	{
		// Update Movement
		UpdateMovement(Tick, Space, External);
	}
	else if(IsValid(CurrentTask) && !CurrentTask->Invariant(Space, External))
	{
		RPORT("Movement invalid on client");
	}

	// Reset and compute force from movement
	if (IsValid(CurrentTask))
	{
		SCOPE_CYCLE_COUNTER(STAT_MovementTick);
		CurrentTask->Update(Space, External, Tick, Output);
	}
	*/

	Super::ComputePhysics(Space, External, Tick, Output);
}

void UTGOR_ArmatureComponent::PostComputePhysics(const FTGOR_MovementSpace& Space, float Energy, float DeltaTime)
{
	/*
	APawn* Pawn = GetOuterAPawn();
	UTGOR_MovementTask* CurrentTask = GetMovementTask();
	if (IsValid(CurrentTask) && IsValid(Pawn))
	{
		UTGOR_AnimationComponent* AnimationComponent = Pawn->FindComponentByClass<UTGOR_AnimationComponent>();
		if (IsValid(AnimationComponent))
		{
			AnimationComponent->SwitchAnimation(PerformanceType, CurrentTask->GetMovement()->MainAnimation);
		}

		CurrentTask->Animate(Space, DeltaTime);
	}
	*/
}

float UTGOR_ArmatureComponent::Simulate(float Time)
{
	Time = Super::Simulate(Time);

	/*
	if (GetOwnerRole() == ENetRole::ROLE_AutonomousProxy)
	{
		ServerInputLight(MovementInput);
	}
	*/
	return Time;
}

void UTGOR_ArmatureComponent::OutOfLevel()
{
	Super::OutOfLevel();
}

void UTGOR_ArmatureComponent::Impact(const FTGOR_MovementDynamic& Dynamic, const FVector& Point, const FVector& Impact)
{
	Super::Impact(Dynamic, Point, Impact);

	/*
	// Knock pawn out if we have movement authority
	APawn* Pawn = GetOuterAPawn();
	const bool HasAuthority = Pawn->HasAuthority();
	if (HasAuthority)
	{
		float Threshold = KnockoutFootThreshold;

		// Find threshold depending on direction
		const FVector Diff = Point - Dynamic.Linear;
		const float SizeSquared = Diff.SizeSquared();
		if (SizeSquared > SMALL_NUMBER)
		{
			const FVector Norm = Diff / FMath::Sqrt(SizeSquared);
			const float Dot = Norm | Dynamic.Angular.GetAxisZ();
			Threshold = FMath::GetMappedRangeValueUnclamped(FVector2D(-1.0f, 1.0f), FVector2D(KnockoutFootThreshold, KnockoutHeadThreshold), Dot);
		}

		// Check whether threshold is reached
		const float ImpactStrength = Impact.Size();
		if (ImpactStrength > Threshold)
		{
			OnKnockout.Broadcast();
			//Frame.Autonomy = 0.0f;
		}
	}
	*/
}

bool UTGOR_ArmatureComponent::CanInflict() const
{
	return true;
}

bool UTGOR_ArmatureComponent::CanRotateOnImpact() const
{
	/*
	UTGOR_MovementTask* CurrentTask = GetMovementTask();
	if (IsValid(CurrentTask))
	{
		return CurrentTask->GetMovement()->CanRotateOnCollision;
	}
	*/
	return false;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
