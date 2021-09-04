// The Gateway of Realities: Planes of Existence.


#include "TGOR_MatterQuery.h"

#include "InventorySystem/Storage/TGOR_ItemStorage.h"
#include "InventorySystem/Storage/Modules/TGOR_MatterModule.h"
#include "InventorySystem/Components/TGOR_MatterComponent.h"

#include "InventorySystem/Content/TGOR_Segment.h"
#include "InventorySystem/Content/TGOR_Matter.h"

FTGOR_MatterPair::FTGOR_MatterPair()
:	Matter(nullptr),
	Quantity(0),
	Capacity(0)
{
}

bool FTGOR_MatterPair::operator<(const FTGOR_MatterPair& Other) const
{
	return Quantity < Other.Quantity;
}

UTGOR_MatterQuery::UTGOR_MatterQuery()
	: Super()
{
}

TArray<UTGOR_CoreContent*> UTGOR_MatterQuery::QueryContent() const
{
	TArray<UTGOR_CoreContent*> List;
	for (const FTGOR_MatterPair& Instance : Instances)
	{
		List.Emplace(Instance.Matter);
	}
	return List;
}

void UTGOR_MatterQuery::AssignMap(const TMap<UTGOR_Matter*, int32>& Matter)
{
	Instances.Reset();
	for (const auto& Pair : Matter)
	{
		FTGOR_MatterPair Instance;
		Instance.Matter = Pair.Key;
		Instance.Quantity = Pair.Value;
		Instances.Emplace(Instance);
	}
	Instances.Sort();
}

void UTGOR_MatterQuery::AssignCapacities(UTGOR_MatterComponent* Component, TSubclassOf<UTGOR_Segment> Segment)
{
	if (IsValid(Component))
	{
		TMap<UTGOR_Matter*, int32> Composition;
		Component->MatterContainers.AddComposition(Segment, Composition);
		AssignMap(Composition);

		TMap<UTGOR_Matter*, int32> Capacity;
		Component->MatterContainers.AddCapacity(Segment, Capacity);
		for (FTGOR_MatterPair& Instance : Instances)
		{
			int32* Ptr = Capacity.Find(Instance.Matter);
			if (Ptr)
			{
				Instance.Capacity = *Ptr;
			}
			else
			{
				Instance.Capacity = 0;
			}
		}
	}
}


void UTGOR_MatterQuery::AssignStorage(UTGOR_ItemStorage* Storage)
{
	AssignMap(Storage->GetComposition());

	TMap<UTGOR_Matter*, int32> Missing = Storage->GetMissing();
	for (FTGOR_MatterPair& Instance : Instances)
	{
		Instance.Capacity = Instance.Quantity + Missing[Instance.Matter];
	}
}