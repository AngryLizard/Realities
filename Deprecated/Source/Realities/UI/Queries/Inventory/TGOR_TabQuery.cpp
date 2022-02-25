// The Gateway of Realities: Planes of Existence.


#include "TGOR_TabQuery.h"
#include "Realities/Components/System/Actions/TGOR_ActionComponent.h"
#include "Realities/Components/Inventory/TGOR_InventoryComponent.h"
#include "Realities/UI/Inventory/TGOR_InventoryTabMenu.h"
#include "Realities/Base/Inventory/TGOR_ItemStorage.h"

#include "Realities/Mod/Spawner/Items/TGOR_Item.h"
#include "Realities/Mod/Actions/TGOR_Action.h"
#include "Realities/Mod/Matters/TGOR_Matter.h"

#include "Realities/Base/TGOR_Singleton.h"
#include "Realities/Base/Content/TGOR_ContentManager.h"

UTGOR_TabQuery::UTGOR_TabQuery()
	: Super()
{
	Tab = nullptr;
}

bool UTGOR_TabQuery::FilterItem(UTGOR_ItemStorage* Storage) const
{
	return Super::FilterItem(Storage);
}

void UTGOR_TabQuery::AssignTab(UTGOR_ActionComponent* ActionComponent, TSubclassOf<UTGOR_InventoryTabMenu> TabMenu)
{
	Tab = TabMenu;

	const FTGOR_SlotIndex& Index = ActionComponent->GetCurrentActionSlot();
	Super::AssignComponent(ActionComponent, Index, SupportedItem);
}


void UTGOR_TabQuery::Refresh(UTGOR_ActionComponent* ActionComponent, const FTGOR_SlotIndex& Index)
{
	Super::AssignComponent(ActionComponent, Index, SupportedItem);
}