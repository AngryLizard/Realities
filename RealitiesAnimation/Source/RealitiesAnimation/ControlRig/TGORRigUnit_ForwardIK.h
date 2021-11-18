// The Gateway of Realities: Planes of Existence.

#pragma once

#include "Units/RigUnit.h"
#include "TGORRigUnit_Utility.h"
#include "TGORRigUnit_IK.h"

#include "Animation/InputScaleBias.h"
#include "TGORRigUnit_ForwardIK.generated.h"

/**
 * Iterative IK for spline aiming based on FABRIK with soft constraint
 */
USTRUCT(meta = (DisplayName = "Anchor IK", Keywords = "TGOR,IK", PrototypeName = "AnchorIK", NodeColor = "1.0 0.44 0.0"))
struct REALITIESANIMATION_API FTGORRigUnit_AnchorIK : public FTGORRigUnit_IK
{
	GENERATED_BODY()

		FTGORRigUnit_AnchorIK() {}

	RIGVM_METHOD()
		virtual void Execute(const FRigUnitContext& Context) override;

	virtual FString ProcessPinLabelForInjection(const FString& InLabel) const override;

public:

	/**
	 * The chain to adapt (Has to be continuous chain)
	 */
	UPROPERTY(meta = (Input, ExpandByDefault))
		FRigElementKeyCollection Chain;

	/*
	 * Max angle change per segment
	 */
	UPROPERTY(meta = (Input))
		float MaxAngle = 20.0f;

	/**
	 * Fabrik iterations
	 */
	UPROPERTY(meta = (Input, DetailsOnly))
		int32 Iterations = 10;

	/**
	 * Objective settings
	 */
	UPROPERTY(meta = (Input, DetailsOnly))
		FRigUnit_ObjectiveSettings AnchorSettings;

	/**
	* Target location/rotation for the chain start
	*/
	UPROPERTY(meta = (Input))
		FTransform Anchor = FTransform::Identity;
};

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * Iterative IK for bending a chain between two points
 */
USTRUCT(meta = (DisplayName = "Bend IK", Keywords = "TGOR,IK", PrototypeName = "BendIK", NodeColor = "1.0 0.44 0.0"))
struct REALITIESANIMATION_API FTGORRigUnit_BendIK : public FTGORRigUnit_IK
{
	GENERATED_BODY()

		FTGORRigUnit_BendIK() {}

	RIGVM_METHOD()
		virtual void Execute(const FRigUnitContext& Context) override;

	virtual FString ProcessPinLabelForInjection(const FString& InLabel) const override;

public:

	/**
	 * The chain to adapt (Has to be continuous chain)
	 */
	UPROPERTY(meta = (Input, ExpandByDefault))
		FRigElementKeyCollection Chain;

	/*
	 * Max angle change per segment towards the objective
	 */
	UPROPERTY(meta = (Input))
		float MaxObjectiveAngle = 40.0f;

	/*
	 * Max angle change per segment towards the anchor
	 */
	UPROPERTY(meta = (Input))
		float MaxAnchorAngle = 20.0f;

	/**
	 * Bending iterations
	 */
	UPROPERTY(meta = (Input, DetailsOnly))
		int32 Iterations = 10;

	/**
	 * Objective settings
	 */
	UPROPERTY(meta = (Input, DetailsOnly))
		FRigUnit_ObjectiveSettings AnchorSettings;

	/**
	* Target location/rotation for the chain start
	*/
	UPROPERTY(meta = (Input))
		FTransform Anchor = FTransform::Identity;
};
