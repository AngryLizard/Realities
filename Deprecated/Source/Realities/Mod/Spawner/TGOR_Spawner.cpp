// The Gateway of Realities: Planes of Existence.


#include "TGOR_Spawner.h"

#include "Realities/Utility/Error/TGOR_Error.h"
#include "Realities/Mod/Animations/TGOR_Animation.h"
#include "Realities/Mod/Movements/TGOR_Movement.h"
#include "Realities/Base/System/Data/TGOR_WorldData.h"
#include "Realities/Base/System/Data/TGOR_DimensionData.h"
#include "Realities/Base/TGOR_Singleton.h"

UTGOR_Spawner::UTGOR_Spawner()
	: Super()
{
}

TSubclassOf<AActor> UTGOR_Spawner::GetSpawnClass_Implementation() const
{
	return AActor::StaticClass();
}
