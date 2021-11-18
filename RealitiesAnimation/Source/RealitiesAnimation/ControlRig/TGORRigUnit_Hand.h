// The Gateway of Realities: Planes of Existence.

#pragma once

#include "Units/RigUnit.h"
#include "TGORRigUnit_IK.h"
#include "TGORRigUnit_Utility.h"

#include "Animation/InputScaleBias.h"
#include "TGORRigUnit_Hand.generated.h"


USTRUCT(BlueprintType)
struct FTGOR_HandParams
{
	GENERATED_BODY()

		FTGOR_HandParams()
	{
	}

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ClampMin = "0.0", ClampMax = "1.0"))
		float Ring = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ClampMin = "0.0", ClampMax = "1.0"))
		float Middle = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ClampMin = "0.0", ClampMax = "1.0"))
		float Index = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ClampMin = "0.0", ClampMax = "1.0"))
		float Thumb = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ClampMin = "0.0", ClampMax = "1.0"))
		float Spread = 0.0f;
};

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * Closest point on an ellipsoid
 */
USTRUCT(meta = (DisplayName = "Finger Transformn", Keywords = "TGOR,Collision", PrototypeName = "FingerTransform", NodeColor = "1.0 0.44 0.0"))
struct REALITIESANIMATION_API FTGORRigUnit_FingerTransform : public FTGORRigUnit_Mutable
{
	GENERATED_BODY()

		FTGORRigUnit_FingerTransform() {}

	RIGVM_METHOD()
		virtual void Execute(const FRigUnitContext& Context) override;

	virtual FString ProcessPinLabelForInjection(const FString& InLabel) const override;

public:

	/**
	 * Base bone to offset
	 */
	UPROPERTY(meta = (Input, ExpandByDefault))
		FRigElementKey BaseKey = FRigElementKey(FName(), ERigElementType::Bone);

	/**
	 * Finger bone to offset
	 */
	UPROPERTY(meta = (Input, ExpandByDefault))
		FRigElementKey FingerKey = FRigElementKey(FName(), ERigElementType::Bone);

	/**
	* Grab rotation [0..1]
	*/
	UPROPERTY(meta = (Input))
		float Grab = 0.0f;

	/**
	* Range in degrees for grab values
	*/
	UPROPERTY(meta = (Input, DetailsOnly))
		FVector2D GrabRange = FVector2D(0.0f, 20.0f);

	/**
	* Lateral rotation [0..1]
	*/
	UPROPERTY(meta = (Input))
		float Spread = 0.0f;

	/**
	* Base range in degrees for spread values
	*/
	UPROPERTY(meta = (Input, DetailsOnly))
		FVector2D BaseSpreadRange = FVector2D(0.0f, 20.0f);

	/**
	* Rotation axis for spread
	*/
	UPROPERTY(meta = (Input, DetailsOnly))
		FVector BaseSpreadAxis = FVector::ForwardVector;

	/**
	* Finger range in degrees for spread values
	*/
	UPROPERTY(meta = (Input, DetailsOnly))
		FVector2D FingerSpreadRange = FVector2D(0.0f, 20.0f);

	/**
	* Rotation axis for spread
	*/
	UPROPERTY(meta = (Input, DetailsOnly))
		FVector FingerSpreadAxis = FVector::ForwardVector;

	/**
	* Output grab angle in degrees
	*/
	UPROPERTY(meta = (Output))
		float Angle = 0.0f;

	// Cache
	UPROPERTY(Transient)
		FCachedRigElement BaseCache;

	UPROPERTY(Transient)
		FCachedRigElement FingerCache;
};

////////////////////////////////////////////////////////////////////////////////////////////////////
