// The Gateway of Realities: Planes of Existence.


#include "TGOR_WalkingMovement.h"

#include "Realities/Actors/Pawns/TGOR_Avatar.h"
#include "Realities/Mod/Animations/TGOR_Animation.h"
#include "Realities/Utility/Structs/TGOR_Direction.h"
#include "Realities/Components/Movement/TGOR_MovementComponent.h"
#include "DrawDebugHelpers.h"

UTGOR_WalkingMovement::UTGOR_WalkingMovement()
	: Super()
{
	MaximumSpeed = 1'200.0f;
	TurnTorque = 1'000.0f;

	LockMovementWithTurning = 4.0f;
	TorqueSpeedSlowdown = 0.4f;
}

float UTGOR_WalkingMovement::GetInputForce(UTGOR_MovementComponent* Component, const FTGOR_MovementTick& Tick, const FTGOR_MovementSpace& Space, const FTGOR_MovementExternal& External, const FTGOR_MovementContact& Contact, const FTGOR_MovementRepel& Repel, float GroundRatio, FTGOR_MovementOutput& Out) const
{
	// Parent implements damping
	Super::GetInputForce(Component, Tick, Space, External, Contact, Repel, GroundRatio, Out);

	const FTGOR_MovementCapture& Capture = Component->GetCapture();
	const FTGOR_MovementFrame& Frame = Component->GetFrame();
	const FTGOR_MovementBody& Body = Component->GetBody();

	// Only worry about velocity along Frame
	const float Speed = Contact.FrameVelocity.Size(); // (Contact.FrameVelocity - External.UpVector * (Contact.FrameVelocity | External.UpVector)).Size(); //
	
	const FTGOR_Direction Input = Contact.FrameInput;
	const float FinalMaximumSpeed = MaximumSpeed * Frame.Agility;
	if (FinalMaximumSpeed >= SMALL_NUMBER)
	{
		// Rotate towards input direction
		const float TotalSpeedRatio = Speed / FinalMaximumSpeed;
		const FVector Direction = Repel.Project(Input.Direction, KINDA_SMALL_NUMBER);

		const float TorqueSpeedRatio = Component->ComputeTorqueSpeedRatio(TotalSpeedRatio, TorqueSpeedSlowdown);
		const FVector Alignment = Component->ComputeTorqueAlong(External.UpVector, Contact.FrameForward, Direction);
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
			// Get input vector transformed to Frame, move forwards if aligned, directly move towards input if not
			const float ForwardRatio = Contact.FrameForward | Input.Direction;
			const float StrengthRatio = Component->ComputeDirectionRatio(ForwardRatio, SpeedRatio, LockMovementWithTurning);
			const float Direct = StrengthRatio * Input.Magnitude * MaximumLegStrength;
			Out.Force += Component->ComputeForceTowards(Contact.FrameForward, Out.Force, Direct, SpeedRatio);

			// Apply centripetal forces for nice curves
			Out.Force += Body.GetMassedLinear(Contact.FrameAngular ^ Contact.FrameVelocity);
		}
		return SpeedRatio;
	}
	return 0.0f;
}