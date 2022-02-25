// The Gateway of Realities: Planes of Existence.


#include "TGOR_MatterQuery.h"
#include "Realities/Mod/Matters/TGOR_Matter.h"
#include "Realities/Base/Inventory/TGOR_ItemStorage.h"
#include "Realities/Base/Inventory/Modules/TGOR_MatterModule.h"
#include "Realities/Components/Inventory/TGOR_MatterComponent.h"

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

TArray<UTGOR_Content*> UTGOR_MatterQuery::QueryContent() const
{
	TArray<UTGOR_Content*> List;
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

void UTGOR_MatterQuery::AssignCapacities(UTGOR_MatterComponent* Component)
{
	for (FTGOR_MatterPair& Instance : Instances)
	{
		Instance.Capacity = Component->GetStorageCapacity(Instance.Matter);
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