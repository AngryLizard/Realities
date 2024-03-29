// The Gateway of Realities: Planes of Existence.

#include "TGOR_MovementComponent.h"

#include "MovementSystem/Content/TGOR_Movement.h"
#include "MovementSystem/Tasks/TGOR_MovementTask.h"
#include "AttributeSystem/Content/TGOR_Attribute.h"
#include "AttributeSystem/Components/TGOR_AttributeComponent.h"

#include "AnimationSystem/Content/TGOR_Animation.h"
#include "AnimationSystem/Content/TGOR_Performance.h"
#include "AnimationSystem/Instances/TGOR_AnimInstance.h"
#include "AnimationSystem/Components/TGOR_AnimationComponent.h"

#include "CoreSystem/TGOR_Singleton.h"
#include "DimensionSystem/Data/TGOR_DimensionData.h"
#include "DimensionSystem/Components/TGOR_PilotComponent.h"
#include "TargetSystem/Components/TGOR_AimComponent.h"
#include "SimulationSystem/Components/TGOR_SimulationComponent.h"
#include "CustomisationSystem/Components/TGOR_ModularSkeletalMeshComponent.h"
#include "DimensionSystem/Components/TGOR_MobilityComponent.h"
#include "DimensionSystem/Volumes/TGOR_LevelVolume.h"
#include "RealitiesUtility/Utility/TGOR_Error.h"
#include "RealitiesUtility/Utility/TGOR_Math.h"
#include "DimensionSystem/Content/TGOR_Spawner.h"

#include "RealitiesUGC/Mod/TGOR_ContentManager.h"
#include "Camera/CameraComponent.h"
#include "DrawDebugHelpers.h"
#include "Net/UnrealNetwork.h"


DECLARE_CYCLE_STAT(TEXT("Movement simulation ticks over deltatime"), STAT_RealTick, STATGROUP_TGOR_MOV);
DECLARE_CYCLE_STAT(TEXT("Movement mode tick"), STAT_MovementTick, STATGROUP_TGOR_MOV);
DECLARE_CYCLE_STAT(TEXT("Movement simulation"), STAT_MovementSim, STATGROUP_TGOR_MOV);

////////////////////////////////////////////////////////////////////////////////////////////////////


UTGOR_MovementComponent::UTGOR_MovementComponent()
:	Super(),

	PerformanceType(UTGOR_Performance::StaticClass()),
	
	RawInput(FVector::ZeroVector),
	InputStrength(0.0f),

	KnockoutThreshold(0.1f),
	KnockoutHeadThreshold(230'000.0),
	KnockoutFootThreshold(320'000.0)
{
	bUseAccelerationForPaths = true;
}

void UTGOR_MovementComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction)
{
	const FVector ConsumedInput = ConsumeInputVector();

	// Grab input
	APawn* Pawn = GetPawnOwner();
	if (Pawn->IsLocallyControlled())
	{
		MovementInput.View = Pawn->GetViewRotation().Vector();
		MovementInput.Input = ConsumedInput;
	}

	UTGOR_MovementTask* CurrentTask = GetMovementTask();
	if (IsValid(CurrentTask))
	{
		CurrentTask->Process(DeltaTime);
	}

	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (GetOwnerRole() == ENetRole::ROLE_AutonomousProxy)
	{
		ServerInputLight(MovementInput);
	}
}

void UTGOR_MovementComponent::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// Replicate to everyone
	DOREPLIFETIME_CONDITION(UTGOR_MovementComponent, MovementFrame, COND_None);
	DOREPLIFETIME_CONDITION(UTGOR_MovementComponent, MovementInput, COND_SimulatedOnly);

	DOREPLIFETIME_CONDITION(UTGOR_MovementComponent, MovementSlots, COND_None); // This MUST be replicated before setup
	DOREPLIFETIME_CONDITION(UTGOR_MovementComponent, MovementTaskState, COND_None);
}

TSet<UTGOR_CoreContent*> UTGOR_MovementComponent::GetActiveContent_Implementation() const
{
	TSet<UTGOR_CoreContent*> ContentContext = ITGOR_RegisterInterface::GetActiveContent_Implementation();
	UTGOR_MovementTask* CurrentTask = GetMovementTask();
	if (IsValid(CurrentTask))
	{
		ContentContext.Emplace(CurrentTask->GetMovement());
	}
	return ContentContext;
}

