// The Gateway of Realities: Planes of Existence.


#include "TGOR_ActionInstance.h"

#include "Realities/Components/System/Actions/TGOR_ActionComponent.h"
#include "Realities/Base/Inventory/TGOR_ActionStorage.h"
#include "Realities/Base/Inventory/TGOR_ItemStorage.h"
#include "Realities/Mod/Spawner/Items/TGOR_Item.h"
#include "Realities/Mod/Actions/TGOR_Action.h"

#include "Realities/Base/TGOR_GameInstance.h"
#include "Realities/Base/Content/TGOR_ContentManager.h"
#include "Realities/Base/TGOR_Singleton.h"
#include "Realities/Utility/Storage/TGOR_Package.h"

SERIALISE_INIT_SOURCE_IMPLEMENT(FTGOR_ActionInstance)
SERIALISE_INIT_SOURCE_IMPLEMENT(FTGOR_ActionState);
SERIALISE_INIT_SOURCE_IMPLEMENT(FTGOR_ActionPayload);




FTGOR_ActionInstance::FTGOR_ActionInstance()
	: Storage(nullptr)
{
}

FTGOR_ActionInstance::FTGOR_ActionInstance(UTGOR_Action* NewAction)
	: FTGOR_ActionInstance()
{
	if (IsValid(NewAction))
	{
		SetupStorage(NewAction);
	}
}

FTGOR_ActionInstance::FTGOR_ActionInstance(UTGOR_ActionStorage* NewStorage)
	: FTGOR_ActionInstance()
{
	Storage = NewStorage;
}

void FTGOR_ActionInstance::Write(FTGOR_GroupWritePackage& Package, UTGOR_Singleton* Context) const
{
	UTGOR_Action* Action = nullptr;
	if (IsValid(Storage)) Action = Storage->GetAction();

	Package.WriteEntry("Action", Action);
	if (IsValid(Storage))
	{
		Package.WriteEntry("Storage", Storage);
	}
}

void FTGOR_ActionInstance::Read(FTGOR_GroupReadPackage& Package, UTGOR_Singleton* Context)
{
	UTGOR_Action* Action = nullptr;
	Package.ReadEntry("Action", Action);

	SetupStorage(Action);
	if (IsValid(Storage))
	{
		Package.ReadEntry("Storage", Storage);
	}
}

void FTGOR_ActionInstance::Send(FTGOR_NetworkWritePackage& Package, UTGOR_Singleton* Context) const
{
	UTGOR_Action* Action = nullptr;
	if (IsValid(Storage)) Action = Storage->GetAction();

	Package.WriteEntry(Action);
	if (IsValid(Storage))
	{
		Package.WriteEntry(Storage);
	}
}

void FTGOR_ActionInstance::Recv(FTGOR_NetworkReadPackage& Package, UTGOR_Singleton* Context)
{
	UTGOR_Action* Action = nullptr;
	Package.ReadEntry(Action);

	SetupStorage(Action);
	if (IsValid(Storage))
	{
		Package.ReadEntry(Storage);
	}
}

bool FTGOR_ActionInstance::operator<(const FTGOR_ActionInstance& Other) const
{
	if (Storage && Other.Storage)
	{
		UTGOR_Action* ActionA = Storage->GetAction();
		UTGOR_Action* ActionB = Other.Storage->GetAction();
		if (ActionA && ActionB)
		{
			return ActionA->GetStorageIndex() < ActionB->GetStorageIndex();
		}
	}
	return false;
}

bool FTGOR_ActionInstance::operator==(const FTGOR_ActionInstance& Other) const
{
	if (Storage == Other.Storage) return true;
	if (!IsValid(Storage) || !IsValid(Other.Storage)) return false;
	return Storage->Equals(Other.Storage);
}

void FTGOR_ActionInstance::SetupStorage(UTGOR_Action* NewAction)
{
	// Initialise storage
	if (IsValid(NewAction))
	{
		if (!IsValid(Storage) || Storage->GetOwner() != NewAction)
		{
			Storage = NewAction->CreateStorage();
		}
	}
	else
	{
		Storage = nullptr;
	}
}




FTGOR_ActionPayload::FTGOR_ActionPayload()
	: Storage(nullptr)
{
}

FTGOR_ActionPayload::FTGOR_ActionPayload(UTGOR_Action* NewAction)
	: FTGOR_ActionPayload()
{
	if (IsValid(NewAction))
	{
		SetupStorage(NewAction);
	}
}

FTGOR_ActionPayload::FTGOR_ActionPayload(UTGOR_ActionStorage* NewStorage)
	: FTGOR_ActionPayload()
{
	Storage = NewStorage;
}

void FTGOR_ActionPayload::Send(FTGOR_NetworkWritePackage& Package, UTGOR_Singleton* Context) const
{
	UTGOR_Action* Action = nullptr;
	if (IsValid(Storage)) Action = Storage->GetAction();

	Package.WriteEntry(Action);
	if (IsValid(Storage))
	{
		Storage->UTGOR_Storage::Send_Implementation(Package, Context);
	}
}

void FTGOR_ActionPayload::Recv(FTGOR_NetworkReadPackage& Package, UTGOR_Singleton* Context)
{
	UTGOR_Action* Action = nullptr;
	Package.ReadEntry(Action);

	SetupStorage(Action);
	if (IsValid(Storage))
	{
		Storage->UTGOR_Storage::Recv_Implementation(Package, Context);
	}
}

void FTGOR_ActionPayload::SetupStorage(UTGOR_Action* NewAction)
{
	// Initialise storage
	if (IsValid(NewAction))
	{
		if (!IsValid(Storage) || Storage->GetOwner() != NewAction)
		{
			Storage = NewAction->CreateStorage();
		}
	}
	else
	{
		Storage = nullptr;
	}
}



FTGOR_ActionState::FTGOR_ActionState()
	: Action(), Item()
{
}

void FTGOR_ActionState::Send(FTGOR_NetworkWritePackage& Package, UTGOR_Singleton* Context) const
{
	Package.WriteEntry(Action);
	Package.WriteEntry(Item);
}

void FTGOR_ActionState::Recv(FTGOR_NetworkReadPackage& Package, UTGOR_Singleton* Context)
{
	Package.ReadEntry(Action);
	Package.ReadEntry(Item);
}
