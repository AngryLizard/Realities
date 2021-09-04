// The Gateway of Realities: Planes of Existence.

#include "TGOR_LevelVolume.h"
#include "CoreSystem/TGOR_Singleton.h"
#include "DimensionSystem/Data/TGOR_DimensionData.h"

// Sets default values
ATGOR_LevelVolume::ATGOR_LevelVolume(const FObjectInitializer& ObjectInitializer)
:	Super(ObjectInitializer)
{
	IsNetworkVolume = true;
}

bool ATGOR_LevelVolume::PreAssemble(UTGOR_DimensionData* Dimension)
{
	Dimension->LevelVolume = this;
	
	return(ATGOR_BoxPhysicsVolume::PreAssemble(Dimension));
}

////////////////////////////////////////////////////////////////////////////////////////////////////


void ATGOR_LevelVolume::SetExternal(const FVector& Force)
{
	Gravity = Force.Size();
	if (Gravity >= SMALL_NUMBER)
	{
		UpVector = Force / -Gravity;
	}
}
