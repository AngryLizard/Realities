// The Gateway of Realities: Planes of Existence.


#include "TGOR_HoverMovement.h"

#include "Realities/Actors/Pawns/TGOR_Avatar.h"
#include "Realities/Mod/Animations/TGOR_Animation.h"
#include "Realities/Utility/Structs/TGOR_Direction.h"
#include "Realities/Components/Movement/TGOR_MovementComponent.h"

UTGOR_HoverMovement::UTGOR_HoverMovement()
	: Super()
{
	MaximumVelocity = 1'200.0f;
	MaximumAngular = 35'000.0f;
	
}

float UTGOR_HoverMovement::GetStretch(UTGOR_MovementComponent* Component, const FTGOR_MovementTick& Tick, const FTGOR_MovementSpace& Space, const FTGOR_MovementContact& Contact, float GroundRatio) const
{
	const FTGOR_MovementCapture& Capture = Component->GetCapture();
	const FTGOR_MovementState& State = Component->GetState();
	return(State.Input | Capture.UpVector);
}

float UTGOR_HoverMovement::GetInputForce(UTGOR_MovementComponent* Component, const FTGOR_MovementTick& Tick, const FTGOR_MovementSpace& Space, const FTGOR_MovementContact& Contact, const FTGOR_MovementRepel& Repel, float GroundRatio, FTGOR_MovementOutput& Out) const
{
	const float LegLength = Component->Avatar->GetLegLength();
	check(LegLength >= SMALL_NUMBER && "LegLength must be bigger than 0.0");

	const FTGOR_MovementCapture& Capture = Component->GetCapture();

	// Only worry about velocity along Frame
	const float UpVelocity = Space.RelativeVelocity | Contact.FrameNormal;
	const FVector PlaneVelocity = Space.RelativeVelocity - UpVelocity * Contact.FrameNormal;
	const float Speed = PlaneVelocity.Size();
	
	// Filter sideways input
	Out.Force = -PlaneVelocity * BrakeCoefficient;
	const FTGOR_Direction FrameInput = Contact.FrameInput;

	// Max velocity depends on current leg length (solved for f(x) = mx + c for f(1)=1 and f(1.0f - Crouch) = SpeedRatio)
	const float CrouchRatio = (1.0f - CrouchSpeedMultiplier) / CrouchMultiplier;
	const float InputRatio = FMath::Min(FrameInput.Magnitude, 1.0f);
	const float LegRatio = CrouchRatio * (Contact.GroundDistance / LegLength - 1.0f) + 1.0f;
	const float FinalMaxVelocity = MaximumVelocity * FMath::Min(LegRatio, 1.0f) * InputRatio;
	
	if (FinalMaxVelocity >= SMALL_NUMBER)
	{
		const float SpeedRatio = FMath::Min(Speed / FinalMaxVelocity, 1.0f);

		// Compute correction vector against current movement direction along ground
		if (FrameInput)
		{
			// Turn off input force if too fast, counteract braking in movement direction if moving
			const float UnderSpeedRatio = FMath::Clamp(1.0f - (Speed - FinalMaxVelocity) / FinalMaxVelocity, 0.0f, 1.0f);

			const FVector Direct = FrameInput.Direction * FrameInput.Magnitude;
			const FVector AntiBrake = FrameInput.Direction * (Out.Force | FrameInput.Direction) * UnderSpeedRatio;
			Out.Force += (Direct * MaximumLegStrength - AntiBrake) * UnderSpeedRatio;
			
			// Rotate towards input direction
			const float ForwardRatio = Contact.FrameForward | FrameInput.Direction;
			const FVector Direction = Repel.Project(FrameInput.Direction);
			const FVector Axis = Contact.FrameForward ^ Direction;
			if (ForwardRatio >= 0.0f)
			{
				const float Torque = (Capture.UpVector | (Axis * MaximumAngular));
				Out.Torque += Capture.UpVector * Torque;
			}
			else
			{
				// Rotate full speed if moving counter to forward vector
				const float Sign = (Axis.Z >= 0.0f ? 1.0f : -1.0f);
				Out.Torque += Capture.UpVector * Sign * MaximumAngular;
			}
		}

		return(SpeedRatio);
	}

	return(1.0f);
}
