// The Gateway of Realities: Planes of Existence.

#pragma once

#include "CoreMinimal.h"
#include "Realities/Utility/Error/TGOR_Error.h"
#include "Realities/Utility/Datastructures/TGOR_Time.h"

#include "Realities/Components/Movement/TGOR_ColliderComponent.h"
#include "TGOR_SymplecticComponent.generated.h"

DECLARE_STATS_GROUP(TEXT("Symplectic Movement"), STATGROUP_SYMPLECTIC, STATCAT_Advanced);
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

/**
* TGOR_SymplecticComponent automatically integrates velocity and location
*/
UCLASS(ClassGroup = (Custom), Blueprintable, meta = (BlueprintSpawnableComponent))
class REALITIES_API UTGOR_SymplecticComponent : public UTGOR_ColliderComponent
{
	GENERATED_BODY()

public:
	UTGOR_SymplecticComponent();

	virtual void InflictPointImpact(const FVector& Point, const FVector& Impulse, int32 Index) override;
	virtual void InflictPointForce(const FVector& Point, const FVector& Force, float DeltaTime, int32 Index) override;

	virtual float Simulate(float Time, bool Replay) override;

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Computes Damping force against an angular velocity */
	UFUNCTION(BlueprintPure, Category = "TGOR Movement", Meta = (Keywords = "C++"))
		void GetLinearDamping(const FTGOR_MovementTick& Tick, const FVector& LinearVelocity, float Damping, FTGOR_MovementOutput& Output) const;

	/** Computes Damping force against a linear velocity */
	UFUNCTION(BlueprintPure, Category = "TGOR Movement", Meta = (Keywords = "C++"))
		void GetAngularDamping(const FTGOR_MovementTick& Tick, const FVector& AngularVelocity, float Damping, FTGOR_MovementOutput& Output) const;

	////////////////////////////////////////////////////////////////////////////////////////////////////
protected:

	/** Get temporal information from a movement base and current object state */
	UFUNCTION(BlueprintPure, Category = "TGOR Movement", Meta = (Keywords = "C++"))
		bool PopulateMovementTick(float Time, FTGOR_MovementTick& OutTick) const;

	////////////////////////////////////////////////////////////////////////////////////////////////////
protected:

	/** Inflict linear momentum */
	UFUNCTION(BlueprintCallable, Category = "TGOR Movement", Meta = (Keywords = "C++"))
		void InflictLinearMomentum(const FTGOR_MovementDynamic& Dynamic, FVector InMomentum, bool CounteractVelocity);

	/** Inflict angular momentum */
	UFUNCTION(BlueprintCallable, Category = "TGOR Movement", Meta = (Keywords = "C++"))
		void InflictAngularMomentum(const FTGOR_MovementDynamic& Dynamic, FVector InAngular, bool CounteractAngular);

	/** Inflict force/torque over a given amount of time */
	UFUNCTION(BlueprintCallable, Category = "TGOR Movement", Meta = (Keywords = "C++"))
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
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TGOR Movement")
		bool SweepsCollision;

	/** Stratisfy timestep into equally sized slizes */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TGOR Movement")
		bool StratisfyTimestep;

	/** Speed with which up-vector is lerped */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TGOR Movement")
		float OrientationSpeed;

	////////////////////////////////////////////////////////////////////////////////////////////////////
protected:

	/** Counts collisions to temporally stratisfy collision detection */
	UPROPERTY()
		float CollisionWeight;

	/** Inflicted movement */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TGOR Movement")
		FTGOR_MovementDelta Inflicted;

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Max velocity clamp to prevent NaN errors */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TGOR Movement")
		float LinearSpeedClamp;

	/** Max velocity clamp to prevent NaN errors */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TGOR Movement")
		float AngularSpeedClamp;

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Grab input parameters from world/parent */
	UFUNCTION(BlueprintCallable, Category = "TGOR Movement", Meta = (Keywords = "C++"))
		virtual void PreComputePhysics(const FTGOR_MovementTick& Tick, bool Replay);

	/** Compute forces and torque */
	UFUNCTION(BlueprintCallable, Category = "TGOR Movement", Meta = (Keywords = "C++"))
		virtual void ComputePhysics(const FTGOR_MovementTick& Tick, const FTGOR_MovementSpace& Space, const FTGOR_MovementExternal& External, bool Replay, FTGOR_MovementOutput& Output);

	/** Returns whether force can be inflicted */
	UFUNCTION(BlueprintPure, Category = "TGOR Movement", Meta = (Keywords = "C++"))
		virtual bool CanInflict() const;

	/** Simulate move for a given time */
	UFUNCTION(BlueprintCallable, Category = "TGOR Movement", Meta = (Keywords = "C++"))
		void SimulateMove(FTGOR_MovementSpace& Space, float Timestep, bool Sweep, bool Replay);

	/** Translate over a given time, returns hit form last iteration */
	UFUNCTION(BlueprintCallable, Category = "TGOR Movement", Meta = (Keywords = "C++"))
		int32 SimulateTranslate(FTGOR_MovementSpace& Space, float Timestep, bool Sweep, float Ratio, int32 Iteration, bool Replay);

	/** Tries to resolve a penetration */
	UFUNCTION(BlueprintCallable, Category = "TGOR Movement", Meta = (Keywords = "C++"))
		bool PullBack(FTGOR_MovementSpace& Space, const FHitResult& OutHit, const FVector& Translation);

	////////////////////////////////////////////////////////////////////////////////////////////////////
protected:

	virtual void LoadFromBufferExternal(int32 Index) override;
	virtual void LoadFromBufferWhole(int32 Index) override;
	virtual void LerpToBuffer(int32 Index, int32 NextIndex, float Alpha) override;
	virtual void StoreToBuffer(int32 Index) override;
	virtual void SetBufferSize(int32 Size) override;

	
	////////////////////////////////////////////////////////////////////////////////////////////////////
private:
	
	/** Inflicted buffer */
	UPROPERTY()
		TArray<FTGOR_MovementDelta> InflictedBuffer;
};
