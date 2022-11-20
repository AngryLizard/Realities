// The Gateway of Realities: Planes of Existence.

#include "TGOR_Trigger.h"

#include "CoreSystem/TGOR_Singleton.h"
#include "DialogueSystem/Components/TGOR_ActivatorComponent.h"
#include "DialogueSystem/Components/TGOR_DialogueComponent.h"

UTGOR_Trigger::UTGOR_Trigger()
	: Super()
{
}

////////////////////////////////////////////////////////////////////////////////////////////////////

float UTGOR_Trigger::TickTriggerValue_Implementation(UTGOR_ActivatorComponent* Activator, float Value, float DeltaTime) const
{
	return Value;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void UTGOR_Trigger::MoveInsertion(UTGOR_Content* Insertion, ETGOR_InsertionActionEnumeration Action, bool& Success)
{
	Super::MoveInsertion(Insertion, Action, Success);

	MOV_INSERTION(ActivatorInsertions);
}