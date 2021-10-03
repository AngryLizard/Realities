// The Gateway of Realities: Planes of Existence.


#include "TGOR_UprightTask.h"
#include "DimensionSystem/Components/TGOR_PilotComponent.h"
#include "MovementSystem/Components/TGOR_MovementComponent.h"

#include "RealitiesUtility/Utility/TGOR_Math.h"
#include "RealitiesUtility/Structures/TGOR_Direction.h"

#include "Net/UnrealNetwork.h"
#include "Engine/ActorChannel.h"

UTGOR_UprightTask::UTGOR_UprightTask()
	: Super()
{
}

void UTGOR_UprightTask::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

}

////////////////////////////////////////////////////////////////////////////////////////////////////

bool UTGOR_UprightTask::Invariant(const FTGOR_MovementSpace& Space, const FTGOR_MovementExternal& External) const
{
	if (!Super::Invariant(Space, External))
	{
		return false;
	}

	return (Space.Angular.GetAxisZ() | External.UpVector) > StandingThreshold;
}

float UTGOR_UprightTask::ComputeCrouchSpeedRatio(float GroundRatio) const
{
	// Max velocity depends on current leg length
	const float Ratio = FMath::Clamp((GroundRatio - CrouchStretch) / (StandingStretch - CrouchStretch), 0.0f, 1.0f);
	return FMath::Lerp(CrouchSpeedMultiplier, 1.0f, Ratio);
}

float UTGOR_UprightTask::GetStretch(const FTGOR_MovementTick& Tick, const FTGOR_MovementSpace& Space, const FVector& Orientation, const FTGOR_MovementExternal& External, const FTGOR_MovementContact& Contact) const
{
	const FTGOR_MovementCapture& Capture = Identifier.Component->GetCapture();
	const FTGOR_MovementInput& State = Identifier.Component->GetState();

	const float X = (State.Input | External.UpVector);
	const float A = ((1.0f + CrouchStretch) / 2 - StandingStretch);
	const float B = (1.0f - CrouchStretch) / 2;
	const float C = StandingStretch;
	return (A * X + B) * X + C;
}
