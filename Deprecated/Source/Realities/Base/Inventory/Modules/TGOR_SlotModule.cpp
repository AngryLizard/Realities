
#include "TGOR_SlotModule.h"

#include "Realities/Base/Inventory/Modules/TGOR_Module.h"
#include "Realities/Base/Inventory/TGOR_Storage.h"
#include "Realities/Mod/Inputs/TGOR_Input.h"

#include "Realities/Utility/Storage/TGOR_Serialisation.h"
#include "Realities/Utility/Storage/TGOR_Package.h"
#include "Realities/Base/TGOR_Singleton.h"
#include "Realities/Base/TGOR_GameState.h"

////////////////////////////////////////////////////////////////////////////////////////////////////

UTGOR_SlotModule::UTGOR_SlotModule()
	: Super()
{
	ModulePriority = ETGOR_NetvarEnumeration::Client;
}

bool UTGOR_SlotModule::Equals(const UTGOR_Module* Other) const
{
	if (!Super::Equals(Other))
	{
		return false;
	}

	const UTGOR_SlotModule* OtherModule = static_cast<const UTGOR_SlotModule*>(Other);
	return SlotIndex == OtherModule->SlotIndex;
}

bool UTGOR_SlotModule::Merge(const UTGOR_Module* Other, ETGOR_NetvarEnumeration Priority)
{
	if (!Super::Merge(Other, Priority))
	{
		return false;
	}

	// Add relevant inputs
	const UTGOR_SlotModule* OtherModule = static_cast<const UTGOR_SlotModule*>(Other);
	SlotIndex = OtherModule->SlotIndex;

	return true;
}

void UTGOR_SlotModule::Write_Implementation(FTGOR_GroupWritePackage& Package, UTGOR_Singleton* Context) const
{
	Package.WriteEntry("SlotIndex", SlotIndex);
	Super::Write_Implementation(Package, Context);
}

bool UTGOR_SlotModule::Read_Implementation(FTGOR_GroupReadPackage& Package, UTGOR_Singleton* Context)
{
	Package.ReadEntry("SlotIndex", SlotIndex);
	return Super::Read_Implementation(Package, Context);
}

void UTGOR_SlotModule::Send_Implementation(FTGOR_NetworkWritePackage& Package, UTGOR_Singleton* Context) const
{
	Package.WriteEntry(SlotIndex);
	Super::Send_Implementation(Package, Context);
}

bool UTGOR_SlotModule::Recv_Implementation(FTGOR_NetworkReadPackage& Package, UTGOR_Singleton* Context)
{
	Package.ReadEntry(SlotIndex);
	return Super::Recv_Implementation(Package, Context);
}


void UTGOR_SlotModule::Clear()
{
	Super::Clear();
	SlotIndex = FTGOR_SlotIndex();
}

FString UTGOR_SlotModule::Print() const
{
	return FString::Printf(TEXT("SlotIndex = %s, %s"), *SlotIndex.Print(), *Super::Print());
}

void UTGOR_SlotModule::SetSlot(const FTGOR_SlotIndex& Index)
{
	SlotIndex = Index;
	MarkDirty();
}

FTGOR_SlotIndex UTGOR_SlotModule::GetSlot() const
{
	return SlotIndex;
}

