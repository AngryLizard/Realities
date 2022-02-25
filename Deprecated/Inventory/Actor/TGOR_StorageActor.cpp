// The Gateway of Realities: Planes of Existence.

#include "TGOR_StorageActor.h"
#include "Realities.h"

#include "Components/Dimension/TGOR_SaveComponent.h"

ATGOR_StorageActor::ATGOR_StorageActor(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = true;

	SaveComponent = ObjectInitializer.CreateDefaultSubobject<UTGOR_SaveComponent>(this, FName(TEXT("SaveComponent")));
}

void ATGOR_StorageActor::BeginPlay()
{
	Super::BeginPlay();
	
}

void ATGOR_StorageActor::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );
}
