// The Gateway of Realities: Planes of Existence.


#include "TGOR_PhysicsActor.h"




ATGOR_PhysicsActor::ATGOR_PhysicsActor(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = true;

	PhysicsContainer = ObjectInitializer.CreateDefaultSubobject<UTGOR_PhysicsFreeComponent>(this, FName(TEXT("PhysicsContainer")));
	SetRootComponent(PhysicsContainer);

	Gravity = FVector(0.0f, 0.0f, -981.0f);
}


void ATGOR_PhysicsActor::BeginPlay()
{
	Super::BeginPlay();
}


void ATGOR_PhysicsActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}


void ATGOR_PhysicsActor::ApplySpin(FVector Axis)
{
	PhysicsContainer->CurrentSpin += Axis;
}

void ATGOR_PhysicsActor::ApplyForce(FVector Force)
{
	PhysicsContainer->CurrentVelocity += Force;
}
