// The Gateway of Realities: Planes of Existence.
#include "TGOR_PhysicsComponent.h"

#include "DimensionSystem/Tasks/TGOR_PilotTask.h"

#include "CoreSystem/TGOR_Singleton.h"
#include "DimensionSystem/Data/TGOR_WorldData.h"
#include "DimensionSystem/Volumes/TGOR_PhysicsVolume.h"

#include "Net/UnrealNetwork.h"

UTGOR_PhysicsComponent::UTGOR_PhysicsComponent()
:	Super(),
	SimulationTimestep(0.003f)
{
}

float UTGOR_PhysicsComponent::Simulate(float Time)
{
	float PendingTime = Time;
	if (SimulationTimestep >= SMALL_NUMBER)
	{
		// Simulate by one tick if not preempted
		while (PendingTime >= SimulationTimestep)
		{
			PendingTime -= TickPhysics(SimulationTimestep);
		}
	}

	return Super::Simulate(Time - PendingTime);
}

bool UTGOR_PhysicsComponent::Teleport(const FTGOR_MovementDynamic& Dynamic)
{
	const bool Success = Super::Teleport(Dynamic);

	// Get main volume for physics
	SINGLETON_RETCHK(Success);
	if (!SurroundingVolume.IsValid())
	{
		UTGOR_WorldData* WorldData = Singleton->GetData<UTGOR_WorldData>();
		if (IsValid(WorldData))
		{
			SurroundingVolume = WorldData->GetMainVolume();

			// Notify owner
			OnVolumeChanged.Broadcast(nullptr, SurroundingVolume.Get());
		}
	}

	// Get surroundings info
	const FTGOR_MovementPosition Position = ComputePosition();
	UpdateSurroundings(Position.Linear);

	// Store in local space
	UTGOR_PilotTask* PilotTask = GetPilotTask();
	if (IsValid(PilotTask))
	{
		const FTGOR_MovementPosition ParentPosition = PilotTask->ComputeBase();
		Capture.UpVector = ParentPosition.Angular.Inverse() * Position.Angular.GetUpVector();
	}

	return Success;
}

void UTGOR_PhysicsComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// Replicate to everyone
	DOREPLIFETIME_CONDITION(UTGOR_PhysicsComponent, Capture, COND_None);
}

void UTGOR_PhysicsComponent::OnPositionChange(const FTGOR_MovementPosition& Position)
{
	Super::OnPositionChange(Position);
	UpdateSurroundings(Position.Linear);
}


void UTGOR_PhysicsComponent::OnReparent(const FTGOR_MovementDynamic& Previous)
{
	// Store in local space
	UTGOR_PilotTask* PilotTask = GetPilotTask();
	if (IsValid(PilotTask))
	{
		const FTGOR_MovementPosition Position = PilotTask->ComputeBase();
		Capture.UpVector = Position.Angular.Inverse() * (Previous.Angular * Capture.UpVector);
	}

	Super::OnReparent(Previous);
}


////////////////////////////////////////////////////////////////////////////////////////////////////


void UTGOR_PhysicsComponent::UpdateSurroundings(const FVector& Location)
{
	if (SurroundingVolume.IsValid())
	{
		Capture.Surroundings = SurroundingVolume->ComputeSurroundings(Location);
	}
	else
	{
		OutOfLevel();
	}
}

void UTGOR_PhysicsComponent::OutOfLevel()
{
	OnOutOfLevel.Broadcast();
}

void UTGOR_PhysicsComponent::RepNotifyCapture(const FTGOR_MovementCapture& Old)
{
}

const FTGOR_MovementCapture& UTGOR_PhysicsComponent::GetCapture() const
{
	return Capture;
}


FVector UTGOR_PhysicsComponent::ComputePhysicsUpVector() const
{
	// Store in local space
	const FTGOR_MovementPosition Position = ComputeBase();
	return Position.Angular * Capture.UpVector;
}


float UTGOR_PhysicsComponent::GetBouyancyRatio() const
{
	const FTGOR_MovementBody& MovementBody = GetBody();
	if (MovementBody.Mass >= SMALL_NUMBER)
	{
		const float Displaced = MovementBody.GetDisplacedMass(Capture.Surroundings);
		return Displaced / MovementBody.Mass;
	}
	return 0.0f;
}


////////////////////////////////////////////////////////////////////////////////////////////////////

float UTGOR_PhysicsComponent::TickPhysics(float Time)
{
	// Make sure we are parented to volume if we aren't parented to anything
	UTGOR_PilotTask* PilotTask = GetPilotTask();
	if (!IsValid(PilotTask))
	{
		if (SurroundingVolume.IsValid())
		{
			UTGOR_MobilityComponent* Movement = SurroundingVolume->GetMovement();
			if (IsValid(Movement))
			{
				Movement->ParentLinear(this, INDEX_NONE, ComputeSpace());
			}
		}

		// Check whether it worked
		PilotTask = GetPilotTask();
		if (!IsValid(PilotTask))
		{
			ERRET("Physics object without pilot found", Error, Time);
		}
	}

	return Time;
}
