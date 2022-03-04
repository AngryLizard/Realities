// The Gateway of Realities: Planes of Existence.

#pragma once

#include "Units/RigUnit.h"
#include "TGORRigUnit_Utility.h"
#include "TGORRigUnit_IK.h"

#include "Animation/InputScaleBias.h"
#include "TGORRigUnit_TriangleIK.generated.h"

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * Analytic IK for hinges
 */
USTRUCT(meta = (DisplayName = "Analytic Hinge IK", Category = "TGOR IK", Keywords = "TGOR,IK", PrototypeName = "HingeIK", NodeColor = "1.0 0.44 0.0"))
struct REALITIESANIMATION_API FTGORRigUnit_HingeIK : public FTGORRigUnit_IK
{
	GENERATED_BODY()

		FTGORRigUnit_HingeIK() {}

	RIGVM_METHOD()
		virtual void Execute(const FRigUnitContext& Context) override;

	

public:

	/**
	 * The chain to adapt (ought to be length 3 and continuous
	 */
	UPROPERTY(meta = (Input, ExpandByDefault))
		FRigElementKeyCollection Chain;

	/**
	 * Desired length of this chain
	 */
	UPROPERTY(meta = (Input))
		float TargetLength = 1.0f;

	/**
	 * Length customisation
	 */
	UPROPERTY(meta = (Input))
		FVector2D Customisation = FVector2D(1.0f, 1.0f);

	/**
	 * Direction to be used for the hinge
	 */
	UPROPERTY(meta = (Input))
		FVector Direction = FVector::ForwardVector;

	/**
	 * How soon hinge starts to stretch
	 */
	UPROPERTY(meta = (Input, ExpandByDefault))
		float StretchThreshold = 0.01f;

};

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * Analytic IK for digitigrade legs
 */
USTRUCT(meta = (DisplayName = "Analytic Digitigrade IK", Category = "TGOR IK", Keywords = "TGOR,IK", PrototypeName = "DigitigradeIK", NodeColor = "1.0 0.44 0.0"))
struct REALITIESANIMATION_API FTGORRigUnit_DigitigradeIK : public FTGORRigUnit_IK
{
	GENERATED_BODY()

		FTGORRigUnit_DigitigradeIK() {}

	RIGVM_METHOD()
		virtual void Execute(const FRigUnitContext& Context) override;

	

public:

	/**
	 * The chain to adapt (ought to be length 4 and continuous
	 */
	UPROPERTY(meta = (Input, ExpandByDefault))
		FRigElementKeyCollection Chain;

	/**
	 * Minimum knee angle in degrees
	 */
	UPROPERTY(meta = (Input))
		float MinKneeAngle = 25.0f;

	/**
	 * Minimum ankle angle in degrees
	 */
	UPROPERTY(meta = (Input))
		float MinAnkleAngle = 25.0f;

	/**
	 * Ankle bend when stretched
	 */
	UPROPERTY(meta = (Input))
		float StandingBend = 3.0f;

	/**
	 * Percentage of how much the ankle direction is determined by the hip direction
	 */
	UPROPERTY(meta = (Input, DetailsOnly))
		float AnkleKneeDirectionWeight = 1.0f;

	/**
	 * Percentage of how much the knee direction is determined by the hip direction
	 */
	UPROPERTY(meta = (Input, DetailsOnly))
		float KneeHipDirectionWeight = 1.0f;

	/**
	 * Length customisation
	 */
	UPROPERTY(meta = (Input))
		FVector Customisation = FVector(1.0f, 1.0f, 1.0f);
};


////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * Analytic IK for clavicles
 */
USTRUCT(meta = (DisplayName = "Analytic Clavicle IK", Category = "TGOR IK", Keywords = "TGOR,IK", PrototypeName = "ClavicleIK", NodeColor = "1.0 0.44 0.0"))
struct REALITIESANIMATION_API FTGORRigUnit_ClavicleIK : public FTGORRigUnit_IK
{
	GENERATED_BODY()

		FTGORRigUnit_ClavicleIK() {}

	RIGVM_METHOD()
		virtual void Execute(const FRigUnitContext& Context) override;

	

public:

	/**
	 * The chain to adapt (ought to be length 4 and continuous
	 */
	UPROPERTY(meta = (Input, ExpandByDefault))
		FRigElementKeyCollection Chain;

	/**
	 * Minimum ellbow angle in degrees
	 */
	UPROPERTY(meta = (Input))
		float MinEllbowAngle = 25.0f;

	/**
	 * Maximum clavicle angle in degrees (can't be more than 90°)
	 */
	UPROPERTY(meta = (Input))
		float MaxClavicleAngle = 10.0f;

	/**
	 * Exponential bias for MaxClavicleAngle in vector direction (clavicle bone space)
	 */
	UPROPERTY(meta = (Input, DetailsOnly))
		FVector ClavicleBias = FVector::ZeroVector;

	/**
	 * Objective axis to use for ellbow alignment
	 */
	UPROPERTY(meta = (Input, DetailsOnly))
		FVector EllbowAlignmentAxis = FVector::ForwardVector;

	/**
	 * Length customisation
	 */
	UPROPERTY(meta = (Input))
		FVector Customisation = FVector(1.0f, 1.0f, 1.0f);
};

