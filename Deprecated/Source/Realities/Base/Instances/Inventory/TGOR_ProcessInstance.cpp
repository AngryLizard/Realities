
#include "TGOR_ProcessInstance.h"

#include "Realities/Base/TGOR_Singleton.h"
#include "Realities/Base/TGOR_GameInstance.h"
#include "Realities/Base/Content/TGOR_ContentManager.h"
#include "Realities/Utility/Storage/TGOR_Serialisation.h"
#include "Realities/Utility/Storage/TGOR_Package.h"

#include "Realities/Base/Inventory/TGOR_ProcessStorage.h"
#include "Realities/Base/Inventory/Modules/TGOR_Module.h"
#include "Realities/Mod/Processes/TGOR_Process.h"

#include "Realities/Utility/Storage/TGOR_Package.h"

SERIALISE_INIT_SOURCE_IMPLEMENT(FTGOR_ProcessInstance)


FTGOR_ProcessInstance::FTGOR_ProcessInstance()
	: Storage(nullptr)
{
}

FTGOR_ProcessInstance::FTGOR_ProcessInstance(UTGOR_Process* NewProcess)
	: FTGOR_ProcessInstance()
{
	if (IsValid(NewProcess))
	{
		SetupStorage(NewProcess);
	}
}

FTGOR_ProcessInstance::FTGOR_ProcessInstance(UTGOR_ProcessStorage* NewStorage)
	: FTGOR_ProcessInstance()
{
	Storage = NewStorage;
}

void FTGOR_ProcessInstance::Write(FTGOR_GroupWritePackage& Package, UTGOR_Singleton* Context) const
{
	UTGOR_Process* Process = nullptr;
	if (IsValid(Storage)) Process = Storage->GetProcess();

	Package.WriteEntry("Process", Process);
	if (IsValid(Storage))
	{
		Package.WriteEntry("Storage", Storage);
	}
}

void FTGOR_ProcessInstance::Read(FTGOR_GroupReadPackage& Package, UTGOR_Singleton* Context)
{
	UTGOR_Process* Process = nullptr;
	Package.ReadEntry("Process", Process);

	SetupStorage(Process);
	if (IsValid(Storage))
	{
		Package.ReadEntry("Storage", Storage);
	}
}

void FTGOR_ProcessInstance::Send(FTGOR_NetworkWritePackage& Package, UTGOR_Singleton* Context) const
{
	UTGOR_Process* Process = nullptr;
	if (IsValid(Storage)) Process = Storage->GetProcess();

	Package.WriteEntry(Process);
	if (IsValid(Storage))
	{
		Package.WriteEntry(Storage);
	}
}

void FTGOR_ProcessInstance::Recv(FTGOR_NetworkReadPackage& Package, UTGOR_Singleton* Context)
{
	UTGOR_Process* Process = nullptr;
	Package.ReadEntry(Process);

	SetupStorage(Process);
	if (IsValid(Storage))
	{
		Package.ReadEntry(Storage);
	}
}

bool FTGOR_ProcessInstance::operator<(const FTGOR_ProcessInstance& Other) const
{
	if (Storage && Other.Storage)
	{
		UTGOR_Process* ItemA = Storage->GetProcess();
		UTGOR_Process* ItemB = Other.Storage->GetProcess();
		if (ItemA && ItemB)
		{
			return ItemA->GetStorageIndex() < ItemB->GetStorageIndex();
		}
	}
	return false;
}

bool FTGOR_ProcessInstance::operator==(const FTGOR_ProcessInstance& Other) const
{
	if (Storage == Other.Storage) return true;
	if(!IsValid(Storage) || !IsValid(Other.Storage)) return false;
	return Storage->Equals(Other.Storage);
}

void FTGOR_ProcessInstance::SetupStorage(UTGOR_Process* NewProcess)
{
	// Initialise storage
	if (IsValid(NewProcess))
	{
		if (!IsValid(Storage) || Storage->GetProcess() != NewProcess)
		{
			Storage = NewProcess->CreateStorage();
		}
	}
	else
	{
		Storage = nullptr;
	}
}

