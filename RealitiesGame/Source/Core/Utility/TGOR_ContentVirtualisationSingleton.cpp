// The Gateway of Realities: Planes of Existence.

#include "TGOR_ContentVirtualisationSingleton.h"

#include "RealitiesUGC/Mod/TGOR_ContentManager.h"



void UTGOR_ContentVirtualisationSingleton::CreateContentManager()
{
	ContentManager = NewObject<UTGOR_ContentManager>(GetWorld());
}