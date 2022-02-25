#include "TGOR_ProcessStorage.h"

#include "Realities/Components/Inventory/TGOR_ProcessComponent.h"
#include "Realities/Mod/Processes/TGOR_Process.h"
#include "Realities/Mod/Matters/TGOR_Matter.h"
#include "Realities/Utility/Storage/TGOR_Serialisation.h"
#include "Realities/Utility/Storage/TGOR_Package.h"
#include "Realities/Base/TGOR_Singleton.h"
#include "Realities/Base/TGOR_GameState.h"

#include "Realities/Base/Inventory/Modules/TGOR_Module.h"
#include "Realities/Utility/Storage/TGOR_Package.h"

void UTGOR_ProcessStorage::Clear()
{
	Super::Clear();
	MarkDirty();
}

FString UTGOR_ProcessStorage::Print() const
{
	FString Out = "";
	TMap<UTGOR_Matter*, int32> Composition = GetComposition();
	for (const auto& Pair : Composition)
	{
		Out += FString::Printf(TEXT("%s = %d, "), *Pair.Key->GetDefaultName(), Pair.Value);
	}
	return FString::Printf(TEXT("Item = %s, Composition = {%s}, %s"), *ProcessRef->GetDefaultName(), *Out, *Super::Print());
}

void UTGOR_ProcessStorage::Tick(UTGOR_DimensionComponent* Owner, float DeltaSeconds)
{
	if (IsValid(ProcessRef))
	{
		ProcessRef->Tick(Owner, this, DeltaSeconds);
	}
	Super::Tick(Owner, DeltaSeconds);
}

bool UTGOR_ProcessStorage::Compatible(const UTGOR_Container* Other) const
{
	if (!Super::Compatible(Other))
	{
		return false;
	}

	const UTGOR_ProcessStorage* OtherStorage = Cast<UTGOR_ProcessStorage>(Other);
	return ProcessRef == OtherStorage->ProcessRef;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void UTGOR_ProcessStorage::Write_Implementation(FTGOR_GroupWritePackage& Package, UTGOR_Singleton* Context) const
{
	Super::Write_Implementation(Package, Context);
	Package.WriteEntry("Accumulation", Accumulation);
}

bool UTGOR_ProcessStorage::Read_Implementation(FTGOR_GroupReadPackage& Package, UTGOR_Singleton* Context)
{
	Super::Read_Implementation(Package, Context);
	Package.ReadEntry("Accumulation", Accumulation);
	return true;
}

void UTGOR_ProcessStorage::Send_Implementation(FTGOR_NetworkWritePackage& Package, UTGOR_Singleton* Context) const
{
	Super::Send_Implementation(Package, Context);
	Package.WriteEntry(Accumulation);
}

bool UTGOR_ProcessStorage::Recv_Implementation(FTGOR_NetworkReadPackage& Package, UTGOR_Singleton* Context)
{
	Super::Recv_Implementation(Package, Context);
	Package.ReadEntry(Accumulation);
	return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

bool UTGOR_ProcessStorage::Equals(const UTGOR_Storage* Other) const
{
	if (!Super::Equals(Other))
	{
		return false;
	}
	return true;
}

UTGOR_Content* UTGOR_ProcessStorage::GetOwner() const
{
	return GetProcess();
}

////////////////////////////////////////////////////////////////////////////////////////////////////

UTGOR_Process* UTGOR_ProcessStorage::GetProcess() const
{
	return ProcessRef;
}

bool UTGOR_ProcessStorage::Process(UTGOR_ProcessComponent* Owner, float Antimatter)
{
	if (IsValid(ProcessRef))
	{
		// Accumulate antimatter for processing
		Accumulation += Antimatter;

		// Terminate if process has become invalid
		if (ProcessRef->CanProcess(Owner, this))
		{
			// Process with given antimatter
			Accumulation -= ProcessRef->Process(Owner, this, Accumulation);
			return true;
		}
	}
	return false;
}
