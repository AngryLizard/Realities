// The Gateway of Realities: Planes of Existence.

#include "TGOR_MovementComponent.h"

#include "MovementSystem/Content/TGOR_Mobile.h"
#include "MovementSystem/Content/TGOR_Movement.h"
#include "MovementSystem/Tasks/TGOR_MovementTask.h"
#include "AttributeSystem/Content/TGOR_Attribute.h"

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
	/*
	// Testing attachment points
	FTGOR_MovementSpace Space = GetBase().ComputeParentSpace();
	DrawDebugPoint(GetWorld(), Space.Linear, 25.0f, FColor::Blue, false, -1.0f, 0);
	*/

	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UTGOR_MovementComponent::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// Replicate to everyone
	DOREPLIFETIME_CONDITION(UTGOR_MovementComponent, MovementFrame, COND_None);
	DOREPLIFETIME_CONDITION(UTGOR_MovementComponent, MovementInput, COND_SimulatedOnly);

	DOREPLIFETIME_CONDITION(UTGOR_MovementComponent, MovementSlots, COND_None); // This MUST be replicated before setup
	DOREPLIFETIME_CONDITION(UTGOR_MovementComponent, MovementSetup, COND_None);
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

void UTGOR_MovementComponent::UpdateContent_Implementation(UTGOR_Spawner* Spawner)
{
	ITGOR_SpawnerInterface::UpdateContent_Implementation(Spawner);

	FTGOR_MovementInstance Setup;
	Setup.Mobile = Spawner->GetMFromType<UTGOR_Mobile>(TargetMobile);
	ApplyMovementSetup(Setup);

	BuildMovementFrame();
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
		MovementState.ActiveSlot = Identifier;
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

bool UTGOR_MovementComponent::ApplyMovementSetup(FTGOR_MovementInstance Setup)
{
	SINGLETON_RETCHK(false);

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
	MovementSetup.Mobile = Setup.Mobile;
	if (IsValid(Setup.Mobile))
	{
		// Get installed movement modes
		const TArray<UTGOR_Movement*>& Movements = Setup.Mobile->Instanced_MovementInsertions.Collection;//Content->GetIListFromType<UTGOR_Movement>();

		// Get all candidates that are part of the movement queue
		TArray<UTGOR_Movement*> Candidates;
		for (UTGOR_Movement* Movement : Movements)
		{
			// Add inserted attributes
			const TArray<UTGOR_Attribute*>& AttributeQueue = Movement->Instanced_AttributeInsertions.Collection;//GetIListFromType<UTGOR_Attribute>();
			for (UTGOR_Attribute* Attribute : AttributeQueue)
			{
				// Attributes were already reset in a parent component
				Attributes.Values.Emplace(Attribute, Attribute->DefaultValue);
			}

			Candidates.Emplace(Movement);
		}

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

					// Get slot from cache or create one
					UTGOR_MovementTask* MovementSlot = nullptr;

					TArray<UTGOR_MovementTask*>* Ptr = Previous.Find(Candidate);
					if (Ptr && Ptr->Num() > 0)
					{
						MovementSlot = Ptr->Pop();
					}
					else
					{
						// No cache was found, create a new one
						MovementSlot = Candidate->CreateMovementTask(this, MovementSlots.Num());
					}

					// Initialise and add to slots
					if (IsValid(MovementSlot))
					{
						MovementSlots.Add(MovementSlot);
					}

					AnyCandidates = true;
				}
			}
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
	return true;
}


void UTGOR_MovementComponent::OnReplicateMovementSetup()
{
	ApplyMovementSetup(MovementSetup);
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

void UTGOR_MovementComponent::UpdateMovement(const FTGOR_MovementTick& Tick, const FTGOR_MovementSpace& Space, const FTGOR_MovementExternal& External)
{
	// No need to update if a manual mode is running
	UTGOR_MovementTask* CurrentTask = GetMovementTask();
	if (IsValid(CurrentTask))
	{
		// Abort invalid movement modes
		if (!CurrentTask->Invariant(Space, External))
		{
			MoveWith(INDEX_NONE);
		}
		else if (CurrentTask->GetMovement()->Mode != ETGOR_MovementEnumeration::Queued)
		{
			// Don't check queue on manual/sticky modes
			return;
		}
	}

	// Go through priority list in reverse order
	for (int32 Index = MovementSlots.Num() - 1; Index >= 0; Index--)
	{
		// Stop at already active movement
		UTGOR_MovementTask* MovementSlot = MovementSlots[Index];
		if (MovementSlot == CurrentTask)
		{
			return;
		}

		// Queue first valid movement
		if (MovementSlot->Invariant(Space, External))
		{
			MoveWith(Index);
			return;
		}
	}
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
