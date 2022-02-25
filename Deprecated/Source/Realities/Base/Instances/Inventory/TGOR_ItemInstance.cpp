
#include "TGOR_ItemInstance.h"

#include "Realities/Base/TGOR_Singleton.h"
#include "Realities/Base/TGOR_GameInstance.h"
#include "Realities/Base/Content/TGOR_ContentManager.h"
#include "Realities/Utility/Storage/TGOR_Serialisation.h"
#include "Realities/Utility/Storage/TGOR_Package.h"
#include "Realities/Base/Inventory/TGOR_ItemStorage.h"
#include "Realities/Base/Inventory/Modules/TGOR_Module.h"
#include "Realities/Mod/Spawner/Items/TGOR_Item.h"

#include "Realities/Utility/Storage/TGOR_Package.h"

SERIALISE_INIT_SOURCE_IMPLEMENT(FTGOR_ItemInstance)


FTGOR_ItemRestriction::FTGOR_ItemRestriction()
{
	ItemType = UTGOR_Item::StaticClass();
	ModuleType = nullptr;
}

FTGOR_ItemInstance::FTGOR_ItemInstance()
	: Storage(nullptr)
{
}

FTGOR_ItemInstance::FTGOR_ItemInstance(UTGOR_Item* NewItem)
	: FTGOR_ItemInstance()
{
	if (IsValid(NewItem))
	{
		SetupStorage(NewItem);
	}
}

FTGOR_ItemInstance::FTGOR_ItemInstance(UTGOR_ItemStorage* NewStorage)
	: FTGOR_ItemInstance()
{
	Storage = NewStorage;
}

void FTGOR_ItemInstance::Write(FTGOR_GroupWritePackage& Package, UTGOR_Singleton* Context) const
{
	UTGOR_Item* Item = nullptr;
	if (IsValid(Storage)) Item = Storage->GetItem();

	Package.WriteEntry("Item", Item);
	if (IsValid(Storage))
	{
		Package.WriteEntry("Storage", Storage);
	}
}

void FTGOR_ItemInstance::Read(FTGOR_GroupReadPackage& Package, UTGOR_Singleton* Context)
{
	UTGOR_Item* Item = nullptr;
	Package.ReadEntry("Item", Item);

	SetupStorage(Item);
	if (IsValid(Storage))
	{
		Package.ReadEntry("Storage", Storage);
	}
}

void FTGOR_ItemInstance::Send(FTGOR_NetworkWritePackage& Package, UTGOR_Singleton* Context) const
{
	UTGOR_Item* Item = nullptr;
	if (IsValid(Storage)) Item = Storage->GetItem();

	Package.WriteEntry(Item);
	if (IsValid(Storage))
	{
		Package.WriteEntry(Storage);
	}
}

void FTGOR_ItemInstance::Recv(FTGOR_NetworkReadPackage& Package, UTGOR_Singleton* Context)
{
	UTGOR_Item* Item = nullptr;
	Package.ReadEntry(Item);

	SetupStorage(Item);
	if (IsValid(Storage))
	{
		Package.ReadEntry(Storage);
	}
}

bool FTGOR_ItemInstance::operator<(const FTGOR_ItemInstance& Other) const
{
	if (Storage && Other.Storage)
	{
		UTGOR_Item* ItemA = Storage->GetItem();
		UTGOR_Item* ItemB = Other.Storage->GetItem();
		if (ItemA && ItemB)
		{
			return ItemA->GetStorageIndex() < ItemB->GetStorageIndex();
		}
	}
	return false;
}

bool FTGOR_ItemInstance::operator==(const FTGOR_ItemInstance& Other) const
{
	if (Storage == Other.Storage) return true;
	if(!IsValid(Storage) || !IsValid(Other.Storage)) return false;
	return Storage->Equals(Other.Storage);
}

void FTGOR_ItemInstance::SetupStorage(UTGOR_Item* NewItem)
{
	// Initialise storage
	if (IsValid(NewItem))
	{
		if (!IsValid(Storage) || Storage->GetItem() != NewItem)
		{
			Storage = NewItem->CreateStorage();
		}
	}
	else
	{
		Storage = nullptr;
	}
}

