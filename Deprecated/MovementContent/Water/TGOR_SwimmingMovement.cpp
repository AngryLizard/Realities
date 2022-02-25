// The Gateway of Realities: Planes of Existence.

#include "TGOR_SwimmingMovement.h"

#include "RealitiesUtility/Structures/TGOR_Direction.h"
#include "Realities/MovementSystem/Components/TGOR_MovementComponent.h"

UTGOR_SwimmingMovement::UTGOR_SwimmingMovement()
	: Super()
{
	MaximumSpeed = 900.0f;
	SwimStrength = 100'000.0f;
	TurnTorque = 1'000.0f;
	SwivelRotation = 2'200.0f;


	LockMovementWithTurning = 4.0f;
	TorqueSpeedSlowdown = 0.3f;

	SurfaceThreshold = 50.0f;


}

void UTGOR_SwimmingMovement::QueryInputVector(const UTGOR_MovementComponent* Component, FVector& OutInput, FVector& OutView) const
{
	const FVector UpVector = Component->ComputePhysicsUpVector();
	const FQuat InputRotation = Component->GetInputRotation();
	const float InputStrength = Component->GetInputStrength();
	const FVector RawInput = Component->GetRawInput();
	OutView = InputRotation.GetAxisX();

	// Separate input into plane and up vector
	const float UpInput = RawInput.Z;
	const FVector2D Plane = FVector2D(RawInput);
	const FVector SideInput = InputRotation * FVector(Plane, 0.0f);

	// Combine back, normalise and scale appropriately
	const FVector UnscaledInput = SideInput + UpVector * UpInput;
	const float Size = UnscaledInput.Size();
	if (Size >= SMALL_NUMBER)
	{
		const float Strength = FMath::Min(1.0f, RawInput.Size());
		OutInput = (UnscaledInput / Size) * (Strength * InputStrength);
		return;
	}
	OutInput = FVector::ZeroVector;
}

float UTGOR_SwimmingMovement::GetInputForce(UTGOR_MovementComponent* Component, const FTGOR_MovementTick& Tick, const FTGOR_MovementSpace& Space, const FTGOR_MovementExternal& External, FTGOR_MovementOutput& Out) const
{
	// Parent implements damping
	Super::GetInputForce(Component, Tick, Space, External, Out);

	const FTGOR_MovementCapture& Capture = Component->GetCapture();
	const FTGOR_MovementInput& State = Component->GetState();
	const FTGOR_MovementFrame& Frame = Component->GetFrame();
	const FTGOR_MovementBody& Body = Component->GetBody();

	// Compute rotation towards upright rotation
	const FVector Forward = Space.Angular.Vector();
	const FVector Upward = Space.Angular.GetAxisZ();
	const FVector UprightAxis = Upward ^ External.UpVector;

	// Only worry about velocity along surface
	const float Speed = Space.RelativeLinearVelocity.Size();
	const FTGOR_Direction Input = State.Input;
	if (Input && SurfaceThreshold >= SMALL_NUMBER)
	{
		// Compute forward direction with reduction towards volume surface
		const float UpPart = Input.Direction | External.UpVector;
		const FVector SidePart = Input.Direction - External.UpVector * UpPart;
		const float DepthRatio = (Capture.Surroundings.Depth - SurfaceThreshold) / SurfaceThreshold;
		const FVector FinalInput = SidePart + External.UpVector * FMath::Min(UpPart, FMath::Max(UpPart * DepthRatio, 0.0f));
		const float FinalMaximumSpeed = MaximumSpeed * GetSpeedRatio(Component);
		if (FinalMaximumSpeed >= SMALL_NUMBER)
		{
			// Rotate towards input direction
			const float TotalSpeedRatio = Speed / FinalMaximumSpeed;
			const float TorqueSpeedRatio = Component->ComputeTorqueSpeedRatio(TotalSpeedRatio, TorqueSpeedSlowdown);
			const FVector Alignment = Component->ComputeTorqueTowards(Forward, FinalInput);
			Out.Torque += Alignment * (TorqueSpeedRatio * TurnTorque * Frame.Strength);
		}

		// Compute desired velocity
		const float InputRatio = FMath::Min(Input.Magnitude, 1.0f);
		const float FinalMaxVelocity = FinalMaximumSpeed * InputRatio;
		if (FinalMaxVelocity >= SMALL_NUMBER)
		{
			const float SpeedRatio = Speed / FinalMaxVelocity;
			if (Input.Magnitude > 0.1f)
			{
				// Get input vector transformed to surface, move forwards if aligned, directly move towards input if not
				const float ForwardRatio = Forward | Input.Direction;
				const float StrengthRatio = Component->ComputeDirectionRatio(ForwardRatio, SpeedRatio, LockMovementWithTurning);
				const float Direct = StrengthRatio * Input.Magnitude * Capture.Surroundings.Density * SwimStrength * Frame.Strength;
				Out.Force += Component->ComputeForceTowards(Forward, Out.Force, Direct, SpeedRatio);

				// Apply centripetal forces for nice curves
				Out.Force += Body.GetMassedLinear(Space.RelativeAngularVelocity ^ Space.RelativeLinearVelocity);
			}

			// Rotate around input axis to swim upright
			const FVector Swivel = FinalInput * (UprightAxis | FinalInput);
			Out.Torque += Swivel * SwivelRotation * Frame.Strength;

			return SpeedRatio;
		}
	}
	else
	{
		// Rotate around forward axis to swim upright
		const FVector Swivel = Forward * (UprightAxis | Forward);
		Out.Torque += Swivel * SwivelRotation * Frame.Strength;
	}
	return 0.0f;
}