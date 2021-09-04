// The Gateway of Realities: Planes of Existence.

#include "TGOR_CapsuleComponent.h"



UTGOR_CapsuleComponent::UTGOR_CapsuleComponent()
: Super()
{
}

bool UTGOR_CapsuleComponent::HasServerAuthority()
{
	AActor* Owner = GetOwner();
	if (IsValid(Owner))
	{
		return(Owner->HasAuthority());
	}
	return(false);
}

bool UTGOR_CapsuleComponent::WasSpawnedToPersistentLevel()
{
	AActor* Owner = GetOwner();
	if (IsValid(Owner))
	{
		return(Owner->IsInPersistentLevel(false));
	}
	return false;
}

UActorComponent* UTGOR_CapsuleComponent::GetOwnerComponent(TSubclassOf<UActorComponent> ComponentClass, ETGOR_FetchEnumeration& Branches)
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


TArray<UActorComponent*> UTGOR_CapsuleComponent::GetOwnerComponents(TSubclassOf<UActorComponent> ComponentClass, ETGOR_FetchEnumeration& Branches)
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