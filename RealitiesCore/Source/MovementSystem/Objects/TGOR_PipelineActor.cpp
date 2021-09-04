// The Gateway of Realities: Planes of Existence.


#include "TGOR_PipelineActor.h"

#include "SocketSystem/Components/TGOR_PipelineComponent.h"
#include "CoreSystem/TGOR_Singleton.h"

#include "Net/UnrealNetwork.h"

ATGOR_PipelineActor::ATGOR_PipelineActor(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;

	SplineComponent = ObjectInitializer.CreateDefaultSubobject<USplineComponent>(this, FName(TEXT("SplineComponent")));
	PipelineComponent = ObjectInitializer.CreateDefaultSubobject<UTGOR_PipelineComponent>(this, FName(TEXT("PipelineComponent")));

	SetRootComponent(SplineComponent);
}

void ATGOR_PipelineActor::BeginPlay()
{
	Super::BeginPlay();
}

void ATGOR_PipelineActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

