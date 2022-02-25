// The Gateway of Realities: Planes of Existence.
#include "TGOR_LinearComponent.h"

#include "Realities/Base/TGOR_Singleton.h"
#include "Realities/Base/System/Data/TGOR_WorldData.h"
#include "Realities/Actors/Dimension/Volumes/TGOR_PhysicsVolume.h"

#include "Components/PrimitiveComponent.h"
#include "Net/UnrealNetwork.h"

UTGOR_LinearComponent::UTGOR_LinearComponent()
:	Super(),
StandupTorque(2.0f),
AngularDamping(1.0f),
ProbeDistance(100.0f)
{
	Friction = 0.5f;
}

void UTGOR_LinearComponent::ComputePhysics(const FTGOR_MovementTick& Tick, const FTGOR_MovementSpace& Space, const FTGOR_MovementExternal& External, bool Replay, FTGOR_MovementOutput& Output)
{
	// Check for movement parent
	FHitResult Hit;
	const FCollisionShape& CollisionShape = BasePrimitiveComponent->GetCollisionShape();
	if (MovementTraceSweep(CollisionShape, Space, -External.UpVector * ProbeDistance, Hit))
	{
		Output.SetBaseFrom(Hit.Component.Get(), Hit.BoneName);
	}

	// Dampen to prevent oscillation, correct against external torque
	const FVector Correction = External.Torque;
	const FVector CurrentUp = Space.Angular.GetAxisZ();

	// Rotate to stand upright 
	const FVector Swivel = CurrentUp ^ External.UpVector;
	const FVector SwivelTorque = Swivel * StandupTorque;

	Output.Torque += (SwivelTorque - Correction);

	GetLinearDamping(Tick, Space.RelativeLinearVelocity, 0.0f, Output);
	GetAngularDamping(Tick, Space.RelativeAngularVelocity, AngularDamping, Output);
}

bool UTGOR_LinearComponent::CanRotateOnImpact() const
{
	return false;
}
