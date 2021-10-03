// The Gateway of Realities: Planes of Existence.
#include "TGOR_PhysicsComponent.h"

#include "CoreSystem/TGOR_Singleton.h"
#include "DimensionSystem/Data/TGOR_WorldData.h"
#include "DimensionSystem/Tasks/TGOR_PilotTask.h"
#include "DimensionSystem/Volumes/TGOR_PhysicsVolume.h"
#include "DimensionSystem/Components/TGOR_PilotComponent.h"
#include "RealitiesUtility/Utility/TGOR_Math.h"

#include "Net/UnrealNetwork.h"

UTGOR_PhysicsComponent::UTGOR_PhysicsComponent()
:	Super(),
	SimulationTimestep(0.003f)
{
	PrimaryComponentTick.bCanEverTick = true;
	SetIsReplicatedByDefault(true);
}

void UTGOR_PhysicsComponent::BeginPlay()
{
	Super::BeginPlay();

	SINGLETON_CHK;
	LastUpdateTimestamp = Singleton->GetGameTimestamp();
}

void UTGOR_PhysicsComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// Only simulate with valid attached pilot
	UTGOR_PilotComponent* RootPilot = GetRootPilot();
	if (IsValid(RootPilot))
	{
		SINGLETON_CHK;
		const FTGOR_Time Timestamp = Singleton->GetGameTimestamp();

		float PendingTime = Timestamp - LastUpdateTimestamp;
		if (SimulationTimestep >= SMALL_NUMBER)
		{
			// Simulate by one tick if not preempted
			while ((Timestamp - LastUpdateTimestamp) >= SimulationTimestep)
			{
				LastUpdateTimestamp += TickPhysics(SimulationTimestep);
			}
		}
	}
}

void UTGOR_PhysicsComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// Replicate to everyone
	DOREPLIFETIME_CONDITION(UTGOR_PhysicsComponent, Capture, COND_None);
}

/*
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
*/

////////////////////////////////////////////////////////////////////////////////////////////////////

void UTGOR_PhysicsComponent::RepNotifyCapture(const FTGOR_MovementCapture& Old)
{
}

const FTGOR_MovementCapture& UTGOR_PhysicsComponent::GetCapture() const
{
	return Capture;
}

UTGOR_PilotComponent* UTGOR_PhysicsComponent::GetRootPilot() const
{
	if (RootCache.IsValid())
	{
		return RootCache.Get();
	}
	RootCache = UTGOR_PilotComponent::FindRootPilot(GetOwner());
	return RootCache.Get();
}

FVector UTGOR_PhysicsComponent::ComputePhysicsUpVector() const
{
	// Stored in local space
	UTGOR_PilotComponent* RootPilot = GetRootPilot();
	if (IsValid(RootPilot))
	{
		const FTGOR_MovementPosition Position = RootPilot->ComputeBase();
		return Position.Angular * Capture.UpVector;
	}
	return Capture.UpVector;
}


float UTGOR_PhysicsComponent::GetBouyancyRatio(const FTGOR_PhysicsProperties& Surroundings) const
{
	UTGOR_PilotComponent* RootPilot = GetRootPilot();
	if (IsValid(RootPilot))
	{
		const FTGOR_MovementBody& MovementBody = RootPilot->GetBody();
		if (MovementBody.Mass >= SMALL_NUMBER)
		{
			const float Displaced = MovementBody.GetDisplacedMass(Surroundings);
			return Displaced / MovementBody.Mass;
		}
	}
	return 0.0f;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

float UTGOR_PhysicsComponent::TickPhysics(float Time)
{
	UTGOR_PilotComponent* RootPilot = GetRootPilot();
	if (IsValid(RootPilot))
	{
		FVector UpVector = ComputePhysicsUpVector();

		FTGOR_MovementSpace Space = RootPilot->ComputeSpace();
		const FTGOR_PhysicsProperties Surroundings = RootPilot->SurroundingVolume->ComputeAllSurroundings(Space.Linear);

		const FTGOR_MovementBody& MovementBody = RootPilot->GetBody();
		const FVector Orientation = (Surroundings.Gravity * MovementBody.Mass) + MovementBody.GetMassedLinear(Space.RelativeLinearAcceleration);

		// Store upvector in local space
		const FTGOR_MovementPosition Position = RootPilot->ComputeBase();
		Capture.UpVector = Position.Angular.Inverse() * UTGOR_Math::Normalize(FMath::VInterpConstantTo(UpVector, -Orientation, Time, OrientationSpeed));
	}
	return Time;
}
