// The Gateway of Realities: Planes of Existence.

#include "TGOR_BodypartListWidget.h"

#include "CustomisationSystem/Components/TGOR_CustomisationComponent.h"
#include "CustomisationSystem/Queries/TGOR_BodypartListQuery.h"
#include "CoreSystem/TGOR_Singleton.h"


UTGOR_BodypartListWidget::UTGOR_BodypartListWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer), BodypartNodeIndex(INDEX_NONE)
{
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void UTGOR_BodypartListWidget::GoToNode(UTGOR_CustomisationComponent* Component, int32 NodeIndex)
{
	// Store node reference
	BodypartNodeIndex = NodeIndex;
	OwnerComponent = Component;
	OnUpdateInterface();
}


bool UTGOR_BodypartListWidget::GoToParent()
{
	if (IsValid(OwnerComponent))
	{
		const int32 ParentIndex = OwnerComponent->GetParent(BodypartNodeIndex);
		if (ParentIndex != INDEX_NONE)
		{
			GoToNode(OwnerComponent, ParentIndex);
		}
		return true;
	}

	return false;
}


void UTGOR_BodypartListWidget::OpenContentList(UTGOR_ContentQuery * Query)
{
	OnOpenContentList(Query);
}

void UTGOR_BodypartListWidget::CloseContentList()
{
	OnCloseContentList();
}

void UTGOR_BodypartListWidget::OpenTransform(UTGOR_Query* Query)
{
	OnOpenTransform(Query);
}

void UTGOR_BodypartListWidget::CloseTransform()
{
	OnCloseTransform();
}

////////////////////////////////////////////////////////////////////////////////////////////////////