// The Gateway of Realities: Planes of Existence.


#include "TGOR_DimensionActor.h"

#include "Realities/Base/System/Data/TGOR_DimensionData.h"
#include "Realities/Base/TGOR_GameInstance.h"

ATGOR_DimensionActor::ATGOR_DimensionActor(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	IdentityComponent = ObjectInitializer.CreateDefaultSubobject<UTGOR_IdentityComponent>(this, FName(TEXT("IdentityComponent")));
}