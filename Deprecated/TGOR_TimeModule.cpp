
#include "TGOR_TimeModule.h"

#include "Realities/Base/Inventory/Modules/TGOR_Module.h"
#include "Realities/Base/Inventory/TGOR_Storage.h"
#include "Realities/Mod/Inputs/TGOR_Input.h"

#include "Realities/CoreSystem/Storage/TGOR_Serialisation.h"
#include "Realities/CoreSystem/Storage/TGOR_Package.h"
#include "Realities/CoreSystem/Singleton/TGOR_Singleton.h"
#include "Realities/Base/TGOR_GameState.h"

////////////////////////////////////////////////////////////////////////////////////////////////////

UTGOR_TimeModule::UTGOR_TimeModule()
	: Super()
{
	ModulePriority = ETGOR_NetvarEnumeration::Server;
}

bool UTGOR_TimeModule::Equals(const UTGOR_Module* Other) const
{
	if (!Super::Equals(Other))
	{
		return false;
	}

	const UTGOR_TimeModule* OtherModule = static_cast<const UTGOR_TimeModule*>(Other);
	return Time == OtherModule->Time;
}

bool UTGOR_TimeModule::Merge(const UTGOR_Module* Other, ETGOR_NetvarEnumeration Priority)
{
	if (!Super::Merge(Other, Priority))
	{
		return false;
	}

	// Add relevant inputs
	const UTGOR_TimeModule* OtherModule = static_cast<const UTGOR_TimeModule*>(Other);
	Time = OtherModule->Time;

	return true;
}

void UTGOR_TimeModule::Write_Implementation(FTGOR_GroupWritePackage& Package, UTGOR_Singleton* Context) const
{
	Package.WriteEntry("Time", Time);
	Super::Write_Implementation(Package, Context);
}

bool UTGOR_TimeModule::Read_Implementation(FTGOR_GroupReadPackage& Package, UTGOR_Singleton* Context)
{
	Package.ReadEntry("Time", Time);
	return Super::Read_Implementation(Package, Context);
}

void UTGOR_TimeModule::Send_Implementation(FTGOR_NetworkWritePackage& Package, UTGOR_Singleton* Context) const
{
	Package.WriteEntry(Time);
	Super::Send_Implementation(Package, Context);
}

bool UTGOR_TimeModule::Recv_Implementation(FTGOR_NetworkReadPackage& Package, UTGOR_Singleton* Context)
{
	Package.ReadEntry(Time);
	return Super::Recv_Implementation(Package, Context);
}


void UTGOR_TimeModule::Clear()
{
	Super::Clear();
	Time = FTGOR_Time();
}

FString UTGOR_TimeModule::Print() const
{
	return FString::Printf(TEXT("Time = %s, %s"), *Time.Print(), *Super::Print());
}

void UTGOR_TimeModule::StartTimer()
{
	SINGLETON_CHK;
	Time = Singleton->GetGameTimestamp();
	MarkDirty();
}

void UTGOR_TimeModule::StartCountdown(float Duration)
{
	SINGLETON_CHK;
	Time = Singleton->GetGameTimestamp() + Duration;
	MarkDirty();
}

void UTGOR_TimeModule::Stop()
{
	SINGLETON_CHK;
	Time.Clear();
	MarkDirty();
}

float UTGOR_TimeModule::GetDuration() const
{
	if (Singleton.IsValid() && Time != FTGOR_Time())
	{
		return FMath::Abs(Singleton->GetGameTimestamp() - Time);
	}
	return 0.0f;
}

bool UTGOR_TimeModule::IsPast() const
{
	if (Singleton.IsValid())
	{
		return Singleton->GetGameTimestamp() > Time;
	}
	return false;
}

bool UTGOR_TimeModule::IsActive() const
{
	return !Time.IsZero();
}