void UTGOR_MovementComponent::UpdateContent_Implementation(FTGOR_SpawnerDependencies& Dependencies)
{
	ITGOR_SpawnerInterface::UpdateContent_Implementation(Dependencies);

	Dependencies.Process<UTGOR_PilotComponent>();
	Dependencies.Process<UTGOR_AnimationComponent>();


	UTGOR_MovementTask* CurrentTask = GetMovementTask();
	TMap<UTGOR_Movement*, TArray<UTGOR_MovementTask*>> Previous;

	// Remove slots but cache both instances and items in case the new loadout can use them
	for (int Slot = 0; Slot < MovementSlots.Num(); Slot++)
	{
		UTGOR_MovementTask* MovementSlot = MovementSlots[Slot];
		if (IsValid(MovementSlot))
		{
			Previous.FindOrAdd(MovementSlot->GetMovement()).Add(MovementSlot);
		}
	}

	MovementSlots.Empty();

	// Get installed movement modes
	TArray<UTGOR_Movement*> Movements = Dependencies.Spawner->GetMListFromType<UTGOR_Movement>(SpawnMovements);

	// Get all candidates that are part of the movement queue
	for (UTGOR_Movement* Movement : Movements)
	{
		// Get slot from cache or create one
		UTGOR_MovementTask* MovementSlot = nullptr;

		TArray<UTGOR_MovementTask*>* Ptr = Previous.Find(Movement);
		if (Ptr && Ptr->Num() > 0)
		{
			MovementSlot = Ptr->Pop();
		}
		else
		{
			// No cache was found, create a new one
			MovementSlot = Movement->CreateMovementTask(this, MovementSlots.Num());
		}

		// Initialise and add to slots
		if (IsValid(MovementSlot))
		{
			MovementSlots.Add(MovementSlot);
		}
	}

	// Schedule with current if available or reset
	const int32 ActiveSlot = MovementSlots.Find(CurrentTask);
	if (MovementSlots.IsValidIndex(ActiveSlot))
	{
		MoveWith(ActiveSlot);
	}
	else
	{
		MoveWith(INDEX_NONE);
	}

	// Discard tasks that got removed
	for (const auto& Pair : Previous)
	{
		for (UTGOR_MovementTask* MovementSlot : Pair.Value)
		{
			MovementSlot->MarkAsGarbage();
		}
	}

	BuildMovementFrame();
}

TMap<int32, UTGOR_SpawnModule*> UTGOR_MovementComponent::GetModuleType_Implementation() const
{
	TMap<int32, UTGOR_SpawnModule*> Modules;

	const int32 Num = MovementSlots.Num();
	for (UTGOR_MovementTask* MovementSlot : MovementSlots)
	{
		Modules.Emplace(MovementSlot->Identifier.Slot, MovementSlot->Identifier.Content);
	}
	return Modules;
}

TSubclassOf<UTGOR_Performance> UTGOR_MovementComponent::GetPerformanceType() const
{
	return PerformanceType;
}

UTGOR_AnimationComponent* UTGOR_MovementComponent::GetAnimationComponent() const
{
	AActor* Actor = GetOwner();
	return Actor->FindComponentByClass<UTGOR_AnimationComponent>();
}

float UTGOR_MovementComponent::GetMaxSpeed() const
{
	UTGOR_MovementTask* CurrentTask = GetMovementTask();
	if (IsValid(CurrentTask))
	{
		return CurrentTask->GetMaxSpeed();
	}
	return 0.0f;
}

void UTGOR_MovementComponent::RequestDirectMove(const FVector& MoveVelocity, bool bForceMaxSpeed)
{
	ERROR("Requesting DirectMove", Error);
}

void UTGOR_MovementComponent::RequestPathMove(const FVector& MoveInput)
{
	Super::RequestPathMove(MoveInput);
}

