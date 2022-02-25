// The Gateway of Realities: Planes of Existence.


#include "TGOR_Component.h"

#include "Realities/Base/System/Data/TGOR_WorldData.h"
#include "Realities/Base/System/Data/TGOR_DimensionData.h"


UTGOR_Component::UTGOR_Component(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{

}
////////////////////////////////////////////////////////////////////////////////////////////////////

bool UTGOR_Component::HasServerAuthority()
{
	AActor* Owner = GetOwner();
	if (IsValid(Owner))
	{
		return(Owner->HasAuthority());
	}
	return(false);
}

bool UTGOR_Component::WasSpawnedToPersistentLevel()
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