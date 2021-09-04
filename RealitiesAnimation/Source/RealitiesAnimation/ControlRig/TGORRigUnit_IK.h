// The Gateway of Realities: Planes of Existence.

#pragma once

#include "Units/RigUnit.h"
#include "TGORRigUnit_Utility.h"

#include "Animation/InputScaleBias.h"
#include "TGORRigUnit_IK.generated.h"

/**
 * IK nodes
 */
USTRUCT(BlueprintType, meta = (Abstract))
struct REALITIESANIMATION_API FTGORRigUnit_IK : public FTGORRigUnit_Mutable
{
	GENERATED_BODY()


	/**
		* End-effector local forward axis
		*/
	UPROPERTY(meta = (Input, DetailsOnly))
		FVector EffectorForwardAxis = FVector::LeftVector;

	/**
	 * End-effector local up axis
	 */
	UPROPERTY(meta = (Input, DetailsOnly))
		FVector EffectorUpAxis = FVector::ForwardVector;

	/**
	* Objective local forward axis
	*/
	UPROPERTY(meta = (Input, DetailsOnly))
		FVector ObjectiveForwardAxis = FVector::RightVector;

	/**
	 * Objective local up axis
	 */
	UPROPERTY(meta = (Input, DetailsOnly))
		FVector ObjectiveUpAxis = FVector::DownVector;

	/**
	* Target location/rotation for the chain end
	*/
	UPROPERTY(meta = (Input))
		FTransform Objective = FTransform::Identity;
};

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * Analytic IK for spline chains
 */
USTRUCT(meta = (DisplayName = "SplineChain IK", Keywords = "TGOR,IK", PrototypeName = "SplineChainIK", NodeColor = "1.0 0.44 0.0"))
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
 * Analytic IK for hinges
 */
USTRUCT(meta = (DisplayName = "Hinge IK", Keywords = "TGOR,IK", PrototypeName = "HingeIK", NodeColor = "1.0 0.44 0.0"))
struct REALITIESANIMATION_API FTGORRigUnit_HingeIK : public FTGORRigUnit_IK
{
	GENERATED_BODY()

		FTGORRigUnit_HingeIK() {}

	RIGVM_METHOD()
		virtual void Execute(const FRigUnitContext& Context) override;

	virtual FString ProcessPinLabelForInjection(const FString& InLabel) const override;

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
USTRUCT(meta = (DisplayName = "Digitigrade IK", Keywords = "TGOR,IK", PrototypeName = "DigitigradeIK", NodeColor = "1.0 0.44 0.0"))
struct REALITIESANIMATION_API FTGORRigUnit_DigitigradeIK : public FTGORRigUnit_IK
{
	GENERATED_BODY()

		FTGORRigUnit_DigitigradeIK() {}

	RIGVM_METHOD()
		virtual void Execute(const FRigUnitContext& Context) override;

	virtual FString ProcessPinLabelForInjection(const FString& InLabel) const override;

public:

	/**
	 * The chain to adapt (ought to be length 4 and continuous
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
		FVector Customisation = FVector(1.0f, 1.0f, 1.0f);

	/**
	 * Direction to be used for the leg
	 */
	UPROPERTY(meta = (Input))
		FVector Direction = FVector::ForwardVector;

	/**
	 * Ratio from knee to ankle that slides along objective direction
	 */
	UPROPERTY(meta = (Input))
		float LowerPivot = 0.5f;

	/**
	 * Ratio from ankle to foot on plantigrade mode
	 */
	UPROPERTY(meta = (Input))
		float FootPivot = 0.5f;

	/**
	 * How much the leg is digitigrade (0 for plantigrade)
	 */
	UPROPERTY(meta = (Input))
		float Digitness = 0.0f;
};


////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * Analytic IK for clavicles
 */
USTRUCT(meta = (DisplayName = "Clavicle IK", Keywords = "TGOR,IK", PrototypeName = "ClavicleIK", NodeColor = "1.0 0.44 0.0"))
struct REALITIESANIMATION_API FTGORRigUnit_ClavicleIK : public FTGORRigUnit_IK
{
	GENERATED_BODY()

		FTGORRigUnit_ClavicleIK() {}

	RIGVM_METHOD()
		virtual void Execute(const FRigUnitContext& Context) override;

	virtual FString ProcessPinLabelForInjection(const FString& InLabel) const override;

public:

	/**
	 * The chain to adapt (ought to be length 4 and continuous
	 */
	UPROPERTY(meta = (Input, ExpandByDefault))
		FRigElementKeyCollection Chain;

	UPROPERTY(meta = (Input))
		float MaxChainLength = 0.0f;

	UPROPERTY(meta = (Input))
		float Strength = 1.0f;

	UPROPERTY(meta = (Input))
		float AngleLimit = 0.1f;


	/**
	* Source clavicle location
	*/
	UPROPERTY(meta = (Input))
		FVector Clavicle = FVector::ZeroVector;


};