
#include "TGOR_EventModule.h"

#include "Realities/Base/Inventory/Modules/TGOR_Module.h"
#include "Realities/Base/Inventory/TGOR_Storage.h"
#include "Realities/Mod/Events/TGOR_Event.h"

#include "Realities/CoreSystem/Storage/TGOR_Serialisation.h"
#include "Realities/CoreSystem/Storage/TGOR_Package.h"
#include "Realities/CoreSystem/Singleton/TGOR_Singleton.h"
#include "Realities/Base/TGOR_GameState.h"

////////////////////////////////////////////////////////////////////////////////////////////////////

UTGOR_EventModule::UTGOR_EventModule()
	: Super()
{
	ModulePriority = ETGOR_NetvarEnumeration::Client;
}

bool UTGOR_EventModule::Equals(const UTGOR_Module* Other) const
{
	if (!Super::Equals(Other))
	{
		return false;
	}

	const UTGOR_EventModule* OtherModule = static_cast<const UTGOR_EventModule*>(Other);
	return Activated == OtherModule->Activated;
}

bool UTGOR_EventModule::Merge(const UTGOR_Module* Other, ETGOR_NetvarEnumeration Priority)
{
	if (!Super::Merge(Other, Priority))
	{
		return false;
	}

	// Add relevant events
	const UTGOR_EventModule* OtherModule = static_cast<const UTGOR_EventModule*>(Other);
	for(int32 Index = 0; Index < Events.Num(); Index++)
	{
		if (Events[Index]->IsRelevant(Priority))
		{
			Activated[Index] = Activated[Index] || OtherModule->Activated[Index];
			Prompted[Index] = Prompted[Index] || OtherModule->Prompted[Index];
		}
	}
	return true;
}

void UTGOR_EventModule::BuildModule(const UTGOR_Storage* Parent)
{
	Super::BuildModule(Parent);

	PopulateEvents(Parent->GetOwner()->GetIListFromType<UTGOR_Event>());
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void UTGOR_EventModule::Write_Implementation(FTGOR_GroupWritePackage& Package, UTGOR_Singleton* Context) const
{
	Package.WriteEntry("Events", Activated);
	Super::Write_Implementation(Package, Context);
}

bool UTGOR_EventModule::Read_Implementation(FTGOR_GroupReadPackage& Package, UTGOR_Singleton* Context)
{
	Package.ReadEntry("Events", Activated);
	return Super::Read_Implementation(Package, Context);
}

void UTGOR_EventModule::Send_Implementation(FTGOR_NetworkWritePackage& Package, UTGOR_Singleton* Context) const
{
	// Only replicate non-local events
	for (int32 Index = 0; Index < Events.Num(); Index++)
	{
		if (Events[Index]->IsRelevant(ETGOR_NetvarEnumeration::Server))
		{
			// Replication (should) happen before tick. Prompted keeps activation state even after tick.
			Package.WriteEntry(Prompted[Index]);
		}
	}
	Super::Send_Implementation(Package, Context);
}

bool UTGOR_EventModule::Recv_Implementation(FTGOR_NetworkReadPackage& Package, UTGOR_Singleton* Context)
{
	for (int32 Index = 0; Index < Events.Num(); Index++)
	{
		if (Events[Index]->IsRelevant(ETGOR_NetvarEnumeration::Server))
		{
			// Keep previous events active, faulty replication could delete an event that wasn't consumed yet.
			bool State;
			Package.ReadEntry(State);
			Activated[Index] = Activated[Index] || State;
		}
	}
	return Super::Recv_Implementation(Package, Context);
}


void UTGOR_EventModule::Clear()
{
	Super::Clear();
	for (bool& State : Activated)
	{
		State = false;
	}
	Prompted = Activated;
}

FString UTGOR_EventModule::Print() const
{
	FString Out = "";
	for (int32 Index = 0; Index < Events.Num(); Index++)
	{
		Out += FString::Printf(TEXT("%s = %s>%s,"), *Events[Index]->GetDefaultName(), (Prompted[Index] ? "t" : "f"), (Activated[Index] ? "t" : "f"));
	}
	return Out + ", " + Super::Print();
}

void UTGOR_EventModule::Post()
{
	Super::Post();
	Prompted = Activated;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void UTGOR_EventModule::PopulateEvents(const TArray<UTGOR_Event*>& Register)
{
	Events = Register;
	Activated.SetNumZeroed(Events.Num());
	Prompted = Activated;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void UTGOR_EventModule::ScheduleEvent(TSubclassOf<UTGOR_Event> Event)
{
	for (int32 Index = 0; Index < Events.Num(); Index++)
	{
		if (Events[Index]->IsA(Event))
		{
			Activated[Index] = true;
			Prompted[Index] = true;
			MarkDirty(true);
		}
	}
}

void UTGOR_EventModule::PeekEvent(TSubclassOf<UTGOR_Event> Event, ETGOR_EventEnumeration& Branches)
{
	Branches = ETGOR_EventEnumeration::Dead;
	for (int32 Index = 0; Index < Events.Num(); Index++)
	{
		if (Activated[Index] && Events[Index]->IsA(Event))
		{
			Branches = ETGOR_EventEnumeration::Running;
		}
	}
}

void UTGOR_EventModule::ConsumeEvent(TSubclassOf<UTGOR_Event> Event, ETGOR_EventEnumeration& Branches)
{
	Branches = ETGOR_EventEnumeration::Dead;
	for (int32 Index = 0; Index < Events.Num(); Index++)
	{
		if (Activated[Index] && Events[Index]->IsA(Event))
		{
			// TODO: It's possible clients call events multiple times, usually when the server does no consume their own events
			Activated[Index] = false;
			Branches = ETGOR_EventEnumeration::Running;

			// Only prompted changes should mark things dirty
			//MarkDirty();
		}
	}
}