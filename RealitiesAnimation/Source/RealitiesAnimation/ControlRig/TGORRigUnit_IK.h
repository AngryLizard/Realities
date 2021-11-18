// The Gateway of Realities: Planes of Existence.

#pragma once

#include "Units/RigUnit.h"
#include "TGORRigUnit_Utility.h"

#include "Animation/InputScaleBias.h"
#include "TGORRigUnit_IK.generated.h"

USTRUCT()
struct REALITIESANIMATION_API FRigUnit_ObjectiveSettings
{
	GENERATED_BODY()

	FRigUnit_ObjectiveSettings()
	{
	}

	/**
	* Objective local forward axis
	*/
	UPROPERTY(EditAnywhere, meta = (Input), Category = "ObjectiveSettings")
		FVector ForwardAxis = FVector::RightVector;

	/**
	 * Objective local up axis
	 */
	UPROPERTY(EditAnywhere, meta = (Input), Category = "ObjectiveSettings")
		FVector UpAxis = FVector::DownVector;

	/**
	 * Objective local offset
	 */
	UPROPERTY(EditAnywhere, meta = (Input), Category = "ObjectiveSettings")
		FVector Offset = FVector::ZeroVector;

	/**
	* End-effector local forward axis
	*/
	UPROPERTY(EditAnywhere, meta = (Input), Category = "ObjectiveSettings")
		FVector EffectorForwardAxis = FVector::LeftVector;

	/**
	 * End-effector local up axis
	 */
	UPROPERTY(EditAnywhere, meta = (Input), Category = "ObjectiveSettings")
		FVector EffectorUpAxis = FVector::ForwardVector;
};


/**
 * IK nodes
 */
USTRUCT(BlueprintType, meta = (Abstract))
struct REALITIESANIMATION_API FTGORRigUnit_IK : public FTGORRigUnit_Mutable
{
	GENERATED_BODY()

	/**
	 * Objective settings
	 */
	UPROPERTY(meta = (Input, DetailsOnly))
		FRigUnit_ObjectiveSettings ObjectiveSettings;

	/**
	* Target location/rotation for the chain end
	*/
	UPROPERTY(meta = (Input))
		FTransform Objective = FTransform::Identity;
};
