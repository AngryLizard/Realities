
// The Gateway of Realities: Planes of Existence.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"

#include "TGOR_PilotSimulationInterface.generated.h"

class UTGOR_PilotComponent;

UINTERFACE(MinimalAPI, meta=(CannotImplementInterfaceInBlueprint))
class UTGOR_PilotSimulationInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class PHYSICSSYSTEM_API ITGOR_PilotSimulationInterface
{
	GENERATED_BODY()

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	ITGOR_PilotSimulationInterface();

	/** Simulate this object over a given amount of time with the current state, returns how much time was actually simulated. */
	UFUNCTION()
		virtual void TickStratified(float Time, float Timestep, float MinTickTime = 0.001f);

	/** Get current UpVector */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Movement", Meta = (Keywords = "C++"))
		virtual FVector ComputePhysicsUpVector() const;

	/** Get current root pilot */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Movement", Meta = (Keywords = "C++"))
		virtual UTGOR_PilotComponent* GetRootPilot() const;

	////////////////////////////////////////////////////////////////////////////////////////////////////
protected:

	UFUNCTION()
		virtual float TickPhysics(float Time);

	UFUNCTION()
		virtual AActor* GetPilotOwner() const;

	////////////////////////////////////////////////////////////////////////////////////////////////////
private:

	/** Pending simulation time in seconds leftover from last TickStratified */
	float PendingTime;

	/** Pilot component driven by this component */
	mutable TWeakObjectPtr<UTGOR_PilotComponent> RootCache;
};
