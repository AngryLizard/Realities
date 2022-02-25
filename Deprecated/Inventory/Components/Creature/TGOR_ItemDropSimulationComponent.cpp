// The Gateway of Realities: Planes of Existence.


#include "TGOR_ItemDropSimulationComponent.h"

#include "Actors/Inventory/TGOR_ItemDropActor.h"
#include "Mod/Items/TGOR_Item.h"
#include "Base/Instances/Inventory/TGOR_ItemInstance_OLD.h"
#include "Mod/Metamorphs/TGOR_Metamorph.h"
#include "Components/Inventory/TGOR_MetamorphComponent.h"
#include "Components/Inventory/TGOR_SingleItemComponent.h"


UTGOR_ItemDropSimulationComponent::UTGOR_ItemDropSimulationComponent()
{
	_item = nullptr;

	ItemDrop = nullptr;
	MetamorphComponent = nullptr;
}



void UTGOR_ItemDropSimulationComponent::BeginPlay()
{
	Super::BeginPlay();

	ItemDrop = Cast<ATGOR_ItemDropActor>(GetOwner());

	if (IsValid(ItemDrop))
	{
		UActorComponent* Component = ItemDrop->GetComponentByClass(UTGOR_MetamorphComponent::StaticClass());
		MetamorphComponent = Cast<UTGOR_MetamorphComponent>(Component);
	}
}

void UTGOR_ItemDropSimulationComponent::RegionTick(float DeltaTime)
{
	Super::RegionTick(DeltaTime);
	
	AmbientInfluence();

	if (IsValid(MetamorphComponent))
	{
		MetamorphComponent->MaintainTimer();
	}
}


void UTGOR_ItemDropSimulationComponent::DirectInfluenced(const FTGOR_ElementInstance& State, AActor* Instigator)
{
	Super::DirectInfluenced( State, Instigator);

	if (IsValid(ItemDrop) && IsValid(ItemDrop->Container))
	{
		// Apply metamorph and get possible output
		FTGOR_ItemInstance_OLD& ItemInstance = ItemDrop->Container->ItemInstance;
		TArray<FTGOR_ItemInstance_OLD> Output = MetamorphComponent->MetamorphItem(ItemInstance, State);

		// Spawn output as new itemdrops
		for (const FTGOR_ItemInstance_OLD& Instance : Output)
		{
			FVector Location = ItemDrop->GetActorLocation();
			ItemDrop->Container->DropItem(Instance, Location);
		}
	}
}

UTGOR_Item* UTGOR_ItemDropSimulationComponent::EnsureItem()
{
	return(nullptr);
}

float UTGOR_ItemDropSimulationComponent::GetMetamorphProgress(FTGOR_ElementInstance& ElementInstance)
{
	if (IsValid(MetamorphComponent))
	{
		return(MetamorphComponent->GetProgress(ElementInstance));
	}
	return(0.0f);
}