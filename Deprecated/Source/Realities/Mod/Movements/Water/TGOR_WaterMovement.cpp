// The Gateway of Realities: Planes of Existence.

#include "TGOR_WaterMovement.h"

#include "Realities/Mod/Movements/Ground/TGOR_GroundMovement.h"
#include "Realities/Mod/Animations/TGOR_Animation.h"
#include "Realities/Utility/Structs/TGOR_Direction.h"
#include "Realities/Components/Movement/TGOR_MovementComponent.h"

UTGOR_WaterMovement::UTGOR_WaterMovement()
	: Super()
{
	WaterControl = 80'000.0f;
	UprightRotation = 60.0f;
	AngularDamping = 300.0f;

	WallRepelStrength = 100'000.0f;
	BouyancyThreshold = 0.90f;

	InsertionClasses.Add(UTGOR_GroundMovement::StaticClass());
}


void UTGOR_WaterMovement::Tick(UTGOR_MovementComponent* Component, const FTGOR_MovementTick& Tick, const FTGOR_MovementSpace& Space, const FTGOR_MovementExternal& External, bool Replay, FTGOR_MovementOutput& Out)
{
	const FTGOR_MovementContent& Content = Component->GetContent();
	const FTGOR_MovementFrame& Frame = Component->GetFrame();
	const FTGOR_MovementBody& Body = Component->GetBody();
	
	// Compute input dependent forces
	const float SpeedRatio = GetInputForce(Component, Tick, Space, External, Out);

	// Counteract external forces and rotate upside
	Out.Force -= External.Force.GetClampedToMaxSize(WaterControl * Frame.Strength);

	FHitResult Hit;
	FVector RepelForce = FVector::ZeroVector;
	//if (Component->Trace(FVector::ZeroVector, FVector::ZeroVector, 0.0f, 100.0f, Hit))
	if (Component->Overlap(Space.Linear, Space.Angular * FQuat(FVector::RightVector, PI / 2), 75.0f, 200.0f, Hit))
	{
		const float RepelSpring = WallRepelStrength * Frame.Strength;
		RepelForce += Hit.Normal * Hit.PenetrationDepth * RepelSpring;

		RepelForce -= Space.RelativeLinearVelocity * FMath::Sqrt(RepelSpring * Body.Mass);
		const float RepelStrength = RepelForce | Hit.Normal;
		RepelForce = Hit.Normal * FMath::Max(0.0f, RepelStrength);
	}

	Out.Force += RepelForce;

	// Correctional torque against external torque
	Out.Torque -= External.Torque;

	if (!Replay)
	{
		// Call on animation
		if (IsValid(Content.Animation))
		{
			SwimmingAnimationTick(Component, Space);
		}
	}
}

bool UTGOR_WaterMovement::CanSwimInCurrentSurroundings(const UTGOR_MovementComponent* Component) const
{
	return Component->GetBouyancyRatio() > BouyancyThreshold;
}


float UTGOR_WaterMovement::GetInputForce(UTGOR_MovementComponent* Component, const FTGOR_MovementTick& Tick, const FTGOR_MovementSpace& Space, const FTGOR_MovementExternal& External, FTGOR_MovementOutput& Out) const
{
	// Dampen to prevent oscillation
	Component->GetAngularDamping(Tick, Space.RelativeAngularVelocity, AngularDamping, Out);
	return 0.0f;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

bool UTGOR_WaterMovement::Invariant(const UTGOR_MovementComponent* Component, const FTGOR_MovementTick& Tick, const FTGOR_MovementSpace& Space, const FTGOR_MovementExternal& External) const
{
	return CanSwimInCurrentSurroundings(Component);
}
