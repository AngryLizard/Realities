// The Gateway of Realities: Planes of Existence.


#include "TGOR_ModularSkeletalMeshComponent.h"

UTGOR_ModularSkeletalMeshComponent::UTGOR_ModularSkeletalMeshComponent()
	: Super()
{
	SetIsReplicatedByDefault(true);

}

void UTGOR_ModularSkeletalMeshComponent::UpdateContent_Implementation(FTGOR_SpawnerDependencies& Dependencies)
{
	ITGOR_SpawnerInterface::UpdateContent_Implementation(Dependencies);
}

TMap<int32, UTGOR_SpawnModule*> UTGOR_ModularSkeletalMeshComponent::GetModuleType_Implementation() const
{
	return TMap<int32, UTGOR_SpawnModule*>();
}
