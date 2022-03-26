// The Gateway of Realities: Planes of Existence.


#include "TGOR_StrafingTask.h"
#include "DimensionSystem/Components/TGOR_PilotComponent.h"
#include "MovementSystem/Components/TGOR_MovementComponent.h"
#include "MovementSystem/Content/TGOR_Movement.h"

#include "RealitiesUtility/Utility/TGOR_Math.h"
#include "RealitiesUtility/Structures/TGOR_Direction.h"

#include "Net/UnrealNetwork.h"
#include "Engine/ActorChannel.h"

UTGOR_StrafingTask::UTGOR_StrafingTask()
	: Super()
{
}

void UTGOR_StrafingTask::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

}

////////////////////////////////////////////////////////////////////////////////////////////////////


bool UTGOR_StrafingTask::Invariant(const FTGOR_MovementSpace& Space, const FTGOR_MovementExternal& External) const
{
	if (GetAttribute(InvariantAttribute, 0.0f) < 0.5f)
	{
		return false;
	}
	return Super::Invariant(Space, External);
}


float UTGOR_StrafingTask::GetInputForce(const FTGOR_MovementTick& Tick, const FTGOR_MovementSpace& Space, const FTGOR_MovementExternal& External, const FTGOR_MovementRepel& Repel, FTGOR_MovementOutput& Out) const
{
	// Parent implements damping
	Super::GetInputForce(Tick, Space, External, Repel, Out);

	const FTGOR_MovementFrame& Frame = Identifier.Component->GetFrame();
	const FTGOR_MovementBody& Body = RootComponent->GetBody();

	// Only worry about velocity along Frame
	const float Speed = MovementContact.FrameVelocity.Size();
	const FTGOR_Direction Input = MovementContact.FrameInput;

	FVector View;
	const float MaximumSpeed = ComputeMaxSpeed(View);
	const float FinalMaximumSpeed = MaximumSpeed * GetSpeedRatio();
	if (FinalMaximumSpeed >= SMALL_NUMBER)
	{
		// Rotate towards input direction
		const float TotalSpeedRatio = Speed / FinalMaximumSpeed;
		const float TorqueSpeedRatio = Identifier.Component->ComputeTorqueSpeedRatio(TotalSpeedRatio, TorqueSpeedSlowdown);
		const FVector Alignment = Identifier.Component->ComputeTorqueAlong(External.UpVector, MovementContact.FrameForward, View);
		Out.Torque += Alignment * (TorqueSpeedRatio * TurnTorque * Frame.Strength);
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

			// Compute movement direction
			const float Forward = Input.Direction | View;
			const float Right = Input.Direction | (MovementContact.FrameNormal ^ View);
			const FVector Direction = Forward * MovementContact.FrameForward + Right * MovementContact.FrameRight;

			// Get input vector transformed to Frame, move forwards if aligned, directly move towards input if not
			const float ForwardRatio = MovementContact.FrameForward | View;
			const float StrengthRatio = Identifier.Component->ComputeDirectionRatio(ForwardRatio, SpeedRatio, LockMovementWithTurning);
			const float Direct = StrengthRatio * Input.Magnitude * MaximumLegStrength;
			Out.Force += Identifier.Component->ComputeForceTowards(Direction, Out.Force, Direct, SpeedRatio);

			// Apply centripetal forces for nice curves
			Out.Force += Body.GetMassedLinear(MovementContact.FrameAngular ^ MovementContact.FrameVelocity);
		}
		return SpeedRatio;
	}
	return 0.0f;
}


float UTGOR_StrafingTask::ComputeMaxSpeed(FVector& View) const
{
	const FTGOR_MovementInput& State = Identifier.Component->GetState();
	View = UTGOR_Math::Normalize(State.View - (State.View | MovementContact.FrameNormal) * MovementContact.FrameNormal);

	// Compute max speed in current input direction
	const float InputSize = MovementContact.FrameInput.Size();
	if (InputSize >= SMALL_NUMBER)
	{
		// Compute ratio in either direction
		const float ForwardRatio = (MovementContact.FrameInput | View) / InputSize;
		const float SquaredRatio = ForwardRatio * ForwardRatio;

		// Compute appropriate maximum speed
		const float MaximumSpeed = ((ForwardRatio >= 0.0f) ? MaximumSpeedForward : MaximumSpeedBackward);
		return MaximumSpeed * SquaredRatio + MaximumSpeedSideways * (1.0f - SquaredRatio);
	}
	return MaximumSpeedBackward;
}
