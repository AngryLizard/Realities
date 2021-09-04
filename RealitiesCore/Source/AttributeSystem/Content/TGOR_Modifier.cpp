// The Gateway of Realities: Planes of Existence.
#include "TGOR_Modifier.h"

#include "AttributeSystem/Content/TGOR_Attribute.h"
#include "RealitiesUGC/Mod/TGOR_ContentManager.h"
#include "CoreSystem/TGOR_Singleton.h"

UTGOR_Modifier::UTGOR_Modifier()
	: Super()
{
}

void UTGOR_Modifier::PostBuildResource()
{
	Super::PostBuildResource();

	SINGLETON_CHK;
	UTGOR_ContentManager* ContentManager = Singleton->GetContentManager();

	// Generate composition
	AttributeValues.Reset();
	for (auto& Pair : Attributes)
	{
		UTGOR_Attribute* Content = ContentManager->GetTFromType<UTGOR_Attribute>(Pair.Key);
		if (IsValid(Content))
		{
			AttributeValues.Add(Content, Pair.Value);
		}
	}
}
