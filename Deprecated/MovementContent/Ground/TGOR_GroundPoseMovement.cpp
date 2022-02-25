// The Gateway of Realities: Planes of Existence.


#include "TGOR_GroundPoseMovement.h"

#include "RealitiesUtility/Utility/TGOR_Math.h"
#include "RealitiesUtility/Structures/TGOR_Matrix3x3.h"

#include "Realities/MovementSystem/Components/TGOR_MovementComponent.h"
#include "Realities/CreatureSystem/TGOR_Pawn.h"


UTGOR_GroundPoseMovement::UTGOR_GroundPoseMovement()
:	Super(),
	ZAxisSnap(FVector2D(1.0f, 0.35f)),
	RotationTorque(1200.0f),
	SnappingTorque(800.0f),
	SwayTorque(0.0f)
{
	StandupTorque = 0.0f;
	TraceUpVector = true;
	RestrictsAxis = false;
}

bool UTGOR_GroundPoseMovement::Invariant(const UTGOR_MovementComponent* Component, const FTGOR_MovementTick& Tick, const FTGOR_MovementSpace& Space, const FTGOR_MovementExternal& External) const
{
	if (!Super::Invariant(Component, Tick, Space, External))
	{
		return false;
	}
	const FTGOR_MovementInput& State = Component->GetState();

	FTGOR_MovementGround Ground;
	const float GroundRatio = TraceForGroundRatio(Component, Space, External.UpVector, Ground);
	const bool OnGround = ((GroundRatio < 1.0f || (State.Input | Ground.SurfaceNormal) < SMALL_NUMBER) && Ground.Slope > 0.4f);
	return OnGround;
}


float UTGOR_GroundPoseMovement::GetStretch(UTGOR_MovementComponent* Component, const FTGOR_MovementTick& Tick, const FTGOR_MovementSpace& Space, const FTGOR_MovementExternal& External, const FTGOR_MovementContact& Contact, float GroundRatio) const
{
	const FTGOR_MovementInput& State = Component->GetState();

	const float UpInput = FMath::Max(State.Input | Contact.FrameNormal, 0.0f);
	const float UpRight = FMath::Abs(Space.Angular.GetAxisZ() | Contact.FrameNormal);
	return UpInput * UpRight * 2.0f - 1.0f;
}

float UTGOR_GroundPoseMovement::GetInputForce(UTGOR_MovementComponent* Component, const FTGOR_MovementTick& Tick, const FTGOR_MovementSpace& Space, const FTGOR_MovementExternal& External, const FTGOR_MovementContact& Contact, const FTGOR_MovementRepel& Repel, float GroundRatio, FTGOR_MovementOutput& Out) const
{
	const FTGOR_MovementInput& State = Component->GetState();
	const FTGOR_MovementFrame& Frame = Component->GetFrame();

	// Parent implements damping
	Super::GetInputForce(Component, Tick, Space, External, Contact, Repel, GroundRatio, Out);

	// Filter sideways input
	Out.Force = -Contact.FrameVelocity * BrakeCoefficient;
	
	const FVector Normal = Contact.FrameNormal;
	const FTGOR_Matrix3x3 Tensor = FTGOR_Matrix3x3(Space.Angular);

	// Find motor axis that fits most to desired rotation axis
	const FVector local = UTGOR_Math::Normalize(State.Input) * 0.5f;
	const FVector Motor = Normal ^ local;
	const FVector Axis = Tensor * Motor;

	if (RestrictsAxis)
	{
		const FVector Input = Tensor * Input;
		const FVector Mask = Axis.GetAbs();
		if (Mask.X > Mask.Y && Mask.X > Mask.Z)
		{
			Out.Torque += Tensor.X * Axis.X * RotationTorque * Frame.Strength;
			Out.Torque -= Normal * (FVector2D(Axis.Y, Axis.Z).Size() * Input.X * SwayTorque);
		}
		else if (Mask.Y > Mask.X && Mask.Y > Mask.Z)
		{
			Out.Torque += Tensor.Y * Axis.Y * RotationTorque * Frame.Strength;
			Out.Torque -= Normal * (FVector2D(Axis.X, Axis.Z).Size() * Input.Y * SwayTorque);
		}
		else if (Mask.Z > Mask.X && Mask.Z > Mask.Y)
		{
			Out.Torque += Tensor.Z * Axis.Z * RotationTorque * Frame.Strength;
			Out.Torque -= Normal * (FVector2D(Axis.X, Axis.Y).Size() * Input.Z * SwayTorque);
		}
	}
	else
	{
		Out.Torque += Tensor.X * Axis.X * RotationTorque * Frame.Strength;
		Out.Torque += Tensor.Y * Axis.Y * RotationTorque * Frame.Strength;
		Out.Torque += Tensor.Z * Axis.Z * RotationTorque * Frame.Strength;
	}

	// Rotate upright on up input
	const float UpInput = Normal | local;
	const FVector Upright = Space.Angular.GetAxisZ() ^ Normal;
	Out.Torque += Upright * UpInput * RotationTorque * Frame.Strength;


	// Scale axis depending on strength
	const FVector Rotated = Tensor * Normal;
	const float SnapScaling = (Rotated.Z >= 0.0f) ? ZAxisSnap.X : ZAxisSnap.Y;
	const FVector Match = UTGOR_Math::Normalize(FVector(Rotated.X, Rotated.Y, Rotated.Z * SnapScaling));
	const FVector Cubic = Match * Match * Match;

	// Rectify non-aligned axis
	Out.Torque -= (Normal ^ Tensor.X) * Cubic.X * SnappingTorque * Frame.Strength;
	Out.Torque -= (Normal ^ Tensor.Y) * Cubic.Y * SnappingTorque * Frame.Strength;
	Out.Torque -= (Normal ^ Tensor.Z) * Cubic.Z * SnappingTorque * Frame.Strength;

	return 0.0f;
}
