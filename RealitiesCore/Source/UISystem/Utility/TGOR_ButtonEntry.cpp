// The Gateway of Realities: Planes of Existence.

#include "TGOR_ButtonEntry.h"
#include "UISystem/Queries/TGOR_SelectionQuery.h"

UTGOR_ButtonEntry::UTGOR_ButtonEntry(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer), OpenedQuery(nullptr), OpenedIndex(INDEX_NONE), IsAutomatic(true)
{
}

void UTGOR_ButtonEntry::NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseEnter(InGeometry, InMouseEvent);
	
	if (GetIsEnabled() && IsAutomatic)
	{
		ProcessHover();
	}
}

void UTGOR_ButtonEntry::NativeOnMouseLeave(const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseLeave(InMouseEvent);

	if (GetIsEnabled() && IsAutomatic)
	{
		ProcessLeave();
	}
}

FReply UTGOR_ButtonEntry::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);

	if (GetIsEnabled() && IsAutomatic)
	{
		ProcessPressed();
		return FReply::Handled();
	}
	return FReply::Unhandled();
}

FReply UTGOR_ButtonEntry::NativeOnMouseButtonUp(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseButtonUp(InGeometry, InMouseEvent);

	if (GetIsEnabled() && IsAutomatic)
	{
		ProcessUnpressed();
		return FReply::Handled();
	}
	return FReply::Unhandled();
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void UTGOR_ButtonEntry::ProcessPressed()
{
	VisualisePressed();
	if (IsValid(OpenedQuery))
	{
		OpenedQuery->Commit();
	}
}

void UTGOR_ButtonEntry::ProcessUnpressed()
{
	VisualiseHover();
}

void UTGOR_ButtonEntry::ProcessLeave()
{
	VisualiseLeave();
}

void UTGOR_ButtonEntry::ProcessHover()
{
	if (IsValid(OpenedQuery))
	{
		OpenedQuery->CallIndex(OpenedIndex);
	}
	VisualiseHover();
}

////////////////////////////////////////////////////////////////////////////////////////////////////

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
