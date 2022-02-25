// The Gateway of Realities: Planes of Existence.


#include "TGOR_Object.h"

#include "Realities/Base/TGOR_GameInstance.h"
#include "TGOR_Singleton.h"


UTGOR_Object::UTGOR_Object(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	AssignedWorld = nullptr;
}

void UTGOR_Object::PostInitProperties()
{
	Super::PostInitProperties();
	if (!IsValid(AssignedWorld))
	{
		AssignWorld(GetOuter());
	}
}

UWorld* UTGOR_Object::GetWorld() const
{
	return(AssignedWorld);
}


void UTGOR_Object::AssignWorld(UObject* WorldContextObject)
{
	if (IsValid(WorldContextObject))
	{
		AssignedWorld = WorldContextObject->GetWorld();
	}
}