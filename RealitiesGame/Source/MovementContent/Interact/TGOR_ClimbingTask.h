// TGOR (C) // CHECKED //
#pragma once

#include "CoreMinimal.h"

#include "MovementSystem/Tasks/TGOR_EuclideanMovementTask.h"
#include "TGOR_ClimbingTask.generated.h"

////////////////////////////////////////////// DECL //////////////////////////////////////////////////////

/**
*
*/
USTRUCT(BlueprintType)
struct FTGOR_ClimbingAttachement
{
	GENERATED_USTRUCT_BODY()

public:
	FTGOR_ClimbingAttachement();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FVector2D Local;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FVector Location;
};


/**
*
*/
UCLASS(Blueprintable)
class MOVEMENTCONTENT_API UTGOR_ClimbingTask : public UTGOR_EuclideanMovementTask
{
	GENERATED_BODY()

public:

	////////////////////////////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////// ENGINE ////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////

	UTGOR_ClimbingTask();
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual void Initialise() override;
	virtual bool Invariant(const FTGOR_MovementSpace& Space, const FTGOR_MovementExternal& External) const override;
	virtual void Reset(const FTGOR_MovementSpace& Space, const FTGOR_MovementExternal& External);
	virtual void QueryInput(FVector& OutInput, FVector& OutView) const override;
	virtual void Update(FTGOR_MovementSpace& Space, const FTGOR_MovementExternal& External, const FTGOR_MovementTick& Tick, FTGOR_MovementOutput& Out) override;

	////////////////////////////////////////////////////////////////////////////////////////////////////
protected:

	/** Max velocity for climbing movement */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "!TGOR Movement")
		float MaximumSpeed = 200.0f;

	/** Max push force (against the wall) per attachement point (hands/feet) */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "!TGOR Movement")
		float MaxPushForce = 100000.0f;

	/** Max pull force (towards the wall) per attachement point (hands/feet) */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "!TGOR Movement")
		float MaxPullForce = 80000.0f;

	/** Max drag force (along the wall) per attachement point (hands/feet) */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "!TGOR Movement")
		float MaxDragForce = 190000.0f;

	/** Desired distance from wall multiple of capsule radius */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "!TGOR Movement")
		float DesiredDistanceMultiplier = 1.2f;


	/** Torque applied to match rotation with current velocity */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "!TGOR Movement")
		float VelocityAdaptTorque = 1.0f;

	/** Torque applied to match rotation with current surface normal */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "!TGOR Movement")
		float SurfaceAdaptTorque = 1000.0f;

	/** Torque damping coefficient */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "!TGOR Movement")
		float AngularDamping = 300.0f;

	/** Speed damping coefficient */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "!TGOR Movement")
		float LinearDamping = 800.0f;

	/** Braking coeffiecient for climbing movement */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "!TGOR Movement")
		float BrakeCoefficient = 500.0f;

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Called every simulation tick */
	UFUNCTION(BlueprintImplementableEvent, Category = "!TGOR Animation", Meta = (Keywords = "C++"))
		void ClimbingAnimationTick(const FTGOR_MovementSpace& Space, const TArray<FTGOR_ClimbingAttachement>& Attachements);

protected:

	/** Compute desired input force */
	FVector ComputeInputForce(const FTGOR_MovementSpace& Space) const;

	/** Trace for attachement point */
	bool TraceAttachementPoint(const FTGOR_MovementSpace& Space, const FVector& Mask, TArray<FHitResult>& HitResults) const;

};