float UTGOR_MovementComponent::GetPathFollowingBrakingDistance(float MaxSpeed) const
{
	return Super::GetPathFollowingBrakingDistance(MaxSpeed * 0.5f);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void UTGOR_MovementComponent::BuildMovementFrame()
{
	// Make sure initial location is correct
	const float Scale = GetOwner()->GetActorScale().GetMin();

	// Client should already have gotten all the updates
	if (GetOwnerRole() != ENetRole::ROLE_SimulatedProxy)
	{
		// Set scale factors
		MovementFrame.Agility = Scale;
		MovementFrame.Strength = Scale * Scale * Scale;

	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void UTGOR_MovementComponent::RepNotifyState(const FTGOR_MovementInput& Old)
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

const FTGOR_MovementInput& UTGOR_MovementComponent::GetState() const
{
	return MovementInput;
}

const FTGOR_MovementFrame& UTGOR_MovementComponent::GetFrame() const
{
	return MovementFrame;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void UTGOR_MovementComponent::AddLocalInputVector(const FVector& Input, float Strength)
{
	RawInput = Input;
	InputStrength = Strength;

	// Get input vector locally
	UTGOR_MovementTask* CurrentTask = GetMovementTask();
	if (IsValid(CurrentTask))
	{
		FVector Vector, View;
		CurrentTask->QueryInput(Vector, View);
		AddInputVector(Vector);
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

UTGOR_MovementTask* UTGOR_MovementComponent::GetMovementTask() const
{
	if (MovementSlots.IsValidIndex(MovementTaskState.ActiveSlot))
	{
		return MovementSlots[MovementTaskState.ActiveSlot];
	}
	return nullptr;
}

bool UTGOR_MovementComponent::IsMovingWith(int32 Identifier) const
{
	return MovementTaskState.ActiveSlot == Identifier;
}

void UTGOR_MovementComponent::MoveWith(int32 Identifier)
{
	if (Identifier != MovementTaskState.ActiveSlot)
	{
		if (UTGOR_MovementTask* Task = GetMovementTask())
		{
			Task->Interrupt();
		}

		MovementTaskState.ActiveSlot = Identifier;

		if (UTGOR_MovementTask* Task = GetMovementTask())
		{
			Task->PrepareStart();
		}

		OnMovementChanged.Broadcast();
	}
}

UTGOR_MovementTask* UTGOR_MovementComponent::GetCurrentMovementOfType(TSubclassOf<UTGOR_MovementTask> Type) const
{
	if (MovementSlots.IsValidIndex(MovementTaskState.ActiveSlot) && MovementSlots[MovementTaskState.ActiveSlot]->IsA(Type))
	{
		return MovementSlots[MovementTaskState.ActiveSlot];
	}
	return nullptr;
}

TArray<UTGOR_MovementTask*> UTGOR_MovementComponent::GetMovementListOfType(TSubclassOf<UTGOR_MovementTask> Type) const
{
	TArray<UTGOR_MovementTask*> Movements;
	if (*Type)
	{
		for (UTGOR_MovementTask* MovementSlot : MovementSlots)
		{
			if (MovementSlot->IsA(Type))
			{
				Movements.Emplace(MovementSlot);
			}
		}
	}
	return Movements;
}

void UTGOR_MovementComponent::ForceUpdateMovement()
{
	UTGOR_PilotComponent* RootPilot = GetRootPilot();
	if (IsValid(RootPilot))
	{
		const FTGOR_MovementBody& MovementBody = RootPilot->GetBody();
		const FTGOR_MovementSpace Space = RootPilot->ComputeSpace();

		FTGOR_MovementOutput Out;
		FTGOR_MovementExternal External;
		ComputeExternal(Space, External, Out);

		FTGOR_MovementTick Tick;
		UpdateMovement(Tick, Space, External);
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void UTGOR_MovementComponent::RepNotifyMovementTaskState(const FTGOR_MovementState& Old)
{
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
	APawn* Pawn = GetPawnOwner();
	return FQuat(Pawn->GetControlRotation());
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
			AActor* Owner = GetOwner();
			const FQuat Angular = Owner->GetActorQuat();
			return Angular.GetAxisY();
		}
	}
}

FVector UTGOR_MovementComponent::ComputeForceTowards(const FVector& Forward, const FVector& Damping, float Strength, float SpeedRatio) const
{
	// Turn off input force if too fast, counteract braking in movement direction if moving
	const float UnderSpeedRatio = FMath::Clamp(2.0f - SpeedRatio, 0.0f, 1.0f);
	const float AntiBrake = (Forward | Damping) * UnderSpeedRatio;
	return Forward * (Strength * MovementFrame.Strength - AntiBrake) * UnderSpeedRatio;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

bool UTGOR_MovementComponent::UpdateMovement(const FTGOR_MovementTick& Tick, const FTGOR_MovementSpace& Space, const FTGOR_MovementExternal& External)
{
	// No need to update if a manual mode is running
	UTGOR_MovementTask* Task = GetMovementTask();
	if (IsValid(Task))
	{
		// Abort invalid movement modes
		if (!Task->Invariant(Space, External))
		{
			MoveWith(INDEX_NONE);
		}
		else if (Task->GetMovement()->Mode != ETGOR_MovementEnumeration::Queued)
		{
			// Don't check queue on manual/sticky modes
			return false;
		}
	}

	// Go through priority list
	for (int32 Index = 0; Index < MovementSlots.Num(); Index++)
	{
		// Stop at already active movement
		UTGOR_MovementTask* MovementSlot = MovementSlots[Index];
		if (MovementSlot == Task)
		{
			return false;
		}

		// Queue first valid movement
		if (MovementSlot->Invariant(Space, External))
		{
			MoveWith(Index);
			return true;
		}
	}

	// If we reach this point the current task is invalid,
	// so if the previous task was valid before the task changed.
	return IsValid(Task);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void UTGOR_MovementComponent::ServerInputLight_Implementation(FTGOR_MovementInput Update)
{
	MovementInput.Input = Update.Input;
	MovementInput.View = Update.View;
}

bool UTGOR_MovementComponent::ServerInputLight_Validate(FTGOR_MovementInput Update)
{
	return true;
}

void UTGOR_MovementComponent::ServerKnockout_Implementation()
{
	APawn* Pawn = GetPawnOwner();
	if (Pawn->IsLocallyControlled())
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

void UTGOR_MovementComponent::PreComputePhysics(const FTGOR_MovementTick& Tick)
{
	Super::PreComputePhysics(Tick);
}

void UTGOR_MovementComponent::ComputePhysics(FTGOR_MovementSpace& Space, const FTGOR_MovementExternal& External, const FTGOR_MovementTick& Tick, FTGOR_MovementOutput& Output)
{
	// Update movement context for all tasks
	for (UTGOR_MovementTask* MovementSlot : MovementSlots)
	{
		MovementSlot->Context(Space, External, Tick);
	}

	if (GetOwnerRole() != ENetRole::ROLE_SimulatedProxy)
	{
		// Update Movement
		UpdateMovement(Tick, Space, External);
	}
	else
	{
		UTGOR_MovementTask* CurrentTask = GetMovementTask();
		if (IsValid(CurrentTask) && !CurrentTask->Invariant(Space, External))
		{
			// Invalid movement, probably currently in transition phase
			return;
		}
	}

	// Reset and compute force from movement
	UTGOR_MovementTask* CurrentTask = GetMovementTask();
	if (IsValid(CurrentTask))
	{
		SCOPE_CYCLE_COUNTER(STAT_MovementTick);
		CurrentTask->Update(Space, External, Tick, Output);

		if (Output.Torque.ContainsNaN())
		{
			Output.Force = FVector::ZeroVector;
			Output.Torque = FVector::ZeroVector;
			ERROR("Torque singularity", Warning);
		}

		if (Output.Force.ContainsNaN())
		{
			Output.Force = FVector::ZeroVector;
			Output.Torque = FVector::ZeroVector;
			ERROR("Force singularity", Warning);
		}

		// Update space
		UTGOR_PilotComponent* RootPilot = GetRootPilot();
		Space = RootPilot->ComputeSpace();

		// Restrict this to only animate if necessary
		CurrentTask->Animate(Space, Tick.DeltaTime);
	}

	Super::ComputePhysics(Space, External, Tick, Output);
}

void UTGOR_MovementComponent::PostComputePhysics(const FTGOR_MovementSpace& Space, float Energy, float DeltaTime)
{
	UTGOR_MovementTask* CurrentTask = GetMovementTask();
	if (IsValid(CurrentTask))
	{
		CurrentTask->Animate(Space, DeltaTime);
	}
}

/*
void UTGOR_MovementComponent::Impact(const FTGOR_MovementDynamic& Dynamic, const FVector& Point, const FVector& Impact)
{
	Super::Impact(Dynamic, Point, Impact);

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
}

bool UTGOR_MovementComponent::CanInflict() const
{
	return true;
}

bool UTGOR_MovementComponent::CanRotateOnImpact() const
{
	UTGOR_MovementTask* CurrentTask = GetMovementTask();
	if (IsValid(CurrentTask))
	{
		return CurrentTask->GetMovement()->CanRotateOnCollision;
	}
	return false;
}
*/
