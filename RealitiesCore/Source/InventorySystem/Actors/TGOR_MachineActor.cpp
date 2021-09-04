// The Gateway of Realities: Planes of Existence.


#include "TGOR_MachineActor.h"

#include "InventorySystem/Components/TGOR_MatterComponent.h"

#include "GameFramework/GameMode.h"
#include "CoreSystem/TGOR_Singleton.h"
#include "Net/UnrealNetwork.h"

// Sets default values
ATGOR_MachineActor::ATGOR_MachineActor(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	SetActorEnableCollision(true);

	MatterComponent = ObjectInitializer.CreateDefaultSubobject<UTGOR_MatterComponent>(this, FName(TEXT("MatterComponent")));

	bReplicates = true;
}

void ATGOR_MachineActor::BeginPlay()
{
	Super::BeginPlay();
	
}

