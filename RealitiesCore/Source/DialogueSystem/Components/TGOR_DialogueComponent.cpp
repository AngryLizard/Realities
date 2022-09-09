// The Gateway of Realities: Planes of Existence.

#include "TGOR_DialogueComponent.h"
#include "RealitiesUtility/Utility/TGOR_Error.h"
#include "RealitiesUtility/Utility/TGOR_Math.h"
#include "DimensionSystem/Content/TGOR_Spawner.h"

#include "DialogueSystem/Tasks/TGOR_DialogueTask.h"
#include "DialogueSystem/Content/TGOR_Dialogue.h"

#include "DrawDebugHelpers.h"
#include "Net/UnrealNetwork.h"

UTGOR_DialogueComponent::UTGOR_DialogueComponent()
	: Super()
{
	PrimaryComponentTick.bCanEverTick = true;
	SetIsReplicatedByDefault(true);
}

void UTGOR_DialogueComponent::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// Replicate to everyone
	DOREPLIFETIME_CONDITION(UTGOR_DialogueComponent, DialogueSlots, COND_None); // This MUST be replicated before setup
	DOREPLIFETIME_CONDITION(UTGOR_DialogueComponent, DialogueTaskState, COND_None);
}

TSet<UTGOR_CoreContent*> UTGOR_DialogueComponent::GetActiveContent_Implementation() const
{
	TSet<UTGOR_CoreContent*> ContentContext = ITGOR_RegisterInterface::GetActiveContent_Implementation();

	UTGOR_DialogueTask* CurrentTask = GetDialogueTask();
	if (IsValid(CurrentTask))
	{
		ContentContext.Emplace(CurrentTask->GetDialogue());
	}
	return ContentContext;
}

void UTGOR_DialogueComponent::UpdateContent_Implementation(FTGOR_SpawnerDependencies& Dependencies)
{
	Super::UpdateContent_Implementation(Dependencies);

	Dependencies.Process<UTGOR_AnimationComponent>();

	UTGOR_DialogueTask* CurrentTask = GetDialogueTask();
	TMap<UTGOR_Dialogue*, TArray<UTGOR_DialogueTask*>> Previous;

	// Remove slots but cache both instances and items in case the new loadout can use them
	for (int Slot = 0; Slot < DialogueSlots.Num(); Slot++)
	{
		UTGOR_DialogueTask* DialogueSlot = DialogueSlots[Slot];
		if (IsValid(DialogueSlot))
		{
			Previous.FindOrAdd(DialogueSlot->GetDialogue()).Add(DialogueSlot);
		}
	}

	DialogueSlots.Empty();

	// Get installed Dialogue modes
	TArray<UTGOR_Dialogue*> Dialogues = Dependencies.Spawner->GetMListFromType<UTGOR_Dialogue>(SpawnDialogues);

	// Get all candidates that are part of the Dialogue queue
	for (UTGOR_Dialogue* Dialogue : Dialogues)
	{
		// Get slot from cache or create one
		UTGOR_DialogueTask* DialogueSlot = nullptr;

		TArray<UTGOR_DialogueTask*>* Ptr = Previous.Find(Dialogue);
		if (Ptr && Ptr->Num() > 0)
		{
			DialogueSlot = Ptr->Pop();
		}
		else
		{
			// No cache was found, create a new one
			DialogueSlot = Dialogue->CreateDialogueTask(this, DialogueSlots.Num());
		}

		// Initialise and add to slots
		if (IsValid(DialogueSlot))
		{
			DialogueSlots.Add(DialogueSlot);
		}
	}

	// Schedule with current if available or reset
	const int32 ActiveSlot = DialogueSlots.Find(CurrentTask);
	if (DialogueSlots.IsValidIndex(ActiveSlot))
	{
		StartDialogueWith(ActiveSlot);
	}
	else
	{
		StartDialogueWith(INDEX_NONE);
	}

	// Discard tasks that got removed
	for (const auto& Pair : Previous)
	{
		for (UTGOR_DialogueTask* DialogueSlot : Pair.Value)
		{
			DialogueSlot->MarkAsGarbage();
		}
	}
}

TMap<int32, UTGOR_SpawnModule*> UTGOR_DialogueComponent::GetModuleType_Implementation() const
{
	TMap<int32, UTGOR_SpawnModule*> Modules = Super::GetModuleType_Implementation();

	const int32 Num = DialogueSlots.Num();
	for (UTGOR_DialogueTask* DialogueSlot : DialogueSlots)
	{
		Modules.Emplace(DialogueSlot->Identifier.Slot, DialogueSlot->Identifier.Content);
	}
	return Modules;
}

TSubclassOf<UTGOR_Performance> UTGOR_DialogueComponent::GetPerformanceType() const
{
	return PerformanceType;
}

UTGOR_AnimationComponent* UTGOR_DialogueComponent::GetAnimationComponent() const
{
	AActor* Actor = GetOwner();
	return Actor->FindComponentByClass<UTGOR_AnimationComponent>();
}

////////////////////////////////////////////////////////////////////////////////////////////////////

UTGOR_DialogueTask* UTGOR_DialogueComponent::GetDialogueTask() const
{
	if (DialogueSlots.IsValidIndex(DialogueTaskState.ActiveSlot))
	{
		return DialogueSlots[DialogueTaskState.ActiveSlot];
	}
	return nullptr;
}

bool UTGOR_DialogueComponent::HasDialogueWith(int32 Identifier) const
{
	return DialogueTaskState.ActiveSlot == Identifier;
}

void UTGOR_DialogueComponent::StartDialogueWith(int32 Identifier)
{
	if (Identifier != DialogueTaskState.ActiveSlot)
	{
		if (UTGOR_DialogueTask* Task = GetDialogueTask())
		{
			Task->Interrupt();
		}

		DialogueTaskState.ActiveSlot = Identifier;

		if (UTGOR_DialogueTask* Task = GetDialogueTask())
		{
			Task->PrepareStart();
		}
	}
}

UTGOR_DialogueTask* UTGOR_DialogueComponent::GetCurrentDialogueOfType(TSubclassOf<UTGOR_DialogueTask> Type) const
{
	if (DialogueSlots.IsValidIndex(DialogueTaskState.ActiveSlot) && DialogueSlots[DialogueTaskState.ActiveSlot]->IsA(Type))
	{
		return DialogueSlots[DialogueTaskState.ActiveSlot];
	}
	return nullptr;
}

TArray<UTGOR_DialogueTask*> UTGOR_DialogueComponent::GetDialogueListOfType(TSubclassOf<UTGOR_DialogueTask> Type) const
{
	TArray<UTGOR_DialogueTask*> Dialogues;
	if (*Type)
	{
		for (UTGOR_DialogueTask* DialogueSlot : DialogueSlots)
		{
			if (DialogueSlot->IsA(Type))
			{
				Dialogues.Emplace(DialogueSlot);
			}
		}
	}
	return Dialogues;
}

void UTGOR_DialogueComponent::RepNotifyDialogueTaskState(const FTGOR_DialogueState& Old)
{
}