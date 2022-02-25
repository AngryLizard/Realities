// The Gateway of Realities: Planes of Existence.


#include "TGOR_TerminalTabQuery.h"
#include "Realities/Components/Dimension/Interactable/TGOR_TerminalComponent.h"
#include "Realities/Components/System/Actions/TGOR_ActionComponent.h"
#include "Realities/Components/Inventory/TGOR_InventoryComponent.h"
#include "Realities/Components/Combat/TGOR_AimComponent.h"
#include "Realities/UI/Inventory/TGOR_InventoryTabMenu.h"
#include "Realities/Base/Inventory/TGOR_ItemStorage.h"
#include "Realities/Mod/Spawner/Items/TGOR_Item.h"

#include "Realities/Mod/Spawner/Items/TGOR_Item.h"
#include "Realities/Mod/Actions/TGOR_Action.h"
#include "Realities/Mod/Matters/TGOR_Matter.h"

#include "Realities/Base/TGOR_Singleton.h"
#include "Realities/Base/Content/TGOR_ContentManager.h"

UTGOR_TerminalTabQuery::UTGOR_TerminalTabQuery()
	: Super()
{
}

bool UTGOR_TerminalTabQuery::FilterItem(UTGOR_ItemStorage* Storage) const
{
	if (!Super::FilterItem(Storage)) return false;
	if (Terminal.IsValid() && IsValid(Storage))
	{
		UTGOR_Item* Content = Storage->GetItem();
		return Content->IsSupportedBy(Terminal->SupportedItem);
	}
	return true;
}

void UTGOR_TerminalTabQuery::AssignTab(UTGOR_ActionComponent* ActionComponent, TSubclassOf<UTGOR_InventoryTabMenu> TabMenu)
{
	Tab = TabMenu;
	if (IsValid(ActionComponent))
	{
		UTGOR_AimComponent* AimComponent = ActionComponent->GetOwnerComponent<UTGOR_AimComponent>();
		if (IsValid(AimComponent))
		{
			const FTGOR_AimInstance Aim = AimComponent->GetCurrentAim();
			Terminal = Cast<UTGOR_TerminalComponent>(Aim.Component);
			if (Terminal.IsValid())
			{
				Tab = Terminal->TabMenu;
				SupportedItem = Terminal->SupportedItem;
			}
		}
	}

	Super::AssignTab(ActionComponent, Tab);
}

