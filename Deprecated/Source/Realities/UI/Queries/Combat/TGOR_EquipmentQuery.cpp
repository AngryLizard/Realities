// The Gateway of Realities: Planes of Existence.


#include "TGOR_EquipmentQuery.h"
#include "Realities/Mod/Actions/TGOR_Action.h"
#include "Realities/Components/System/Actions/TGOR_ActionComponent.h"

UTGOR_EquipmentQuery::UTGOR_EquipmentQuery()
	: Super()
{
}

TArray<UTGOR_Content*> UTGOR_EquipmentQuery::QueryContent() const
{
	return MigrateContentArray(Actions);
}

void UTGOR_EquipmentQuery::Assign(UTGOR_ActionComponent* ActionComponent, TSubclassOf<UTGOR_Action> Type)
{
	if (!IsValid(ActionComponent)) return;

	// Get container and valid slot, default to first
	const FTGOR_ContainerIndex Container = ActionComponent->GetContainerTypeIndex(Type);
	const FTGOR_SlotIndex ActiveSlot = ActionComponent->GetContainerLastSlot(Container);
	if (ActiveSlot.IsValid())
	{
		CallIndex(ActiveSlot.Slot);
	}
	else
	{
		CallIndex(0);
	}

	// Only fail if there are no actions to go by
	const int32 Size = ActionComponent->GetContainerSize(Container);
	if (Size > 0)
	{
		// Grab rest
		Actions.Empty(Size - 1);
		for (uint8 Index = 0; Index < Size; Index++)
		{
			// Shift forward to always show the next weapon in line first
			const FTGOR_SlotIndex Slot = FTGOR_SlotIndex(Container, Index);
			UTGOR_Action* Action = ActionComponent->GetAction(Slot);
			if (ActionComponent->Execute_SupportsContent(ActionComponent, Action))
			{
				Actions.Emplace(Action);
			}
		}
	}
	else
	{
		Actions.Empty();
	}
}

UTGOR_Action* UTGOR_EquipmentQuery::GetSelectedAction() const
{
	if (Actions.IsValidIndex(Selection))
	{
		return Actions[Selection];
	}
	return nullptr;
}
