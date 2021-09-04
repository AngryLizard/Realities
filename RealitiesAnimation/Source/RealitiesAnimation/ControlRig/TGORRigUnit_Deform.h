// The Gateway of Realities: Planes of Existence.

#pragma once

#include "Units/RigUnit.h"
#include "TGORRigUnit_Utility.h"

#include "Animation/InputScaleBias.h"
#include "TGORRigUnit_Deform.generated.h"

/**
 * Retargeting bones around some pivot
 */
USTRUCT(BlueprintType, meta = (Abstract))
struct REALITIESANIMATION_API FTGORRigUnit_Deform : public FTGORRigUnit_Mutable
{
	GENERATED_BODY()

	void DeformReset();
	bool DeformUpdate(const FRigUnitContext& Context);

	/**
	 * Abstractor bone local forward direction
	 */
	UPROPERTY(meta = (Input, DetailsOnly))
		FVector AbsBoneAimAxis = FVector::LeftVector;

	/**
	 * Abstractor bone local up direction
	 */
	UPROPERTY(meta = (Input, DetailsOnly))
		FVector AbsBoneUpAxis = FVector::ForwardVector;

	/**
	 * Rotation offset
	 */
	UPROPERTY(meta = (Input, Constant))
		FRotator OffsetRotation = FRotator::ZeroRotator;

	/**
	 * The base abstractor element
	 */
	UPROPERTY(meta = (Input, ExpandByDefault))
		FRigElementKey AbsBaseKey = FRigElementKey(FName(), ERigElementType::Control);


	// Cache
	UPROPERTY(Transient)
		FCachedRigElement AbsBaseCache;
};

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * Retargeting deform chain
 */
USTRUCT(BlueprintType, meta = (Abstract))
struct REALITIESANIMATION_API FTGORRigUnit_DeformRetarget : public FTGORRigUnit_Deform
{
	GENERATED_BODY()

};

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * Retarget a bone around the source initial pose
 */
USTRUCT(meta = (DisplayName = "Deform Orient", Keywords = "TGOR,Deformation", PrototypeName = "DeformOrient", NodeColor = "1.0 0.44 0.0"))
struct REALITIESANIMATION_API FTGORRigUnit_DeformOrient : public FTGORRigUnit_DeformRetarget
{
	GENERATED_BODY()

		FTGORRigUnit_DeformOrient() {}

	RIGVM_METHOD()
		virtual void Execute(const FRigUnitContext& Context) override;

	virtual FString ProcessPinLabelForInjection(const FString& InLabel) const override;


	/**
	 * How much we offset the direction according to initial pose
	 */
	UPROPERTY(meta = (Input))
		float OffsetDirection = 0.0f;

	/**
	 * Output tangent
	 */
	UPROPERTY(meta = (Output))
		FVector Tangent = FVector::ForwardVector;
};


////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * Adapts Deformation bones to a chain of bones
 */
USTRUCT(meta = (DisplayName = "Deform Chain", Keywords = "TGOR,Deformation", PrototypeName = "DeformChain", NodeColor = "1.0 0.44 0.0"))
struct REALITIESANIMATION_API FTGORRigUnit_DeformChain : public FTGORRigUnit_DeformRetarget
{
	GENERATED_BODY()

		FTGORRigUnit_DeformChain() {}

	RIGVM_METHOD()
		virtual void Execute(const FRigUnitContext& Context) override;

	virtual FString ProcessPinLabelForInjection(const FString& InLabel) const override;

	/**
	* Bone on deform to retarget around of
	*/
	UPROPERTY(meta = (Input, ExpandByDefault))
		FRigElementKey DeformBaseKey = FRigElementKey(FName(), ERigElementType::Bone);

	/**
	 * Pivot to reproject around
	 */
	UPROPERTY(meta = (Input))
		FTransform Pivot = FTransform::Identity;

	/**
	 * Maximum chain length
	 */
	UPROPERTY(meta = (Input))
		float ChainMaxLength = 1.0f;

	/**
	 * How much we offset the direction according to initial pose
	 */
	UPROPERTY(meta = (Input))
		float OffsetDirection = 0.0f;

	/**
	 * The tip of the chain on the Abstractor skeleton  (E.g ABS_Foot_L)
	 */
	UPROPERTY(meta = (Input, ExpandByDefault))
		FRigElementKey AbsTipKey = FRigElementKey(FName(), ERigElementType::Control);


	/**
	 * Output direction
	 */
	UPROPERTY(meta = (Output))
		FVector Direction = FVector::ForwardVector;

	 /**
	  * Objective control
	  */
	UPROPERTY(meta = (Input, ExpandByDefault))
		FRigElementKey ObjBaseKey = FRigElementKey(FName(), ERigElementType::Control);

	/**
	 * Chain's Max Length in cm, determined from measuring length of each bone in the chain and summing it up, then multiplying it by the length multiplier.
	 */
	UPROPERTY(meta = (Output))
		float CurrentLength = 0.0f;

	// Cache
	UPROPERTY(Transient)
		FCachedRigElement DeformCache;
	UPROPERTY(Transient)
		FCachedRigElement AbsTipCache;
	UPROPERTY(Transient)
		FCachedRigElement ObjBaseCache;

};

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * Retargeting bones around some pivot
 */
USTRUCT(BlueprintType, meta = (Abstract))
struct REALITIESANIMATION_API FTGORRigUnit_DeformPivot : public FTGORRigUnit_Deform
{
	GENERATED_BODY()

	void PivotReset();
	bool PivotUpdate(const FRigUnitContext& Context);

	/**
	 * Pivot transform on the abstractor skeleton
	 */
	UPROPERTY(meta = (Output))
		FTransform AbsPivot = FTransform::Identity;

