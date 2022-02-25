// The Gateway of Realities: Planes of Existence.

#pragma once

#include "Units/RigUnit.h"
#include "TGORRigUnit_Triplet.h"

#include "Animation/InputScaleBias.h"
#include "TGORRigUnit_Abstract.generated.h"

/**
 * Retargeting bones around some pivot
 */
USTRUCT(BlueprintType, meta = (Abstract))
struct REALITIESANIMATION_API FTGORRigUnit_Abstract : public FTGORRigUnit_Mutable
{
	GENERATED_BODY()

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
	 * Cone forward vector
	 */
	UPROPERTY(meta = (Input, DetailsOnly))
		FVector ConeAimAxis = FVector::LeftVector;

	/**
	 * Rotation offset
	 */
	UPROPERTY(meta = (Input, Constant))
		FRotator OffsetRotation = FRotator::ZeroRotator;

	/**
	 * Multiplier to all computed lengths
	 */
	UPROPERTY(meta = (Input))
		float LengthMultiplier = 1.0f;

	/**
	 * The base abstractor element
	 */
	UPROPERTY(meta = (Input, ExpandByDefault))
		FRigElementKey AbsBaseKey = FRigElementKey(FName(), ERigElementType::Control);

	UPROPERTY(Transient)
		FCachedRigElement AbsBaseCache;
};

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * Retargeting source chain
 */
USTRUCT(BlueprintType, meta = (Abstract))
struct REALITIESANIMATION_API FTGORRigUnit_AbstractRetarget : public FTGORRigUnit_Abstract
{
	GENERATED_BODY()

	/**
	 * The chain to orient along
	 */
	UPROPERTY(meta = (Input, ExpandByDefault))
		FRigElementKeyCollection SourceChain;

};

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * Orient abstractor along a chain
 */
USTRUCT(meta = (DisplayName = "Abstract Orient", Keywords = "TGOR,Abstractor", PrototypeName = "AbstractOrient", NodeColor = "1.0 0.44 0.0"))
struct REALITIESANIMATION_API FTGORRigUnit_AbstractOrient : public FTGORRigUnit_AbstractRetarget
{
	GENERATED_BODY()

		FTGORRigUnit_AbstractOrient() {}

	RIGVM_METHOD()
		virtual void Execute(const FRigUnitContext& Context) override;

	virtual FString ProcessPinLabelForInjection(const FString& InLabel) const override;

	/**
	 * Cone cone to reference direction
	 */
	UPROPERTY(meta = (Input))
		FRigElementKey ConKey = FRigElementKey(FName(), ERigElementType::Control);

	/**
	 * The abstractor element to orient
	 */
	UPROPERTY(meta = (Input, ExpandByDefault))
		FRigElementKey AbsOrientKey = FRigElementKey(FName(), ERigElementType::Control);


	// Cache
	UPROPERTY(Transient)
		FCachedRigElement ConCache;
	UPROPERTY(Transient)
		FCachedRigElement AbsOrientCache;
};


////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * Adapts abstractor bones to a chain of bones
 */
USTRUCT(meta = (DisplayName = "Abstract Chain", Keywords = "TGOR,Abstractor", PrototypeName = "AbstractChain", NodeColor = "1.0 0.44 0.0"))
struct REALITIESANIMATION_API FTGORRigUnit_AbstractChain : public FTGORRigUnit_AbstractRetarget
{
	GENERATED_BODY()

		FTGORRigUnit_AbstractChain() {}

	RIGVM_METHOD()
		virtual void Execute(const FRigUnitContext& Context) override;

	virtual FString ProcessPinLabelForInjection(const FString& InLabel) const override;

	/**
	 * Abstractor cone to reference direction
	 */
	UPROPERTY(meta = (Input))
		FRigElementKey ConKey = FRigElementKey(FName(), ERigElementType::Control);

