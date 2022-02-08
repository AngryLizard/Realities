// The Gateway of Realities: Planes of Existence.


#include "TGOR_ActionDebugQuery.h"

#include "ActionSystem/Content/TGOR_Action.h"
#include "ActionSystem/Components/TGOR_ActionComponent.h"

UTGOR_ActionDebugQuery::UTGOR_ActionDebugQuery()
{
}

TArray<FTGOR_Display> UTGOR_ActionDebugQuery::QueryDisplays() const
{
	TArray<FTGOR_Display> Displays;
	for (const FTGOR_ActionDebugInfo& Entry : Entries)
	{
		if (IsValid(Entry.Action))
		{
			Displays.Emplace(Entry.Action->GetDisplay());
		}
	}
	return Displays;
}

void UTGOR_ActionDebugQuery::AssignComponent(UTGOR_ActionComponent* Component, float Duration)
{
	Entries.Empty();
	if (IsValid(Component))
	{
		Component->CollectDebugInfos(Duration, Entries);
	}
}
