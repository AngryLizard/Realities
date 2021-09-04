// The Gateway of Realities: Planes of Existence.

#include "TGOR_WorldObject.h"

UTGOR_WorldObject::UTGOR_WorldObject(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	AssignedWorld = nullptr;
}

void UTGOR_WorldObject::PostInitProperties()
{
	Super::PostInitProperties();
	if (!IsValid(AssignedWorld))
	{
		AssignWorld(GetOuter());
	}
}

UWorld* UTGOR_WorldObject::GetWorld() const
{
	return(AssignedWorld);
}


void UTGOR_WorldObject::AssignWorld(UObject* WorldContextObject)
{
	if (IsValid(WorldContextObject))
	{
		AssignedWorld = WorldContextObject->GetWorld();
	}
}