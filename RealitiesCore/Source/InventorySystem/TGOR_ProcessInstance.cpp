
#include "TGOR_ProcessInstance.h"

#include "RealitiesUGC/Mod/TGOR_ContentManager.h"

#include "CoreSystem/TGOR_Singleton.h"
#include "CoreSystem/Gameplay/TGOR_GameInstance.h"
#include "CoreSystem/Storage/TGOR_Serialisation.h"
#include "CoreSystem/Storage/TGOR_Package.h"

#include "InventorySystem/Storage/TGOR_ProcessStorage.h"
#include "InventorySystem/Content/TGOR_Process.h"

SERIALISE_INIT_SOURCE_IMPLEMENT(FTGOR_ProcessInstance)


FTGOR_ProcessInstance::FTGOR_ProcessInstance()
	: Storage(nullptr),
	Serial(0)
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
	return Other.Storage == Storage && Other.Serial == Serial;
}

bool FTGOR_ProcessInstance::Update(UTGOR_DimensionComponent* Owner, float DeltaSeconds)
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

void FTGOR_ProcessInstance::Post()
{
	if (IsValid(Storage))
	{
		Storage->Post();
	}
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

