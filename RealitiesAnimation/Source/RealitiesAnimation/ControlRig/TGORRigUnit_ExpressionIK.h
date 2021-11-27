// The Gateway of Realities: Planes of Existence.

#pragma once

#include "Units/RigUnit.h"
#include "TGORRigUnit_Utility.h"
#include "TGORRigUnit_IK.h"

#include "Animation/InputScaleBias.h"
#include "TGORRigUnit_ExpressionIK.generated.h"

/**
 * Analytic IK for look-at targets
 */
USTRUCT(meta = (DisplayName = "Constrained Look At IK", Category = "TGOR Expression", Keywords = "TGOR,IK", PrototypeName = "LookAtIK", NodeColor = "1.0 0.44 0.0"))
struct REALITIESANIMATION_API FTGORRigUnit_LookAtIK : public FTGORRigUnit_IK
{
	GENERATED_BODY()

		FTGORRigUnit_LookAtIK() {}

	RIGVM_METHOD()
		virtual void Execute(const FRigUnitContext& Context) override;

	virtual FString ProcessPinLabelForInjection(const FString& InLabel) const override;

public:

	/**
	 * Bone to aim
	 */
	UPROPERTY(meta = (Input, ExpandByDefault))
		FRigElementKey Key = FRigElementKey(FName(), ERigElementType::Bone);

	/**
	 * Look-at intensity
	 */
	UPROPERTY(meta = (Input))
		float Intensity = 1.0f;

	/**
	 * Offset bias in degrees
	 */
	UPROPERTY(meta = (Input))
		FVector2D Bias = FVector2D::ZeroVector;

	/**
	 * Max range in each direction in degrees
	 */
	UPROPERTY(meta = (Input))
		FVector2D Range = FVector2D(40.0f, 30.0f);

	// Cache
	UPROPERTY(Transient)
		FCachedRigElement Cache;
};

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * Analytic IK for eyelid targets
 */
USTRUCT(meta = (DisplayName = "Eyelid IK", Category = "TGOR Expression", Keywords = "TGOR,IK", PrototypeName = "EyelidIK", NodeColor = "1.0 0.44 0.0"))
struct REALITIESANIMATION_API FTGORRigUnit_EyelidIK : public FTGORRigUnit_IK
{
	GENERATED_BODY()

		FTGORRigUnit_EyelidIK() {}

	RIGVM_METHOD()
		virtual void Execute(const FRigUnitContext& Context) override;

	virtual FString ProcessPinLabelForInjection(const FString& InLabel) const override;

public:

	/**
	 * Bone for eyelid to move
	 */
	UPROPERTY(meta = (Input, ExpandByDefault))
		FRigElementKey Key = FRigElementKey(FName(), ERigElementType::Bone);

	/**
	 * Forward axis of the eye bone
	 */
	UPROPERTY(meta = (Input, DetailsOnly))
		FVector EyeDirection = FVector::ForwardVector;

	/**
	 * Amount the eye lid is open (from fully open to iris, used for emoting)
	 */
	UPROPERTY(meta = (Input))
		float Openness = 0.5f;

	/**
	 * Amount the lid is closed (from openess to fully closed, used for blinking)
	 */
	UPROPERTY(meta = (Input))
		float Closeness = 0.0f;

	/**
	 * Iris offset in radians
	 */
	UPROPERTY(meta = (Input))
		float Offset = 0.0f;

	/**
	 * Offset bias in degrees
	 */
	UPROPERTY(meta = (Input))
		float Bias = 0.0f;

	/**
	 * Max range in each direction in degrees
	 */
	UPROPERTY(meta = (Input))
		float Range = 30.0f;

	// Cache
	UPROPERTY(Transient)
		FCachedRigElement Cache;
};
