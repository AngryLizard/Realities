// The Gateway of Realities: Planes of Existence.


#include "TGOR_InteractableComponent.h"
#include "TargetSystem/Content/TGOR_Target.h"
#include "DimensionSystem/Content/TGOR_Spawner.h"
#include "CoreSystem/TGOR_Singleton.h"

#define LOCTEXT_NAMESPACE "TGOR" 

UTGOR_InteractableComponent::UTGOR_InteractableComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UTGOR_InteractableComponent::UpdateContent_Implementation(FTGOR_SpawnerDependencies& Dependencies)
{
	Targets = Dependencies.Spawner->GetMListFromType<UTGOR_Target>(SpawnTargets);
}

TMap<int32, UTGOR_SpawnModule*> UTGOR_InteractableComponent::GetModuleType_Implementation() const
{
	TMap<int32, UTGOR_SpawnModule*> Modules;

	const int32 Num = Targets.Num();
	for (int32 Index = 0; Index < Num; Index++)
	{
		Modules.Emplace(Index, Targets[Index]);
	}
	return Modules;
}

void UTGOR_InteractableComponent::Influence(const FTGOR_InfluenceInstance& Influence)
{
}

#undef LOCTEXT_NAMESPACE