// The Gateway of Realities: Planes of Existence.

#include "TGOR_ProcessComponent.h"

#include "InventorySystem/Content/TGOR_Process.h"
#include "InventorySystem/Content/TGOR_Item.h"
#include "DimensionSystem/Components/TGOR_PilotComponent.h"
#include "InventorySystem/Tasks/TGOR_ProcessTask.h"

#include "CoreSystem/TGOR_Singleton.h"
#include "CoreSystem/Gameplay/TGOR_GameState.h"
#include "Net/UnrealNetwork.h"


////////////////////////////////////////////////////////////////////////////////////////////////////


UTGOR_ProcessComponent::UTGOR_ProcessComponent()
	: Super(),
	ProcessingRate(2.0f),
	ProcessAntimatter(2.0f)
{
	PrimaryComponentTick.bCanEverTick = true;
	SetIsReplicatedByDefault(true);
}

void UTGOR_ProcessComponent::BeginPlay()
{
	Super::BeginPlay();

}

void UTGOR_ProcessComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	SINGLETON_CHK;
	const FTGOR_Time Timestamp = Singleton->GetGameTimestamp();

	// Check whether requirements are met
	if (HasRunningProcess())
	{
		// Generate Antimatter
		const float Antimatter = (Timestamp - LastProcessing) * ProcessingRate;
		if (Antimatter > ProcessAntimatter)
		{
			/*
			LastProcessing = Timestamp;
			if (ProcessState.Storage->Process(this, Antimatter))
			{
				ProcessState.Update(this, DeltaTime);
				ProcessState.Post();

				OnAntimatterGenerate.Broadcast(Antimatter);
			}
			else
			{
				ProcessState.Storage = nullptr;
				OnAntimatterOutlet.Broadcast(Antimatter);
			}
			*/
		}
	}
}

void UTGOR_ProcessComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(UTGOR_ProcessComponent, ProcessSlots, COND_None);
	DOREPLIFETIME_CONDITION(UTGOR_ProcessComponent, ProcessAntimatter, COND_None);
	DOREPLIFETIME_CONDITION(UTGOR_ProcessComponent, LastProcessing, COND_None);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void UTGOR_ProcessComponent::UpdateContent_Implementation(FTGOR_SpawnerDependencies& Dependencies)
{
	TMap<UTGOR_Process*, TArray<UTGOR_ProcessTask*>> Previous;

	// Remove slots but cache both instances and Processs in case the new loadout can use them
	for (int Slot = 0; Slot < ProcessSlots.Num(); Slot++)
	{
		UTGOR_ProcessTask* ProcessSlot = ProcessSlots[Slot];
		if (IsValid(ProcessSlot))
		{
			Previous.FindOrAdd(ProcessSlot->GetProcess()).Add(ProcessSlot);
		}
	}

	ProcessSlots.Empty();

	// Get installed Process modes
	TArray<UTGOR_Process*> Processs = Dependencies.Spawner->GetMListFromType<UTGOR_Process>(SpawnProcesses);

	// Get all candidates that are part of the movement queue
	for (UTGOR_Process* Process : Processs)
	{
		// Get slot from cache or create one
		UTGOR_ProcessTask* ProcessSlot = nullptr;

		TArray<UTGOR_ProcessTask*>* Ptr = Previous.Find(Process);
		if (Ptr && Ptr->Num() > 0)
		{
			ProcessSlot = Ptr->Pop();
		}
		else
		{
			// No cache was found, create a new one
			ProcessSlot = Process->CreateProcessTask(this, ProcessSlots.Num());
		}

		// Initialise and add to slots
		if (IsValid(ProcessSlot))
		{
			ProcessSlots.Add(ProcessSlot);
		}
	}

	// Discard tasks that got removed
	for (const auto& Pair : Previous)
	{
		for (UTGOR_ProcessTask* ProcessSlot : Pair.Value)
		{
			ProcessSlot->MarkAsGarbage();
		}
	}
}

TMap<int32, UTGOR_SpawnModule*> UTGOR_ProcessComponent::GetModuleType_Implementation() const
{
	TMap<int32, UTGOR_SpawnModule*> Modules;

	const int32 Num = ProcessSlots.Num();
	for (UTGOR_ProcessTask* ProcessSlot : ProcessSlots)
	{
		Modules.Emplace(ProcessSlot->Identifier.Slot, ProcessSlot->Identifier.Content);
	}
	return Modules;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void UTGOR_ProcessComponent::ScheduleProcess(UTGOR_Process* Process)
{
	/*
	SINGLETON_CHK;
	if (IsValid(Process) && Process->CanProcess(this, nullptr))
	{
		ProcessState.SetupStorage(Process);
		LastProcessing = Singleton->GetGameTimestamp();
		OnAntimatterOutlet.Broadcast(0.0f);
	}
	else
	{
		CancelProcess();
	}
	*/
}

void UTGOR_ProcessComponent::CancelProcess()
{
	/*
	SINGLETON_CHK;
	const FTGOR_Time Timestamp = Singleton->GetGameTimestamp();
	const float Antimatter = (Timestamp - LastProcessing) * ProcessingRate;
	if (IsValid(ProcessState.Storage))
	{
		ProcessState.Storage = nullptr;
		OnAntimatterOutlet.Broadcast(FMath::Max(Antimatter, 0.0f));
		LastProcessing = Timestamp;
	}
	*/
}

bool UTGOR_ProcessComponent::HasRunningProcess() const
{
	return false;// IsValid(ProcessState.Storage);
}

bool UTGOR_ProcessComponent::IsRunningProcess(TSubclassOf<UTGOR_Process> Type) const
{
	return false; // HasRunningProcess() && ProcessState.Storage->GetProcess()->IsA(Type);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

TArray<UTGOR_ProcessTask*> UTGOR_ProcessComponent::GetProcessListOfType(TSubclassOf<UTGOR_ProcessTask> Type) const
{
	TArray<UTGOR_ProcessTask*> Processes;
	if (*Type)
	{
		for (UTGOR_ProcessTask* ProcessSlot : ProcessSlots)
		{
			if (ProcessSlot->IsA(Type))
			{
				Processes.Emplace(ProcessSlot);
			}
		}
	}
	return Processes;
}
