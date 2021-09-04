// The Gateway of Realities: Planes of Existence.


#include "TGOR_Component.h"


UTGOR_Component::UTGOR_Component(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{

}
////////////////////////////////////////////////////////////////////////////////////////////////////

bool UTGOR_Component::HasServerAuthority() const
{
	AActor* Owner = GetOwner();
	if (IsValid(Owner))
	{
		return(Owner->HasAuthority());
	}
	return(false);
}

bool UTGOR_Component::WasSpawnedToPersistentLevel() const
{
	AActor* Owner = GetOwner();
	if (IsValid(Owner))
	{
		return(Owner->IsInPersistentLevel(false));
	}
	return false;
}

UActorComponent* UTGOR_Component::GetOwnerComponent(TSubclassOf<UActorComponent> ComponentClass, ETGOR_FetchEnumeration& Branches)
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

TArray<UActorComponent*> UTGOR_Component::GetOwnerComponents(TSubclassOf<UActorComponent> ComponentClass, ETGOR_FetchEnumeration& Branches)
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

USceneComponent* GetMatchingAttachChild(USceneComponent* Component, TSubclassOf<USceneComponent> Type)
{
	if (Component->IsA(Type))
	{
		return Component;
	}

	const TArray<USceneComponent*>& Children = Component->GetAttachChildren();
	for (USceneComponent* Child : Children)
	{
		USceneComponent* Match = GetMatchingAttachChild(Child, Type);
		if (IsValid(Match))
		{
			return Match;
		}
	}
	return nullptr;
}

USceneComponent* UTGOR_Component::GetOwnerRootScene(TSubclassOf<USceneComponent> ComponentClass, ETGOR_FetchEnumeration& Branches) const
{
	Branches = ETGOR_FetchEnumeration::Empty;

	AActor* Owner = GetOwner();
	USceneComponent* Root = Owner->GetRootComponent();
	USceneComponent* Matching = GetMatchingAttachChild(Root, ComponentClass);
	if (IsValid(Matching))
	{
		Branches = ETGOR_FetchEnumeration::Found;
		return Matching;
	}
	return nullptr;
}