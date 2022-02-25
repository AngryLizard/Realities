// The Gateway of Realities: Planes of Existence.


#include "TGOR_InventoryAction.h"
#include "Realities/Components/Combat/TGOR_AimComponent.h"
#include "Realities/Components/System/Actions/TGOR_AbilityComponent.h"
#include "Realities/Components/Inventory/TGOR_InventoryComponent.h"
#include "Realities/UI/Inventory/TGOR_InventoryTabMenu.h"
#include "Realities/Base/Inventory/TGOR_ItemStorage.h"
#include "Realities/Mod/Spawner/Items/TGOR_InventoryItem.h"

UTGOR_InventoryAction::UTGOR_InventoryAction()
	: Super()
{
	// There is no reason for inventory not to fix aim (necessary for terminal coherence)
	FixesAim = true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void UTGOR_InventoryAction::ForceEquip(UTGOR_ActionStorage* Storage, UTGOR_ActionComponent* Component)
{
	// Open menu
	if (Component->IsLocallyControlled())
	{
		OnMenuOpen(Storage, Component);
	}

	Super::ForceEquip(Storage, Component);
}

bool UTGOR_InventoryAction::Operate(UTGOR_ActionStorage* Storage, UTGOR_ActionComponent* Component, float Deltatime)
{
	ETGOR_TerminationEnumeration State = ETGOR_TerminationEnumeration::Continue;
	if (IsValid(Component) && Component->IsLocallyControlled())
	{
		OnMenuTick(Storage, Component, State);
		if (State == ETGOR_TerminationEnumeration::Terminate)
		{
			return false;
		}
	}
	return Super::Operate(Storage, Component, Deltatime);
}

void UTGOR_InventoryAction::ForceUnequip(UTGOR_ActionStorage* Storage, UTGOR_ActionComponent* Component)
{
	if (IsValid(Component) && Component->IsLocallyControlled())
	{
		OnMenuClose(Storage, Component);
	}
	Super::ForceUnequip(Storage, Component);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

UTGOR_ItemStorage* UTGOR_InventoryAction::TakeFromInventory(UTGOR_ActionComponent* Component, const FTGOR_SlotIndex& Index)
{
	if (IsValid(Component))
	{
		UTGOR_InventoryComponent* Inventory = Component->GetOwnerComponent<UTGOR_InventoryComponent>();
		if (IsValid(Inventory))
		{
			return Inventory->SwapItemStorage(Index, nullptr);
		}
	}
	return nullptr;
}

void UTGOR_InventoryAction::PutIntoInventoryAt(UTGOR_ActionComponent* Component, const FTGOR_SlotIndex& Index, UTGOR_ItemStorage* Storage)
{
	if (IsValid(Component))
	{
		UTGOR_InventoryComponent* Inventory = Component->GetOwnerComponent<UTGOR_InventoryComponent>();
		if (IsValid(Inventory))
		{
			Storage = Inventory->PutItemInto(Index, Storage);
			Storage = Inventory->PutItem(Storage);
			Inventory->DropItemStorage(Storage);
		}
	}
}

void UTGOR_InventoryAction::PutIntoInventory(UTGOR_ActionComponent* Component, UTGOR_ItemStorage* Storage)
{
	if (IsValid(Component))
	{
		UTGOR_InventoryComponent* Inventory = Component->GetOwnerComponent<UTGOR_InventoryComponent>();
		if (IsValid(Inventory))
		{
			Storage = Inventory->PutItem(Storage);
			Inventory->DropItemStorage(Storage);
		}
	}
}

ATGOR_ItemDropActor* UTGOR_InventoryAction::DropItem(UTGOR_ActionComponent* Component, UTGOR_ItemStorage* Storage)
{
	if (IsValid(Component))
	{
		UTGOR_InventoryComponent* Inventory = Component->GetOwnerComponent<UTGOR_InventoryComponent>();
		if (IsValid(Inventory))
		{
			return Inventory->DropItemStorage(Storage);
		}
	}
	return nullptr;
}

void UTGOR_InventoryAction::InventoryBranch(UTGOR_ActionComponent* Component, ETGOR_BoolEnumeration& Branches)
{
	if (IsValid(Component))
	{
		Branches = ETGOR_BoolEnumeration::IsNot;
		UTGOR_InventoryComponent* Inventory = Component->GetOwnerComponent<UTGOR_InventoryComponent>();
		if (IsValid(Inventory) && Inventory->HasAccess())
		{
			Branches = ETGOR_BoolEnumeration::Is;
		}
	}
}