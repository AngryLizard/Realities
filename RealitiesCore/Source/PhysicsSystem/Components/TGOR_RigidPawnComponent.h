// The Gateway of Realities: Planes of Existence.

#pragma once

#include "CoreMinimal.h"
#include "PhysicsSystem/TGOR_PhysicsInstance.h"
#include "DimensionSystem/TGOR_PilotInstance.h"

#include "GameFramework/PawnMovementComponent.h"
#include "PhysicsSystem/Interfaces/TGOR_PilotSimulationInterface.h"
#include "TGOR_RigidPawnComponent.generated.h"

DECLARE_STATS_GROUP(TEXT("Rigid Pawn Movement"), STATGROUP_RIGID, STATCAT_Advanced);

///////////////////////////////////////////// DELEGATES ///////////////////////////////////////////////////

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FEnergyUsageDelegate, float, Energy);

/**
* TGOR_RigidPawnComponent automatically integrates velocity and location
*/
UCLASS(ClassGroup = (Custom), Blueprintable, meta = (BlueprintSpawnableComponent))
class PHYSICSSYSTEM_API UTGOR_RigidPawnComponent : public UPawnMovementComponent, public ITGOR_PilotSimulationInterface
{
	GENERATED_BODY()

public:
	UTGOR_RigidPawnComponent();
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual FVector ComputePhysicsUpVector() const override;
	virtual AActor* GetPilotOwner() const override;
	virtual float TickPhysics(float Time) override;

	//////////////////////////////////////////// IMPLEMENTABLES ////////////////////////////////////////

	UPROPERTY(BlueprintAssignable, Category = "!TGOR Movement")
		FEnergyUsageDelegate OnEnergyUsage;

	////////////////////////////////////////////////////////////////////////////////////////////////////
protected:

	/** Current capture */
	UPROPERTY(ReplicatedUsing = RepNotifyCapture)
		FTGOR_MovementCapture Capture;

	UFUNCTION()
		virtual void RepNotifyCapture(const FTGOR_MovementCapture& Old);

	/** Get current movement capture */
	UFUNCTION(BlueprintPure, Category = "!TGOR Movement", Meta = (Keywords = "C++"))
		const FTGOR_MovementCapture& GetCapture() const;

	////////////////////////////////////////////////////////////////////////////////////////////////////
protected:

	/** Get temporal information from a movement base and current object state */
	UFUNCTION(BlueprintPure, Category = "!TGOR Movement|Internal", Meta = (Keywords = "C++"))
		bool PopulateMovementTick(float Time, FTGOR_MovementTick& OutTick) const;

	////////////////////////////////////////////////////////////////////////////////////////////////////
protected:

	/** Inflict linear momentum */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Movement|Internal", Meta = (Keywords = "C++"))
		void InflictLinearMomentum(const FTGOR_MovementDynamic& Dynamic, FVector InMomentum, bool CounteractVelocity);

	/** Inflict angular momentum */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Movement|Internal", Meta = (Keywords = "C++"))
		void InflictAngularMomentum(const FTGOR_MovementDynamic& Dynamic, FVector InAngular, bool CounteractAngular);

	/** Inflict force/torque over a given amount of time */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Movement|Internal", Meta = (Keywords = "C++"))
		void InflictForceTorque(const FVector& Force, const FVector& Torque, float DeltaTime);

	/** Compute current external properties and initialise movement output with proper dampers */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Movement|Internal", Meta = (Keywords = "C++"))
		void ComputeExternal(const FTGOR_MovementSpace& Space, FTGOR_MovementExternal& External, FTGOR_MovementOutput& Out);


	////////////////////////////////////////////////////////////////////////////////////////////////////
protected:

	/** Max damping time factor */
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		float MaxDampingStratification;

	/** Collision detection enabled */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "!TGOR Movement|Internal")
		bool SweepsCollision;

	/** Speed with which up-vector is lerped */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "!TGOR Movement|Internal")
		float OrientationSpeed;

	/** Timestep per simulation tick, 0 for non-stratified */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "!TGOR Movement|Internal")
		float SimulationTimestep;

	////////////////////////////////////////////////////////////////////////////////////////////////////
protected:

	/** Inflicted movement */
	UPROPERTY( BlueprintReadWrite, Category = "!TGOR Movement")
		FTGOR_MovementDelta Inflicted;

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Grab input parameters from world/parent */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Movement|Internal", Meta = (Keywords = "C++"))
		virtual void PreComputePhysics(const FTGOR_MovementTick& Tick);

	/** Compute one physics simulation tick, return energy used */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Movement|Internal", Meta = (Keywords = "C++"))
		virtual void ComputePhysics(UPARAM(Ref) FTGOR_MovementSpace& Space, const FTGOR_MovementExternal& External, const FTGOR_MovementTick& Tick, FTGOR_MovementOutput& Output);

	/** Compute effects after we're done computing physics */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Movement|Internal", Meta = (Keywords = "C++"))
		virtual void PostComputePhysics(const FTGOR_MovementSpace& Space, float Energy, float DeltaTime);

	/** Returns whether force can be inflicted */
	UFUNCTION(BlueprintPure, Category = "!TGOR Movement|Internal", Meta = (Keywords = "C++"))
		virtual bool CanInflict() const;

	////////////////////////////////////////////////////////////////////////////////////////////////////
private:

	int32 SimulationsPerTick = 0;

};
