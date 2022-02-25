// The Gateway of Realities: Planes of Existence.


#include "TGOR_RagdollMovement.h"
#include "Realities/Utility/TGOR_Math.h"

#include "Realities/Components/Movement/TGOR_MovementComponent.h"

UTGOR_RagdollMovement::UTGOR_RagdollMovement()
: Super()
{
	AngularDamping = 1'000.0f;
	BrakeCoefficient = 6'000.0f;

	DetectionMultiplier = 1.2f;
	StandupForce = 80'000.0f;
	StandupTorque = 3'500.0f;

	MaxOnGroundForce = 240'000.0f;
	MaxOnGroundTorque = 8'000.0f;

	InAirMultiplier = 0.1f;

	CanRotateOnCollision = true;
}

void UTGOR_RagdollMovement::QueryInputVector(const UTGOR_MovementComponent* Component, FVector& OutInput, FVector& OutView) const
{
	const float InputStrength = Component->GetInputStrength();
	const FVector UpVector = Component->ComputePhysicsUpVector();
	const FVector RawInput = Component->GetRawInput();
	
	OutInput = UpVector * FMath::Min(InputStrength * RawInput.Size() * 2.0f, 1.0f);
}

void UTGOR_RagdollMovement::Tick(UTGOR_MovementComponent* Component, const FTGOR_MovementTick& Tick, const FTGOR_MovementSpace& Space, const FTGOR_MovementExternal& External, bool Replay, FTGOR_MovementOutput& Out)
{
	const FTGOR_MovementCapture& Capture = Component->GetCapture();
	const FTGOR_MovementState& State = Component->GetState();
	const FTGOR_MovementFrame& Frame = Component->GetFrame();
	const FTGOR_MovementBody& Body = Component->GetBody();

	// Dampen forces
	Component->GetLinearDamping(Tick, Space.RelativeLinearVelocity, BrakeCoefficient * Frame.Strength, Out);
	Component->GetAngularDamping(Tick, Space.RelativeAngularVelocity, AngularDamping * Frame.Strength, Out);

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
	if (Component->Overlap(Space.Linear, Space.Angular, CapsuleRadius, CapsuleHalfHeight, Hit))
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
		RagdollAnimationTickOnGround(Component, Space, External, Hit.Normal, Hit.ImpactPoint);
	}
	else
	{
		Out.Force = UTGOR_Math::ClampToSize(Out.Force, MaxOnGroundForce * InAirMultiplier * Frame.Strength);
		Out.Torque = UTGOR_Math::ClampToSize(Out.Torque, MaxOnGroundTorque * InAirMultiplier * Frame.Strength);
		RagdollAnimationTickInAir(Component, Space, External);
	}
}

bool UTGOR_RagdollMovement::Invariant(const UTGOR_MovementComponent* Component, const FTGOR_MovementTick& Tick, const FTGOR_MovementSpace& Space, const FTGOR_MovementExternal& External) const
{
	return true;
}
