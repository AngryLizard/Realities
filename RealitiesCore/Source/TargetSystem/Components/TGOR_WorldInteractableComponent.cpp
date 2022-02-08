// The Gateway of Realities: Planes of Existence.


#include "TGOR_WorldInteractableComponent.h"
#include "TargetSystem/Content/TGOR_Target.h"

#include "RealitiesUGC/Mod/TGOR_ContentManager.h"
#include "CoreSystem/TGOR_Singleton.h"

UTGOR_WorldInteractableComponent::UTGOR_WorldInteractableComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	SetCollisionProfileName("Interaction");
}

void UTGOR_WorldInteractableComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UTGOR_WorldInteractableComponent::Influence(const FTGOR_InfluenceInstance& Influence)
{
	OnServerInfluence.Broadcast(Influence);
}


void UTGOR_WorldInteractableComponent::ReplicateInfluence_Implementation()
{
	OnInfluence.Broadcast();
}
