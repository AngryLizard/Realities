// The Gateway of Realities: Planes of Existence.


#include "TGOR_InteractableComponent.h"
#include "Realities/Base/Controller/TGOR_PlayerController.h"
#include "Realities/Components/System/Actions/TGOR_ActionComponent.h"
#include "Realities/Mod/Spawner/Creatures/TGOR_Creature.h"
#include "Realities/Mod/Actions/TGOR_Action.h"
#include "Realities/Mod/Targets/TGOR_Target.h"
#include "Realities/Base/TGOR_Singleton.h"

#include "GameFramework/Hud.h"

#define LOCTEXT_NAMESPACE "TGOR" 

UTGOR_InteractableComponent::UTGOR_InteractableComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UTGOR_InteractableComponent::Influence(const FTGOR_InfluenceInstance& Influence)
{

}

void UTGOR_InteractableComponent::BuildTargets(UTGOR_Content* Content)
{
	Targets.Append(Content->GetIListFromType<UTGOR_Target>());
}

#undef LOCTEXT_NAMESPACE