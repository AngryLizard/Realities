// The Gateway of Realities: Planes of Existence.


#include "TGOR_InventoryAction.h"
#include "Realities/Components/Combat/TGOR_AimComponent.h"
#include "Realities/Components/System/TGOR_ActionComponent.h"
#include "Realities/Components/Inventory/TGOR_InventoryComponent.h"
#include "Realities/Components/Inventory/TGOR_ItemComponent.h"
#include "Realities/Components/Movement/TGOR_SocketComponent.h"
#include "Realities/Components/Movement/TGOR_LinearComponent.h"
#include "Realities/Actors/Inventory/TGOR_ItemDropActor.h"
#include "Realities/UI/Inventory/TGOR_InventoryTabMenu.h"
#include "Realities/Base/Inventory/TGOR_ItemStorage.h"
#include "Realities/Mod/Targets/Sockets/TGOR_NamedSocket.h"
#include "Realities/Mod/Unlocks/Spawner/Items/TGOR_InventoryItem.h"

UTGOR_InventoryAction::UTGOR_InventoryAction()
	: Super()
{
	// There is no reason for inventory not to fix aim (necessary when switching between menus)
	//LocksAimTarget = true;

	ClientAuthority = ETGOR_ClientAuthority::SkipOperate;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

/*
void UTGOR_InventoryAction::OperateStart(UTGOR_ActionTask* Storage, UTGOR_ActionComponent* Component)
{
	// Open menu
	if (Component->IsLocallyControlled())
	{
		OnMenuOpen(Storage, Component);
	}

	Super::OperateStart(Storage, Component);
}

bool UTGOR_InventoryAction::OperateState(UTGOR_ActionTask* Storage, UTGOR_ActionComponent* Component, float Deltatime)
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
	return Super::OperateState(Storage, Component, Deltatime);
}

void UTGOR_InventoryAction::Interrupt(UTGOR_ActionTask* Storage, UTGOR_ActionComponent* Component)
{
	if (IsValid(Component) && Component->IsLocallyControlled())
	{
		OnMenuClose(Storage, Component);
	}
	Super::Interrupt(Storage, Component);
}
*/

////////////////////////////////////////////////////////////////////////////////////////////////////

UTGOR_ItemStorage* UTGOR_InventoryAction::TakeFromInventory(UTGOR_Component* Component, int32 Identifier)
{
	if (IsValid(Component))
	{
		UTGOR_InventoryComponent* Inventory = Component->GetOwnerComponent<UTGOR_InventoryComponent>();
		if (IsValid(Inventory))
		{
			return Inventory->SwapItemStorage(Identifier, nullptr);
		}
	}
	return nullptr;
}

void UTGOR_InventoryAction::PutIntoInventoryAt(UTGOR_Component* Component, int32 Identifier, UTGOR_ItemStorage* Storage)
{
	if (IsValid(Component))
	{
		UTGOR_InventoryComponent* Inventory = Component->GetOwnerComponent<UTGOR_InventoryComponent>();
		if (IsValid(Inventory))
		{
			Storage = Inventory->SwapItemStorage(Identifier, Storage);
			Storage = Inventory->PutItem(Storage);
			Inventory->DropItemStorage(Storage);
		}
	}
}

void UTGOR_InventoryAction::PutIntoInventory(UTGOR_Component* Component, UTGOR_ItemStorage* Storage)
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

ATGOR_ItemDropActor* UTGOR_InventoryAction::DropItem(UTGOR_Component* Component, UTGOR_ItemStorage* Storage)
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

void UTGOR_InventoryAction::InventoryBranch(UTGOR_Component* Component, ETGOR_BoolEnumeration& Branches)
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

////////////////////////////////////////////////////////////////////////////////////////////////////

ATGOR_ItemDropActor* UTGOR_InventoryAction::PutIntoSocket(UTGOR_SocketComponent* Component, UTGOR_ItemStorage* Storage, UTGOR_NamedSocket* Socket)
{
	if(IsValid(Component) && IsValid(Socket))
	{
		ATGOR_ItemDropActor* DropActor = DropItem(Component, Storage);
		if (IsValid(DropActor))
		{
			ETGOR_ComputeEnumeration State;
			UTGOR_LinearComponent* Movement = DropActor->GetMovement();
			Component->AttachToSocket(Movement, Socket, State);
		}
		return DropActor;
	}
	return nullptr;
}

UTGOR_ItemStorage* UTGOR_InventoryAction::TakeFromSocket(UTGOR_SocketComponent* Component, UTGOR_NamedSocket* Socket)
{
	if (IsValid(Component))
	{
		UTGOR_PilotComponent* Pilot = Component->GetAttached(Socket);
		if (IsValid(Pilot))
		{
			UTGOR_ItemComponent* Container = Pilot->GetOwnerComponent<UTGOR_ItemComponent>();
			if (IsValid(Container))
			{
				return Container->PickItem();
			}
		}
	}
	return nullptr;
}

