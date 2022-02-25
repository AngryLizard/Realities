// The Gateway of Realities: Planes of Existence.


#include "TGOR_InteractionQuery.h"

#include "Realities/Mod/Actions/TGOR_Action.h"
#include "Realities/Utility/TGOR_CustomEnumerations.h"
#include "Realities/Mod/Spawner/Creatures/TGOR_Creature.h"
#include "Realities/Actors/Pawns/TGOR_Avatar.h"

#include "Realities/Components/System/Actions/TGOR_ActionComponent.h"

UTGOR_InteractionQuery::UTGOR_InteractionQuery()
	: Super()
{
}

TArray<FTGOR_Display> UTGOR_InteractionQuery::QueryDisplays() const
{
	TArray<FTGOR_Display> Displays;
	for (const FTGOR_InteractionEntry& Entry : Entries)
	{
		Displays.Emplace(Entry.Display);
	}
	return(Displays);
}

void UTGOR_InteractionQuery::CallIndex(int32 Index)
{
	if (Entries.IsValidIndex(Index))
	{
		const FTGOR_InteractionEntry& Entry = Entries[Index];
		if (IsValid(Entry.Caller) && Entry.SlotIndex.IsValid() && Entry.Caller->IsLocallyControlled())
		{
			Entry.Caller->ScheduleSlotAction(Entry.SlotIndex);
		}
	}
	Super::CallIndex(Index);
}

void UTGOR_InteractionQuery::Emplace(UTGOR_ActionComponent* Component, TSubclassOf<UTGOR_Action> ActionClass)
{
	if (!IsValid(Component))
	{
		ERROR("No Interactable provided", Error)
	}

	// Iterate through all possible actions and find callable ones
	for (uint8 i = 0; i < Component->ActionContainers.Num(); i++)
	{
		const FTGOR_ActionContainer& Container = Component->ActionContainers[i];
		for (uint8 j = 0; j < Container.Slots.Num(); j++)
		{
			const FTGOR_ActionSlot& Slot = Container.Slots[j];
			if (IsValid(Slot.Action) && Slot.Action->IsA(ActionClass))
			{
				// Build Index
				const FTGOR_SlotIndex Index = FTGOR_SlotIndex(i, j);

				// Check whether this action can be called at all
				if (Slot.Action->CanCall(Component, Index))
				{
					// Build entry
					FTGOR_InteractionEntry Entry;
					Entry.Caller = Component;
					Entry.Display = Slot.Action->GetDisplay();
					Entry.SlotIndex = Index;
					Entries.Emplace(Entry);
				}
			}
		}
	}
}

void UTGOR_InteractionQuery::Reset()
{
	Entries.Empty();
}