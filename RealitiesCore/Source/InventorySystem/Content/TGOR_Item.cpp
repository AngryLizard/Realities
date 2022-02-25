// The Gateway of Realities: Planes of Existence. By Salireths.

#include "TGOR_Item.h"

#include "RealitiesUtility/Utility/TGOR_Math.h"

#include "InventorySystem/Components/TGOR_ItemComponent.h"
#include "InventorySystem/Actors/TGOR_ItemDropActor.h"
#include "InventorySystem/Tasks/TGOR_ItemTask.h"

#include "CoreSystem/TGOR_Singleton.h"
#include "InventorySystem/Content/TGOR_Matter.h"
#include "CoreSystem/Content/TGOR_CoreContent.h"


const float UTGOR_Item::DEFAULT_ITEM_LIFETIME = 300.0f;

UTGOR_Item::UTGOR_Item()
	: Super()
{
}

TSubclassOf<AActor> UTGOR_Item::GetSpawnClass_Implementation() const
{
	return DefaultItemActor;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

UTGOR_ItemTask* UTGOR_Item::CreateItemTask(UTGOR_ItemComponent* Component, int32 SlotIdentifier)
{
	return UTGOR_Task::CreateTask(this, TaskType, Component, SlotIdentifier);
}

void UTGOR_Item::TaskInitialise(UTGOR_ItemTask* ItemTask)
{
	OnTaskInitialise(ItemTask);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


bool UTGOR_Item::CanAssembleWith(const TMap<UTGOR_Matter*, int32>& Matter) const
{
	// Check for each matter type whether entry exists and is sufficient
	for (const auto& Pair : Instanced_MatterInsertions.Collection)
	{
		if (Pair.Value > 0)
		{
			const int32* Ptr = Matter.Find(Pair.Key);
			if (!Ptr || (*Ptr) < Pair.Value)
			{
				return false;
			}
		}
	}
	return true;
}

bool UTGOR_Item::IsCompatibleWith(const TMap<UTGOR_Matter*, int32>& Matter) const
{
	// Check for each matter type whether entry exists and is sufficient
	for (const auto& Pair : Instanced_MatterInsertions.Collection)
	{
		if (Pair.Value > 0)
		{
			const int32* Ptr = Matter.Find(Pair.Key);
			if (Ptr && (*Ptr) > 0)
			{
				return true;
			}
		}
	}
	return false;
}

bool UTGOR_Item::CanDisassembleInto(const TSet<UTGOR_Matter*>& Matter) const
{
	// Check for each matter type whether entry exists
	for (const auto& Pair : Instanced_MatterInsertions.Collection)
	{
		if (!Matter.Contains(Pair.Key))
		{
			return false;
		}
	}
	return true;
}

bool UTGOR_Item::HasComposition(UTGOR_Matter* Matter) const
{
	return Instanced_MatterInsertions.Collection.Contains(Matter);
}

int32 UTGOR_Item::GetCapacityBits(UTGOR_Matter* Matter) const
{
	const int32 Quantity = Instanced_MatterInsertions.Collection[Matter];
	return UTGOR_Math::Log2(Quantity) + 1;
}

float UTGOR_Item::GetDefaultItemLifeTime()
{
	return UTGOR_Item::DEFAULT_ITEM_LIFETIME;
}

const TMap<TObjectPtr<UTGOR_Matter>, int32>& UTGOR_Item::GetComponentCapacity() const
{
	return Instanced_MatterInsertions.Collection;
}

void UTGOR_Item::MoveInsertion(UTGOR_Content* Insertion, ETGOR_InsertionActionEnumeration Action, bool& Success)
{
	Super::MoveInsertion(Insertion, Action, Success);

	MOV_INSERTION(MatterInsertions);
}

