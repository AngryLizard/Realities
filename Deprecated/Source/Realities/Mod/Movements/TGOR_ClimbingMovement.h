// TGOR (C) // CHECKED //
#pragma once

#include "CoreMinimal.h"
#include "Engine/EngineTypes.h"
#include "Realities/Mod/Movements/TGOR_Movement.h"
#include "TGOR_ClimbingMovement.generated.h"

////////////////////////////////////////////// DECL //////////////////////////////////////////////////////

class UTGOR_MovementComponent;


////////////////////////////////////////////// STRUCT //////////////////////////////////////////////////////

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
* TGOR_GroundMovement is a movement mode on the ground
*/
UCLASS()
class REALITIES_API UTGOR_ClimbingMovement : public UTGOR_Movement
{
	GENERATED_BODY()

public:
	
	////////////////////////////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////// ENGINE ////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////

	UTGOR_ClimbingMovement();

	virtual void QueryInputVector(const UTGOR_MovementComponent* Component, FVector& OutInput, FVector& OutView) const override;
	virtual void Tick(UTGOR_MovementComponent* Component, const FTGOR_MovementTick& Tick, const FTGOR_MovementSpace& Space, const FTGOR_MovementExternal& External, bool Replay, FTGOR_MovementOutput& Out) override;
	virtual bool Invariant(const UTGOR_MovementComponent* Component, const FTGOR_MovementTick& Tick, const FTGOR_MovementSpace& Space, const FTGOR_MovementExternal& External) const override;

	////////////////////////////////////////////////////////////////////////////////////////////////////
protected:

	/** Max velocity for climbing movement */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TGOR Movement")
		float MaximumSpeed;

	/** Max push force (against the wall) per attachement point (hands/feet) */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TGOR Movement")
		float MaxPushForce;

	/** Max pull force (towards the wall) per attachement point (hands/feet) */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TGOR Movement")
		float MaxPullForce;

	/** Max drag force (along the wall) per attachement point (hands/feet) */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TGOR Movement")
		float MaxDragForce;

	/** Desired distance from wall multiple of capsule radius */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TGOR Movement")
		float DesiredDistanceMultiplier;
	

	/** Torque applied to match rotation with current velocity */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TGOR Movement")
		float VelocityAdaptTorque;

	/** Torque applied to match rotation with current surface normal */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TGOR Movement")
		float SurfaceAdaptTorque;

	/** Torque damping coefficient */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TGOR Movement")
		float AngularDamping;

	/** Speed damping coefficient */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TGOR Movement")
		float LinearDamping;

	/** Braking coeffiecient for climbing movement */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TGOR Movement")
		float BrakeCoefficient;

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Called every simulation tick */
	UFUNCTION(BlueprintImplementableEvent, Category = "TGOR Animation", Meta = (Keywords = "C++"))
		void ClimbingAnimationTick(UTGOR_MovementComponent* Component, const FTGOR_MovementSpace& Space, const TArray<FTGOR_ClimbingAttachement>& Attachements);
	
protected:

	/** Compute desired input force */
	FVector ComputeInputForce(UTGOR_MovementComponent* Component, const FTGOR_MovementSpace& Space) const;

	/** Trace for attachement point */
	bool TraceAttachementPoint(const UTGOR_MovementComponent* Component, const FTGOR_MovementSpace& Space, const FVector& Mask, TArray<FHitResult>& HitResults) const;
	
private:

};
