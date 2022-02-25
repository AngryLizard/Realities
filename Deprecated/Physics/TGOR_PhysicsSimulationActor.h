// TGOR (C) // CHECKED //
#pragma once

#include "CoreMinimal.h"
#include "Realities/Actors/Physics/TGOR_PhysicsActor.h"
#include "TGOR_PhysicsSimulationActor.generated.h"

/**
 * ATGOR_PhysicsSimulationActor defines actors that simulates physics
 */
UCLASS()
class REALITIES_API ATGOR_PhysicsSimulationActor : public ATGOR_PhysicsActor
{
	GENERATED_BODY()
	
public:

	////////////////////////////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////// ENGINE ////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////

	ATGOR_PhysicsSimulationActor(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	//////////////////////////////////////////////// IMPLEMENTABLES /////////////////////////////////////////

	/** */

	////////////////////////////////////////////// COMPONENTS //////////////////////////////////////////////////////
private:

	/** */

	////////////////////////////////////////////////////////////////////////////////////////////////////
protected:

	/** max recorded velocity in the system */
	UPROPERTY(BlueprintReadOnly, Category = "TGOR Physics", Meta = (Keywords = "C++"))
		float MaxVelocity;

	/** Deltatime offset from last tick */
	UPROPERTY(BlueprintReadOnly, Category = "TGOR Physics", Meta = (Keywords = "C++"))
		float TimestepCache;
	
	/** Maximum automatic iterations per tick */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TGOR Physics")
		int32 MaxIterations;

	/** Maximum speed per simulation tick (if in iteration range) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TGOR Physics")
		float SimulationSpeed;

	/** Simulates physics always the same amount if set */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TGOR Physics")
		float Stabilize;

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Simulate multiple timesteps (returns number of iterations) */
	UFUNCTION(BlueprintCallable, Category = "TGOR Physics", Meta = (Keywords = "C++"))
		int32 SimulateOver(float DeltaTime);

	/** Simulate a timestep */
	UFUNCTION(BlueprintCallable, Category = "TGOR Physics", Meta = (Keywords = "C++"))
		void Simulate(float DeltaTime);

	/////////////////////////////////////////////// INTERNAL ///////////////////////////////////////////

protected:

private:

};
