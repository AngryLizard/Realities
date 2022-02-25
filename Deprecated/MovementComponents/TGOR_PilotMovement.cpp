// The Gateway of Realities: Planes of Existence.


#include "TGOR_PilotMovement.h"

#include "Realities/Components/Movement/TGOR_MovementComponent.h"
#include "Realities/Mod/Animations/TGOR_Animation.h"

#include "Realities/Actors/Pawns/TGOR_Pawn.h"
#include "DrawDebugHelpers.h"


UTGOR_PilotMovement::UTGOR_PilotMovement()
:	Super(),
LinearDamping(1200.0f),
AngularDamping(150.0f),
AttachForce(800.0f),
AttachTorque(800.0f)
{
	Mode = ETGOR_MovementEnumeration::Override;
}

bool UTGOR_PilotMovement::Invariant(const UTGOR_MovementComponent* Component, const FTGOR_MovementTick& Tick, const FTGOR_MovementSpace& Space) const
{
	if (!Super::Invariant(Component, Tick, Space))
	{
		return false;
	}
	return true;
}

TSubclassOf<UTGOR_Animation> UTGOR_PilotMovement::QueryAnimation(const UTGOR_MovementComponent* Component, const FTGOR_MovementTick& Tick, const FTGOR_MovementSpace& Space) const
{
	const FTGOR_MovementBase& Base = Component->GetBase();
	if (Base.Parent.IsValid() && *Base.Parent->AttachAnimation)
	{
		return Base.Parent->AttachAnimation;
	}
	return Super::QueryAnimation(Component, Tick, Space);
}

void UTGOR_PilotMovement::Tick(UTGOR_MovementComponent* Component, const FTGOR_MovementTick& Tick, const FTGOR_MovementSpace& Space, bool Replay, FTGOR_MovementOutput& Out)
{
	const FTGOR_MovementBase& Base = Component->GetBase();
	if (Base.Parent.IsValid())
	{
		Out.CollisionEnabled = Base.Parent->AttachEnablesCollision;

		// Counteract external forces
		Out.Force -= Space.ExternalForce;
		Out.Torque -= Space.ExternalTorque;
		
		// Filter sideways input
		Component->GetLinearDamping(Tick, Space.RelativeVelocity, LinearDamping, Out);
		Component->GetAngularDamping(Tick, Space.RelativeAngular, AngularDamping, Out);

		const FTGOR_MovementBase& ParentBase = Base.Parent->GetBase();
		Out.Force += (ParentBase.Location - Base.Location) * AttachForce;

		// Rotate towards attachment direction
		Out.Torque += Component->ComputeTorqueTowards(Base.Rotation.GetAxisX(), ParentBase.Rotation.GetAxisX()) * AttachTorque;
		Out.Torque += Component->ComputeTorqueTowards(Base.Rotation.GetAxisZ(), ParentBase.Rotation.GetAxisZ()) * AttachTorque;

		PilotAnimationTick(Component, Space, true);
	}
	else
	{
		PilotAnimationTick(Component, Space, false);
	}
}
