#include "TGOR_SocketStorage.h"

#include "Realities/Components/Movement/TGOR_MobilityComponent.h"
#include "Realities/Mod/Targets/Sockets/TGOR_Socket.h"
#include "Realities/Mod/Energies/TGOR_Matter.h"

#include "Realities/Utility/Storage/TGOR_Serialisation.h"
#include "Realities/Utility/Storage/TGOR_Package.h"
#include "Realities/Base/TGOR_Singleton.h"
#include "Realities/Base/TGOR_GameState.h"

#include "Realities/Base/Inventory/Modules/TGOR_Module.h"
#include "Realities/Utility/Storage/TGOR_Package.h"

void UTGOR_SocketStorage::Clear()
{
	Super::Clear();
	MarkDirty();
}

FString UTGOR_SocketStorage::Print() const
{
	FString Out = "";
	TMap<UTGOR_Matter*, int32> Composition = GetComposition();
	for (const auto& Pair : Composition)
	{
		Out += FString::Printf(TEXT("%s = %d, "), *Pair.Key->GetDefaultName(), Pair.Value);
	}
	return FString::Printf(TEXT("Socket = %s, Composition = {%s}, %s"), *SocketRef->GetDefaultName(), *Out, *Super::Print());
}

void UTGOR_SocketStorage::Tick(UTGOR_DimensionComponent* Owner, float DeltaSeconds)
{
	if (IsValid(SocketRef))
	{
		SocketRef->Tick(Owner, this, DeltaSeconds);
	}
	Super::Tick(Owner, DeltaSeconds);
}

bool UTGOR_SocketStorage::Compatible(const UTGOR_Container* Other) const
{
	if (!Super::Compatible(Other))
	{
		return false;
	}

	const UTGOR_SocketStorage* OtherStorage = Cast<UTGOR_SocketStorage>(Other);
	return SocketRef == OtherStorage->SocketRef;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void UTGOR_SocketStorage::Write_Implementation(FTGOR_GroupWritePackage& Package, UTGOR_Singleton* Context) const
{
	Super::Write_Implementation(Package, Context);
}

bool UTGOR_SocketStorage::Read_Implementation(FTGOR_GroupReadPackage& Package, UTGOR_Singleton* Context)
{
	Super::Read_Implementation(Package, Context);
	return true;
}

void UTGOR_SocketStorage::Send_Implementation(FTGOR_NetworkWritePackage& Package, UTGOR_Singleton* Context) const
{
	Super::Send_Implementation(Package, Context);
}

bool UTGOR_SocketStorage::Recv_Implementation(FTGOR_NetworkReadPackage& Package, UTGOR_Singleton* Context)
{
	Super::Recv_Implementation(Package, Context);
	return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

bool UTGOR_SocketStorage::Equals(const UTGOR_Storage* Other) const
{
	if (!Super::Equals(Other))
	{
		return false;
	}
	return true;
}

UTGOR_Content* UTGOR_SocketStorage::GetOwner() const
{
	return GetSocket();
}

////////////////////////////////////////////////////////////////////////////////////////////////////

bool UTGOR_SocketStorage::HasCycle() const
{
	UTGOR_MobilityComponent* Parent = GetParent();
	return IsValid(Parent) && Parent->HasParent(Parent);
}

FTGOR_MovementDynamic UTGOR_SocketStorage::PackParent() const
{
	return FTGOR_MovementDynamic();
}

void UTGOR_SocketStorage::UnpackParent(const FTGOR_MovementDynamic& Dynamic)
{

}

////////////////////////////////////////////////////////////////////////////////////////////////////

UTGOR_MobilityComponent* UTGOR_SocketStorage::GetParent() const
{
	if (IsValid(SocketRef))
	{
		return SocketRef->GetParent(this);
	}
	return false;
}

FTGOR_MovementDynamic UTGOR_SocketStorage::ComputeBase() const
{
	UTGOR_MobilityComponent* Parent = GetParent();
	if (IsValid(Parent))
	{
		return Parent->ComputeSpace();
	}
	return FTGOR_MovementDynamic();
}

FTGOR_MovementPosition UTGOR_SocketStorage::ComputePosition() const
{
	if (IsValid(SocketRef))
	{
		return SocketRef->ComputePosition(this);
	}
	return FTGOR_MovementPosition();
}

FTGOR_MovementSpace UTGOR_SocketStorage::ComputeSpace() const
{
	if (IsValid(SocketRef))
	{
		return SocketRef->ComputeSpace(this);
	}
	return FTGOR_MovementSpace();
}

////////////////////////////////////////////////////////////////////////////////////////////////////

UTGOR_Socket* UTGOR_SocketStorage::GetSocket() const
{
	return SocketRef;
}
