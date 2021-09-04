// The Gateway of Realities: Planes of Existence.
#include "TGOR_LinearComponent.h"

#include "DimensionSystem/Data/TGOR_WorldData.h"
#include "DimensionSystem/Volumes/TGOR_PhysicsVolume.h"
#include "DimensionSystem/Tasks/TGOR_EuclideanPilotTask.h"

#include "Components/PrimitiveComponent.h"

UTGOR_LinearComponent::UTGOR_LinearComponent()
:	Super(),
StandupTorque(2.0f),
AngularDamping(1.0f),
ProbeDistance(100.0f)
{
	Friction = 0.5f;
}

void UTGOR_LinearComponent::ComputePhysics(FTGOR_MovementSpace& Space, const FTGOR_MovementExternal& External, const FTGOR_MovementTick& Tick, FTGOR_MovementOutput& Output)
{
	UTGOR_EuclideanPilotTask* PilotTask = GetPOfType<UTGOR_EuclideanPilotTask>();
	if (IsValid(PilotTask))
	{
		// Check for movement parent
		FHitResult Hit;
		const FCollisionShape& CollisionShape = GetCollisionShape();
		if (MovementTraceSweep(CollisionShape, Space, -External.UpVector * ProbeDistance, Hit))
		{
			//Output.Parent = FindReparentToComponent(Hit.Component.Get(), Hit.BoneName);
		}

		// Dampen to prevent oscillation, correct against external torque
		const FVector Correction = External.Torque;
		const FVector CurrentUp = Space.Angular.GetAxisZ();

		// Rotate to stand upright 
		const FVector Swivel = CurrentUp ^ External.UpVector;
		const FVector SwivelTorque = Swivel * StandupTorque;

		Output.Torque += (SwivelTorque - Correction);

		GetDampingForce(Tick, Space.RelativeLinearVelocity, 0.0f, Output);
		GetDampingTorque(Tick, Space.RelativeAngularVelocity, AngularDamping, Output);

		SimulateSymplectic(Space, Output, External, Tick.Deltatime, true);
		PilotTask->SimulateDynamic(Space);
	}
}

bool UTGOR_LinearComponent::CanRotateOnImpact() const
{
	return false;
}
