// The Gateway of Realities: Planes of Existence.


#include "TGOR_SwimmingTask.h"
#include "PhysicsSystem/Components/TGOR_RigidComponent.h"
#include "MovementSystem/Components/TGOR_MovementComponent.h"
#include "MovementSystem/Content/TGOR_Movement.h"

#include "RealitiesUtility/Utility/TGOR_Math.h"
#include "RealitiesUtility/Structures/TGOR_Direction.h"

#include "Net/UnrealNetwork.h"
#include "Engine/ActorChannel.h"

UTGOR_SwimmingTask::UTGOR_SwimmingTask()
	: Super()
{
}

void UTGOR_SwimmingTask::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

}


void UTGOR_SwimmingTask::QueryInput(FVector& OutInput, FVector& OutView) const
{
	const FVector UpVector = RigidComponent->ComputePhysicsUpVector();
	const FQuat InputRotation = Identifier.Component->GetInputRotation();
	const float InputStrength = Identifier.Component->GetInputStrength();
	const FVector RawInput = Identifier.Component->GetRawInput();
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

float UTGOR_SwimmingTask::GetInputForce(const FTGOR_MovementTick& Tick, const FTGOR_MovementSpace& Space, const FTGOR_MovementExternal& External, FTGOR_MovementOutput& Out) const
{
	// Parent implements damping
	Super::GetInputForce(Tick, Space, External, Out);

	const FTGOR_MovementCapture& Capture = RigidComponent->GetCapture();
	const FTGOR_MovementInput& State = Identifier.Component->GetState();
	const FTGOR_MovementFrame& Frame = Identifier.Component->GetFrame();
	const FTGOR_MovementBody& Body = RigidComponent->GetBody();

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
		const float FinalMaximumSpeed = MaximumSpeed * GetSpeedRatio();
		if (FinalMaximumSpeed >= SMALL_NUMBER)
		{
			// Rotate towards input direction
			const float TotalSpeedRatio = Speed / FinalMaximumSpeed;
			const float TorqueSpeedRatio = Identifier.Component->ComputeTorqueSpeedRatio(TotalSpeedRatio, TorqueSpeedSlowdown);
			const FVector Alignment = Identifier.Component->ComputeTorqueTowards(Forward, FinalInput);
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
				const float StrengthRatio = Identifier.Component->ComputeDirectionRatio(ForwardRatio, SpeedRatio, LockMovementWithTurning);
				const float Direct = StrengthRatio * Input.Magnitude * Capture.Surroundings.Density * SwimStrength * Frame.Strength;
				Out.Force += Identifier.Component->ComputeForceTowards(Forward, Out.Force, Direct, SpeedRatio);

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