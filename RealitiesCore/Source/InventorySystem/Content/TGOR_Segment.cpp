// The Gateway of Realities: Planes of Existence.

#include "TGOR_Segment.h"

#include "CoreSystem/TGOR_Singleton.h"
#include "InventorySystem/Content/TGOR_Matter.h"
#include "RealitiesUGC/Mod/TGOR_ContentManager.h"
#include "CoreSystem/Content/TGOR_CoreContent.h"

UTGOR_Segment::UTGOR_Segment()
	: Super()
{
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void UTGOR_Segment::PostBuildResource()
{
	Super::PostBuildResource();

	SINGLETON_CHK;
	UTGOR_ContentManager* ContentManager = Singleton->GetContentManager();

	// Generate composition
	MatterCapacity.Reset();
	for (auto& Pair : MatterComposition)
	{
		UTGOR_Matter* Content = ContentManager->GetTFromType<UTGOR_Matter>(Pair.Key); // *this breaks if Matter isn't included in item.h, see header for more.
		if (IsValid(Content))
		{
			MatterCapacity.Add(Content, Pair.Value);
		}
	}

	// Create serialisation list
	TArray<UTGOR_Matter*> Keys;
	MatterCapacity.GetKeys(Keys);
	Keys.Sort([](const UTGOR_Matter& A, const UTGOR_Matter& B)->bool { return A.GetStorageIndex() < B.GetStorageIndex(); });
	
	MatterRanks.Empty();
	for (int32 Index = 0; Index < Keys.Num(); Index++)
	{
		MatterRanks.Emplace(Keys[Index], Index);
	}
}