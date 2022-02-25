// The Gateway of Realities: Planes of Existence.

#include "TGOR_ActionSlotWidget.h"
#include "Base/Inventory/TGOR_Storage.h"
#include "UI/Queries/Inventory/TGOR_ActionSlotQuery.h"

UTGOR_ActionSlotWidget::UTGOR_ActionSlotWidget()
	: ActionSlotQuery(nullptr)
{
}

UTGOR_Storage* UTGOR_ActionSlotWidget::GetItem() const
{
	if (IsValid(ActionSlotQuery) && IsValid(ActionSlotQuery->OwnerComponent))
	{
		return ActionSlotQuery->OwnerComponent->GetSlot(SlotIndex);
	}
	return nullptr;
}


void UTGOR_ActionSlotWidget::Assign(UTGOR_ActionSlotQuery* Query, const FTGOR_SlotIndex& Item)
{
	SlotIndex = Item;
	ActionSlotQuery = Query;
	if (IsValid(ActionSlotQuery) && IsValid(ActionSlotQuery->OwnerComponent))
	{
		UTGOR_Action* Action = ActionSlotQuery->OwnerComponent->GetAction(Item);
		VisualiseAction(Action);

		UTGOR_Storage* Storage = ActionSlotQuery->OwnerComponent->GetSlot(Item);
		if (IsValid(Storage))
		{
			VisualiseItem(Storage->GetItem());
		}
		else
		{
			VisualiseItem(nullptr);
		}
	}
}

void UTGOR_ActionSlotWidget::SelectItem()
{
	if (IsValid(ActionSlotQuery))
	{
		ActionSlotQuery->CallIndex((int32)SlotIndex.Slot);
	}
}

void UTGOR_ActionSlotWidget::CommitItem()
{
	if (IsValid(ActionSlotQuery))
	{
		ActionSlotQuery->Commit();
	}
}
