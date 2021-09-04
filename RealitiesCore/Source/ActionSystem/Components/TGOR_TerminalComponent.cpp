// The Gateway of Realities: Planes of Existence.


#include "TGOR_TerminalComponent.h"
#include "../Tasks/TGOR_ActionTask.h"

UTGOR_TerminalComponent::UTGOR_TerminalComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UTGOR_TerminalComponent::BeginPlay()
{
	Super::BeginPlay();

}

void UTGOR_TerminalComponent::ContentInteract(UTGOR_ActionTask* ActionSlot, UTGOR_Content* Content)
{
	if (IsSupported(ActionSlot->Identifier.Component))
	{
		OnTerminalContentInteract.Broadcast(ActionSlot, Content);
		OnContentInteract(ActionSlot, Content);
	}
}

void UTGOR_TerminalComponent::RegisterInteract(UTGOR_ActionTask* ActionSlot, int32 Index)
{
	if (IsSupported(ActionSlot->Identifier.Component))
	{
		OnTerminalRegisterInteract.Broadcast(ActionSlot, Index);
		OnRegisterInteract(ActionSlot, Index);
	}
}

bool UTGOR_TerminalComponent::IsSupported(UTGOR_ActionComponent* Component) const
{
	return true;
}

void UTGOR_TerminalComponent::GetServerSlotData(UTGOR_ActionComponent* Component, int32& Data) const
{
	Data = INDEX_NONE;
	GetTerminalSlotData(Component, Data);
}