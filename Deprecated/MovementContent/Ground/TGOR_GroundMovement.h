// TGOR (C) // CHECKED //
#pragma once

#include "Realities/MovementSystem/Content/TGOR_Movement.h"
#include "TGOR_GroundMovement.generated.h"

////////////////////////////////////////////// DECL //////////////////////////////////////////////////////

class UTGOR_MovementComponent;

/**
* TGOR_GroundMovement is a movement mode on the ground
*/
UCLASS()
class REALITIES_API UTGOR_GroundMovement : public UTGOR_Movement
{
	GENERATED_BODY()

public:
	
	////////////////////////////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////// ENGINE ////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////

	UTGOR_GroundMovement();
	virtual void QueryInputVector(const UTGOR_MovementComponent* Component, FVector& OutInput, FVector& OutView) const override;
	virtual void Tick(UTGOR_MovementComponent* Component, const FTGOR_MovementTick& Tick, const FTGOR_MovementSpace& Space, const FTGOR_MovementExternal& External, FTGOR_MovementOutput& Out) override;
	virtual bool Invariant(const UTGOR_MovementComponent* Component, const FTGOR_MovementTick& Tick, const FTGOR_MovementSpace& Space, const FTGOR_MovementExternal& External) const override;

	////////////////////////////////////////////////////////////////////////////////////////////////////
protected:

	/** Height multiplier to get leg length */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "!TGOR Movement")
		float LegLengthMultiplier;


	/** Friction coeffiecient for ground movement */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "!TGOR Movement")
		float FootFrictionCoefficient;

	/** Braking coeffiecient for ground movement */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "!TGOR Movement")
		float BrakeCoefficient;

	/** Aircontrol coeffiecient for ground movement */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "!TGOR Movement")
		float AirControl;

	/** Aircontrol coeffiecient for up and down movement */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "!TGOR Movement")
		float FloatControl;

	/** Grip relative of slope (]0,1], 1 for full grip) */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "!TGOR Movement")
		float SlopeGrip;

	
	/** Threshold (linear Speed) for when character can still stand up */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "!TGOR Movement")
		float LinearSpeedThreshold;

	/** Threshold (angular speed) for when character can still stand up */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "!TGOR Movement")
		float AngularSpeedThreshold;


	/** Applied torque for upright rotation */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "!TGOR Movement")
		float StandupTorque;

	/** Torque damping coefficient */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "!TGOR Movement")
		float AngularDamping;

	/** Force dropoff at the end of and relative to leg length (]0, 1], 1 for falloff range of leg length) */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "!TGOR Movement")
		float LegDropOffMultiplier;

	/** Max force legs can output */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "!TGOR Movement")
		float MaximumLegStrength;

	/** Leg spring Damping multiplier, 1 for (approx) critical damping */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "!TGOR Movement")
		float LegDampingMultiplayer;

	/** Spring multiplier for legs */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "!TGOR Movement")
		float LegSpringCoefficient;

	/** Lean torque coefficient */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "!TGOR Movement")
		float LeanCoefficient;

	/** Multiplier on leg length for tracing [1, 2] */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "!TGOR Movement")
		float TraceLengthMultiplier;

	/** Multiplier on leg radius for tracing [0, 2] */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "!TGOR Movement")
		float TraceRadiusMultiplier;

	/** Use current upvector for tracing instead of current rotation state */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "!TGOR Movement")
		bool TraceUpVector;

	/** Multiplies input (Input still clamps to 1.0). Can be used to change the sprinting/walking speed ratio. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "!TGOR Movement")
		float InputMultiplier;



	/** Speed threshold at which we assume to be moving */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "!TGOR Movement")
		float SpeedThreshold;

	/** Weight towards sampled slope against averaged slope (1.0 weights both the same) */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "!TGOR Movement")
		float SlopeMultiplier;

	/** Amount character can stretch beyond leg length [0, 1] */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "!TGOR Movement")
		float StretchMultiplier;

	/** Crouch factor in terms of Leg length ]0, 1] */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "!TGOR Movement")
		float CrouchMultiplier;

	/** Crouch speed multiplier to maximum speed [0, 1] */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "!TGOR Movement")
		float CrouchSpeedMultiplier;

	

	/** Repel capsule radius multiplier from core capsule radius */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "!TGOR Movement")
		float RepelRadiusMultiplier;

	/** Repel capsule halfheight multiplier from core capsule radius */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "!TGOR Movement")
		float RepelHalfHeightMultiplier;

	/** Repel force */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "!TGOR Movement")
		float WallRepelStrength;

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Called every simulation tick */
	UFUNCTION(BlueprintImplementableEvent, Category = "!TGOR Animation", Meta = (Keywords = "C++"))
		void GroundAnimationTick(UTGOR_MovementComponent* Component, const FTGOR_MovementSpace& Space, const FTGOR_MovementExternal& External, float GroundRatio, const FTGOR_MovementContact& Contact);

	/** Leg length */
	UFUNCTION(BlueprintPure, Category = "!TGOR Movement", Meta = (Keywords = "C++"))
		float GetLegLength(const UTGOR_MovementComponent* Component) const;

	/** Compute crouch speed slowdown */
	UFUNCTION(BlueprintPure, Category = "!TGOR Movement", Meta = (Keywords = "C++"))
		float ComputeCrouchSpeedRatio(float GroundRatio) const;

	/** Compute crouch ratio from Ground ratio. 0.0 means fully crouching, 1.0 means upstanding */
	UFUNCTION(BlueprintPure, Category = "!TGOR Movement", Meta = (Keywords = "C++"))
		float ComputeCrouchRatio(float GroundRatio) const;

	/** Compute height offset from Ground ratio */
	UFUNCTION(BlueprintPure, Category = "!TGOR Movement", Meta = (Keywords = "C++"))
		float ComputeHeightOffset(float GroundRatio, float LegLength) const;

	/** Trace to ground and extract contact point */
	UFUNCTION(BlueprintPure, Category = "!TGOR Movement", Meta = (Keywords = "C++"))
		float TraceForGroundContact(UTGOR_MovementComponent* Component, const FTGOR_MovementSpace& Space, const FVector& UpVector, FTGOR_MovementContact& Contact) const;

	/** Trace to ground and extract only the ground ratio */
	UFUNCTION(BlueprintPure, Category = "!TGOR Movement", Meta = (Keywords = "C++"))
		float TraceForGroundRatio(const UTGOR_MovementComponent* Component, const FTGOR_MovementSpace& Space, const FVector& UpVector, FTGOR_MovementGround& Ground) const;

