// The Gateway of Realities: Planes of Existence.

#pragma once

#include "Units/RigUnit.h"
#include "TGORRigUnit_Utility.h"
#include "TGORRigUnit_IK.h"

#include "Animation/InputScaleBias.h"
#include "TGORRigUnit_SplineIK.generated.h"

/**
 * Analytic IK for spline chains
 */
USTRUCT(meta = (DisplayName = "Spline Chain IK", Category = "TGOR IK", Keywords = "TGOR,IK", PrototypeName = "SplineChainIK", NodeColor = "1.0 0.44 0.0"))
struct REALITIESANIMATION_API FTGORRigUnit_SplineChainIK : public FTGORRigUnit_IK
{
	GENERATED_BODY()

		FTGORRigUnit_SplineChainIK() {}

	RIGVM_METHOD()
		virtual void Execute(const FRigUnitContext& Context) override;

	virtual FString ProcessPinLabelForInjection(const FString& InLabel) const override;

public:

	/**
	 * The chain to adapt (Has to be continuous chain)
	 */
	UPROPERTY(meta = (Input, ExpandByDefault))
		FRigElementKeyCollection Chain;


	/**
	 * Starting direction of the chain
	 */
	UPROPERTY(meta = (Input))
		FVector TangentStart = FVector::ForwardVector;

	/**
	 * Ending direction of the chain
	 */
	UPROPERTY(meta = (Input))
		FVector TangentEnd = FVector::BackwardVector;


	/**
	 * Bendiness of the chain
	 */
	UPROPERTY(meta = (Input))
		float Bend = 1.0f;
};

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * Analytic IK for aiming
 */
USTRUCT(meta = (DisplayName = "Bend To Aim Towards IK", Category = "TGOR IK", Keywords = "TGOR,IK", PrototypeName = "BendTargetIK", NodeColor = "1.0 0.44 0.0"))
struct REALITIESANIMATION_API FTGORRigUnit_BendTargetIK : public FTGORRigUnit_IK
{
	GENERATED_BODY()

		FTGORRigUnit_BendTargetIK() {}

	RIGVM_METHOD()
		virtual void Execute(const FRigUnitContext& Context) override;

	virtual FString ProcessPinLabelForInjection(const FString& InLabel) const override;

public:

	/**
	 * The chain to adapt (Has to be continuous chain)
	 */
	UPROPERTY(meta = (Input, ExpandByDefault))
		FRigElementKeyCollection Chain;

	/**
	 * Bend intensity applied to the chain
	 */
	UPROPERTY(meta = (Input))
		float Intensity = 0.75f;

	/**
	 * Max bending angle per segment
	 */
	UPROPERTY(meta = (Input))
		float MaxBendAngle = 90.0f;

	/**
	 * Max twist angle per segment
	 */
	UPROPERTY(meta = (Input))
		float MaxTwistAngle = 75.0f;

	/**
	 * Desired distance between EE and target
	 */
	UPROPERTY(meta = (Input))
		float Distance = 50.0f;
};
