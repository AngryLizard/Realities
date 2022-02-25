// The Gateway of Realities: Planes of Existence.

#include "TGOR_ButtonEntry.h"
#include "Realities/UI/Queries/TGOR_SelectionQuery.h"


void UTGOR_ButtonEntry::NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseEnter(InGeometry, InMouseEvent);

	if (bIsEnabled)
	{
		if (IsValid(OpenedQuery))
		{
			OpenedQuery->CallIndex(OpenedIndex);
		}
		VisualiseHover();
	}
}

void UTGOR_ButtonEntry::NativeOnMouseLeave(const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseLeave(InMouseEvent);

	if (bIsEnabled)
	{
		VisualiseLeave();
	}
}

FReply UTGOR_ButtonEntry::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);

	if (bIsEnabled)
	{
		VisualisePressed();
		if (IsValid(OpenedQuery))
		{
			OpenedQuery->Commit();
		}
		return FReply::Handled();
	}
	return FReply::Unhandled();
}

FReply UTGOR_ButtonEntry::NativeOnMouseButtonUp(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseButtonUp(InGeometry, InMouseEvent);

	if (bIsEnabled)
	{
		VisualiseHover();
		return FReply::Handled();
	}
	return FReply::Unhandled();
}


void UTGOR_ButtonEntry::Assign(const FTGOR_Display& Display, UTGOR_SelectionQuery* Query, int32 Index)
{
	OpenedQuery = Query;
	OpenedIndex = Index;
	if (IsValid(Query))
	{
		OnAssign(Query, Index);
	}
	OnDisplay(Display);
}
