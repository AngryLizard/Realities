// The Gateway of Realities: Planes of Existence.


#include "TGOR_Creature.h"

#include "Realities/Utility/Error/TGOR_Error.h"
#include "Realities/Mod/Animations/TGOR_Animation.h"
#include "Realities/Mod/Movements/TGOR_Movement.h"
#include "Realities/Base/TGOR_Singleton.h"
#include "Realities/Base/Content/TGOR_ContentManager.h"

UTGOR_Creature::UTGOR_Creature()
	: Super()
{
	Weight = 120.0f;
	SurfaceArea = FVector(0.5f, 0.5f, 0.5f);
}

TSubclassOf<AActor> UTGOR_Creature::GetSpawnClass_Implementation() const
{
	return Avatar;
}

