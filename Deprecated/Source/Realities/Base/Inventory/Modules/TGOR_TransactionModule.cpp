
#include "TGOR_TransactionModule.h"

#include "Realities/Base/Inventory/Modules/TGOR_Module.h"
#include "Realities/Base/Inventory/TGOR_Storage.h"
#include "Realities/Mod/Inputs/TGOR_Input.h"

#include "Realities/Utility/Storage/TGOR_Serialisation.h"
#include "Realities/Utility/Storage/TGOR_Package.h"
#include "Realities/Base/TGOR_Singleton.h"
#include "Realities/Base/TGOR_GameState.h"

////////////////////////////////////////////////////////////////////////////////////////////////////

UTGOR_TransactionModule::UTGOR_TransactionModule()
	: Super()
{
}

bool UTGOR_TransactionModule::Equals(const UTGOR_Module* Other) const
{
	if (!Super::Equals(Other))
	{
		return false;
	}

	const UTGOR_TransactionModule* OtherModule = static_cast<const UTGOR_TransactionModule*>(Other);
	return TargetIndex == OtherModule->TargetIndex;
}

bool UTGOR_TransactionModule::Merge(const UTGOR_Module* Other, ETGOR_NetvarEnumeration Priority)
{
	if (!Super::Merge(Other, Priority))
	{
		return false;
	}

	const UTGOR_TransactionModule* OtherModule = static_cast<const UTGOR_TransactionModule*>(Other);
	TargetIndex = OtherModule->TargetIndex;

	return true;
}

void UTGOR_TransactionModule::Write_Implementation(FTGOR_GroupWritePackage& Package, UTGOR_Singleton* Context) const
{
	Package.WriteEntry("TargetIndex", TargetIndex);
	Super::Write_Implementation(Package, Context);
}

bool UTGOR_TransactionModule::Read_Implementation(FTGOR_GroupReadPackage& Package, UTGOR_Singleton* Context)
{
	Package.ReadEntry("TargetIndex", TargetIndex);
	return Super::Read_Implementation(Package, Context);
}

void UTGOR_TransactionModule::Send_Implementation(FTGOR_NetworkWritePackage& Package, UTGOR_Singleton* Context) const
{
	Package.WriteEntry(TargetIndex);
	Super::Send_Implementation(Package, Context);
}

bool UTGOR_TransactionModule::Recv_Implementation(FTGOR_NetworkReadPackage& Package, UTGOR_Singleton* Context)
{
	Package.ReadEntry(TargetIndex);
	return Super::Recv_Implementation(Package, Context);
}


void UTGOR_TransactionModule::Clear()
{
	Super::Clear();
	TargetIndex = FTGOR_SlotIndex();
}

FString UTGOR_TransactionModule::Print() const
{
	return FString::Printf(TEXT("TargetIndex = %s, %s"), *TargetIndex.Print(), *Super::Print());
}

void UTGOR_TransactionModule::SetTargetSlot(const FTGOR_SlotIndex& Index)
{
	TargetIndex = Index;
	MarkDirty();
}

FTGOR_SlotIndex UTGOR_TransactionModule::GetTargetSlot() const
{
	return TargetIndex;
}
