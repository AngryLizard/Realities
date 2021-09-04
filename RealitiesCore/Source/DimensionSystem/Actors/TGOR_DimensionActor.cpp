// The Gateway of Realities: Planes of Existence.


#include "TGOR_DimensionActor.h"

#include "CoreSystem/Gameplay/TGOR_GameInstance.h"

#include "DimensionSystem/Components/TGOR_IdentityComponent.h"

ATGOR_DimensionActor::ATGOR_DimensionActor(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	IdentityComponent = ObjectInitializer.CreateDefaultSubobject<UTGOR_IdentityComponent>(this, FName(TEXT("IdentityComponent")));
}