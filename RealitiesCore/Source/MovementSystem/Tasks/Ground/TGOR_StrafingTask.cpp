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


float UTGOR_StrafingTask::GetInputForce(const FTGOR_MovementTick& Tick, const FTGOR_MovementSpace& Space, const FVector& Orientation, const FTGOR_MovementExternal& External, const FTGOR_MovementContact& Contact, const FTGOR_MovementRepel& Repel, FTGOR_MovementOutput& Out) const
{
	// Parent implements damping
	Super::GetInputForce(Tick, Space, Orientation, External, Contact, Repel, Out);

	const FTGOR_MovementFrame& Frame = Identifier.Component->GetFrame();
	const FTGOR_MovementBody& Body = RootComponent->GetBody();

	// Only worry about velocity along Frame
	const float Speed = Contact.FrameVelocity.Size();
	const FTGOR_Direction Input = Contact.FrameInput;

	FVector View;
	const float MaximumSpeed = ComputeMaxSpeed(Contact, View);
	const float FinalMaximumSpeed = MaximumSpeed * GetSpeedRatio();
	if (FinalMaximumSpeed >= SMALL_NUMBER)
	{
		// Rotate towards input direction
		const float TotalSpeedRatio = Speed / FinalMaximumSpeed;
		const float TorqueSpeedRatio = Identifier.Component->ComputeTorqueSpeedRatio(TotalSpeedRatio, TorqueSpeedSlowdown);
		const FVector Alignment = Identifier.Component->ComputeTorqueAlong(External.UpVector, Contact.FrameForward, View);
		Out.Torque += Alignment * (TorqueSpeedRatio * TurnTorque * Frame.Strength);
	}


	// Filter sideways input
	const float LegRatio = ComputeCrouchSpeedRatio(Contact.GroundRatio);
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
			const float StrengthRatio = Identifier.Component->ComputeDirectionRatio(ForwardRatio, SpeedRatio, LockMovementWithTurning);
			const float Direct = StrengthRatio * Input.Magnitude * MaximumLegStrength;
			Out.Force += Identifier.Component->ComputeForceTowards(Direction, Out.Force, Direct, SpeedRatio);

			// Apply centripetal forces for nice curves
			Out.Force += Body.GetMassedLinear(Contact.FrameAngular ^ Contact.FrameVelocity);
		}
		return SpeedRatio;
	}
	return 0.0f;
}


float UTGOR_StrafingTask::ComputeMaxSpeed(const FTGOR_MovementContact& Contact, FVector& View) const
{
	const FTGOR_MovementInput& State = Identifier.Component->GetState();
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
