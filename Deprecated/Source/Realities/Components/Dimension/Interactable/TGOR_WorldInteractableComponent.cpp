// The Gateway of Realities: Planes of Existence.


#include "TGOR_WorldInteractableComponent.h"
#include "Realities/Mod/Targets/TGOR_Target.h"

#include "Realities/Base/Content/TGOR_ContentManager.h"
#include "Realities/Base/TGOR_Singleton.h"

UTGOR_WorldInteractableComponent::UTGOR_WorldInteractableComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	SetCollisionProfileName("Interaction");
}

void UTGOR_WorldInteractableComponent::BeginPlay()
{
	Super::BeginPlay();

	// Fetch all targets
	SINGLETON_CHK;
	UTGOR_ContentManager* ContentManager = Singleton->GetContentManager();

	for (TSubclassOf<UTGOR_Target> TargetClass : TargetClasses)
	{
		Targets.Append(ContentManager->GetTListFromType(TargetClass));
	}
}

void UTGOR_WorldInteractableComponent::Influence(const FTGOR_InfluenceInstance& Influence)
{
	OnServerInfluence.Broadcast(Influence);
}


void UTGOR_WorldInteractableComponent::ReplicateInfluence_Implementation()
{
	OnInfluence.Broadcast();
}
