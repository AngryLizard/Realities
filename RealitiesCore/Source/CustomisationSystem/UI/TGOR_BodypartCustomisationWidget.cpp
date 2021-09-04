// The Gateway of Realities: Planes of Existence.


#include "TGOR_BodypartCustomisationWidget.h"
#include "CustomisationSystem/UI/TGOR_BodypartListWidget.h"
#include "CustomisationSystem/Content/Customisations/TGOR_Customisation.h"
#include "CustomisationSystem/Components/TGOR_CustomisationComponent.h"

UTGOR_BodypartCustomisationWidget::UTGOR_BodypartCustomisationWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	
}


void UTGOR_BodypartCustomisationWidget::Commit(UTGOR_Customisation* Customisation, const FTGOR_CustomisationPayload& Payload)
{
	UTGOR_CustomisationComponent* OwnerComponent = BodypartListWidget->OwnerComponent;
	if (IsValid(OwnerComponent))
	{
		FTGOR_CustomisationInstance Instance;

		Instance.Content = Customisation;
		Instance.Payload = Payload;
		// Instance.Content gets set automatically

		OwnerComponent->ApplyCustomisationInstance(GetBodypartNodeIndex(), Instance, EntryIndex);

		UpdateDisplay( Customisation, Payload, true);
	}
}

void UTGOR_BodypartCustomisationWidget::Query(UTGOR_Customisation* Customisation, FTGOR_CustomisationPayload& Payload) const
{
	UTGOR_CustomisationComponent* OwnerComponent = BodypartListWidget->OwnerComponent;
	if (IsValid(OwnerComponent))
	{
		FTGOR_CustomisationInstance Instance;

		Instance.Content = Customisation;

		// Use empty payload if query fails
		if (IsValid(Instance.Content))
		{
			Payload = Instance.Content->CreatePayload();
		}

		// Instance.Content gets set automatically
		if (OwnerComponent->QueryCustomisationInstance(GetBodypartNodeIndex(), Instance))
		{
			Payload = Instance.Payload;
		}
	}
}

void UTGOR_BodypartCustomisationWidget::Reset()
{
	UTGOR_CustomisationComponent* OwnerComponent = BodypartListWidget->OwnerComponent;
	if (IsValid(OwnerComponent))
	{
		//BodypartNode->ResetCustomisationInstance(EntryIndex);
	}
}