// The Gateway of Realities: Planes of Existence.


#include "TGOR_ChildListQuery.h"

#include "CustomisationSystem/Content/TGOR_Bodypart.h"
#include "CustomisationSystem/Components/TGOR_CustomisationComponent.h"
#include "CustomisationSystem/UI/TGOR_BodypartListWidget.h"
#include "CustomisationSystem/UI/TGOR_BodypartSelectionWidget.h"
#include "CustomisationSystem/Queries/TGOR_BodypartContentQuery.h"
#include "CustomisationSystem/TGOR_BodypartInstance.h"
#include "RealitiesUGC/Mod/TGOR_ContentManager.h"
#include "CoreSystem/TGOR_Singleton.h"



UTGOR_ChildListQuery::UTGOR_ChildListQuery()
	: Super()
{

}

TArray<UTGOR_CoreContent*> UTGOR_ChildListQuery::QueryContent() const
{
	TArray<UTGOR_CoreContent*> Output;
	for (const FTGOR_ChildListEntry& Entry : ConnectedChildren)
	{
		Output.Emplace(GetBodypartContent(Entry));
	}
	return Output;
}

void UTGOR_ChildListQuery::CommitContent(int32 Index)
{
	Super::CommitContent(Index);

	if (IsValid(CallbackWidget) && IsValid(CallbackWidget->OwnerComponent) && ConnectedChildren.IsValidIndex(Index))
	{
		const FTGOR_ChildListEntry& Entry = ConnectedChildren[Index];
		const int32 NodeIndex = CallbackWidget->OwnerComponent->GetFirstChildFromType(CallbackWidget->BodypartNodeIndex, Entry.Child.Class);
		if (NodeIndex != INDEX_NONE)
		{
			CallbackWidget->GoToNode(CallbackWidget->OwnerComponent, NodeIndex);
		}
	}
}

UTGOR_Bodypart* UTGOR_ChildListQuery::GetBodypartContent(const FTGOR_ChildListEntry& Entry) const
{
	SINGLETON_RETCHK(nullptr);
	if (IsValid(CallbackWidget) && IsValid(CallbackWidget->OwnerComponent))
	{
		const int32 NodeIndex = CallbackWidget->OwnerComponent->GetFirstChildFromType(CallbackWidget->BodypartNodeIndex, Entry.Child.Class);
		if (NodeIndex != INDEX_NONE)
		{
			return CallbackWidget->OwnerComponent->CurrentAppearance.Bodyparts[NodeIndex].Content;
		}
		else
		{
			UTGOR_ContentManager* ContentManager = Singleton->GetContentManager();
			return Cast<UTGOR_Bodypart>(ContentManager->FindFirstOfType(Entry.Child.Class));
		}
	}
	return nullptr;
}

void UTGOR_ChildListQuery::SetupFromParentNode(UTGOR_BodypartListWidget* BodypartListWidget)
{
	SINGLETON_CHK;

	if (!IsValid(BodypartListWidget))
	{
		ERROR("No callback defined", Error);
	}

	const FTGOR_BodypartInstance& Instance = BodypartListWidget->OwnerComponent->CurrentAppearance.Bodyparts[BodypartListWidget->BodypartNodeIndex];
	if (!IsValid(Instance.Content))
	{
		ERROR("No bodypart defined", Error);
	}

	// Store node reference
	CallbackWidget = BodypartListWidget;
	ConnectedChildren.Empty();

	// Create all bodypart selection buttons
	for (const FTGOR_BodypartChild& Child : Instance.Content->Children)
	{
		if (Child.Type != ETGOR_BodypartChildType::Automatic)
		{
			const int32 NodeIndex = BodypartListWidget->OwnerComponent->GetFirstChildFromType(BodypartListWidget->BodypartNodeIndex, Child.Class);
			const bool Active = (NodeIndex != INDEX_NONE);

			FTGOR_ChildListEntry Entry;
			Entry.Child = Child;
			Entry.Index = ConnectedChildren.Num();
			Entry.IsActive = Active;
			Entry.IsRequired = (Child.Type == ETGOR_BodypartChildType::Required);

			ConnectedChildren.Emplace(Entry);
		}
	}
}

