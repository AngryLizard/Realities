// The Gateway of Realities: Planes of Existence.


#include "TGOR_ItemDropActor.h"

#include "InventorySystem/Components/TGOR_DropComponent.h"
#include "DimensionSystem/Components/TGOR_PilotComponent.h"

#include "InventorySystem/Tasks/TGOR_ItemTask.h"
#include "InventorySystem/Content/TGOR_Item.h"

#include "GameFramework/GameMode.h"
#include "CoreSystem/TGOR_Singleton.h"
#include "Net/UnrealNetwork.h"

// Sets default values
ATGOR_ItemDropActor::ATGOR_ItemDropActor(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	SetActorEnableCollision(true);

	PilotComponent = ObjectInitializer.CreateDefaultSubobject<UTGOR_PilotComponent>(this, FName(TEXT("Pilot")));
	PilotComponent->SetIsReplicated(true);
	SetRootComponent(PilotComponent);

	DropComponent = ObjectInitializer.CreateDefaultSubobject<UTGOR_DropComponent>(this, FName(TEXT("Drop")));
	DropComponent->SetIsReplicated(true);

	bReplicates = true;
}

/*
UTGOR_ItemStorage* ATGOR_ItemDropActor::AssignItem(UTGOR_ItemStorage* Storage, const FTGOR_MovementSpace& ParentSpace, const FVector& Impulse)
{
	// Set item
	UTGOR_ItemStorage* Residual = ItemComponent->PutItem(Storage);

	// Set body danymics
	FTGOR_MovementBody Body;
	UTGOR_Item* Item = Storage->GetItem();
	const float Mass = Storage->GetMass();
	Body.SetFromCapsule(FVector(0.1f, 0.1f, 0.1f), Item->Radius, Item->HalfHeight, Mass);
	PilotComponent->SetBody(Body);

	// Set phyiscs settings
	PilotComponent->Teleport(ParentSpace);
	PilotComponent->InflictPointImpact(ParentSpace.Linear, Impulse * Body.Mass);

	OnItemAssigned(Storage);
	return Residual;
}
*/