	/**
	 * The tip of the chain on the abstractor skeleton  (E.g ABS_Foot_L)
	 */
	UPROPERTY(meta = (Input, ExpandByDefault))
		FRigElementKey AbsTipKey = FRigElementKey(FName(), ERigElementType::Control);


	/**
	 * Upvector Chain Bone Index (What bone index of the Source chain is used for determining the upvector)
	 */
	UPROPERTY(meta = (Input, DetailsOnly))
		int32 ChainUpBoneIdx = 0;

	/**
	 * Upvector Axis (What axis of the Source chain's selected upvector bone to use as upvector direction)
	 */
	UPROPERTY(meta = (Input, DetailsOnly))
		FVector ChainUpAxis = FVector::UpVector;


	/**
	 * What is the current lenght of a straight line from the start of the chain to the end, in cm.
	 */
	UPROPERTY(meta = (Output))
		float CurrentLength = 0.0f;


	// Cache
	UPROPERTY(Transient)
		FCachedRigElement ConCache;
	UPROPERTY(Transient)
		FCachedRigElement AbsTipCache;
};


////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * Retargeting bones around some pivot
 */
USTRUCT(BlueprintType, meta = (Abstract))
struct REALITIESANIMATION_API FTGORRigUnit_AbstractPivot : public FTGORRigUnit_Abstract
{
	GENERATED_BODY()

	/**
	 * Pivot transform on the abstractor skeleton
	 */
	UPROPERTY(meta = (Output))
		FTransform AbsPivot = FTransform::Identity;

	/**
	 * Pivot transform on the source skeleton
	 */
	UPROPERTY(meta = (Output))
		FTransform SourcePivot = FTransform::Identity;
};

////////////////////////////////////////////////////////////////////////////////////////////////////


/**
 * Adapts abstractor bones to linear movement around a given pivot
 */
USTRUCT(meta = (DisplayName = "Abstract Translate", Keywords = "TGOR,Abstractor", PrototypeName = "AbstractTranslate", NodeColor = "1.0 0.44 0.0"))
struct REALITIESANIMATION_API FTGORRigUnit_AbstractTranslate : public FTGORRigUnit_AbstractPivot
{
	GENERATED_BODY()

		FTGORRigUnit_AbstractTranslate() {}

	RIGVM_METHOD()
		virtual void Execute(const FRigUnitContext& Context) override;

	virtual FString ProcessPinLabelForInjection(const FString& InLabel) const override;


	/**
	 * The chain to source motion from
	 */
	UPROPERTY(meta = (Input, ExpandByDefault))
		FRigElementKeyCollection SourceChain;

	/**
	 * Target to orient ourselves towards
	 */
	UPROPERTY(meta = (Input))
		FVector TargetDirection = FVector::ZeroVector;



	/**
	 * Source direction bone aim axis (What axis of the Abstractor's direction bone is pointing towards the end of the chain)
	 */
	UPROPERTY(meta = (Input, DetailsOnly))
		FVector SourceForwardAxis = FVector::LeftVector;

	/**
	 * Source direction upvector axis (What axis of the Abstractor's direction bone is pointing towards the upvector)
	 */
	UPROPERTY(meta = (Input, DetailsOnly))
		FVector SourceUpAxis = FVector::UpVector;


	/**
	 * The abstractor bone acting as pivot attached to the base
	 */
	UPROPERTY(meta = (Input, ExpandByDefault))
		FRigElementKey AbsPivotKey = FRigElementKey(FName(), ERigElementType::Control);

	/**
	 * The abstractor bone moving around the pivot
	 */
	UPROPERTY(meta = (Input, ExpandByDefault))
		FRigElementKey AbsTargetKey = FRigElementKey(FName(), ERigElementType::Control);



	/**
	 * Lerp for pivot offset location between initial and current
	 */
	UPROPERTY(meta = (Input))
		float PivotInitialLerp = 0.0f;

	/**
	 * Lerp for pivot between initial length and measured length (how much to split chain length between pivot and translation)
	 */
	UPROPERTY(meta = (Input))
		float LengthInitialLerp = 0.5f;


