
#include "TGOR_SocketInstance.h"

#include "Realities/Base/TGOR_Singleton.h"
#include "Realities/Base/TGOR_GameInstance.h"
#include "Realities/Base/Content/TGOR_ContentManager.h"
#include "Realities/Utility/Storage/TGOR_Serialisation.h"
#include "Realities/Utility/Storage/TGOR_Package.h"

#include "Realities/Base/Inventory/TGOR_SocketStorage.h"
#include "Realities/Base/Inventory/Modules/TGOR_Module.h"
#include "Realities/Mod/Targets/Sockets/TGOR_Socket.h"

#include "Realities/Utility/Storage/TGOR_Package.h"

SERIALISE_INIT_SOURCE_IMPLEMENT(FTGOR_SocketInstance)


FTGOR_SocketInstance::FTGOR_SocketInstance()
	: Storage(nullptr)
{
}

FTGOR_SocketInstance::FTGOR_SocketInstance(UTGOR_Socket* NewSocket)
	: FTGOR_SocketInstance()
{
	if (IsValid(NewSocket))
	{
		SetupStorage(NewSocket);
	}
}

FTGOR_SocketInstance::FTGOR_SocketInstance(UTGOR_SocketStorage* NewStorage)
	: FTGOR_SocketInstance()
{
	Storage = NewStorage;
}

void FTGOR_SocketInstance::Write(FTGOR_GroupWritePackage& Package, UTGOR_Singleton* Context) const
{
	UTGOR_Socket* Socket = nullptr;
	if (IsValid(Storage)) Socket = Storage->GetSocket();

	Package.WriteEntry("Socket", Socket);
	if (IsValid(Storage))
	{
		Package.WriteEntry("Storage", Storage);
	}
}

void FTGOR_SocketInstance::Read(FTGOR_GroupReadPackage& Package, UTGOR_Singleton* Context)
{
	UTGOR_Socket* Socket = nullptr;
	Package.ReadEntry("Socket", Socket);

	SetupStorage(Socket);
	if (IsValid(Storage))
	{
		Package.ReadEntry("Storage", Storage);
	}
}

void FTGOR_SocketInstance::Send(FTGOR_NetworkWritePackage& Package, UTGOR_Singleton* Context) const
{
	UTGOR_Socket* Socket = nullptr;
	if (IsValid(Storage)) Socket = Storage->GetSocket();

	Package.WriteEntry(Socket);
	if (IsValid(Storage))
	{
		Package.WriteEntry(Storage);
	}
}

void FTGOR_SocketInstance::Recv(FTGOR_NetworkReadPackage& Package, UTGOR_Singleton* Context)
{
	UTGOR_Socket* Socket = nullptr;
	Package.ReadEntry(Socket);

	SetupStorage(Socket);
	if (IsValid(Storage))
	{
		Package.ReadEntry(Storage);
	}
}

bool FTGOR_SocketInstance::operator<(const FTGOR_SocketInstance& Other) const
{
	if (Storage && Other.Storage)
	{
		UTGOR_Socket* ItemA = Storage->GetSocket();
		UTGOR_Socket* ItemB = Other.Storage->GetSocket();
		if (ItemA && ItemB)
		{
			return ItemA->GetStorageIndex() < ItemB->GetStorageIndex();
		}
	}
	return false;
}

bool FTGOR_SocketInstance::operator==(const FTGOR_SocketInstance& Other) const
{
	if (Storage == Other.Storage) return true;
	if(!IsValid(Storage) || !IsValid(Other.Storage)) return false;
	return Storage->Equals(Other.Storage);
}

void FTGOR_SocketInstance::SetupStorage(UTGOR_Socket* NewSocket)
{
	// Initialise storage
	if (IsValid(NewSocket))
	{
		if (!IsValid(Storage) || Storage->GetSocket() != NewSocket)
		{
			Storage = NewSocket->CreateStorage();
		}
	}
	else
	{
		Storage = nullptr;
	}
}

