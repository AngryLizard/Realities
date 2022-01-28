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
#include "TargetSystem/Components/TGOR_AimComponent.h"
#include "SimulationSystem/Components/TGOR_SimulationComponent.h"
#include "CustomisationSystem/Components/TGOR_ModularSkeletalMeshComponent.h"
#include "DimensionSystem/Components/TGOR_MobilityComponent.h"
#include "DimensionSystem/Volumes/TGOR_LevelVolume.h"
#include "RealitiesUtility/Utility/TGOR_Error.h"
#include "RealitiesUtility/Utility/TGOR_Math.h"
#include "DimensionSystem/Content/TGOR_Spawner.h"

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
	InputRotation(FRotator::ZeroRotator),
	DirectInput(false),

	KnockoutThreshold(0.1f),
	KnockoutHeadThreshold(230'000.0),
	KnockoutFootThreshold(320'000.0)
{
}

void UTGOR_MovementComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction)
{
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
	DOREPLIFETIME_CONDITION(UTGOR_MovementComponent, MovementState, COND_None);
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
	Movements = Dependencies.Spawner->GetMListFromType<UTGOR_Movement>(SpawnMovements);

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
			MovementSlot->MarkPendingKill();
		}
	}

	BuildMovementFrame();
}

TMap<int32, UTGOR_SpawnModule*> UTGOR_MovementComponent::GetModuleType_Implementation() const
{
	TMap<int32, UTGOR_SpawnModule*> Modules;

	const int32 Num = Movements.Num();
	for (int32 Index = 0; Index < Num; Index++)
	{
		Modules.Emplace(Index, Movements[Index]);
	}
	return Modules;
}

TSubclassOf<UTGOR_Performance> UTGOR_MovementComponent::GetPerformanceType() const
{
	return PerformanceType;
}

UTGOR_AnimationComponent* UTGOR_MovementComponent::GetAnimationComponent() const
{
	return GetOwnerComponent<UTGOR_AnimationComponent>();
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

void UTGOR_MovementComponent::GetMovementInput(FVector& Input, FVector& View) const
{
	if (DirectInput)
	{
		Input = RawInput;
		View = InputRotation.GetAxisX();
	}
	else
	{
		// Get input vector locally
		UTGOR_MovementTask* CurrentTask = GetMovementTask();
		if (IsValid(CurrentTask))
		{
			CurrentTask->QueryInput(Input, View);
		}
	}
}

void UTGOR_MovementComponent::SetInput(const FVector& Input, float Strength)
{
	RawInput = Input;
	DirectInput = false;
	InputStrength = Strength;

	InputRotation = FQuat::Identity;
	if (UCameraComponent* Camera = GetOwnerComponent<UCameraComponent>())
	{
		InputRotation = Camera->GetComponentQuat();
	}
}

void UTGOR_MovementComponent::ForwardInput(const FVector& Input, const FQuat& View)
{
	RawInput = Input;
	DirectInput = true;
	InputStrength = 1.0f;

	InputRotation = View;
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
	if (MovementSlots.IsValidIndex(MovementState.ActiveSlot))
	{
		return MovementSlots[MovementState.ActiveSlot];
	}
	return nullptr;
}


bool UTGOR_MovementComponent::IsMovingWith(int32 Identifier) const
{
	return MovementState.ActiveSlot == Identifier;
}

void UTGOR_MovementComponent::MoveWith(int32 Identifier)
{
	if (Identifier != MovementState.ActiveSlot)
	{
		if (UTGOR_MovementTask* Task = GetMovementTask())
		{
			Task->Interrupt();
		}

		MovementState.ActiveSlot = Identifier;

		if (UTGOR_MovementTask* Task = GetMovementTask())
		{
			Task->PrepareStart();
		}

		OnMovementChanged.Broadcast();
	}
}

UTGOR_MovementTask* UTGOR_MovementComponent::GetMovementOfType(TSubclassOf<UTGOR_MovementTask> Type) const
{
	if (*Type)
	{
		for (UTGOR_MovementTask* MovementSlot : MovementSlots)
		{
			if (MovementSlot->IsA(Type))
			{
				return MovementSlot;
			}
		}
	}
	return nullptr;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void UTGOR_MovementComponent::RepNotifyMovementState(const FTGOR_MovementState& Old)
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
	return InputRotation;
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
	APawn* Pawn = Cast<APawn>(GetOwner());
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
	// Grab input
	APawn* Pawn = Cast<APawn>(GetOwner());
	if (Pawn->IsLocallyControlled())
	{
		GetMovementInput(MovementInput.Input, MovementInput.View);
	}
	Super::PreComputePhysics(Tick);
}

void UTGOR_MovementComponent::ComputePhysics(FTGOR_MovementSpace& Space, const FTGOR_MovementExternal& External, const FTGOR_MovementTick& Tick, FTGOR_MovementOutput& Output)
{
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
