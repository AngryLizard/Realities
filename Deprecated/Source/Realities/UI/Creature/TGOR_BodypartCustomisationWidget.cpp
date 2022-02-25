// The Gateway of Realities: Planes of Existence.


#include "TGOR_BodypartCustomisationWidget.h"
#include "Realities/Base/Creature/TGOR_BodypartNode.h"
#include "Realities/Mod/Customisations/TGOR_Customisation.h"
#include "Realities/Mod/Bodyparts/TGOR_Bodypart.h"

UTGOR_BodypartCustomisationWidget::UTGOR_BodypartCustomisationWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	
}


void UTGOR_BodypartCustomisationWidget::Commit(const TArray<uint8>& Payload)
{
	if (IsValid(BodypartNode))
	{
		FTGOR_CustomisationInstance Instance;

		Instance.Content = Customisation;
		Instance.Payload = Payload;
		Instance.Index = EntryIndex;
		// Instance.Content gets set automatically
		BodypartNode->ApplyCustomisationInstance(Instance);
	}
}

void UTGOR_BodypartCustomisationWidget::Query(TArray<uint8>& Payload) const
{
	if (IsValid(BodypartNode))
	{
		FTGOR_CustomisationInstance Instance;

		Instance.Content = Customisation;
		Instance.Index = EntryIndex;

		// Use empty payload if query fails
		if (IsValid(Instance.Content))
		{
			Payload = Instance.Content->CreatePayload();
		}

		// Instance.Content gets set automatically
		if (BodypartNode->QueryCustomisationInstance(Instance))
		{
			Payload = Instance.Payload;
		}
	}
}

void UTGOR_BodypartCustomisationWidget::Reset()
{
	if (IsValid(BodypartNode))
	{
		BodypartNode->ResetCustomisationInstance(EntryIndex);
	}
}