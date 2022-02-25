// The Gateway of Realities: Planes of Existence.


#include "TGOR_PhysicsSimulationActor.h"


ATGOR_PhysicsSimulationActor::ATGOR_PhysicsSimulationActor(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = true;

	TimestepCache = 0.0f;
	MaxVelocity = 0.0f;

	MaxIterations = 100;
	SimulationSpeed = 1.0f;
	Stabilize = 0.0f;
}


void ATGOR_PhysicsSimulationActor::BeginPlay()
{
	Super::BeginPlay();

	
}


void ATGOR_PhysicsSimulationActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Allow only simulations of a certain speed if enabled
	if (Stabilize > SMALL_NUMBER)
	{
		TimestepCache += DeltaTime;
		while (TimestepCache > Stabilize)
		{
			SimulateOver(Stabilize);
			TimestepCache -= Stabilize;
		}
	}
	else
	{
		int32 Iterations = SimulateOver(DeltaTime);
	}
}


int32 ATGOR_PhysicsSimulationActor::SimulateOver(float DeltaTime)
{
	// Simulate until one deltatime has been played through
	int32 Iterations = 0;
	while (DeltaTime > 0.0)
	{
		// Make sure simulation speed is followed
		float MaxDelta = SimulationSpeed / 1.0f;
		if (Iterations >= MaxIterations)
		{
			MaxDelta = DeltaTime;
		}

		// Count up iterations
		Iterations++;

		// Simulate with pending deltatime
		const float Delta = FMath::Min(DeltaTime, MaxDelta);
		Simulate(Delta);

		// Deduce deltatime
		DeltaTime -= MaxDelta;
	}

	return(Iterations);
}

void ATGOR_PhysicsSimulationActor::Simulate(float DeltaTime)
{
	TArray<AActor*> Ignore;

	UTGOR_PhysicsFreeComponent* Physics = GetPhysics();
	if (IsValid(Physics))
	{
		// Updates inertia tensor to take new locations into account
		FVector Correction = Physics->ApplyMove(DeltaTime, Gravity, Ignore);
	}
	else
	{
		ERROR("Physics container invalid", Error)
	}
}