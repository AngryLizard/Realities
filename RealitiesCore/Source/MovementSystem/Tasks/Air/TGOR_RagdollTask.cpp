// The Gateway of Realities: Planes of Existence.


#include "TGOR_RagdollTask.h"
#include "PhysicsSystem/Components/TGOR_RigidComponent.h"
#include "MovementSystem/Components/TGOR_MovementComponent.h"

#include "RealitiesUtility/Utility/TGOR_Math.h"

#include "Net/UnrealNetwork.h"
#include "Engine/ActorChannel.h"

UTGOR_RagdollTask::UTGOR_RagdollTask()
	: Super()
{
}

void UTGOR_RagdollTask::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

}

void UTGOR_RagdollTask::Initialise()
{

}

bool UTGOR_RagdollTask::Invariant(const FTGOR_MovementSpace& Space, const FTGOR_MovementExternal& External) const
{
	return true;
}

void UTGOR_RagdollTask::Reset(const FTGOR_MovementSpace& Space, const FTGOR_MovementExternal& External)
{

}

void UTGOR_RagdollTask::QueryInput(FVector& OutInput, FVector& OutView) const
{
	const float InputStrength = Identifier.Component->GetInputStrength();
	const FVector UpVector = RigidComponent->ComputePhysicsUpVector();
	const FVector RawInput = Identifier.Component->GetRawInput();

	OutInput = UpVector * FMath::Min(InputStrength * RawInput.Size() * 2.0f, 1.0f);
}

void UTGOR_RagdollTask::Update(FTGOR_MovementSpace& Space, const FTGOR_MovementExternal& External, const FTGOR_MovementTick& Tick, FTGOR_MovementOutput& Out)
{
	const FTGOR_MovementCapture& Capture = RigidComponent->GetCapture();
	const FTGOR_MovementInput& State = Identifier.Component->GetState();
	const FTGOR_MovementFrame& Frame = Identifier.Component->GetFrame();
	const FTGOR_MovementBody& Body = RigidComponent->GetBody();

	// Dampen forces
	RigidComponent->GetDampingForce(Tick, Space.RelativeLinearVelocity, BrakeCoefficient * Frame.Strength, Out);
	RigidComponent->GetDampingTorque(Tick, Space.RelativeAngularVelocity, AngularDamping * Frame.Strength, Out);

	// Force towards upright
	const FVector UpVector = Space.Angular.GetAxisZ();
	//Out.Force += UpVector * StandupForce * Frame.Strength;

	// Rotate to towards upright 
	const float InputTorque = StandupTorque * Frame.Strength;
	const FVector Swivel = UpVector ^ State.Input;
	const float Ratio = UpVector | State.Input;
	if (Ratio >= 0.0f)
	{
		Out.Torque += Swivel * InputTorque;
	}
	else
	{
		const float Angle = Swivel.Size();
		if (Angle >= SMALL_NUMBER)
		{
			Out.Torque += Swivel / Angle * InputTorque;
		}
		else
		{
			Out.Torque += Space.Angular.GetAxisY() * InputTorque;
		}
	}

	// Check whether we are hitting the ground, clamp forces to physical ability
	FHitResult Hit;
	const float CapsuleRadius = Body.Radius * DetectionMultiplier;
	const float CapsuleHalfHeight = Body.Height * DetectionMultiplier / 2;
	if (RigidComponent->Overlap(Space.Linear, Space.Angular, CapsuleRadius, CapsuleHalfHeight, Hit))
	{

		// Rotate towards laying
		const FVector SideVector = (Hit.Normal ^ UpVector) ^ UpVector;
		const float SideNorm = SideVector.Size();
		if (SideNorm >= SMALL_NUMBER)
		{
			const FVector LayAxis = Hit.Normal ^ (SideVector / SideNorm);
			Out.Torque += LayAxis * FMath::Max(1.0f - State.Input.Size(), 0.0f) * InputTorque;
		}

		Out.Force -= External.Force.ProjectOnToNormal(Hit.Normal);
		Out.Torque -= External.Torque;

		Out.Force = UTGOR_Math::ClampToSize(Out.Force, MaxOnGroundForce * Frame.Strength);
		Out.Torque = UTGOR_Math::ClampToSize(Out.Torque, MaxOnGroundTorque * Frame.Strength);
		RagdollAnimationTickOnGround(Space, External, Hit.Normal, Hit.ImpactPoint);
	}
	else
	{
		Out.Force = UTGOR_Math::ClampToSize(Out.Force, MaxOnGroundForce * InAirMultiplier * Frame.Strength);
		Out.Torque = UTGOR_Math::ClampToSize(Out.Torque, MaxOnGroundTorque * InAirMultiplier * Frame.Strength);
		RagdollAnimationTickInAir(Space, External);
	}

	Super::Update(Space, External, Tick, Out);
}
