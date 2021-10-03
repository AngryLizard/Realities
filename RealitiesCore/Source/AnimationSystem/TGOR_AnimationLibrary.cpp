// The Gateway of Realities: Planes of Existence.

#include "TGOR_AnimationLibrary.h"

#include "AnimationSystem/Components/TGOR_AttachComponent.h"
#include "AnimationSystem/Components/TGOR_HandleComponent.h"
#include "DimensionSystem/Content/TGOR_Primitive.h"


UTGOR_AnimationLibrary::UTGOR_AnimationLibrary(class FObjectInitializer const& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

UTGOR_HandleComponent* UTGOR_AnimationLibrary::GetHandleComponent(AActor* Owner, TSubclassOf<UTGOR_Primitive> Type)
{
	if (IsValid(Owner))
	{
		TArray<UTGOR_HandleComponent*> Pilots;
		Owner->GetComponents(Pilots);
		for (UTGOR_HandleComponent* Pilot : Pilots)
		{
			if (*Pilot->SpawnPrimitive && Pilot->SpawnPrimitive->IsChildOf(Type))
			{
				return Pilot;
			}
		}
	}
	return nullptr;
}

TArray<UTGOR_HandleComponent*> UTGOR_AnimationLibrary::GetHandleComponents(AActor* Owner, TSubclassOf<UTGOR_Primitive> Type)
{
	TArray<UTGOR_HandleComponent*> Out;

	if (IsValid(Owner))
	{
		TArray<UTGOR_HandleComponent*> Pilots;
		Owner->GetComponents(Pilots);
		for (UTGOR_HandleComponent* Pilot : Pilots)
		{
			if (*Pilot->SpawnPrimitive && Pilot->SpawnPrimitive->IsChildOf(Type))
			{
				Out.Emplace(Pilot);
			}
		}
	}
	return Out;
}