// The Gateway of Realities: Planes of Existence.

#pragma once

#include "CoreMinimal.h"

#include "PhysicsSystem/TGOR_PhysicsInstance.h"

#include "SocketSystem/Components/TGOR_SocketComponent.h"
#include "TGOR_PhysicsComponent.generated.h"

/**
 * UTGOR_PhysicsComponent Keeps track of physics surroundings.
 */
UCLASS(ClassGroup = (Custom), Blueprintable, meta = (BlueprintSpawnableComponent))
class PHYSICSSYSTEM_API UTGOR_PhysicsComponent : public UTGOR_SocketComponent
{
	GENERATED_BODY()
	
public:
	UTGOR_PhysicsComponent();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void OnPositionChange(const FTGOR_MovementPosition& Position) override;
	virtual void OnReparent(const FTGOR_MovementDynamic& Previous) override;

	virtual float Simulate(float Time) override;
	virtual bool Teleport(const FTGOR_MovementDynamic& Dynamic) override;

	////////////////////////////////////////////////////////////////////////////////////////////////////
protected:

	/** Current capture */
	UPROPERTY(ReplicatedUsing = RepNotifyCapture)
		FTGOR_MovementCapture Capture;

	UFUNCTION()
		virtual void RepNotifyCapture(const FTGOR_MovementCapture& Old);

	/** Update surroundings in capture from current state */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Movement|Internal", Meta = (Keywords = "C++"))
		void UpdateSurroundings(const FVector& Location);

	/** Called if out of level */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Movement|Internal", Meta = (Keywords = "C++"))
		virtual void OutOfLevel();

	////////////////////////////////////////////////////////////////////////////////////////////////////
protected:

	/** Timestep per simulation tick */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "!TGOR Movement|Internal")
		float SimulationTimestep;

	/** Simulate this object over a given amount of time over the current state */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Movement|Internal", Meta = (Keywords = "C++"))
		virtual float TickPhysics(float Time);

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Get current UpVector (Use External if available instead for better performance) */
	UFUNCTION(BlueprintPure, Category = "!TGOR Movement", Meta = (Keywords = "C++"))
		FVector ComputePhysicsUpVector() const;

	/** Get current bouyancy ratio (1.0 for displaced mass being equal to own mass) */
	UFUNCTION(BlueprintPure, Category = "!TGOR Movement", Meta = (Keywords = "C++"))
		float GetBouyancyRatio() const;

	/** Get current movement capture */
	UFUNCTION(BlueprintPure, Category = "!TGOR Movement", Meta = (Keywords = "C++"))
		const FTGOR_MovementCapture& GetCapture() const;



};
