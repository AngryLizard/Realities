// The Gateway of Realities: Planes of Existence.


#include "TGOR_TerminalComponent.h"
#include "Realities/Base/Controller/TGOR_OnlineController.h"

#include "Realities/Base/Content/TGOR_ContentManager.h"
#include "Realities/Base/TGOR_Singleton.h"

UTGOR_TerminalComponent::UTGOR_TerminalComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UTGOR_TerminalComponent::BeginPlay()
{
	Super::BeginPlay();

}

void UTGOR_TerminalComponent::Interact(UTGOR_ActionStorage* Storage, UTGOR_ActionComponent* Component, const FTGOR_SlotIndex& Slot)
{
	if (IsSupported(Component))
	{
		OnTerminalInteract.Broadcast(Storage, Component, Slot);
		OnInteract(Storage, Component, Slot);
	}
}

bool UTGOR_TerminalComponent::IsSupported(UTGOR_ActionComponent* Component) const
{
	return true;
}

FTGOR_SlotIndex UTGOR_TerminalComponent::GetServerSlotData(UTGOR_ActionComponent* Component) const
{
	FTGOR_SlotIndex Index;
	Index.Container = 0;
	Index.Slot = 0;
	GetTerminalSlotData(Component, Index);
	Index.Valid = IsSupported(Component);
	return Index;
}