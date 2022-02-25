// The Gateway of Realities: Planes of Existence.


#include "TGOR_BodypartWidget.h"

#include "Realities/Base/Creature/TGOR_BodypartNode.h"
#include "Realities/UI/Creature/TGOR_BodypartListWidget.h"

UTGOR_BodypartWidget::UTGOR_BodypartWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer), BodypartListWidget(nullptr), BodypartNode(nullptr), EntryIndex(-1)
{
}

UTGOR_BodypartListWidget* UTGOR_BodypartWidget::GetBodypartList() const
{
	return BodypartListWidget;
}

UTGOR_BodypartNode* UTGOR_BodypartWidget::GetBodypartNode() const
{
	return BodypartNode;
}

void UTGOR_BodypartWidget::OpenContentQuery(UTGOR_ContentQuery* Query)
{
	if (IsValid(BodypartListWidget))
	{
		BodypartListWidget->OpenContentList(Query);
	}
}

void UTGOR_BodypartWidget::Connect(UTGOR_BodypartListWidget* ListWidget, UTGOR_BodypartNode* Node, uint8 Index)
{
	BodypartListWidget = ListWidget;
	BodypartNode = Node;
	EntryIndex = Index;

	ConnectInit();
}