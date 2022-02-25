// The Gateway of Realities: Planes of Existence.


#include "TGOR_ProjectileActor.h"

#include "Realities/Components/Movement/TGOR_ProjectileMovementComponent.h"


// Sets default values
ATGOR_ProjectileActor::ATGOR_ProjectileActor(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	ProjectileMovement = ObjectInitializer.CreateDefaultSubobject<UTGOR_ProjectileMovementComponent>(this, FName(TEXT("ProjectileMovement")));
	ProjectileMovement->SetIsReplicated(true);
}

// Called when the game starts or when spawned
void ATGOR_ProjectileActor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ATGOR_ProjectileActor::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

}

