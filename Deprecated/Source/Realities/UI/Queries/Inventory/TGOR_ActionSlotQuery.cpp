// The Gateway of Realities: Planes of Existence.

#include "TGOR_ActionSlotQuery.h"
#include "Realities/Base/Content/TGOR_Content.h"
#include "Realities/Mod/Spawner/Items/TGOR_Item.h"
#include "Realities/Mod/Actions/Equipables/Inventories/TGOR_InventoryAction.h"
#include "Realities/Base/Inventory/TGOR_Storage.h"

#include "Realities/Components/System/Actions/TGOR_ActionComponent.h"

UTGOR_ActionSlotQuery::UTGOR_ActionSlotQuery()
	: Super()
{
}

TArray<UTGOR_Content*> UTGOR_ActionSlotQuery::QueryContent() const
{
	TArray<UTGOR_Content*> Contents;
	if (IsValid(OwnerComponent))
	{
		for (const FTGOR_SlotIndex& Slot : Slots)
		{
			Contents.Emplace(OwnerComponent->GetAction(Slot));
		}
	}
	return Contents;
}

void UTGOR_ActionSlotQuery::AssignComponent(UTGOR_ActionComponent* ActionComponent, TSubclassOf<UTGOR_Action> Type, bool CheckCanCall)
{
	OwnerComponent = ActionComponent;

	// Read current ActionComponent state
	const FTGOR_ContainerIndex Container = OwnerComponent->GetContainerTypeIndex(Type);
	const int32 Num = OwnerComponent->GetContainerSize(Container);

	// Extract items in current container
	Slots.Reset();
	for (uint8 i = 0; i < Num; i++)
	{
		const FTGOR_SlotIndex Slot = FTGOR_SlotIndex(Container, i);
		if (CheckCanCall)
		{
			UTGOR_Action* Action = OwnerComponent->GetAction(Slot);
			if (Action->CanCall(OwnerComponent, Slot))
			{
				Slots.Emplace(Slot);
			}
		}
		else
		{
			Slots.Emplace(Slot);
		}
	}

	// Call and initialise initial selection
	const FTGOR_SlotIndex Index = OwnerComponent->GetContainerLastSlot(Container);
	InitialiseSelection((int32)(Index.Slot));
}