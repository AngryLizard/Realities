// The Gateway of Realities: Planes of Existence.

#include "TGOR_SingleItemComponent.h"
#include "Realities.h"

#include "Mod/Items/TGOR_Item.h"

UTGOR_SingleItemComponent::UTGOR_SingleItemComponent()
	: Super()
{
	ItemInstance.Content = nullptr;
}

void UTGOR_SingleItemComponent::InventoryUpdate()
{
	TickSlot(0, ItemInstance);
}

void UTGOR_SingleItemComponent::InventoryRebuild()
{
	AssignItem(ItemInstance);
}

void UTGOR_SingleItemComponent::ForEachSlot(std::function<bool(const FTGOR_SlotIndex& Index, FTGOR_ItemInstance_OLD& ItemInstance)> func)
{
	func(FTGOR_SlotIndex(0, 0), ItemInstance);
}

FTGOR_ItemInstance_OLD& UTGOR_SingleItemComponent::GetSlot(const FTGOR_SlotIndex& Index, ETGOR_FetchEnumeration& Branches)
{
	Branches = ETGOR_FetchEnumeration::Found;
	if (!Index.IsValid()) Branches = ETGOR_FetchEnumeration::Empty;

	return(ItemInstance);
}

void UTGOR_SingleItemComponent::Load_Implementation(const FTGOR_Buffer& Buffer)
{
	UTGOR_BufferManager::FillInstanceFromBuffer(Singleton, ItemInstance, Buffer);
}

bool UTGOR_SingleItemComponent::Store_Implementation(FTGOR_Buffer& Buffer)
{
	UTGOR_BufferManager::FillBufferFromInstance(Singleton, ItemInstance, Buffer);
	return(true);
}


void UTGOR_SingleItemComponent::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UTGOR_SingleItemComponent, ItemInstance);
}


void UTGOR_SingleItemComponent::AssignItem_Implementation(const FTGOR_ItemInstance_OLD& Instance)
{
	!ItemInstance = Instance;
}