protected:

	/** Gets stretch coefficient */
	virtual float GetStretch(UTGOR_MovementComponent* Component, const FTGOR_MovementTick& Tick, const FTGOR_MovementSpace& Space, const FTGOR_MovementExternal& External, const FTGOR_MovementContact& Contact, float GroundRatio) const;
	
	/** Computes forces depending on input, return speed ratio [0, 1] */
	virtual float GetInputForce(UTGOR_MovementComponent* Component, const FTGOR_MovementTick& Tick, const FTGOR_MovementSpace& Space, const FTGOR_MovementExternal& External, const FTGOR_MovementContact& Contact, const FTGOR_MovementRepel& Repel, float GroundRatio, FTGOR_MovementOutput& Out) const;
		
	/** Get current friction towards ground */
	float GetFrictionForce(const FTGOR_MovementOutput& Out, const FTGOR_MovementContact& Contact) const;

	/** Trace surroundings for sideways collision detection */
	void GetRepelForce(UTGOR_MovementComponent* Component, const FTGOR_MovementTick& Tick, const FTGOR_MovementSpace& Space, FTGOR_MovementRepel& Repel, FTGOR_MovementOutput& Out) const;

	/** Computes force to make character stand up, stretching legs by the given amount (-1 for crouch, 0 for standing) */
	void GetStandingForce(UTGOR_MovementComponent* Component, const FTGOR_MovementTick& Tick, const FTGOR_MovementSpace& Space, const FTGOR_MovementExternal& External, const FVector& Normal, const FTGOR_MovementContact& Contact, float Stretch, FTGOR_MovementOutput& Out) const;

	/** Computes torque to make character stand up and lean into currently applied force */
	void GetStandingTorque(UTGOR_MovementComponent* Component, const FTGOR_MovementTick& Tick, const FTGOR_MovementSpace& Space, const FTGOR_MovementExternal& External, const FVector& Normal, float SpeedRatio, FTGOR_MovementOutput& Out) const;

	/** Limit forces to current physical ability and apply to output */
	void LimitForces(UTGOR_MovementComponent* Component, const FTGOR_MovementSpace& Space, const FTGOR_MovementExternal& External, const FTGOR_MovementContact& Contact, float SpeedRatio, FTGOR_MovementOutput& Out) const;

private:

};
