// The Gateway of Realities: Planes of Existence.

#include "TGOR_ActionSlotQuery.h"
#include "CoreSystem/Content/TGOR_CoreContent.h"
#include "InventorySystem/Content/TGOR_Item.h"
#include "ActionSystem/Content/TGOR_Action.h"

#include "ActionSystem/Components/TGOR_ActionComponent.h"

UTGOR_ActionSlotQuery::UTGOR_ActionSlotQuery()
	: Super()
{
}

TArray<UTGOR_CoreContent*> UTGOR_ActionSlotQuery::QueryContent() const
{
	TArray<UTGOR_CoreContent*> Contents;
	if (IsValid(OwnerComponent))
	{
		for (int32 Identifier : SlotIdentifiers)
		{
			Contents.Emplace(OwnerComponent->GetAction(Identifier));
		}
	}
	return Contents;
}

void UTGOR_ActionSlotQuery::AssignComponent(UTGOR_ActionComponent* ActionComponent, TSubclassOf<UTGOR_Action> Type, const FTGOR_AimInstance& Aim, bool CheckCanCall)
{
	OwnerComponent = ActionComponent;

	if (IsValid(OwnerComponent))
	{
		// Get callables
		SlotIdentifiers = ActionComponent->GetCallableActionIdentifiers(Type, Aim, CheckCanCall);

		// Call and initialise initial selection
		InitialiseSelection(0);
	}
}