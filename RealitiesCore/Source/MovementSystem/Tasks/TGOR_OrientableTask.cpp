// The Gateway of Realities: Planes of Existence.

#include "TGOR_OrientableTask.h"
#include "AnimationSystem/TGOR_AnimationLibrary.h"
#include "AnimationSystem/Components/TGOR_HandleComponent.h"
#include "AnimationSystem/Components/TGOR_AttachComponent.h"

#include "RealitiesUtility/Utility/TGOR_Math.h"

#include "Net/UnrealNetwork.h"
#include "Engine/ActorChannel.h"

UTGOR_OrientableTask::UTGOR_OrientableTask()
	: Super(),
	LocalUpVector(FVector::UpVector)
{
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void UTGOR_OrientableTask::Initialise()
{
	Super::Initialise();

	UTGOR_AnimationLibrary::GetHandleComponents(Primitives, Identifier.Component->GetOwner(), PrimitiveTypes, true);

	LocalUpVector = FVector::ZeroVector;
	for (UTGOR_HandleComponent* Primitive : Primitives)
	{
		LocalUpVector -= Primitive->MovementCone->GetAlignmentDirection(FVector2D::ZeroVector);
	}
	LocalUpVector.Normalize();
}

bool UTGOR_OrientableTask::Invariant(const FTGOR_MovementSpace& Space, const FTGOR_MovementExternal& External) const
{
	if (!Super::Invariant(Space, External))
	{
		return false;
	}

	if (Primitives.Num() == 0)
	{
		return false;
	}

	return true;
}
