// The Gateway of Realities: Planes of Existence.


#include "TGOR_PortalComponent.h"

#include "Realities/Base/System/Data/TGOR_WorldData.h"


UTGOR_PortalComponent::UTGOR_PortalComponent()
	: Super()
{
	Name = FName("");
}


void UTGOR_PortalComponent::BeginPlay()
{
	Super::BeginPlay();
}

FVector UTGOR_PortalComponent::GetLocation_Implementation()
{
	return(GetOwner()->GetActorLocation());
}