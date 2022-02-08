// The Gateway of Realities: Planes of Existence.


#include "TGOR_PilotSimulationInterface.h"

#include "DimensionSystem/Components/TGOR_PilotComponent.h"

ITGOR_PilotSimulationInterface::ITGOR_PilotSimulationInterface()
	: PendingTime(0.0f)
{
}

void ITGOR_PilotSimulationInterface::TickStratified(float Time, float Timestep, float MinTickTime)
{
	PendingTime += Time;

	// Always tick at least once
	if (PendingTime < MinTickTime)
	{
		TickPhysics(0.0f);
		return;
	}

	// Use up total pending time
	while (PendingTime >= Timestep)
	{
		PendingTime -= FMath::Max(TickPhysics(Timestep), MinTickTime);
	}
}

FVector ITGOR_PilotSimulationInterface::ComputePhysicsUpVector() const
{
	return FVector::UpVector;
}

UTGOR_PilotComponent* ITGOR_PilotSimulationInterface::GetRootPilot() const
{
	if (RootCache.IsValid())
	{
		return RootCache.Get();
	}
	RootCache = UTGOR_PilotComponent::FindRootPilot(GetPilotOwner());
	return RootCache.Get();
}

AActor* ITGOR_PilotSimulationInterface::GetPilotOwner() const
{
	return nullptr;
}

float ITGOR_PilotSimulationInterface::TickPhysics(float Time)
{
	return Time;
}
