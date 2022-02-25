
#include "TGOR_TransactionModule.h"

#include "Realities/Base/Inventory/Modules/TGOR_Module.h"
#include "Realities/Base/Inventory/TGOR_Storage.h"
#include "Realities/Mod/Inputs/TGOR_Input.h"

#include "Realities/CoreSystem/Storage/TGOR_Serialisation.h"
#include "Realities/CoreSystem/Storage/TGOR_Package.h"
#include "Realities/CoreSystem/Singleton/TGOR_Singleton.h"
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
	return TargetIdentifier == OtherModule->TargetIdentifier && TargetFlag == OtherModule->TargetFlag;
}

bool UTGOR_TransactionModule::Merge(const UTGOR_Module* Other, ETGOR_NetvarEnumeration Priority)
{
	if (!Super::Merge(Other, Priority))
	{
		return false;
	}

	const UTGOR_TransactionModule* OtherModule = static_cast<const UTGOR_TransactionModule*>(Other);
	TargetIdentifier = OtherModule->TargetIdentifier;

	return true;
}

void UTGOR_TransactionModule::Write_Implementation(FTGOR_GroupWritePackage& Package, UTGOR_Singleton* Context) const
{
	Package.WriteEntry("TargetIdentifier", TargetIdentifier);
	Package.WriteEntry("TargetFlag", TargetFlag);
	Super::Write_Implementation(Package, Context);
}

bool UTGOR_TransactionModule::Read_Implementation(FTGOR_GroupReadPackage& Package, UTGOR_Singleton* Context)
{
	Package.ReadEntry("TargetIdentifier", TargetIdentifier);
	Package.ReadEntry("TargetFlag", TargetFlag);
	return Super::Read_Implementation(Package, Context);
}

void UTGOR_TransactionModule::Send_Implementation(FTGOR_NetworkWritePackage& Package, UTGOR_Singleton* Context) const
{
	Package.WriteEntry(TargetIdentifier);
	Package.WriteEntry(TargetFlag);
	Super::Send_Implementation(Package, Context);
}

bool UTGOR_TransactionModule::Recv_Implementation(FTGOR_NetworkReadPackage& Package, UTGOR_Singleton* Context)
{
	Package.ReadEntry(TargetIdentifier);
	Package.ReadEntry(TargetFlag);
	return Super::Recv_Implementation(Package, Context);
}


void UTGOR_TransactionModule::Clear()
{
	Super::Clear();
	TargetIdentifier = INDEX_NONE;
	TargetFlag = false;
}

FString UTGOR_TransactionModule::Print() const
{
	return FString::Printf(TEXT("TargetIdentifier = %d, TargetFlag = %d, %s"), TargetIdentifier, (int32)TargetFlag, *Super::Print());
}

void UTGOR_TransactionModule::SetTargetSlotIdentifier(int32 Identifier, bool Flag)
{
	TargetIdentifier = Identifier;
	TargetFlag = Flag;
	MarkDirty();
}

int32 UTGOR_TransactionModule::GetTargetSlotIdentifier() const
{
	return TargetIdentifier;
}

bool UTGOR_TransactionModule::GetTargetSlotFlag() const
{
	return TargetFlag;
}
