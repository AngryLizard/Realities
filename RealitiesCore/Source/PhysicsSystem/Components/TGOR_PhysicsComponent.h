// The Gateway of Realities: Planes of Existence.

#pragma once

#include "CoreMinimal.h"

#include "PhysicsSystem/TGOR_PhysicsInstance.h"

#include "CoreSystem/Components/TGOR_Component.h"
#include "TGOR_PhysicsComponent.generated.h"

class UTGOR_PilotComponent;

/**
 * UTGOR_PhysicsComponent Keeps track of physics surroundings.
 */
UCLASS(ClassGroup = (Custom), Blueprintable, meta = (BlueprintSpawnableComponent))
class PHYSICSSYSTEM_API UTGOR_PhysicsComponent : public UTGOR_Component
{
	GENERATED_BODY()
	
public:

	UTGOR_PhysicsComponent();
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	//virtual void OnReparent(const FTGOR_MovementDynamic& Previous) override;

	////////////////////////////////////////////////////////////////////////////////////////////////////
protected:

	/** Current capture */
	UPROPERTY(ReplicatedUsing = RepNotifyCapture)
		FTGOR_MovementCapture Capture;

	UFUNCTION()
		virtual void RepNotifyCapture(const FTGOR_MovementCapture& Old);

	////////////////////////////////////////////////////////////////////////////////////////////////////
protected:

	/** Speed with which up-vector is lerped */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "!TGOR Movement|Internal")
		float OrientationSpeed;

	/** Timestep per simulation tick */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "!TGOR Movement|Internal")
		float SimulationTimestep;

	/** Simulate this object over a given amount of time over the current state, returns how much time was actually simulated.
		By default orients itself towards (real and artificial) gravity */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Movement|Internal", Meta = (Keywords = "C++"))
		virtual float TickPhysics(float Time);

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Get current root pilot */
	UFUNCTION(BlueprintPure, Category = "!TGOR Movement", Meta = (Keywords = "C++"))
		UTGOR_PilotComponent* GetRootPilot() const;

	/** Get current UpVector (Use External if available instead for better performance) */
	UFUNCTION(BlueprintPure, Category = "!TGOR Movement", Meta = (Keywords = "C++"))
		FVector ComputePhysicsUpVector() const;

	/** Get current bouyancy ratio (1.0 for displaced mass being equal to own mass) */
	UFUNCTION(BlueprintPure, Category = "!TGOR Movement", Meta = (Keywords = "C++"))
		float GetBouyancyRatio(const FTGOR_PhysicsProperties& Surroundings) const;

	/** Get current movement capture */
	UFUNCTION(BlueprintPure, Category = "!TGOR Movement", Meta = (Keywords = "C++"))
		const FTGOR_MovementCapture& GetCapture() const;

	////////////////////////////////////////////////////////////////////////////////////////////////////
private:

	/** Time of last movement update */
	UPROPERTY()
		FTGOR_Time LastUpdateTimestamp;

	/** Pilot component driven by this component */
	UPROPERTY()
		mutable TWeakObjectPtr<UTGOR_PilotComponent> RootCache;
};
