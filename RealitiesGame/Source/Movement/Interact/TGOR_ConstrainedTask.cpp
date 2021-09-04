// The Gateway of Realities: Planes of Existence.


#include "TGOR_ConstrainedTask.h"
#include "PhysicsSystem/Components/TGOR_RigidComponent.h"
#include "MovementSystem/Components/TGOR_MovementComponent.h"

#include "RealitiesUtility/Utility/TGOR_Math.h"
#include "RealitiesUtility/Structures/TGOR_Direction.h"
#include "SocketSystem/Tasks/TGOR_NamedSocketTask.h"

#include "Net/UnrealNetwork.h"
#include "Engine/ActorChannel.h"

UTGOR_ConstrainedTask::UTGOR_ConstrainedTask()
	: Super()
{
}

void UTGOR_ConstrainedTask::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

}

////////////////////////////////////////////////////////////////////////////////////////////////////

void UTGOR_ConstrainedTask::Initialise()
{

}

bool UTGOR_ConstrainedTask::Invariant(const FTGOR_MovementSpace& Space, const FTGOR_MovementExternal& External) const
{
	return !IsValid(RigidComponent->GetPilotOfType(UTGOR_NamedSocketTask::StaticClass()));
}

void UTGOR_ConstrainedTask::Reset(const FTGOR_MovementSpace& Space, const FTGOR_MovementExternal& External)
{

}

void UTGOR_ConstrainedTask::QueryInput(FVector& OutInput, FVector& OutView) const
{
	const float InputStrength = Identifier.Component->GetInputStrength();
	const FVector RawInput = Identifier.Component->GetRawInput();
	const FQuat InputQuat = Identifier.Component->GetInputRotation();

	OutInput = InputQuat * RawInput * InputStrength;
}

void UTGOR_ConstrainedTask::Update(FTGOR_MovementSpace& Space, const FTGOR_MovementExternal& External, const FTGOR_MovementTick& Tick, FTGOR_MovementOutput& Out)
{
	const FTGOR_MovementFrame& Frame = Identifier.Component->GetFrame();
	const FTGOR_MovementInput& State = Identifier.Component->GetState();
	const FTGOR_MovementBody& Body = RigidComponent->GetBody();

	Out.Force = State.Input * InputForce * Frame.Strength - External.Force.GetClampedToMaxSize(MaxForce * Frame.Strength);
	Out.Torque = FVector::ZeroVector - External.Torque.GetClampedToMaxSize(MaxTorque * Frame.Strength);

	AttachmentAnimationTick(Space, External);

	Super::Update(Space, External, Tick, Out);
}


