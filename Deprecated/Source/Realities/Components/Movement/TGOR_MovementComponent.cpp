// The Gateway of Realities: Planes of Existence.

#include "TGOR_MovementComponent.h"

#include "Realities/Mod/Movements/TGOR_Movement.h"
#include "Realities/Mod/Spawner/Creatures/TGOR_Creature.h"
#include "Realities/Mod/Animations/TGOR_Animation.h"
#include "Realities/Mod/Targets/Sockets/TGOR_NamedSocket.h"

#include "Realities/Animation/TGOR_AnimInstance.h"

#include "Realities/Base/TGOR_Singleton.h"
#include "Realities/Base/Content/TGOR_ContentManager.h"
#include "Realities/Base/System/Data/TGOR_DimensionData.h"
#include "Realities/Actors/Pawns/TGOR_Avatar.h"
#include "Realities/Actors/Dimension/TGOR_MainRegionActor.h"
#include "Realities/Actors/Dimension/Volumes/TGOR_PhysicsVolume.h"
#include "Realities/Components/Combat/TGOR_AimComponent.h"
#include "Realities/Components/Creature/TGOR_SimulationComponent.h"
#include "Realities/Components/Creature/TGOR_ModularSkeletalMeshComponent.h"
//#include "Realities/Components/Movement/TGOR_GuidedComponent.h"
#include "Realities/Components/Movement/TGOR_MobilityComponent.h"
#include "Realities/Base/Controller/TGOR_OnlineController.h"
#include "Realities/Utility/Storage/TGOR_Package.h"
#include "Realities/Utility/Error/TGOR_Error.h"
#include "Realities/Utility/TGOR_Math.h"


#include "Components/BoxComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/PhysicsVolume.h"
#include "DrawDebugHelpers.h"
#include "Net/UnrealNetwork.h"


DECLARE_CYCLE_STAT(TEXT("Movement simulation ticks over deltatime"), STAT_RealTick, STATGROUP_TGOR_MOV);
DECLARE_CYCLE_STAT(TEXT("Movement mode tick"), STAT_MovementTick, STATGROUP_TGOR_MOV);
DECLARE_CYCLE_STAT(TEXT("Movement simulation"), STAT_MovementSim, STATGROUP_TGOR_MOV);

TAutoConsoleVariable<int32> CVarMovementOverlapDebugDrawEnable(TEXT("a.Movement.DebugOverlapDraw"), 0, TEXT("Toggle whether movement overlap are drawn."));
TAutoConsoleVariable<int32> CVarMovementSurfaceDebugDrawEnable(TEXT("a.Movement.DebugSurfaceDraw"), 0, TEXT("Toggle whether movement surfaces are drawn."));

////////////////////////////////////////////////////////////////////////////////////////////////////


UTGOR_MovementComponent::UTGOR_MovementComponent()
:	Super(),

	Pawn(nullptr),
	AnimationSlotName("Movement"),
	
	RawInput(FVector::ZeroVector),
	InputStrength(0.0f),
	InputRotation(FRotator::ZeroRotator),

	KnockoutThreshold(0.1f),
	KnockoutHeadThreshold(230'000.0),
	KnockoutFootThreshold(320'000.0)
{
	AuthorityMode = ETGOR_MovementAuthorityMode::SlaveClient;
	AuthoritiveSlave = (AuthorityMode == ETGOR_MovementAuthorityMode::SlaveServer);
}

void UTGOR_MovementComponent::BeginPlay()
{
	Super::BeginPlay();

	AActor* Owner = GetOwner();
	Pawn = Cast<ATGOR_Pawn>(Owner);

	ResetToComponent();
}

void UTGOR_MovementComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	/*
	// Testing attachment points
	FTGOR_MovementSpace Space = GetBase().ComputeParentSpace();
	DrawDebugPoint(GetWorld(), Space.Linear, 25.0f, FColor::Blue, false, -1.0f, 0);
	*/
}

void UTGOR_MovementComponent::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// Replicate to everyone
	DOREPLIFETIME_CONDITION(UTGOR_MovementComponent, Frame, COND_None);
	DOREPLIFETIME_CONDITION(UTGOR_MovementComponent, State, COND_SimulatedOnly);
	DOREPLIFETIME_CONDITION(UTGOR_MovementComponent, Content, COND_SimulatedOnly);
}

