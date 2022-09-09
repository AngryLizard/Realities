// The Gateway of Realities: Planes of Existence.

#include "TGOR_DialogueTask.h"
#include "DialogueSystem/Components/TGOR_DialogueComponent.h"
#include "AttributeSystem/Components/TGOR_AttributeComponent.h"
#include "DialogueSystem/Content/TGOR_Dialogue.h"
#include "AttributeSystem/Content/TGOR_Attribute.h"

UTGOR_DialogueTask::UTGOR_DialogueTask()
: Super()
{
}

void UTGOR_DialogueTask::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(UTGOR_DialogueTask, Identifier, COND_InitialOnly);
}

TScriptInterface<ITGOR_AnimationInterface> UTGOR_DialogueTask::GetAnimationOwner() const
{
	return GetOwnerComponent();
}

////////////////////////////////////////////////////////////////////////////////////////////////////

UTGOR_Dialogue* UTGOR_DialogueTask::GetDialogue() const
{
	return Identifier.Content;
}

UTGOR_DialogueComponent* UTGOR_DialogueTask::GetOwnerComponent() const
{
	return Identifier.Component;
}

int32 UTGOR_DialogueTask::GetSlotIndex() const
{
	return Identifier.Slot;
}

float UTGOR_DialogueTask::GetAttribute(TSubclassOf<UTGOR_Attribute> Type, float Default) const
{
	return UTGOR_AttributeComponent::GetAttributeValue(Identifier.Component->GetOwner(), Type, Default);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void UTGOR_DialogueTask::Initialise()
{
	Identifier.Content->TaskInitialise(this);
	OnInitialise();
}

void UTGOR_DialogueTask::PrepareStart()
{
	PlayAnimation();
}

void UTGOR_DialogueTask::Interrupt()
{
	ResetAnimation();
}

////////////////////////////////////////////////////////////////////////////////////////////////////
