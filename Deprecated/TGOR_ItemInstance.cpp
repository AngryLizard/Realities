
#include "TGOR_ItemInstance.h"

#include "RealitiesUGC/Mod/TGOR_ContentManager.h"

#include "CoreSystem/TGOR_Singleton.h"
#include "CoreSystem/Gameplay/TGOR_GameInstance.h"
#include "CoreSystem/Storage/TGOR_Serialisation.h"
#include "CoreSystem/Storage/TGOR_Package.h"
#include "InventorySystem/Storage/TGOR_ItemStorage.h"
#include "InventorySystem/Content/TGOR_Item.h"

SERIALISE_INIT_SOURCE_IMPLEMENT(FTGOR_ItemInstance)


FTGOR_ItemRestriction::FTGOR_ItemRestriction()
{
	ItemType = UTGOR_Item::StaticClass();
	ModuleType = nullptr;
}

FTGOR_ItemInstance::FTGOR_ItemInstance()
	: Storage(nullptr),
	Serial(0)
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
	return Other.Storage == Storage && Other.Serial == Serial;
}

bool FTGOR_ItemInstance::Update(UTGOR_DimensionComponent* Owner, float DeltaSeconds)
{
	if (IsValid(Storage))
	{
		Storage->Tick(Owner, DeltaSeconds);
		if (Storage->PurgeDirt())
		{
			// Update so it gets replicated by server
			Serial = (Serial + 1) & 0xFFFF;
			return true;
		}
	}
	return false;
}

void FTGOR_ItemInstance::Post()
{
	if (IsValid(Storage))
	{
		Storage->Post();
	}
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

