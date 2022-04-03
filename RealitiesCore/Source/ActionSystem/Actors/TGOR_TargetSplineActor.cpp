// The Gateway of Realities: Planes of Existence.

#include "TGOR_TargetSplineActor.h"

#include "Components/SplineComponent.h"

#include "GameFramework/GameMode.h"
#include "CoreSystem/TGOR_Singleton.h"
#include "Net/UnrealNetwork.h"

// Sets default values
ATGOR_TargetSplineActor::ATGOR_TargetSplineActor(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	SetActorEnableCollision(true);

	SplineComponent = ObjectInitializer.CreateDefaultSubobject<USplineComponent>(this, FName(TEXT("SplineComponent")));

	bReplicates = true;
}

void ATGOR_TargetSplineActor::BeginPlay()
{
	Super::BeginPlay();
	
}
