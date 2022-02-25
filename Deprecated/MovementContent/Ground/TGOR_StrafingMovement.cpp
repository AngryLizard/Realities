// The Gateway of Realities: Planes of Existence.


#include "TGOR_StrafingMovement.h"

#include "RealitiesUtility/Utility/TGOR_Math.h"

#include "Realities/CreatureSystem/TGOR_Avatar.h"
#include "RealitiesUtility/Structures/TGOR_Direction.h"
#include "Realities/MovementSystem/Components/TGOR_MovementComponent.h"

UTGOR_StrafingMovement::UTGOR_StrafingMovement()
	: Super()
{	
	MaximumSpeedForward = 600.0f;
	MaximumSpeedSideways = 300.0f;
	MaximumSpeedBackward = 300.0f;

	TurnTorque = 1000.0f;

	CrouchSpeedMultiplier = 1.0f;
	InputMultiplier = 2.0f;

	LockMovementWithTurning = 4.0f;
	TorqueSpeedSlowdown = 0.4f;
}

bool UTGOR_StrafingMovement::Invariant(const UTGOR_MovementComponent* Component, const FTGOR_MovementTick& Tick, const FTGOR_MovementSpace& Space, const FTGOR_MovementExternal& External) const
{
	if (GetAttribute(Component, InvariantAttribute, 0.0f) < 0.5f)
	{
		return false;
	}
	return Super::Invariant(Component, Tick, Space, External);
}


float UTGOR_StrafingMovement::GetInputForce(UTGOR_MovementComponent* Component, const FTGOR_MovementTick& Tick, const FTGOR_MovementSpace& Space, const FTGOR_MovementExternal& External, const FTGOR_MovementContact& Contact, const FTGOR_MovementRepel& Repel, float GroundRatio, FTGOR_MovementOutput& Out) const
{
	// Parent implements damping
	Super::GetInputForce(Component, Tick, Space, External, Contact, Repel, GroundRatio, Out);

	const FTGOR_MovementFrame& Frame = Component->GetFrame();
	const FTGOR_MovementBody& Body = Component->GetBody();

	// Grab parameters
	const float LegLength = GetLegLength(Component);

	// Only worry about velocity along Frame
	const float Speed = Contact.FrameVelocity.Size();
	const FTGOR_Direction Input = Contact.FrameInput;

	FVector View;
	const float MaximumSpeed = ComputeMaxSpeed(Component, Contact, View);
	const float FinalMaximumSpeed = MaximumSpeed * GetSpeedRatio(Component);
	if (FinalMaximumSpeed >= SMALL_NUMBER)
	{
		// Rotate towards input direction
		const float TotalSpeedRatio = Speed / FinalMaximumSpeed;
		const float TorqueSpeedRatio = Component->ComputeTorqueSpeedRatio(TotalSpeedRatio, TorqueSpeedSlowdown);
		const FVector Alignment = Component->ComputeTorqueAlong(External.UpVector, Contact.FrameForward, View);
		Out.Torque += Alignment * (TorqueSpeedRatio * TurnTorque * Frame.Strength);
	}


	// Filter sideways input
	const float LegRatio = ComputeCrouchSpeedRatio(GroundRatio);
	const float InputRatio = FMath::Min(Input.Magnitude, 1.0f);
	const float FinalMaxVelocity = FinalMaximumSpeed * LegRatio * InputRatio;
	if (FinalMaxVelocity >= SMALL_NUMBER)
	{
		const float SpeedRatio = Speed / FinalMaxVelocity;
		if (Input.Magnitude > 0.1f)
		{

			// Compute movement direction
			const float Forward = Input.Direction | View;
			const float Right = Input.Direction | (Contact.FrameNormal ^ View);
			const FVector Direction = Forward * Contact.FrameForward + Right * Contact.FrameRight;

			// Get input vector transformed to Frame, move forwards if aligned, directly move towards input if not
			const float ForwardRatio = Contact.FrameForward | View;
			const float StrengthRatio = Component->ComputeDirectionRatio(ForwardRatio, SpeedRatio, LockMovementWithTurning);
			const float Direct = StrengthRatio * Input.Magnitude * MaximumLegStrength;
			Out.Force += Component->ComputeForceTowards(Direction, Out.Force, Direct, SpeedRatio);

			// Apply centripetal forces for nice curves
			Out.Force += Body.GetMassedLinear(Contact.FrameAngular ^ Contact.FrameVelocity);
		}
		return SpeedRatio;
	}
	return 0.0f;
}


float UTGOR_StrafingMovement::ComputeMaxSpeed(UTGOR_MovementComponent* Component, const FTGOR_MovementContact& Contact, FVector& View) const
{
	const FTGOR_MovementInput& State = Component->GetState();
	View = UTGOR_Math::Normalize(State.View - (State.View | Contact.FrameNormal) * Contact.FrameNormal);

	// Compute max speed in current input direction
	const float InputSize = Contact.FrameInput.Size();
	if (InputSize >= SMALL_NUMBER)
	{
		// Compute ratio in either direction
		const float ForwardRatio = (Contact.FrameInput | View) / InputSize;
		const float SquaredRatio = ForwardRatio * ForwardRatio;

		// Compute appropriate maximum speed
		const float MaximumSpeed = ((ForwardRatio >= 0.0f) ? MaximumSpeedForward : MaximumSpeedBackward);
		return MaximumSpeed * SquaredRatio + MaximumSpeedSideways * (1.0f - SquaredRatio);
	}
	return MaximumSpeedBackward;
}