int32 UTGOR_MovementComponent::GetIndexFromName(const FName& Name) const
{
	if (!Name.IsNone())
	{
		USkeletalMeshComponent* Mesh = Pawn->GetSkeletalMesh();
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

FName UTGOR_MovementComponent::GetNameFromIndex(int32 Index) const
{
	if (Index >= 0)
	{
		USkeletalMeshComponent* Mesh = Pawn->GetSkeletalMesh();
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

FTGOR_MovementDynamic UTGOR_MovementComponent::GetIndexTransform(UTGOR_Socket* Socket, int32 Index) const
{
 	if (!IsValid(Socket) && Index != INDEX_NONE)
	{
		// Get bone transform
		USkeletalMeshComponent* Mesh = Pawn->GetSkeletalMesh();
		if (IsValid(Mesh) && Index < Mesh->GetNumBones())
		{
			const FTransform& BoneTransform = Mesh->GetBoneTransform(Index, FTransform::Identity);

			FTGOR_MovementDynamic Dynamic;
			Dynamic.Linear = BoneTransform.GetLocation();
			Dynamic.Angular = BoneTransform.GetRotation();
			return Dynamic;
		}
	}
	return Super::GetIndexTransform(Socket, Index);
}

void UTGOR_MovementComponent::AttachToMovement(USceneComponent* Component, UTGOR_Socket* Socket, int32 Index)
{
	if (IsValid(Socket) || Index == INDEX_NONE)
	{
		Super::AttachToMovement(Component, Socket, Index);
		return;
	}

	const FName Name = GetNameFromIndex(Index);
	USkeletalMeshComponent* Mesh = Pawn->GetSkeletalMesh();

	check(!Mesh->IsAttachedTo(Component) && "Attachment loop detected");
	Component->AttachToComponent(Mesh, FAttachmentTransformRules::KeepWorldTransform, Name);
}

bool UTGOR_MovementComponent::PreAssemble(UTGOR_DimensionData* Dimension)
{
	if (IsValid(Dimension))
	{
		Capture.PhysicsVolume = Dimension->GetLevelVolume();
	}

	return ITGOR_DimensionInterface::PreAssemble(Dimension);
}


TSet<UTGOR_Content*> UTGOR_MovementComponent::GetActiveContent_Implementation() const
{
	TSet<UTGOR_Content*> ContentContext = Super::GetActiveContent_Implementation();
	if (IsValid(Content.Movement)) ContentContext.Emplace(Content.Movement);
	if (IsValid(Content.Animation)) ContentContext.Emplace(Content.Animation);

	return ContentContext;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void UTGOR_MovementComponent::BuildMovementQueues(UTGOR_Creature* OwningCreature)
{
	if (!IsValid(OwningCreature))
	{
		ERROR("Creature invalid", Error)
	}

	USceneComponent* Component = GetBaseComponent();
	if (!IsValid(Component))
	{
		ERROR("Base component invalid", Error)
	}

	BuildMovementFrame(OwningCreature);

	// Get socket from creature
	Sockets.Append(OwningCreature->GetIListFromType<UTGOR_Socket>(UTGOR_NamedSocket::StaticClass()));

	// Get installed movement modes
	TArray<UTGOR_Movement*> Movements = OwningCreature->GetIListFromType<UTGOR_Movement>();

	// Get all candidates that are part of the movement queue
	TArray<UTGOR_Movement*> Candidates;
	for (UTGOR_Movement* Movement : Movements)
	{
		if (Movement->Mode == ETGOR_MovementEnumeration::Queued ||
			Movement->Mode == ETGOR_MovementEnumeration::Sticky)
		{
			Candidates.Emplace(Movement);
		}
	}

	// Reset movement queue
	MovementQueue.Empty();

	// Fill movement queue until all candidates are sorted in
	bool AnyCandidates = Candidates.Num() > 0;
	while (AnyCandidates)
	{
		AnyCandidates = false;

		// Try to find candidates that are not overridden by anything
		for (int32 Index = Candidates.Num() - 1; Index >= 0; Index--)
		{
			UTGOR_Movement* Candidate = Candidates[Index];

			// See whether candidate overrides an
			bool Overrides = false;
			for (UTGOR_Movement* Movement : Candidates)
			{
				if (Movement != Candidate && Movement->IsOverriddenBy(Candidate))
				{
					Overrides = true;
				}
			}

			// Add to MovementQueue if current candidate is a root
			if (!Overrides)
			{
				Candidates.RemoveAt(Index);
				MovementQueue.Emplace(Candidate);
				AnyCandidates = true;
			}
		}
	}
}

void UTGOR_MovementComponent::BuildMovementFrame(UTGOR_Creature* OwningCreature)
{
	if (!IsValid(Pawn))
	{
		// Get owner
		AActor* Owner = GetOwner();
		Pawn = Cast<ATGOR_Pawn>(Owner);
	}

	// Make sure initial location is correct
	ResetToComponent();

	const float Scale = Pawn->GetActorScale().GetMin();
	const FVector Dimension = Pawn->GetBoundingDimensions(); // Already scaled

	// Client should already have gotten all the updates
	if (GetOwnerRole() != ENetRole::ROLE_SimulatedProxy)
	{
		// Reset current mode and queues
		Content.Movement = nullptr;

		// Set scale factors
		Frame.Agility = Scale;
		Frame.Strength = Scale * Scale * Scale;

	}

	/*
	// Compute total volume from physics asset
	UPhysicsAsset* PhysicsAsset = Pawn->SkeletalMeshComponent->GetPhysicsAsset();
	if (IsValid(PhysicsAsset))
	{
		Current.Volume = 0.0f;
		const FVector Scale = Pawn->GetActorScale3D();
		for (UBodySetup* Setup : PhysicsAsset->SkeletalBodySetups)
		{
			Current.Volume += Setup->GetVolume(Scale);
		}
		// Convert to cubic meters
		Current.Volume /= 1'000'000.0f;
	}
	else
	{
		// Use capsule volume
		UCapsuleComponent* Component = Pawn->CapsuleComponent;
		const float Radius = Component->GetScaledCapsuleRadius();
		const float Height = Component->GetScaledCapsuleHalfHeight() * 2;
		Current.Volume = PI * Radius * Radius * Height / 1'000'000.0f;
	}

	// Compute volume from character density
	if (Current.Volume > SMALL_NUMBER)
	{
		OwningCreature->Density * Current.Volume;
	}
	*/

	FTGOR_MovementBody NewBody;
	NewBody.SetFromBox(OwningCreature->SurfaceArea, Dimension, OwningCreature->Weight * Frame.Strength);
	SetBody(NewBody);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

bool UTGOR_MovementComponent::PreemptSimulation(float Timestep)
{
	// Preempt simulation
	// TODO: Do check to make sure we don't preempt too much. We do have to go negative for tick not to steal our frame
	//if (PendingTime > SMALL_NUMBER)
	{
		Content.HasOverride = true;
		PendingTime -= Timestep;
		PendingTime += Simulate(Timestep, false);
		MovementUpdate();
		return true;
	}
	return false;
}

void UTGOR_MovementComponent::PreemptMovement(float Timestep, const FVector& Input, const FVector& View, UTGOR_Movement* Override)
{
	// Override movement if necessary (Invariant is *not* checked, will get gutted by simulation next simulation tick though)
	if (IsValid(Override))
	{
		Content.Movement = Override;
	}

	State.Input = Input;
	State.View = View;
	PreemptSimulation(Timestep);
}

void UTGOR_MovementComponent::PreemptMovementInput(float Timestep, const FVector& Input, const FVector& View)
{
	PreemptMovement(Timestep, Input, View, nullptr);
}

void UTGOR_MovementComponent::PreemptMovementOverride(float Timestep, TSubclassOf<UTGOR_Movement> Movement)
{
	UTGOR_Spawner* Spawner = Pawn->GetSpawnerContent();
	if (IsValid(Spawner))
	{
		UTGOR_Movement* Reference = Spawner->GetIFromType<UTGOR_Movement>(Movement);
		if (IsValid(Reference))
		{
			FVector Input, View;
			Reference->QueryInputVector(this, Input, View);
			PreemptMovement(Timestep, Input, View, Reference);
		}
	}
}

void UTGOR_MovementComponent::PreemptMovementBaseOverride(float Timestep, TSubclassOf<UTGOR_Movement> Movement, UTGOR_MobilityComponent* Component)
{
	SetParent(Component, nullptr, INDEX_NONE);
	PreemptMovementOverride(Timestep, Movement);
}

void UTGOR_MovementComponent::PreemptMovementInputOverride(float Timestep, const FVector& Input, const FVector& View, TSubclassOf<UTGOR_Movement> Movement)
{
	SINGLETON_CHK;
	UTGOR_ContentManager* ContentManager = Singleton->GetContentManager();
	UTGOR_Spawner* Spawner = Pawn->GetSpawnerContent();
	if (IsValid(Spawner))
	{
		UTGOR_Movement* Reference = Spawner->GetIFromType<UTGOR_Movement>(Movement);
		if (IsValid(Reference))
		{
			PreemptMovement(Timestep, Input, View, Reference);
		}
	}
}

bool UTGOR_MovementComponent::UpdatesBase() const
{
	return Pawn && (Pawn->IsLocallyControlled());
}

void UTGOR_MovementComponent::SetScale(float Scale)
{
	
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void UTGOR_MovementComponent::RepNotifyState(const FTGOR_MovementState& Old)
{

}

void UTGOR_MovementComponent::RepNotifyContent(const FTGOR_MovementContent& Old)
{
}

void UTGOR_MovementComponent::RepNotifyFrame(const FTGOR_MovementFrame& Old)
{
	/*
	if (Frame.Autonomy < 0.1f && Old.Autonomy >= 0.1f)
	{
		OnKnockout.Broadcast();
	}
	*/
}

const FTGOR_MovementState& UTGOR_MovementComponent::GetState() const
{
	return State;
}

const FTGOR_MovementContent& UTGOR_MovementComponent::GetContent() const
{
	return Content;
}

const FTGOR_MovementFrame& UTGOR_MovementComponent::GetFrame() const
{
	return Frame;
}

void UTGOR_MovementComponent::ForceAdjustCurrentToClient()
{
	BufferForce();
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void UTGOR_MovementComponent::GetMovementInput(FVector& Input, FVector& View) const
{
	// Get input vector locally
	if (IsValid(Content.Movement))
	{
		Content.Movement->QueryInputVector(this, Input, View);
	}
}

void UTGOR_MovementComponent::SetInput(const FVector& Input, float Strength)
{
	RawInput = Input;
	InputStrength = Strength;

	if (IsValid(Pawn))
	{
		InputRotation = Pawn->GetCameraRotation();
	}
	else
	{
		InputRotation = FQuat::Identity;
	}
}

bool UTGOR_MovementComponent::IsKnockedOut() const
{
	return false;//Frame.Autonomy < KnockoutThreshold;
}

void UTGOR_MovementComponent::Knockin(float Speed)
{
	//Frame.Autonomy = FMath::Min(Frame.Autonomy + Speed, 1.0f);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

FVector UTGOR_MovementComponent::GetRawInput() const
{
	return RawInput;
}

float UTGOR_MovementComponent::GetInputStrength() const
{
	return InputStrength;
}

FQuat UTGOR_MovementComponent::GetInputRotation() const
{
	return InputRotation;
}


void UTGOR_MovementComponent::VisualTrace(const FVector& Offset, const FVector& Direction, float Distance, float Radius, FTGOR_VisualContact& Contact) const
{
	UWorld* World = GetWorld();
	USceneComponent* Component = GetBaseComponent();
	USceneComponent* VisualComponent = Pawn->GetSkeletalMesh();
	if (IsValid(Component) && IsValid(VisualComponent))
	{
		FHitResult HitOut;
		const FTransform Transform = Component->GetComponentTransform();
		const FTransform VisualTransform = VisualComponent->GetComponentTransform();

		const FVector Location = Transform.TransformPosition(Offset);
		const FVector Normal = UTGOR_Math::Normalize(Transform.TransformVectorNoScale(Direction));
		const FVector Translation = Normal * Distance;
		if (MovementTraceSweep(Radius, Location, Translation, HitOut))
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

bool UTGOR_MovementComponent::CenteredTrace(const FVector& Location, const FVector& Offset, const FVector& Direction, float Distance, float Radius, FHitResult& HitOut) const
{
	UWorld* World = GetWorld();
	const FVector Start = Location + Offset;
	
	if (MovementTraceSweep(Radius, Start, Direction * Distance, HitOut))
	{
		return true;
	}
	return false;
}

bool UTGOR_MovementComponent::SampleTrace(const FVector& Location, const FVector& Direction, const FVector& Principal, float Distance, float Radius, FHitResult& HitOut) const
{
	if (CenteredTrace(Location, FVector::ZeroVector, Direction, Distance, Radius, HitOut))
	{
		UWorld* World = GetWorld();
		check(World && "World invalid");

		// Compute orthonormal base
		FVector Orth = Principal ^ Direction;
		if (Orth.SizeSquared() < KINDA_SMALL_NUMBER)
		{
			// Find world aligned vector to orthogonalise
			if (1.0f - Orth.X * Orth.X < KINDA_SMALL_NUMBER)
			{
				Orth = FVector::RightVector;
			}
			else
			{
				Orth = FVector::ForwardVector;
			}
		}
		const float TraceRadius = Radius / 4;
		const FVector Forward = UTGOR_Math::Normalize(Orth ^ Direction) * (Radius - TraceRadius);
		const FVector Right = Forward ^ Direction;

		bool HitAll = true;
		FHitResult HitForward, HitBackward, HitRight, HitLeft;
		HitAll = CenteredTrace(Location, Forward, Direction, Distance, TraceRadius, HitForward) && HitAll;
		HitAll = CenteredTrace(Location, -Forward, Direction, Distance, TraceRadius, HitBackward) && HitAll;
		HitAll = CenteredTrace(Location, Right, Direction, Distance, TraceRadius, HitRight) && HitAll;
		HitAll = CenteredTrace(Location, -Right, Direction, Distance, TraceRadius, HitLeft) && HitAll;

		if (HitAll)
		{
			// Compute normal from samples
			const FVector Scaled = (HitForward.Location - HitBackward.Location) ^ (HitRight.Location - HitLeft.Location);
			HitOut.ImpactNormal = UTGOR_Math::Normalize(Scaled);

			// Lerp between directions to account for ledges
			const float ForwardMatch = HitOut.ImpactNormal | HitForward.ImpactNormal;
			const float BackwardMatch = HitOut.ImpactNormal | HitBackward.ImpactNormal;
			const float RightMatch = HitOut.ImpactNormal | HitRight.ImpactNormal;
			const float LeftMatch = HitOut.ImpactNormal | HitLeft.ImpactNormal;
			const FVector Accumulate = ForwardMatch * HitForward.ImpactNormal + BackwardMatch * HitBackward.ImpactNormal + RightMatch * HitRight.ImpactNormal + LeftMatch * HitLeft.ImpactNormal;
			const float Sum = ForwardMatch + BackwardMatch + RightMatch + LeftMatch;
			if (Sum >= SMALL_NUMBER)
			{
				HitOut.Normal = UTGOR_Math::Normalize(Accumulate / Sum);
			}
			else
			{
				HitOut.Normal = HitOut.ImpactNormal;
			}
			
			// Compute average location
			HitOut.ImpactPoint = (HitForward.Location + HitBackward.Location + HitRight.Location + HitLeft.Location) / 4;

			// Project onto direction vector
			const FVector Diff = Location - HitOut.ImpactPoint;
			HitOut.Location = Location + Diff.ProjectOnToNormal(Direction);


			// Debug draw
			if (CVarMovementSurfaceDebugDrawEnable.GetValueOnAnyThread() == 1)
			{
				DrawDebugPoint(World, HitForward.ImpactPoint, 4.0f, FColor::Blue, false, -1.0f, 0);
				DrawDebugPoint(World, HitBackward.ImpactPoint, 4.0f, FColor::Blue, false, -1.0f, 0);
				DrawDebugLine(World, HitForward.ImpactPoint, HitBackward.ImpactPoint, FColor::Green, false, -1.0f, 0, 2.0f);
				DrawDebugPoint(World, HitRight.ImpactPoint, 4.0f, FColor::Blue, false, -1.0f, 0);
				DrawDebugPoint(World, HitLeft.ImpactPoint, 4.0f, FColor::Blue, false, -1.0f, 0);
				DrawDebugLine(World, HitLeft.ImpactPoint, HitRight.ImpactPoint, FColor::Green, false, -1.0f, 0, 2.0f);
				DrawDebugLine(World, HitOut.ImpactPoint, HitOut.ImpactPoint + HitOut.Normal * 100.0f, FColor::Red, false, -1.0f, 0, 4.0f);
			}
		}

		return HitOut.bBlockingHit;
	}
	return false;
}


bool UTGOR_MovementComponent::Overlap(const FVector& Location, const FQuat& Rotation, float Radius, float HalfHeight, FHitResult& HitOut) const
{
	USceneComponent* Component = GetBaseComponent();
	if (!IsValid(Component)) return false;
	UWorld* World = GetWorld();
	check(World);

	const float DoubleThreshold = 10.0f;

	TArray<AActor*> Ignore;
	FCollisionQueryParams TraceParams(FName(TEXT("MovementModeTrace")), true, GetOwner());
	TraceParams.bTraceComplex = false;
	TraceParams.bReturnFaceIndex = false;
	TraceParams.bReturnPhysicalMaterial = false;
	TraceParams.bIgnoreTouches = true;

	//Re-initialize hit info
	TArray<FHitResult> Hits;
	const ECollisionChannel CollisionChannel = Component->GetCollisionObjectType();
	const FCollisionShape Shape = FCollisionShape::MakeCapsule(Radius, HalfHeight);

	if (CVarMovementOverlapDebugDrawEnable.GetValueOnAnyThread() == 1)
	{
		DrawDebugCapsule(World, Location, HalfHeight, Radius, Rotation, FColor::Blue, false, -1.0f, 0, 3.0f);
	}

	if (World->SweepMultiByChannel(Hits, Location, Location + FVector(0.001f, 0.0f, 0.0f), Rotation, CollisionChannel, Shape, TraceParams))
	{
		// Always assume first as default
		if (FilterSweepResults(Hits, FVector::ZeroVector, HitOut))
		{
			// Compute hit plane
			const FPlane Plane = FPlane(HitOut.ImpactPoint, HitOut.Normal);

			// Sum up all points in front of the hit plane
			int32 Count = 0;
			FVector Acc = FVector::ZeroVector;
			for (const FHitResult& Hit : Hits)
			{
				if (Plane.PlaneDot(Hit.ImpactPoint) >= 0.0f)
				{
					if (CVarMovementOverlapDebugDrawEnable.GetValueOnAnyThread() == 1)
					{
						DrawDebugPoint(World, Hit.ImpactPoint, 10.0f, FColor::Red, false, -1.0f, 0);
					}
					Acc += Hit.Normal * Hit.PenetrationDepth;
					Count += 1;
				}
			}

			// Compute average normal and depth
			if (Count > 0)
			{
				const FVector Average = Acc / Count;
				const float AverageDepth = Average.Size();
				if (AverageDepth >= SMALL_NUMBER)
				{
					HitOut.Normal = Average / AverageDepth;
				}
			}
		}
		else if (CVarMovementOverlapDebugDrawEnable.GetValueOnAnyThread() == 1)
		{
			DrawDebugPoint(World, HitOut.ImpactPoint, 10.0f, FColor::Red, false, -1.0f, 0);
			DrawDebugLine(World, HitOut.ImpactPoint, HitOut.ImpactPoint + HitOut.Normal * 1000.0f, FColor::Yellow, false, -1.0f, 0, 6.0f);
		}

		return true;
	}
	HitOut = FHitResult(ForceInit);
	return false;
}

////////////////////////////////////////////////////////////////////////////////////////////////////


float UTGOR_MovementComponent::ComputeDirectionRatio(float ForwardRatio, float SpeedRatio, float LockTurn) const
{
	const float ClampedSpeedRatio = FMath::Min(SpeedRatio, 1.0f);
	if (LockTurn >= SMALL_NUMBER)
	{
		// Compute cutoff "angle" at which directionRatio starts being non-zero
		const float Cutoff = (1.0f - 1.0f / LockTurn);
		const float DirectionRatio = FMath::Max(0.0f, ForwardRatio - Cutoff) * LockTurn;

		// Prioritise speed if we're going fast
		const float InverseSpeedRatio = 1.0f - ClampedSpeedRatio;
		return FMath::Max(ClampedSpeedRatio, DirectionRatio) * InverseSpeedRatio;
	}
	return 1.0f;
}


float UTGOR_MovementComponent::ComputeTorqueSpeedRatio(float SpeedRatio, float TorqueSpeedCutoff) const
{
	return FMath::Clamp(1.0f - SpeedRatio + TorqueSpeedCutoff, 0.0f, 1.0f);
}

FVector UTGOR_MovementComponent::ComputeTorqueAlong(const FVector& TorqueAxis, const FVector& Forward, const FVector& Direction) const
{
	const float Ratio = Forward | Direction;
	const FVector Axis = Forward ^ Direction;
	const float Alignment = (TorqueAxis | Axis);
	if (Ratio >= 0.0f)
	{
		return TorqueAxis * Alignment;
	}
	else
	{
		// Rotate full speed if moving counter to forward vector
		return TorqueAxis * (Alignment >= 0.0f ? 1.0f : -1.0f);
	}
}

FVector UTGOR_MovementComponent::ComputeTorqueTowards(const FVector& Forward, const FVector& Direction) const
{
	// Rotate towards input vector
	const FVector Axis = Forward ^ Direction;
	const float Ratio = Forward | Direction;
	if (Ratio >= 0.0f)
	{
		return Axis;
	}
	else
	{
		// If rotation is greater than 90 deg rotate with full speed
		const float AxisSize = Axis.Size();
		if (AxisSize >= SMALL_NUMBER)
		{
			// Rotate full speed if moving counter to forward vector
			return Axis / AxisSize;
		}
		else
		{
			// Take actor's current axis if input is opposite
			const FTGOR_MovementPosition Position = ComputePosition();
			return Position.Angular.GetAxisY();
		}
	}
}

FVector UTGOR_MovementComponent::ComputeForceTowards(const FVector& Forward, const FVector& Damping, float Strength, float SpeedRatio) const
{
	// Turn off input force if too fast, counteract braking in movement direction if moving
	const float UnderSpeedRatio = FMath::Clamp(2.0f - SpeedRatio, 0.0f, 1.0f);
	const float AntiBrake = (Forward | Damping) * UnderSpeedRatio;
	return Forward * (Strength * Frame.Strength - AntiBrake) * UnderSpeedRatio;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void UTGOR_MovementComponent::TickReset(float Time)
{
	Content.HasOverride = false;
	Super::TickReset(Time);
}

void UTGOR_MovementComponent::PreComputePhysics(const FTGOR_MovementTick& Tick, bool Replay)
{
	// Grab input
	if (UpdatesBase())
	{
		if (!Replay)
		{
			if (!Content.HasOverride)
			{
				GetMovementInput(State.Input, State.View);
			}
		}
		else if (!Pawn->IsPawnControlled())
		{
			State.Input = FVector::ZeroVector;
		}
	}

	Super::PreComputePhysics(Tick, Replay);
}

void UTGOR_MovementComponent::ComputePhysics(const FTGOR_MovementTick& Tick, const FTGOR_MovementSpace& Space, const FTGOR_MovementExternal& External, bool Replay, FTGOR_MovementOutput& Output)
{

	if (Replay || (GetOwnerRole() != ENetRole::ROLE_SimulatedProxy))
	{
		// Update Movement
		UpdateMovement(Tick, Space, External);
	}
	else if(IsValid(Content.Movement) && !Content.Movement->Invariant(this, Tick, Space, External))
	{
		RPORT("Movement invalid on client");
	}


	if (!Replay)
	{
		UpdateAnimation(Tick, Space, External);
	}

	// Reset and compute force from movement
	if (IsValid(Content.Movement))
	{
		SCOPE_CYCLE_COUNTER(STAT_MovementTick);
		Content.Movement->Tick(this, Tick, Space, External, Replay, Output);
	}

	Super::ComputePhysics(Tick, Space, External,  Replay, Output);
}

void UTGOR_MovementComponent::MovementUpdate()
{
	Super::MovementUpdate();

	if (GetOwnerRole() == ENetRole::ROLE_AutonomousProxy)
	{
		ServerInputLight(State);
	}
}

void UTGOR_MovementComponent::OutOfLevel()
{
	if (IsValid(Pawn))
	{
		Pawn->OutOfLevel();
	}
	Super::OutOfLevel();
}

void UTGOR_MovementComponent::Impact(const FTGOR_MovementDynamic& Dynamic, const FVector& Point, const FVector& Impact)
{
	Super::Impact(Dynamic, Point, Impact);

	// Knock pawn out if we have movement authority
	const bool HasAuthority = Pawn->HasAuthority();
	const bool IsMaster = (UpdatesBase() && GetMasterRole() == ENetRole::ROLE_AutonomousProxy);
	if (HasAuthority || IsMaster)
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

			// Let server know if we have movement authority
			if (!HasAuthority && IsMaster)
			{
				ServerKnockout();
			}
		}
	}
}

bool UTGOR_MovementComponent::CanInflict() const
{
	return true;
}

bool UTGOR_MovementComponent::CanRotateOnImpact() const
{
	if (IsValid(Content.Movement))
	{
		return Content.Movement->CanRotateOnCollision;
	}
	return false;
}


void UTGOR_MovementComponent::UpdateMovement(const FTGOR_MovementTick& Tick, const FTGOR_MovementSpace& Space, const FTGOR_MovementExternal& External)
{
	// No need to update if a manual mode is running
	if (IsValid(Content.Movement))
	{
		// Abort invalid movement modes
		if (!Content.Movement->Invariant(this, Tick, Space, External) ||
			(Content.Movement->Mode == ETGOR_MovementEnumeration::Override && !Content.HasOverride))
		{
			Content.Movement = nullptr;
		}
		else if(Content.Movement->Mode != ETGOR_MovementEnumeration::Queued)
		{
			// Don't check queue on manual/sticky modes
			return;
		}
	}

	// Go through priority list in reverse order
	for(int i = MovementQueue.Num() - 1; i >= 0; i--)
	{
		// Stop at already active movement
		UTGOR_Movement* Movement = MovementQueue[i];
		if (Movement == Content.Movement)
		{
			return;
		}

		// Queue first valid movement
		if(Movement->Invariant(this, Tick, Space, External))
		{
			Content.Movement = Movement;
			return;
		}
	}
}

void UTGOR_MovementComponent::UpdateAnimation(const FTGOR_MovementTick& Tick, const FTGOR_MovementSpace& Space, const FTGOR_MovementExternal& External)
{
	UTGOR_Animation* OldAnimation = Content.Animation;
	if (IsValid(Content.Movement) && IsValid(Pawn))
	{
		// Get animation from current movement
		Content.Animation = Content.Movement->QueryAnimation(this, Tick, Space, External);
		if (OldAnimation  == Content.Animation)
		{
			return;
		}

		// Don't need to update if already valid
		if (OldAnimation != Content.Animation)
		{
			if (IsValid(Content.Animation))
			{
				UTGOR_AnimInstance* Instance = Pawn->GetAnimation();
				if (IsValid(Instance))
				{
					// Register animation in AnimInstance
					Instance->AssignAnimationInstance(AnimationSlotName, Content.Animation);
				}
			}

			// Notify pawn about change
			Pawn->AnimationChange(OldAnimation, Content.Animation);
		}
	}
	else if (IsValid(Content.Animation))
	{
		// Reset on invalid mode
		Content.Animation = nullptr;
		Pawn->AnimationChange(OldAnimation, nullptr);
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////


void UTGOR_MovementComponent::ServerInputLight_Implementation(FTGOR_MovementState Update)
{
	State.Input = Update.Input;
	State.View = Update.View;
}

bool UTGOR_MovementComponent::ServerInputLight_Validate(FTGOR_MovementState Update)
{
	return true;
}

void UTGOR_MovementComponent::ServerKnockout_Implementation()
{
	if (GetMasterRole() == ENetRole::ROLE_AutonomousProxy || Pawn->IsLocallyControlled())
	{
		OnKnockout.Broadcast();
		//Frame.Autonomy = 0.0f;
	}
}

bool UTGOR_MovementComponent::ServerKnockout_Validate()
{
	return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void UTGOR_MovementComponent::ServerContentForceUpdate_Implementation(FTGOR_MovementInputUpdate Update)
{
	ERROR("Authoritive client trying to send force update to server", Error);
}

bool UTGOR_MovementComponent::ServerContentForceUpdate_Validate(FTGOR_MovementInputUpdate Update)
{
	return true;
}

void UTGOR_MovementComponent::ClientContentForceUpdate_Implementation(FTGOR_MovementContentUpdate Update)
{
	const ENetRole NetRole = GetOwnerRole();
	if (NetRole == GetMasterRole())
	{
		auto Override = [&Update, this](int32 Index) { return ContentBufferOverride(Update, Index); };
		TemplateUpdate(Override);
	}
}


void UTGOR_MovementComponent::ServerContentBufferAdjust_Implementation(FTGOR_MovementInputUpdate Update)
{
	const ENetRole NetRole = GetOwnerRole();
	if (NetRole == GetSlaveRole())
	{
		ServerInputLight_Implementation(Update.State);

		auto Compare = [&Update, this](int32 Index, const FTGOR_MovementThreshold& Threshold) { return ContentBufferCompare(Update, Index, Threshold); };
		auto Override = [&Update, this](int32 Index) { return ContentBufferOverride(Update, Index); };
		TemplateAdjust(Compare, Override);
	}
}

bool UTGOR_MovementComponent::ServerContentBufferAdjust_Validate(FTGOR_MovementInputUpdate Update)
{
	return true;
}

void UTGOR_MovementComponent::ClientContentBufferAdjust_Implementation(FTGOR_MovementContentUpdate Update)
{
	const ENetRole NetRole = GetOwnerRole();
	if (NetRole == GetSlaveRole())
	{
		auto Compare = [&Update, this](int32 Index, const FTGOR_MovementThreshold& Threshold) { return ContentBufferCompare(Update, Index, Threshold); };
		auto Override = [&Update, this](int32 Index) { return ContentBufferOverride(Update, Index); };
		TemplateAdjust(Compare, Override);
	}
}

void UTGOR_MovementComponent::MulticastContentBufferAdjust_Implementation(FTGOR_MovementContentUpdate Update)
{
	const ENetRole NetRole = GetOwnerRole();
	if (NetRole == GetSlaveRole())
	{
		auto Compare = [&Update, this](int32 Index, const FTGOR_MovementThreshold& Threshold) { return ContentBufferCompare(Update, Index, Threshold); };
		auto Override = [&Update, this](int32 Index) { return ContentBufferOverride(Update, Index); };
		TemplateAdjust(Compare, Override);
	}
}


/////////////////////

bool UTGOR_MovementComponent::BufferPeerAdjust(int32 PrevIndex, int32 NextIndex, const FTGOR_MovementThreshold& Threshold)
{
	// Compare with current state and adjust peer if necessary
	const FTGOR_MovementInputUpdate Update = CreateMovementInputUpdate(NextIndex);
	if (NextIndex == PrevIndex || (BufferCompare(Update, PrevIndex, AdjustThreshold) >= 1.0f))
	{
		switch (GetSlaveRole())
		{
		case ENetRole::ROLE_SimulatedProxy: MulticastContentBufferAdjust(Update); break;
		case ENetRole::ROLE_AutonomousProxy: ClientContentBufferAdjust(Update); break;
		case ENetRole::ROLE_Authority: ServerContentBufferAdjust(Update); break;
		}
		return true;
	}
	return false;
}

void UTGOR_MovementComponent::BufferReplicate(int32 Index)
{
	const FTGOR_MovementInputUpdate Update = CreateMovementInputUpdate(Index);
	switch (GetMasterRole())
	{
	case ENetRole::ROLE_SimulatedProxy: break;
	case ENetRole::ROLE_AutonomousProxy: ClientContentForceUpdate(Update); break;
	case ENetRole::ROLE_Authority: ServerContentForceUpdate(Update); break;  // This should never happen
	}
}

/////////////////////

void UTGOR_MovementComponent::LoadFromBufferExternal(int32 Index)
{
	State = StateBuffer[Index];
	Frame = FrameBuffer[Index];

	// Movement mode only copies on inheritance
	Content.HasOverride = ContentBuffer[Index].HasOverride;
	if (Content.HasOverride)
	{
		Content.Movement = ContentBuffer[Index].Movement;
	}
	Super::LoadFromBufferExternal(Index);
}

void UTGOR_MovementComponent::LoadFromBufferWhole(int32 Index)
{
	Content = ContentBuffer[Index];
	Super::LoadFromBufferWhole(Index);
}

void UTGOR_MovementComponent::LerpToBuffer(int32 PrevIndex, int32 NextIndex, float Alpha)
{
	StateBuffer[PrevIndex].Lerp(StateBuffer[NextIndex], Alpha);
	ContentBuffer[PrevIndex].Lerp(ContentBuffer[NextIndex], Alpha);
	FrameBuffer[PrevIndex].Lerp(FrameBuffer[NextIndex], Alpha);
	Super::LerpToBuffer(PrevIndex, NextIndex, Alpha);
}

void UTGOR_MovementComponent::StoreToBuffer(int32 Index)
{
	StateBuffer[Index] = GetState();
	ContentBuffer[Index] = GetContent();
	FrameBuffer[Index] = GetFrame();
	Super::StoreToBuffer(Index);
}

void UTGOR_MovementComponent::SetBufferSize(int32 Size)
{
	StateBuffer.SetNum(Size);
	ContentBuffer.SetNum(Size);
	FrameBuffer.SetNum(Size);
	Super::SetBufferSize(Size);
}

/////////////////////

FTGOR_MovementInputUpdate UTGOR_MovementComponent::CreateMovementInputUpdate(int32 Index) const
{
	FTGOR_MovementInputUpdate Update(CreateMovementUpdate(Index));
	Update.Movement = ContentBuffer[Index].Movement;
	Update.State = StateBuffer[Index];
	return Update;
}

float UTGOR_MovementComponent::ContentBufferCompare(const FTGOR_MovementContentUpdate& Update, int32 Index, const FTGOR_MovementThreshold& Threshold) const
{
	const FTGOR_MovementContent& BContent = ContentBuffer[Index];
	if (Update.Movement != BContent.Movement)
	{
		return 1.0f;
	}

	return Super::BufferCompare(Update, Index, Threshold);
}

void UTGOR_MovementComponent::ContentBufferOverride(const FTGOR_MovementContentUpdate& Update, int32 Index)
{
	Super::BufferOverride(Update, Index);
	ContentBuffer[Index].Movement = Update.Movement;
}

