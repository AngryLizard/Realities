// The Gateway of Realities: Planes of Existence.


#include "TGOR_BodypartWidget.h"

#include "CustomisationSystem/UI/TGOR_BodypartListWidget.h"

UTGOR_BodypartWidget::UTGOR_BodypartWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer), BodypartListWidget(nullptr), EntryIndex(-1)
{
}

UTGOR_BodypartListWidget* UTGOR_BodypartWidget::GetBodypartList() const
{
	return BodypartListWidget;
}

int32 UTGOR_BodypartWidget::GetBodypartNodeIndex() const
{
	if (IsValid(BodypartListWidget))
	{
		return BodypartListWidget->BodypartNodeIndex;
	}
	return INDEX_NONE;
}

void UTGOR_BodypartWidget::OpenContentQuery(UTGOR_ContentQuery* Query)
{
	if (IsValid(BodypartListWidget))
	{
		BodypartListWidget->OpenContentList(Query);
	}
}

void UTGOR_BodypartWidget::Connect(UTGOR_BodypartListWidget* ListWidget, int32 Index)
{
	BodypartListWidget = ListWidget;
	EntryIndex = Index;

	ConnectInit();
}