	/**
	 * Pivot transform on the source skeleton
	 */
	UPROPERTY(meta = (Output))
		FTransform DeformPivot = FTransform::Identity;

	/**
	 * Objective control
	 */
	UPROPERTY(meta = (Input, ExpandByDefault))
		FRigElementKey ObjBaseKey = FRigElementKey(FName(), ERigElementType::Control);

	// Cache
	UPROPERTY(Transient)
		FCachedRigElement ObjBaseCache;
};

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * Adapts Deformation bones to linear movement around a given pivot
 */
USTRUCT(meta = (DisplayName = "Deform Translate", Keywords = "TGOR,Deformation", PrototypeName = "DeformTranslate", NodeColor = "1.0 0.44 0.0"))
struct REALITIESANIMATION_API FTGORRigUnit_DeformTranslate : public FTGORRigUnit_DeformPivot
{
	GENERATED_BODY()

		FTGORRigUnit_DeformTranslate() {}

	RIGVM_METHOD()
		virtual void Execute(const FRigUnitContext& Context) override;

	virtual FString ProcessPinLabelForInjection(const FString& InLabel) const override;

	/**
	 * The key the objective is relative to
	 */
	UPROPERTY(meta = (Input, ExpandByDefault))
		FRigElementKey DeformKey = FRigElementKey(FName(), ERigElementType::Bone);

	/**
	 * Maximum chain length
	 */
	UPROPERTY(meta = (Input))
		float ChainMaxLength = 1.0f;

	/**
	 * The Abstractor bone acting as pivot attached to the base
	 */
	UPROPERTY(meta = (Input, ExpandByDefault))
		FRigElementKey AbsPivotKey = FRigElementKey(FName(), ERigElementType::Control);

	/**
	 * The Abstractor bone moving around the pivot
	 */
	UPROPERTY(meta = (Input, ExpandByDefault))
		FRigElementKey AbsTargetKey = FRigElementKey(FName(), ERigElementType::Control);

	/**
	 * Lerp for pivot offset location between initial and current
	 */
	UPROPERTY(meta = (Input))
		float PivotInitialLerp = 0.0f;


	// Cache
	UPROPERTY(Transient)
		FCachedRigElement DeformCache;
	UPROPERTY(Transient)
		FCachedRigElement AbsPivotCache;
	UPROPERTY(Transient)
		FCachedRigElement AbsTargetCache;
};


////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * Retarget a bone around the source initial pose
 */
USTRUCT(meta = (DisplayName = "Deform Initial", Keywords = "TGOR,Deformation", PrototypeName = "DeformInitial", NodeColor = "1.0 0.44 0.0"))
struct REALITIESANIMATION_API FTGORRigUnit_DeformInitial : public FTGORRigUnit_DeformPivot
{
	GENERATED_BODY()

		FTGORRigUnit_DeformInitial() {}

	RIGVM_METHOD()
		virtual void Execute(const FRigUnitContext& Context) override;

	virtual FString ProcessPinLabelForInjection(const FString& InLabel) const override;


	/**
	 * Bone on abstractor to reference for size
	 */
	UPROPERTY(meta = (Input, ExpandByDefault))
		FRigElementKey AbsRefKey = FRigElementKey(FName(), ERigElementType::Control);

	/**
	 * Bone on deform to retarget around of
	 */
	UPROPERTY(meta = (Input, ExpandByDefault))
		FRigElementKey DeformBaseKey = FRigElementKey(FName(), ERigElementType::Bone);

	/**
	 * Bone on deform to reference for size
	 */
	UPROPERTY(meta = (Input, ExpandByDefault))
		FRigElementKey DeformRefKey = FRigElementKey(FName(), ERigElementType::Bone);


	// Cache
	UPROPERTY(Transient)
		FCachedRigElement AbsRefCache;
	UPROPERTY(Transient)
		FCachedRigElement DeformBaseCache;
	UPROPERTY(Transient)
		FCachedRigElement DeformRefCache;
};

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * Estimate root transform from a pair of end-points
 */
USTRUCT(meta = (DisplayName = "Deform Root", Keywords = "TGOR,Deform", PrototypeName = "DeformRoot", NodeColor = "1.0 0.44 0.0"))
struct REALITIESANIMATION_API FTGORRigUnit_DeformRoot : public FTGORRigUnit_DeformPivot
{
	GENERATED_BODY()

		FTGORRigUnit_DeformRoot() {}

	RIGVM_METHOD()
		virtual void Execute(const FRigUnitContext& Context) override;

	virtual FString ProcessPinLabelForInjection(const FString& InLabel) const override;

	/**
	 * Reference abs root bone used to compute relative movement with abstractor
	 */
	UPROPERTY(meta = (Input))
		FRigElementKey AbsRootKey = FRigElementKey(FName(), ERigElementType::Control);

	/**
	 * Bone on deform to retarget around of
	 */
	UPROPERTY(meta = (Input, ExpandByDefault))
		FRigElementKey DeformBaseKey = FRigElementKey(FName(), ERigElementType::Bone);

	/**
	 * Reference deform root bone used to compute relative movement with abstractor
	 */
	UPROPERTY(meta = (Input))
		FRigElementKey DeformRootKey = FRigElementKey(FName(), ERigElementType::Bone);

	/**
	 * Default height of root bone
	 */
	UPROPERTY(meta = (Input))
		float RootHeight = 1.0f;


	// Cache
	UPROPERTY(Transient)
		FCachedRigElement AbsRootCache;
	UPROPERTY(Transient)
		FCachedRigElement DeformBaseCache;
	UPROPERTY(Transient)
		FCachedRigElement DeformRootCache;
};
