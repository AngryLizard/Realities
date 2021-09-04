// The Gateway of Realities: Planes of Existence.


#include "TGOR_InteractionQuery.h"

#include "ActionSystem/Content/TGOR_Action.h"
#include "ActionSystem/Components/TGOR_ActionComponent.h"

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
		if (IsValid(Entry.Caller) && Entry.Identifier != INDEX_NONE && Entry.Caller->IsLocallyControlled())
		{
			Entry.Caller->ScheduleSlotAction(Entry.Identifier);
		}
	}
	Super::CallIndex(Index);
}

void UTGOR_InteractionQuery::Emplace(UTGOR_ActionComponent* Component, UTGOR_Action* Action)
{
	if (!IsValid(Component))
	{
		ERROR("No Interactable provided", Error);
	}

	if (!IsValid(Action))
	{
		ERROR("No Action provided", Error);
	}

	FTGOR_InteractionRequest Request;
	Request.Caller = Component;
	Request.Action = Action;
	Requests.Emplace(Request);

}

void UTGOR_InteractionQuery::Update()
{
	Entries.Empty();

	for (const FTGOR_InteractionRequest& Request : Requests)
	{
		TArray<int32> SlotIdentifiers = Request.Caller->GetCallableSubactionIdentifiers(Request.Action, true);
		for (int32 Identifier : SlotIdentifiers)
		{
			// Build entry
			FTGOR_InteractionEntry Entry;
			Entry.Caller = Request.Caller;
			Entry.Display = Request.Caller->GetAction(Identifier)->GetDisplay();
			Entry.Identifier = Identifier;
			Entries.Emplace(Entry);
		}
	}
}