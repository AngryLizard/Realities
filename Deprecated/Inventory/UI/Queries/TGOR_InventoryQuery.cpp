// The Gateway of Realities: Planes of Existence.


#include "TGOR_InventoryQuery.h"

#include "Base/Instances/Inventory/TGOR_InventoryInstance.h"
#include "Base/Controller/TGOR_PlayerController.h"
#include "UI/Queries/TGOR_InventoryQuery.h"

UTGOR_InventoryQuery::UTGOR_InventoryQuery()
	: Super()
{

}


TArray<FTGOR_Display> UTGOR_InventoryQuery::QueryDisplays() const
{
	TArray<FTGOR_Display> Displays;

	return(Displays);
}


void UTGOR_InventoryQuery::CallIndex(int32 Index)
{

}

float UTGOR_InventoryQuery::GetHotkeyQuantity(const FTGOR_SlotIndex& Index) const
{
	if (IsValid(ContainerOwner))
	{
		if (ContainerOwner->IsInputKeyDown(EKeys::LeftControl)) return(1.0f);

		ETGOR_FetchEnumeration State;
		FTGOR_ItemInstance_OLD Instance = SourceContainer->GetSlot(Index, State);
		if (ContainerOwner->IsInputKeyDown(EKeys::LeftShift)) return(Instance.Quantity / 2);
	}
	return (0.0f);
}

void UTGOR_InventoryQuery::DropIndex(const FTGOR_SlotIndex& Index, const FVector& Location)
{
	if (IsValid(SourceContainer) && IsValid(ContainerOwner))
	{
		const float Quantity = GetHotkeyQuantity(Index);
		//ContainerOwner->Drop(SourceContainer, Index, Location, Quantity);
	}
}

void UTGOR_InventoryQuery::SwapIndex(const FTGOR_SlotIndex& Index, UTGOR_InventoryQuery* Target, const FTGOR_SlotIndex& TargetIndex)
{
	if (IsValid(SourceContainer) && IsValid(ContainerOwner))
	{
		const float Quantity = GetHotkeyQuantity(Index);
		UTGOR_ContainerComponent* Inventory = Target->SourceContainer;
		//ContainerOwner->Swap(SourceContainer, Index, Inventory, TargetIndex, Quantity);
	}
}

void UTGOR_InventoryQuery::Assign(ATGOR_PlayerController* Owner, UTGOR_ContainerComponent* Container, const FTGOR_SlotIndex& Index)
{
	ContainerOwner = Owner;
	SourceContainer = Container;
	SourceIndex = Index;
}

