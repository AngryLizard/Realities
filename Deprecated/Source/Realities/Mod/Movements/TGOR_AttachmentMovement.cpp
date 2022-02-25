// The Gateway of Realities: Planes of Existence.


#include "TGOR_AttachmentMovement.h"
#include "Realities/Utility/TGOR_Math.h"

#include "Realities/Components/Movement/TGOR_MovementComponent.h"
#include "Realities/Mod/Movements/TGOR_RagdollMovement.h"

#include "DrawDebugHelpers.h"

UTGOR_AttachmentMovement::UTGOR_AttachmentMovement()
: Super()
{
	InputForce = 320'000.0f;
	MaxForce = 240'000.0f;
	MaxTorque = 8'000.0f;
}

void UTGOR_AttachmentMovement::QueryInputVector(const UTGOR_MovementComponent* Component, FVector& OutInput, FVector& OutView) const
{
	const float InputStrength = Component->GetInputStrength();
	//const FVector UpVector = Component->ComputePhysicsUpVector();
	const FVector RawInput = Component->GetRawInput();
	const FQuat InputQuat = Component->GetInputRotation();
	
	OutInput = InputQuat * RawInput * InputStrength;
}

void UTGOR_AttachmentMovement::Tick(UTGOR_MovementComponent* Component, const FTGOR_MovementTick& Tick, const FTGOR_MovementSpace& Space, const FTGOR_MovementExternal& External, bool Replay, FTGOR_MovementOutput& Out)
{
	const FTGOR_MovementFrame& Frame = Component->GetFrame();
	const FTGOR_MovementState& State = Component->GetState();
	const FTGOR_MovementBody& Body = Component->GetBody();

	Out.Force = State.Input * InputForce * Frame.Strength - External.Force.GetClampedToMaxSize(MaxForce * Frame.Strength);
	Out.Torque = FVector::ZeroVector - External.Torque.GetClampedToMaxSize(MaxTorque * Frame.Strength);

	AttachmentAnimationTick(Component, Space, External);
}

bool UTGOR_AttachmentMovement::Invariant(const UTGOR_MovementComponent* Component, const FTGOR_MovementTick& Tick, const FTGOR_MovementSpace& Space, const FTGOR_MovementExternal& External) const
{
	return Component->IsAttached();
}
