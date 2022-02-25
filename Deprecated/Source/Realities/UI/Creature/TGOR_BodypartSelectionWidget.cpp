// The Gateway of Realities: Planes of Existence.


#include "TGOR_BodypartSelectionWidget.h"

#include "Realities/Mod/Customisations/TGOR_Customisation.h"
#include "Realities/Mod/Bodyparts/TGOR_Bodypart.h"
#include "Realities/Base/Creature/TGOR_BodypartNode.h"

#include "Realities/UI/Queries/Creature/TGOR_BodypartListQuery.h"
#include "Realities/UI/Creature/TGOR_BodypartListWidget.h"
#include "Realities/UI/Creature/TGOR_BodypartCustomisationWidget.h"
#include "Realities/Base/Instances/Creature/TGOR_BodypartInstance.h"


UTGOR_BodypartSelectionWidget::UTGOR_BodypartSelectionWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer), BodypartType(nullptr)
{
	BodypartListQuery = ObjectInitializer.CreateDefaultSubobject<UTGOR_BodypartListQuery>(this, TEXT("BodypartListQuery"));
}

void UTGOR_BodypartSelectionWidget::SetQuery(bool Required)
{
	if (IsValid(BodypartNode))
	{
		// Get bodypart if available
		UTGOR_Bodypart* Parent = BodypartNode->GetBodypart();
		BodypartListQuery->SetPossibleBodyparts(this, Parent, EntryType, Required);
	}
}


UTGOR_BodypartNode* UTGOR_BodypartSelectionWidget::GetChildNode()
{
	if (IsValid(BodypartNode))
	{
		// Find bodypart that fits tag
		const TArray<UTGOR_BodypartNode*>& Children = BodypartNode->GetChildren();
		int32 Num = Children.Num();
		for (int i = Num - 1; i >= 0; i--)
		{
			UTGOR_BodypartNode* Node = Children[i];
			UTGOR_Bodypart* Bodypart = Node->GetBodypart();
			if (IsValid(Bodypart) && Bodypart->IsA(EntryType))
			{
				return(Node);
			}
		}
	}
	return(nullptr);
}

void UTGOR_BodypartSelectionWidget::RemoveBodypart()
{
	if (IsValid(BodypartNode))
	{
		// Get bodypart if available
		UTGOR_BodypartNode* Node = GetChildNode();
		if(IsValid(Node))
		{
			BodypartNode->RemoveChild(Node);

			// Update parent
			BodypartListWidget->BodypartUpdate(Node);
		}
	}

}

void UTGOR_BodypartSelectionWidget::OpenBodypart()
{
	if (IsValid(BodypartNode) && IsValid(BodypartListWidget))
	{
		UTGOR_BodypartNode* Node = GetChildNode();
		if (IsValid(Node))
		{
			// Get bodypart if available
			BodypartListWidget->SetupFromBodypart(Node);
			BodypartListWidget->BodypartOpen(Node);
		}
	}
}

void UTGOR_BodypartSelectionWidget::SelectBodypart()
{
	if (IsValid(BodypartListWidget))
	{
		BodypartListWidget->OpenContentList(BodypartListQuery);
	}
}

void UTGOR_BodypartSelectionWidget::ReplaceBodypart(UTGOR_Bodypart* Content)
{
	if (IsValid(BodypartNode) && IsValid(BodypartListWidget))
	{
		// Change or add bodypart to node
		UTGOR_BodypartNode* Node = GetChildNode();
		if (IsValid(Node))
		{
			Node->ChangeBodypart(Content);
		}
		else
		{
			BodypartNode->AddChildBodypart(Content);
		}

		// Update parent
		BodypartListWidget->BodypartUpdate(Node);
	}
}
