
#include "TGOR_SlotModule.h"

#include "Realities/Base/Inventory/Modules/TGOR_Module.h"
#include "Realities/Base/Inventory/TGOR_Storage.h"
#include "Realities/Mod/Inputs/TGOR_Input.h"

#include "Realities/CoreSystem/Storage/TGOR_Serialisation.h"
#include "Realities/CoreSystem/Storage/TGOR_Package.h"
#include "Realities/CoreSystem/Singleton/TGOR_Singleton.h"
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
	return SlotIdentifier == OtherModule->SlotIdentifier;
}

bool UTGOR_SlotModule::Merge(const UTGOR_Module* Other, ETGOR_NetvarEnumeration Priority)
{
	if (!Super::Merge(Other, Priority))
	{
		return false;
	}

	// Add relevant inputs
	const UTGOR_SlotModule* OtherModule = static_cast<const UTGOR_SlotModule*>(Other);
	SlotIdentifier = OtherModule->SlotIdentifier;

	return true;
}

void UTGOR_SlotModule::Write_Implementation(FTGOR_GroupWritePackage& Package, UTGOR_Singleton* Context) const
{
	Package.WriteEntry("SlotIdentifier", SlotIdentifier);
	Super::Write_Implementation(Package, Context);
}

bool UTGOR_SlotModule::Read_Implementation(FTGOR_GroupReadPackage& Package, UTGOR_Singleton* Context)
{
	Package.ReadEntry("SlotIdentifier", SlotIdentifier);
	return Super::Read_Implementation(Package, Context);
}

void UTGOR_SlotModule::Send_Implementation(FTGOR_NetworkWritePackage& Package, UTGOR_Singleton* Context) const
{
	Package.WriteEntry(SlotIdentifier);
	Super::Send_Implementation(Package, Context);
}

bool UTGOR_SlotModule::Recv_Implementation(FTGOR_NetworkReadPackage& Package, UTGOR_Singleton* Context)
{
	Package.ReadEntry(SlotIdentifier);
	return Super::Recv_Implementation(Package, Context);
}


void UTGOR_SlotModule::Clear()
{
	Super::Clear();
	SlotIdentifier = INDEX_NONE;
}

FString UTGOR_SlotModule::Print() const
{
	return FString::Printf(TEXT("SlotIndex = %d, Flag = %d, %s"), SlotIdentifier, *Super::Print());
}

void UTGOR_SlotModule::SetSlotIdentifier(int32 Identifier)
{
	SlotIdentifier = Identifier;
	MarkDirty();
}

int32 UTGOR_SlotModule::GetSlotIdentifier() const
{
	return SlotIdentifier;
}

