// The Gateway of Realities: Planes of Existence.

#pragma once

#include "CoreMinimal.h"
#include "DimensionSystem/TGOR_PilotInstance.h"

#include "TGOR_ColliderComponent.h"
#include "TGOR_RigidComponent.generated.h"

DECLARE_STATS_GROUP(TEXT("Rigid Movement"), STATGROUP_RIGID, STATCAT_Advanced);
/*
UENUM(BlueprintType)
enum class ETGOR_BufferUpdateMode : uint8
{
	NoBuffer			UMETA(DisplayName = "No buffer recording"),
	BufferOnSimulated	UMETA(DisplayName = "Record buffer on simulated client"),
	BufferOnAutonomous	UMETA(DisplayName = "Record buffer on autonomous client"),
	BufferOnAuthority	UMETA(DisplayName = "Record buffer on authoritive client")
};
*/

///////////////////////////////////////////// DELEGATES ///////////////////////////////////////////////////

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FEnergyUsageDelegate, float, Energy);

/**
* TGOR_RigidComponent automatically integrates velocity and location
*/
UCLASS(ClassGroup = (Custom), Blueprintable, meta = (BlueprintSpawnableComponent))
class PHYSICSSYSTEM_API UTGOR_RigidComponent : public UTGOR_ColliderComponent
{
	GENERATED_BODY()

public:
	UTGOR_RigidComponent();

	virtual void InflictPointImpact(const FVector& Point, const FVector& Impulse) override;
	virtual void InflictPointForce(const FVector& Point, const FVector& Force, float DeltaTime) override;

	virtual float TickPhysics(float Time) override;

	//////////////////////////////////////////// IMPLEMENTABLES ////////////////////////////////////////

	UPROPERTY(BlueprintAssignable, Category = "!TGOR Movement")
		FEnergyUsageDelegate OnEnergyUsage;

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Computes Damping force against an linear velocity including oscillation prevention */
	UFUNCTION(BlueprintPure, Category = "!TGOR Movement|Internal", Meta = (Keywords = "C++"))
		void GetDampingForce(const FTGOR_MovementTick& Tick, const FVector& LinearVelocity, float Damping, FTGOR_MovementOutput& Output) const;

	/** Computes Damping force against a angular velocity including oscillation prevention */
	UFUNCTION(BlueprintPure, Category = "!TGOR Movement|Internal", Meta = (Keywords = "C++"))
		void GetDampingTorque(const FTGOR_MovementTick& Tick, const FVector& AngularVelocity, float Damping, FTGOR_MovementOutput& Output) const;

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

	////////////////////////////////////////////////////////////////////////////////////////////////////
protected:

	/** Max collision iterations */
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		int32 MaxCollisionIterations;

	/** Max damping time factor */
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		float MaxDampingStratification;

	/** Collision detection enabled */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "!TGOR Movement|Internal")
		bool SweepsCollision;

	/** Stratisfy timestep into equally sized slizes */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "!TGOR Movement|Internal")
		bool StratisfyTimestep;

	/** Speed with which up-vector is lerped */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "!TGOR Movement|Internal")
		float OrientationSpeed;

	////////////////////////////////////////////////////////////////////////////////////////////////////
protected:

	/** Counts collisions to temporally stratisfy collision detection */
	UPROPERTY()
		float CollisionWeight;

	/** Inflicted movement */
	UPROPERTY( BlueprintReadWrite, Category = "!TGOR Movement")
		FTGOR_MovementDelta Inflicted;

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Max velocity clamp to prevent NaN errors */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "!TGOR Movement|Internal")
		float LinearSpeedClamp;

	/** Max velocity clamp to prevent NaN errors */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "!TGOR Movement|Internal")
		float AngularSpeedClamp;

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

	/** Simulate symplectic integration for a given time */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Movement|Internal", Meta = (Keywords = "C++"))
		void SimulateSymplectic(FTGOR_MovementSpace& Space, const FTGOR_MovementForce& Force, const FTGOR_MovementExternal& External, float Timestep, bool Sweep);

	/** Simulate move for a given time */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Movement|Internal", Meta = (Keywords = "C++"))
		void SimulateMove(FTGOR_MovementSpace& Space, float Timestep, bool Sweep);

	/** Translate over a given time, returns hit form last iteration */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Movement|Internal", Meta = (Keywords = "C++"))
		int32 SimulateTranslate(FTGOR_MovementSpace& Space, float Timestep, bool Sweep, float Ratio, int32 Iteration);

	/** Tries to resolve a penetration */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Movement|Internal", Meta = (Keywords = "C++"))
		bool PullBack(FTGOR_MovementSpace& Space, const FHitResult& OutHit, const FVector& Translation);

	/** Tries to find reparent to a given actor (defaults to current volume if null or invalid) */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Movement|Internal", Meta = (Keywords = "C++"))
		FTGOR_MovementParent FindReparentToActor(AActor* Actor, const FName& Name) const;

	/** Tries to find reparent to a given component (defaults to current volume if null or invalid) */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Movement|Internal", Meta = (Keywords = "C++"))
		FTGOR_MovementParent FindReparentToComponent(UActorComponent* Component, const FName& Name) const;

};
