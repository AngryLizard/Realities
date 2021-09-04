// The Gateway of Realities: Planes of Existence.

#include "TGOR_ProcessComponent.h"

#include "InventorySystem/Content/TGOR_Process.h"
#include "InventorySystem/Content/TGOR_Item.h"
#include "DimensionSystem/Components/TGOR_PilotComponent.h"
#include "InventorySystem/Storage/TGOR_ProcessStorage.h"

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
		}
	}
}

void UTGOR_ProcessComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(UTGOR_ProcessComponent, ProcessState, COND_None);
	DOREPLIFETIME_CONDITION(UTGOR_ProcessComponent, ProcessAntimatter, COND_None);
	DOREPLIFETIME_CONDITION(UTGOR_ProcessComponent, LastProcessing, COND_None);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void UTGOR_ProcessComponent::ScheduleProcess(UTGOR_Process* Process)
{
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
}

void UTGOR_ProcessComponent::CancelProcess()
{
	SINGLETON_CHK;
	const FTGOR_Time Timestamp = Singleton->GetGameTimestamp();
	const float Antimatter = (Timestamp - LastProcessing) * ProcessingRate;
	if (IsValid(ProcessState.Storage))
	{
		ProcessState.Storage = nullptr;
		OnAntimatterOutlet.Broadcast(FMath::Max(Antimatter, 0.0f));
		LastProcessing = Timestamp;
	}
}

bool UTGOR_ProcessComponent::HasRunningProcess() const
{
	return IsValid(ProcessState.Storage);
}

bool UTGOR_ProcessComponent::IsRunningProcess(TSubclassOf<UTGOR_Process> Type) const
{
	return HasRunningProcess() && ProcessState.Storage->GetProcess()->IsA(Type);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

