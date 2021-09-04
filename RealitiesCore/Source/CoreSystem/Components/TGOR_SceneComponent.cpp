// The Gateway of Realities: Planes of Existence.


#include "TGOR_SceneComponent.h"



UTGOR_SceneComponent::UTGOR_SceneComponent()
: Super()
{
}

bool UTGOR_SceneComponent::HasServerAuthority()
{
	AActor* Owner = GetOwner();
	if (IsValid(Owner))
	{
		return(Owner->HasAuthority());
	}
	return(false);
}

bool UTGOR_SceneComponent::WasSpawnedToPersistentLevel()
{
	AActor* Owner = GetOwner();
	if (IsValid(Owner))
	{
		return(Owner->IsInPersistentLevel(false));
	}
	return false;
}

UActorComponent* UTGOR_SceneComponent::GetOwnerComponent(TSubclassOf<UActorComponent> ComponentClass, ETGOR_FetchEnumeration& Branches)
{
	Branches = ETGOR_FetchEnumeration::Empty;
	AActor* Actor = GetOwner();
	UActorComponent* ActorComponent = Actor->GetComponentByClass(ComponentClass);
	if (IsValid(ActorComponent))
	{
		Branches = ETGOR_FetchEnumeration::Found;
		return ActorComponent;
	}
	return nullptr;
}


TArray<UActorComponent*> UTGOR_SceneComponent::GetOwnerComponents(TSubclassOf<UActorComponent> ComponentClass, ETGOR_FetchEnumeration& Branches)
{
	Branches = ETGOR_FetchEnumeration::Empty;
	AActor* Actor = GetOwner();
	TArray<UActorComponent*> ActorComponents;
	Actor->GetComponents(ComponentClass, ActorComponents);
	if (ActorComponents.Num() > 0)
	{
		Branches = ETGOR_FetchEnumeration::Found;
	}
	return ActorComponents;
}