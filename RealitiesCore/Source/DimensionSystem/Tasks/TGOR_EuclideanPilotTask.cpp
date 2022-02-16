// The Gateway of Realities: Planes of Existence.


#include "TGOR_EuclideanPilotTask.h"
#include "DimensionSystem/Components/TGOR_PilotComponent.h"

#include "Net/UnrealNetwork.h"
#include "Engine/ActorChannel.h"

UTGOR_EuclideanPilotTask::UTGOR_EuclideanPilotTask()
{
}

void UTGOR_EuclideanPilotTask::EnsureRelativeVelocityAlong(const FVector& Normal, float Velocity, float Alpha)
{
	FTGOR_MovementSpace Space = ComputeSpace();
	const FVector VelocityDelta = Normal * ((Velocity - (Space.RelativeLinearVelocity | Normal)) * Alpha);
	Space.RelativeLinearVelocity += VelocityDelta;
	Space.LinearVelocity += VelocityDelta;
	SimulateDynamic(Space);
}