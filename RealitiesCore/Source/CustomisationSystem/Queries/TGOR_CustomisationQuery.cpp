// The Gateway of Realities: Planes of Existence.


#include "TGOR_CustomisationQuery.h"

#include "CustomisationSystem/Content/TGOR_Bodypart.h"
#include "CustomisationSystem/Content/Customisations/TGOR_Customisation.h"
#include "CustomisationSystem/Components/TGOR_CustomisationComponent.h"
#include "CustomisationSystem/UI/TGOR_BodypartListWidget.h"
#include "CustomisationSystem/UI/TGOR_BodypartSelectionWidget.h"
#include "CustomisationSystem/TGOR_BodypartInstance.h"
#include "CoreSystem/TGOR_Singleton.h"


UTGOR_CustomisationEntryQuery::UTGOR_CustomisationEntryQuery()
	: Super()
{

}

TArray<UTGOR_CoreContent*> UTGOR_CustomisationEntryQuery::QueryContent() const
{
	TArray<UTGOR_CoreContent*> Output;
	for (const FTGOR_CustomisationPayload& Entry : ConnectedCustomisations)
	{
		Output.Emplace(Customisation);
	}
	return Output;
}

void UTGOR_CustomisationEntryQuery::SetupFromBodypartNode(UTGOR_BodypartListWidget* BodypartListWidget, UTGOR_Customisation* Type, ETGOR_CustomisationQueryEnumeration Query)
{
	if (!IsValid(BodypartListWidget)) ERROR("No callback defined", Error);
	if (!IsValid(BodypartListWidget->OwnerComponent)) ERROR("No node defined", Error);
	if (!IsValid(Type)) ERROR("No type defined", Error);

	// Store node reference
	Customisation = Type;
	CallbackWidget = BodypartListWidget;

	ConnectedCustomisations.Empty();

	// Create all material customisation widgets
	const FTGOR_BodypartInstance& BodypartInstance = BodypartListWidget->OwnerComponent->CurrentAppearance.Bodyparts[BodypartListWidget->BodypartNodeIndex];
	if (IsValid(BodypartInstance.Content))
	{
		TArray<FTGOR_CustomisationInstance> Instances;
		BodypartListWidget->OwnerComponent->GetCustomisationMatching(BodypartListWidget->BodypartNodeIndex, Instances, Type, Query);

		for (const FTGOR_CustomisationInstance& Instance : Instances)
		{
			ConnectedCustomisations.Emplace(Instance.Payload);
		}
	}
}


UTGOR_CustomisationListQuery::UTGOR_CustomisationListQuery()
	: Super()
{

}

TArray<UTGOR_CoreContent*> UTGOR_CustomisationListQuery::QueryContent() const
{
	TArray<UTGOR_CoreContent*> Output;
	for (UTGOR_Customisation* ConnectedCustomisation : ConnectedCustomisations)
	{
		Output.Emplace(ConnectedCustomisation);
	}
	return Output;
}

void UTGOR_CustomisationListQuery::SetupFromBodypartNode(UTGOR_BodypartListWidget* BodypartListWidget, TSubclassOf<UTGOR_Customisation> Type)
{
	SINGLETON_CHK;
	UTGOR_ContentManager* ContentManager = Singleton->GetContentManager();

	if (!IsValid(BodypartListWidget)) ERROR("No callback defined", Error);
	if (!IsValid(BodypartListWidget->OwnerComponent)) ERROR("No node defined", Error);

	// Store node reference
	CustomisationType = Type;
	CallbackWidget = BodypartListWidget;

	ConnectedCustomisations.Empty();

	// Create all material customisation widgets
	const FTGOR_BodypartInstance& BodypartInstance = BodypartListWidget->OwnerComponent->CurrentAppearance.Bodyparts[BodypartListWidget->BodypartNodeIndex];
	if (IsValid(BodypartInstance.Content))
	{
		TArray<UTGOR_Customisation*> CustomList = BodypartInstance.Content->Instanced_CustomisationInsertions.GetListOfType<UTGOR_Customisation>(Type);
		for (UTGOR_Customisation* Entry : CustomList)
		{
			ConnectedCustomisations.Emplace(Entry);
		}
	}
}

