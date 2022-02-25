#include "TGOR_StorageSlotWidget.h"

#include "Realities.h"

UTGOR_StorageSlotWidget::UTGOR_StorageSlotWidget()
	: Super(),
	Index(-1)
{
}

void UTGOR_StorageSlotWidget::InitializeSlot(UTGOR_StorageContainerMenu* NewParentContainer, int32 NewIndex)
{
	ParentContainer = NewParentContainer;
	Index = NewIndex;
}

bool UTGOR_StorageSlotWidget::SwapContents(UTGOR_StorageSlotWidget* Other)
{
	if (IsValid(Other))
	{
		if (IsValid(ParentContainer) && IsValid(Other->ParentContainer))
		{
			int32 TempIndex = Other->Index;
			Other->Index = Index;
			Index = TempIndex;

			auto TempContainer = Other->ParentContainer;
			Other->ParentContainer = ParentContainer;
			ParentContainer = TempContainer;

			// TODO: Fire an update event?

			return true;
		}
	}

	return false;
}

int32 UTGOR_StorageSlotWidget::GetIndex() const
{
	return Index;
}

UTGOR_StorageContainerMenu* UTGOR_StorageSlotWidget::GetParentContainer()
{
	return ParentContainer;
}

