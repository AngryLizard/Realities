// The Gateway of Realities: Planes of Existence.


#include "TGOR_BodypartListWidget.h"

#include "Realities/Mod/Bodyparts/TGOR_Bodypart.h"
#include "Realities/Mod/Customisations/TGOR_Customisation.h"
#include "Realities/Base/Creature/TGOR_BodypartNode.h"
#include "Realities/UI/Creature/TGOR_BodypartCustomisationWidget.h"
#include "Realities/UI/Creature/TGOR_BodypartSelectionWidget.h"
#include "Realities/UI/Queries/Creature/TGOR_BodypartGradientQuery.h"
#include "Realities/UI/Queries/Creature/TGOR_BodypartSkinQuery.h"
#include "Realities/UI/Queries/Creature/TGOR_BodypartListQuery.h"
#include "Realities/Base/Instances/Creature/TGOR_BodypartInstance.h"
#include "Realities/Base/Content/TGOR_ContentManager.h"
#include "Realities/Actors/Pawns/TGOR_Pawn.h"
#include "Realities/Base/TGOR_Singleton.h"


UTGOR_BodypartListWidget::UTGOR_BodypartListWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer), BodypartNode(nullptr), ContentListOpened(false)
{}

////////////////////////////////////////////////////////////////////////////////////////////////////

void UTGOR_BodypartListWidget::SetupFromBodypart(UTGOR_BodypartNode* Node)
{
	SINGLETON_CHK;
	UTGOR_ContentManager* ContentManager = Singleton->GetContentManager();
		
	if (!IsValid(Node))
	{
		ERROR("No Node defined", Error)
	}

	UTGOR_Bodypart* NodeBodypart = Node->GetBodypart();
	if (!IsValid(NodeBodypart))
	{
		ERROR("No bodypart defined", Error)
	}

	// Store node reference
	BodypartNode = Node;

	UTGOR_BodypartNode* Parent = Node->GetParent();
	const bool IsRoot = !IsValid(Parent);
	
	// Clear all customisation widgets
	ClearCustomisationBodypartWidgets();
	MaterialWidgets.Empty();
	SelectionWidgets.Empty();
	
	// Create all material customisation widgets
	const TArray<FTGOR_BodypartCustomisationMapping>& Customisation = NodeBodypart->Customisation;
	const int32 CustomisationNum = Customisation.Num();
	for (uint8 i = 0; i < CustomisationNum; i++)
	{
		const FTGOR_BodypartCustomisationMapping& Mapping = Customisation[i];
		
		UTGOR_Customisation* Content = ContentManager->GetTFromType<UTGOR_Customisation>(Mapping.Type);
		if (IsValid(Content) && *Content->Widget)
		{
			UTGOR_BodypartCustomisationWidget* BodypartWidget = CreateWidget<UTGOR_BodypartCustomisationWidget>(this, Content->Widget);
			if (IsValid(BodypartWidget))
			{
				BodypartWidget->Customisation = Content;
				BodypartWidget->Connect(this, Node, i);
				MaterialWidgets.Add(BodypartWidget);

				bool PayloadFound = false;
				const FTGOR_BodypartInstance& BodypartInstance = Node->GetInstance();
				for (const FTGOR_CustomisationInstance& Instance : BodypartInstance.CustomisationInstances)
				{
					if (Instance.Index == i)
					{
						BodypartWidget->SetData(Instance.Payload, true);
						PayloadFound = true;
					}
				}

				// Query payload if none exists
				if (!PayloadFound)
				{
					BodypartWidget->SetData(Content->CreatePayload(), false);
				}
			}
			AddBodypartWidget(BodypartWidget);
		}
	}

	// Create all bodypart selection buttons
	for (const FTGOR_BodypartChild& Child : NodeBodypart->Children)
	{
		if (Child.Type != ETGOR_BodypartChildType::Automatic)
		{
			bool Active = Node->ContainsType(Child.Class);

			UTGOR_BodypartSelectionWidget* BodypartWidget = CreateBodypartSelectionWidget();
			if (IsValid(BodypartWidget))
			{
				UTGOR_Bodypart* Bodypart = ContentManager->GetTFromType<UTGOR_Bodypart>(Child.Class);
				if (IsValid(Bodypart))
				{
					BodypartWidget->EntryType = Child.Class;
					BodypartWidget->BodypartType = Bodypart;
					BodypartWidget->Connect(this, Node, SelectionWidgets.Num());
					SelectionWidgets.Add(BodypartWidget);

					BodypartWidget->SetQuery(Child.Type == ETGOR_BodypartChildType::Required);
					BodypartWidget->SetData(Child.Type == ETGOR_BodypartChildType::Required, Active);
				}
				else
				{
					ERROR("Bodypart Category not found", Error)
				}

				AddBodypartWidget(BodypartWidget);
			}
		}
	}

	SetInterface(!IsValid(Parent));
}


bool UTGOR_BodypartListWidget::GoToParent()
{
	if (IsValid(BodypartNode))
	{
		UTGOR_BodypartNode* Parent = BodypartNode->GetParent();
		if (IsValid(Parent))
		{
			SetupFromBodypart(Parent);
		}
		return(true);
	}

	return(false);
}


void UTGOR_BodypartListWidget::OpenContentList(UTGOR_ContentQuery * Query)
{
	ContentListOpened = true;
	OnOpenContentList(Query);
}

void UTGOR_BodypartListWidget::CloseContentList()
{
	ContentListOpened = false;
	OnCloseContentList();
}

////////////////////////////////////////////////////////////////////////////////////////////////////