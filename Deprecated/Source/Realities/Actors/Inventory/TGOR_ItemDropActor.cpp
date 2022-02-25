// The Gateway of Realities: Planes of Existence.


#include "TGOR_ItemDropActor.h"

#include "Realities/Base/System/Data/TGOR_DimensionData.h"
#include "Realities/Components/Inventory/TGOR_ItemComponent.h"
#include "Realities/Components/Movement/TGOR_LinearComponent.h"

#include "Realities/Base/Inventory/TGOR_ItemStorage.h"
#include "Realities/Mod/Spawner/Items/TGOR_Item.h"

#include "GameFramework/GameMode.h"
#include "Realities/Base/TGOR_Singleton.h"
#include "Net/UnrealNetwork.h"

// Sets default values
ATGOR_ItemDropActor::ATGOR_ItemDropActor(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	SetActorEnableCollision(true);

	CapsuleComponent = ObjectInitializer.CreateDefaultSubobject<UCapsuleComponent>(this, FName(TEXT("Root")));
	SetRootComponent(CapsuleComponent);

	MovementComponent = ObjectInitializer.CreateDefaultSubobject<UTGOR_LinearComponent>(this, FName(TEXT("Movement")));
	MovementComponent->SetIsReplicated(true);
	MovementComponent->SetupAttachment(CapsuleComponent);

	ItemComponent = ObjectInitializer.CreateDefaultSubobject<UTGOR_ItemComponent>(this, FName(TEXT("Item")));
	ItemComponent->SetIsReplicated(true);

	bReplicates = true;
}

void ATGOR_ItemDropActor::BeginPlay()
{
	Super::BeginPlay();
	
}

UTGOR_ItemStorage* ATGOR_ItemDropActor::AssignItem(UTGOR_ItemStorage* Storage, const FTGOR_MovementSpace& ParentSpace, const FVector& Impulse)
{
	// Set item
	UTGOR_ItemStorage* Residual = ItemComponent->PutItem(Storage);

	// Set body danymics
	FTGOR_MovementBody Body;
	UTGOR_Item* Item = Storage->GetItem();
	const float Mass = Storage->GetMass();
	Body.SetFromCapsule(FVector(0.1f, 0.1f, 0.1f), Item->Radius, Item->HalfHeight, Mass);
	MovementComponent->SetBody(Body);

	// Set phyiscs settings
	MovementComponent->SimulateDynamic(ParentSpace);
	MovementComponent->InflictPointImpact(ParentSpace.Linear, Impulse * Body.Mass, INDEX_NONE);

	OnItemAssigned(Storage);
	return Residual;
}
