#include "TGOR_StorageContainerMenu.h"

#include "Mod/Spawner/Items/TGOR_Item.h"
#include "Base/Inventory/TGOR_InventoryStorage.h"
#include "Base/Inventory/TGOR_DefaultStorage.h"
#include "UI/Inventory/TGOR_StorageSlotWidget.h"
#include "UMG/Public/Components/PanelWidget.h"

UTGOR_StorageContainerMenu::UTGOR_StorageContainerMenu()
	: Super(),
	ItemComponent(nullptr)
{
}

void UTGOR_StorageContainerMenu::Initzialize(TSubclassOf<UTGOR_StorageSlotWidget> SlotWidget, UTGOR_ItemComponent* NewComponent, UPanelWidget* NewContainerWidget)
{
	if (!SlotWidget) ERROR("Slot widget is not valid!", Error)
	if (!NewComponent) ERROR("Item component is not valid!", Error)
	if (!IsValid(NewComponent->GetItemStorage())) ERROR("Item component have no valid item storage!", Error)
	if (NewComponent->GetItemStorage()->IsA(UTGOR_DefaultStorage::StaticClass())) ERROR("The item instance of the item component has no valid storage!", Error)

	this->ItemComponent = NewComponent;
	this->SlotWidgetClass = SlotWidget;
	this->ContainerWidget = NewContainerWidget;

	RebuildSlots();
}

void UTGOR_StorageContainerMenu::RebuildSlots()
{
	Slots.Empty();

	if (ContainerWidget)
	{
		ContainerWidget->ClearChildren();
	}

	/* TODO: Not implemented anymore 
	for (int32 i = 0; i < ItemComponent->GetItemStorage()->GetCurrentSize(); i++)
	{
		auto NewSlot = CreateWidget<UTGOR_StorageSlotWidget>(this, SlotWidgetClass);

		NewSlot->ParentContainer = this;
		NewSlot->Index = i;
		Slots.Add(NewSlot);

		if (ContainerWidget)
		{
			ContainerWidget->AddChild(NewSlot);
		}
	}
	*/
}

bool UTGOR_StorageContainerMenu::RemoveSlotAt(int32 Index)
{
	if (Slots.IsValidIndex(Index))
	{
		Slots.RemoveAt(Index);
		return true;
	}

	return false;
}

bool UTGOR_StorageContainerMenu::RemoveItemAndSlotAt(int32 Index)
{
	bool success;

	success = RemoveSlotAt(Index);
	/* TODO: Not implemented anymore
	success &= ItemComponent->GetItemStorage()->RemoveAt(Index);
	*/

	return success;
}

UTGOR_StorageSlotWidget* UTGOR_StorageContainerMenu::GetSlotAt(int32 Index)
{
	if (Slots.IsValidIndex(Index))
	{
		return Slots[Index];
	}

	return nullptr;
}

UTGOR_Storage* UTGOR_StorageContainerMenu::GetStorage()
{
	if (IsValid(ItemComponent))
	{
		return ItemComponent->GetItemStorage();
	}

	return nullptr;
}

const TArray<UTGOR_StorageSlotWidget*>& UTGOR_StorageContainerMenu::GetSlots() const
{
	return Slots;
}