	// Cache
	UPROPERTY(Transient)
		FCachedRigElement AbsPivotCache;

	UPROPERTY(Transient)
		FCachedRigElement AbsTargetCache;
};


////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * Retarget a bone around the source initial pose
 */
USTRUCT(meta = (DisplayName = "Abstract Initial", Keywords = "TGOR,Abstractor", PrototypeName = "AbstractInitial", NodeColor = "1.0 0.44 0.0"))
struct REALITIESANIMATION_API FTGORRigUnit_AbstractInitial : public FTGORRigUnit_AbstractPivot
{
	GENERATED_BODY()

		FTGORRigUnit_AbstractInitial() {}

	RIGVM_METHOD()
		virtual void Execute(const FRigUnitContext& Context) override;

	virtual FString ProcessPinLabelForInjection(const FString& InLabel) const override;

	/**
	 * Bone on abstractor to reference for size
	 */
	UPROPERTY(meta = (Input, ExpandByDefault))
		FRigElementKey AbsRefKey = FRigElementKey(FName(), ERigElementType::Control);

	/**
	 * Bone on source to retarget around of
	 */
	UPROPERTY(meta = (Input, ExpandByDefault))
		FRigElementKey SourceBaseKey = FRigElementKey(FName(), ERigElementType::Bone);

	/**
	 * Bone on source to reference for size
	 */
	UPROPERTY(meta = (Input, ExpandByDefault))
		FRigElementKey SourceRefKey = FRigElementKey(FName(), ERigElementType::Bone);


	// Cache
	UPROPERTY(Transient)
		FCachedRigElement AbsRefCache;

	UPROPERTY(Transient)
		FCachedRigElement SourceBaseCache;

	UPROPERTY(Transient)
		FCachedRigElement SourceRefCache;
};


////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * Estimate anchor transform from a triplet of anchor cones
 */
USTRUCT(meta = (DisplayName = "Abstract Anchor", Keywords = "TGOR,Abstractor", PrototypeName = "AbstractAnchor", NodeColor = "1.0 0.44 0.0"))
struct REALITIESANIMATION_API FTGORRigUnit_AbstractAnchor : public FTGORRigUnit_AbstractPivot
{
	GENERATED_BODY()

		FTGORRigUnit_AbstractAnchor() {}

	RIGVM_METHOD()
		virtual void Execute(const FRigUnitContext& Context) override;

	virtual FString ProcessPinLabelForInjection(const FString& InLabel) const override;

	/**
	 * Bone on abstractor to reference for size
	 */
	UPROPERTY(meta = (Input, ExpandByDefault))
		FRigElementKey AbsRefKey = FRigElementKey(FName(), ERigElementType::Control);

	/**
	 * Bone triplet on source end (attached to last source chain member)
	 */
	UPROPERTY(meta = (Input, ExpandByDefault))
		FRigElementKey SourceBaseKey = FRigElementKey(FName(), ERigElementType::Bone);
	UPROPERTY(meta = (Input, ExpandByDefault))
		FRigElementKey SourceLeftKey = FRigElementKey(FName(), ERigElementType::Bone);
	UPROPERTY(meta = (Input, ExpandByDefault))
		FRigElementKey SourceRightKey = FRigElementKey(FName(), ERigElementType::Bone);

	/**
	 * Corresponding cone triplet
	 */
	UPROPERTY(meta = (Input))
		FTGOR_TripletAnalysis ConeAnalysis;

	/**
	 * Reference cone triplet
	 */
	UPROPERTY(meta = (Input))
		FTGOR_TripletAnalysis RefAnalysis;

	// Cache
	UPROPERTY(Transient)
		FCachedRigElement AbsRefCache;

	UPROPERTY(Transient)
		FCachedRigElement SourceBaseCache;
	UPROPERTY(Transient)
		FCachedRigElement SourceLeftCache;
	UPROPERTY(Transient)
		FCachedRigElement SourceRightCache;

};
