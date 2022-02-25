// The Gateway of Realities: Planes of Existence.

#include "TGOR_InventoryTabMenu.h"

#include "Realities/UI/Queries/Inventory/TGOR_TabQuery.h"

UTGOR_InventoryTabMenu::UTGOR_InventoryTabMenu()
	: Super(), HasItemRow(true)
{

	Type = ETGOR_MenuTypeEnumeration::Permanent;
}

void UTGOR_InventoryTabMenu::OpenMenu(UTGOR_Query* Query)
{
	TabQuery = Cast<UTGOR_TabQuery>(Query);

	Super::OpenMenu(Query);
}

