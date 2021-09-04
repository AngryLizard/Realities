// The Gateway of Realities: Planes of Existence.


#include "TGOR_InteractableComponent.h"
#include "TargetSystem/Content/TGOR_Entity.h"
#include "TargetSystem/Content/TGOR_Target.h"
#include "DimensionSystem/Content/TGOR_Spawner.h"
#include "CoreSystem/TGOR_Singleton.h"

#define LOCTEXT_NAMESPACE "TGOR" 

UTGOR_InteractableComponent::UTGOR_InteractableComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UTGOR_InteractableComponent::UpdateContent_Implementation(UTGOR_Spawner* Spawner)
{
	UTGOR_Entity* Entity = Spawner->GetMFromType<UTGOR_Entity>(TargetEntity);
	if (IsValid(Entity))
	{
		Targets = Entity->Instanced_TargetInsertions.Collection;
	}
}

void UTGOR_InteractableComponent::Influence(const FTGOR_InfluenceInstance& Influence)
{
}

#undef LOCTEXT_NAMESPACE