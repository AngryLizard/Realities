// TGOR (C) // CHECKED //
#pragma once

#include "CoreMinimal.h"

#include "MovementSystem/Tasks/TGOR_OrientableTask.h"
#include "TGOR_GroundTask.generated.h"

////////////////////////////////////////////// DECL //////////////////////////////////////////////////////

class UTGOR_HandleComponent;
class UTGOR_ArmatureComponent;
class UTGOR_Primitive;

/**
*
*/
UCLASS(Blueprintable)
class MOVEMENTSYSTEM_API UTGOR_GroundTask : public UTGOR_OrientableTask
{
	GENERATED_BODY()

public:

	////////////////////////////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////// ENGINE ////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////

	UTGOR_GroundTask();

	virtual bool Initialise() override;
	virtual bool Invariant(const FTGOR_MovementSpace& Space, const FTGOR_MovementExternal& External) const override;
	virtual void Reset(const FTGOR_MovementSpace& Space, const FTGOR_MovementExternal& External);
	virtual void QueryInput(FVector& OutInput, FVector& OutView) const override;
	virtual void Context(const FTGOR_MovementSpace& Space, const FTGOR_MovementExternal& External, const FTGOR_MovementTick& Tick) override;
	virtual void Update(const FTGOR_MovementSpace& Space, const FTGOR_MovementExternal& External, const FTGOR_MovementTick& Tick, FTGOR_MovementOutput& Out) override;

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Braking coeffiecient for ground movement */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "!TGOR Movement|Control")
		float BrakeCoefficient = 1200.0f;

	/** Aircontrol coeffiecient for ground movement */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "!TGOR Movement|Control")
		float AirControl = 75000.0f;

	/** Aircontrol coeffiecient for up and down movement */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "!TGOR Movement|Control")
		float FloatControl = 0.0f;

	/** Repel force */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "!TGOR Movement|Control")
		float WallRepelStrength = 25000.0f;

	/** Applied torque for upright rotation */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "!TGOR Movement|Control")
		float StandupTorque = 200.0f;

	/** Torque damping coefficient */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "!TGOR Movement|Control")
		float AngularDamping = 15.0f;

	/** Force dropoff at the end of and relative to leg length (]0, 1], 1 for falloff range of leg length) */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "!TGOR Movement|Control")
		float LegDropOffMultiplier = 0.2f;

	/** Lean torque coefficient */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "!TGOR Movement|Control")
		float LeanCoefficient = 15.0f;

	/** Jump Force coefficient */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "!TGOR Movement|Control")
		float JumpCoefficient = 1.0f;



	/** Friction coeffiecient for ground movement */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "!TGOR Movement|Friction")
		float FootFrictionCoefficient = 5.0f;

	/** Grip relative of slope (]0,1], 1 for full grip) */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "!TGOR Movement|Friction")
		float SlopeGrip = 0.4f;


	/** Leg spring Damping multiplier, 1 for (approx) critical damping */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "!TGOR Movement|Spring")
		float LegDampingMultiplayer = 0.05f;

	/** Spring multiplier for legs */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "!TGOR Movement|Spring")
		float LegSpringCoefficient = 0.3f;



	/** Multiplier on leg length for tracing [1, 2] */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "!TGOR Movement|Detection")
		float TraceLengthMultiplier = 1.2f;

	/** Use current upvector for tracing instead of current rotation state */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "!TGOR Movement|Detection")
		bool TraceUpVector = false;

	/** Speed threshold at which we assume to be moving */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "!TGOR Movement|Detection")
		float SpeedThreshold = 64.0f;

	/** Threshold (linear Speed) for when character can still stand up */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "!TGOR Movement|Detection")
		float LinearSpeedThreshold = 5500.0f;

	/** Threshold (angular speed) for when character can still stand up */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "!TGOR Movement|Detection")
		float AngularSpeedThreshold = 40.0f;



	/** Max force legs can output */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "!TGOR Movement|Anatomy")
		float MaximumLegStrength = 1200000.0f;

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Get simple ground contact from current state */
	UFUNCTION(BlueprintPure, Category = "!TGOR Animation", Meta = (Keywords = "C++"))
		void GetGroundContact(const FTGOR_MovementSpace& Space, FVector& SurfaceLocation, FVector& SurfaceNormal) const;

	////////////////////////////////////////////////////////////////////////////////////////////////////
protected:

	/** Update ground contact handles and return current ground properties */
	virtual void UpdateGroundHandles(const FTGOR_MovementSpace& Space, const FVector& UpVector);

	/** Gets stretch coefficient */
	virtual float GetStretch(const FTGOR_MovementTick& Tick, const FTGOR_MovementSpace& Space, const FTGOR_MovementExternal& External) const;

	/** Computes forces depending on input, return speed ratio [0, 1] */
	virtual float GetInputForce(const FTGOR_MovementTick& Tick, const FTGOR_MovementSpace& Space, const FTGOR_MovementExternal& External, const FTGOR_MovementRepel& Repel, FTGOR_MovementOutput& Out) const;

	/** Trace to ground and extract contact point */
	void TraceForGroundContact(const FTGOR_MovementSpace& Space, const FVector& UpVector);

	/** Get current friction towards ground */
	float GetFrictionForce(const FTGOR_MovementOutput& Out) const;

	/** Trace surroundings for sideways collision detection */
	void GetRepelForce(const FTGOR_MovementTick& Tick, const FTGOR_MovementSpace& Space, FTGOR_MovementRepel& Repel, FTGOR_MovementOutput& Out) const;

	/** Computes force to make character stand up, stretching legs by the given amount (-1 for crouch, 0 for standing) */
	void GetStandingForce(const FTGOR_MovementTick& Tick, const FTGOR_MovementSpace& Space, const FTGOR_MovementExternal& External, const FVector& Normal, float Stretch, FTGOR_MovementOutput& Out) const;

	/** Computes torque to make character stand up and lean into currently applied force */
	void GetStandingTorque(const FTGOR_MovementTick& Tick, const FTGOR_MovementSpace& Space, const FTGOR_MovementExternal& External, const FVector& Normal, float SpeedRatio, FTGOR_MovementOutput& Out) const;

	/** Limit forces to current physical ability and apply to output */
	void LimitForces(const FTGOR_MovementSpace& Space, const FTGOR_MovementExternal& External, float SpeedRatio, FTGOR_MovementOutput& Out) const;

	////////////////////////////////////////////////////////////////////////////////////////////////////
protected:

	/** Ground contact */
	UPROPERTY(Transient, BlueprintReadOnly, Category = "!TGOR Movement")
		FTGOR_MovementContact MovementContact;

	/** Local up vector used for movement */
	UPROPERTY(Transient, BlueprintReadOnly, Category = "!TGOR Movement")
		FVector MovementOrientation;

	/** Maximum ground ratio supported by current leg setup */
	UPROPERTY( BlueprintReadOnly, Category = "!TGOR Movement")
		float MaxGroundRatio = 1.0f;

	/** Minimum ground ratio supported by current leg setup */
	UPROPERTY( BlueprintReadOnly, Category = "!TGOR Movement")
		float MinGroundRatio = 0.0f;
};
