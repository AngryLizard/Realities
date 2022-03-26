// The Gateway of Realities: Planes of Existence.


#include "TGOR_WalkingTask.h"
#include "DimensionSystem/Components/TGOR_PilotComponent.h"
#include "MovementSystem/Components/TGOR_MovementComponent.h"
#include "MovementSystem/Content/TGOR_Movement.h"

#include "RealitiesUtility/Utility/TGOR_Math.h"
#include "RealitiesUtility/Structures/TGOR_Direction.h"

#include "Net/UnrealNetwork.h"
#include "Engine/ActorChannel.h"

UTGOR_WalkingTask::UTGOR_WalkingTask()
	: Super()
{
}

void UTGOR_WalkingTask::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

}

float UTGOR_WalkingTask::GetMaxSpeed() const
{
	return MaximumSpeed;
}

void UTGOR_WalkingTask::Initialise()
{
	Super::Initialise();

	//IPC.Generate(Pendulum, Riccati);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

float UTGOR_WalkingTask::GetInputForce(const FTGOR_MovementTick& Tick, const FTGOR_MovementSpace& Space, const FTGOR_MovementExternal& External, const FTGOR_MovementRepel& Repel, FTGOR_MovementOutput& Out) const
{
	// Parent implements damping
	Super::GetInputForce(Tick, Space, External, Repel, Out);

	const FTGOR_MovementFrame& Frame = Identifier.Component->GetFrame();
	const FTGOR_MovementBody& Body = RootComponent->GetBody();

	// Only worry about velocity along Frame
	const float Speed = MovementContact.FrameVelocity.Size(); // (MovementContact.FrameVelocity - External.UpVector * (MovementContact.FrameVelocity | External.UpVector)).Size(); //

	const FTGOR_Direction Input = MovementContact.FrameInput;
	const float FinalMaximumSpeed = MaximumSpeed * GetSpeedRatio();
	if (FinalMaximumSpeed >= SMALL_NUMBER)
	{
		// Rotate towards input direction
		const float TotalSpeedRatio = Speed / FinalMaximumSpeed;
		if (Input.Magnitude > 0.1f)
		{
			const FVector Direction = Repel.Project(Input.Direction, KINDA_SMALL_NUMBER);
			
			const float TorqueSpeedRatio = Identifier.Component->ComputeTorqueSpeedRatio(TotalSpeedRatio, TorqueSpeedSlowdown);
			const FVector Alignment = Identifier.Component->ComputeTorqueAlong(External.UpVector, MovementContact.FrameForward, Direction);
			Out.Torque += Alignment * (TorqueSpeedRatio * TurnTorque * Frame.Strength);
		}
	}

	// Filter sideways input
	const float LegRatio = ComputeCrouchSpeedRatio(MovementContact.GroundRatio);
	const float InputRatio = FMath::Min(Input.Magnitude, 1.0f);
	const float FinalMaxVelocity = FinalMaximumSpeed * LegRatio * InputRatio;
	if (FinalMaxVelocity >= SMALL_NUMBER)
	{
		const float SpeedRatio = Speed / FinalMaxVelocity;
		if (Input.Magnitude > 0.1f)
		{
			// Get input vector transformed to Frame, move forwards if aligned, directly move towards input if not
			const float ForwardRatio = MovementContact.FrameForward | Input.Direction;
			const float StrengthRatio = Identifier.Component->ComputeDirectionRatio(ForwardRatio, SpeedRatio, LockMovementWithTurning);
			const float Direct = StrengthRatio * Input.Magnitude * MaximumLegStrength;

			const FVector Direction = FMath::Lerp(MovementContact.FrameForward, Input.Direction, DirectMovement).GetSafeNormal();
			Out.Force += Identifier.Component->ComputeForceTowards(Direction, Out.Force, Direct, SpeedRatio);

			// Apply centripetal forces for nice curves
			Out.Force += Body.GetMassedLinear(MovementContact.FrameAngular ^ MovementContact.FrameVelocity);
		}
		return Speed / FinalMaximumSpeed;
	}
	return 0.0f;
}
