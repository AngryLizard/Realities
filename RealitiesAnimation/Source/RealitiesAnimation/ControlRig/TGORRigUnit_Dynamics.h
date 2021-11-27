// The Gateway of Realities: Planes of Existence.

#pragma once

#include "Units/RigUnit.h"
#include "TGORRigUnit_Utility.h"

#include "Animation/InputScaleBias.h"
#include "TGORRigUnit_Dynamics.generated.h"



USTRUCT()
struct FRigUnit_SplineChain_WorkData
{
	GENERATED_BODY()

		FRigUnit_SplineChain_WorkData()
	{
	}

	UPROPERTY()
		TArray<FCachedRigElement> CachedItems;

	UPROPERTY()
		TArray<FVector> Positions;

	UPROPERTY()
		TArray<FVector> Velocities;
};

/**
 * Dynamics for spline chains
 */
USTRUCT(meta = (DisplayName = "SplineChain Dynamics", Category = "TGOR Dynamic", Keywords = "TGOR,Dynamics", PrototypeName = "SplineChainDynamics", NodeColor = "1.0 0.44 0.0"))
struct REALITIESANIMATION_API FTGORRigUnit_SplineChainDynamics : public FTGORRigUnit_Mutable
{
	GENERATED_BODY()

		FTGORRigUnit_SplineChainDynamics() {}

	RIGVM_METHOD()
		virtual void Execute(const FRigUnitContext& Context) override;

	virtual FString ProcessPinLabelForInjection(const FString& InLabel) const override;


	/**
	 * The chain to simulate (Has to be a continuous chain)
	 */
	UPROPERTY(meta = (Input, ExpandByDefault))
		FRigElementKeyCollection Chain;


	/**
	 * Starting tangent
	 */
	UPROPERTY(meta = (Input))
		FVector StartTangent = FVector::ForwardVector;

	/**
	 * Middle tangent
	 */
	UPROPERTY(meta = (Input))
		FVector MiddleTangent = FVector::ForwardVector;

	/**
	 * Ending tangent
	 */
	UPROPERTY(meta = (Input))
		FVector EndTangent = FVector::ForwardVector;

	/**
	 * Applied damping coefficient
	 */
	UPROPERTY(meta = (Input))
		float Damping = 5.0f;

	/**
	 * Spring intensity
	 */
	UPROPERTY(meta = (Input))
		float Spring = 5.0f;

	/**
	 * Constant external force applied to all members
	 */
	UPROPERTY(meta = (Input))
		FVector External = FVector::ZeroVector;

	/**
	 * Dynamics factor
	 */
	UPROPERTY(meta = (Input))
		float Intensity = 0.5f;

	/**
	 * Ending tangent
	 */
	UPROPERTY(meta = (Input))
		FPlane Collider = FPlane(FVector::ZeroVector, FVector::UpVector);


	UPROPERTY(transient)
		FRigUnit_SplineChain_WorkData WorkData;
};

////////////////////////////////////////////////////////////////////////////////////////////////////


USTRUCT()
struct FRigUnit_RetractGrip_WorkData
{
	GENERATED_BODY()

	FRigUnit_RetractGrip_WorkData()
	{
	}

	// Cache
	UPROPERTY()
		FCachedRigElement RetractCache;

	UPROPERTY()
		FVector4 Position = FVector4(0, 0, 0, 0);

	UPROPERTY()
		FVector4 Velocity = FVector4(0, 0, 0, 0);

	UPROPERTY()
		float Compensation = 0.0f;

	UPROPERTY()
		bool bInitialized = false;
};

/**
 * Dynamics for a location retracting on big target changes
 */
USTRUCT(meta = (DisplayName = "Retract and Grip Dynamics", Category = "TGOR Dynamic", Keywords = "TGOR,Dynamics", PrototypeName = "RetractGripDynamics", NodeColor = "1.0 0.44 0.0"))
struct REALITIESANIMATION_API FTGORRigUnit_RetractGripDynamics : public FTGORRigUnit_Mutable
{
	GENERATED_BODY()

		FTGORRigUnit_RetractGripDynamics() {}

	RIGVM_METHOD()
		virtual void Execute(const FRigUnitContext& Context) override;

	virtual FString ProcessPinLabelForInjection(const FString& InLabel) const override;

	/**
	 * Bone we restract towards
	 */
	UPROPERTY(meta = (Input, ExpandByDefault))
		FRigElementKey RetractKey = FRigElementKey(FName(), ERigElementType::Bone);

	/**
	 * Strength of retraction (i.e. relative acceleration)
	 */
	UPROPERTY(meta = (Input))
		float RetractStrength = 1.0f;

	/**
	 * Maximum amount of retraction relative to target distance
	 */
	UPROPERTY(meta = (Input))
		float RetractRatio = 0.75f;

	/**
	 * Maximum distance used to cap velocity
	 */
	UPROPERTY(meta = (Input))
		float MaxDistance = 100.0f;

	/**
	 * Square root of the spring coefficient
	 */
	UPROPERTY(meta = (Input, DetailsOnly))
		float SqrtSpring = 1.0f;

	/**
	 * Maximum acceleration before we start retracting
	 */
	UPROPERTY(meta = (Input, DetailsOnly))
		float AccThreshold = 100.0f;

	/**
	 * Desired target location
	 */
	UPROPERTY(meta = (Input))
		FVector Target = FVector::ZeroVector;

	/**
	 * Current location
	 */
	UPROPERTY(meta = (Output))
		FVector Output = FVector::ZeroVector;


	UPROPERTY(transient)
		FRigUnit_RetractGrip_WorkData WorkData;
};

