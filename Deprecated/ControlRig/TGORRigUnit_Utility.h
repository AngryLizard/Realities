// The Gateway of Realities: Planes of Existence.

#pragma once

#include "Units/RigUnit.h"
#include "Animation/InputScaleBias.h"
#include "TGORRigUnit_Utility.generated.h"



UENUM(BlueprintType)
enum class ETGOR_Propagation : uint8
{
	/** Don't propagate to children */
	Off,
	/** Only propagate to children of last node (fast) */
	OnlyLast,
	/** Propagate to all children (slowest) */
	All
};


USTRUCT()
struct REALITIESANIMATION_API FRigUnit_DebugSettings
{
	GENERATED_BODY()

		FRigUnit_DebugSettings()
	{
		bEnabled = false;
		Scale = 1.f;
	}

	/**
	 * If enabled debug information will be drawn
	 */
	UPROPERTY(EditAnywhere, meta = (Input), Category = "DebugSettings")
		bool bEnabled;

	/**
	 * The size of the debug drawing information
	 */
	UPROPERTY(EditAnywhere, meta = (Input, EditCondition = "bEnabled"), Category = "DebugSettings")
		float Scale;
};

/**
 * Nodes with debug settings
 */
USTRUCT(BlueprintType, meta = (Abstract))
struct REALITIESANIMATION_API FTGORRigUnit_Mutable : public FRigUnitMutable
{
	GENERATED_BODY()

	UPROPERTY(meta = (Input, DetailsOnly))
		FRigUnit_DebugSettings DebugSettings;

	/**
	 * If set to true all of the global transforms of the children
	 * of this bone will be recalculated based on their local transforms.
	 * Note: This is computationally more expensive than turning it off.
	 */
	UPROPERTY(meta = (Input, Constant))
		ETGOR_Propagation PropagateToChildren = ETGOR_Propagation::All;
};


/**
 * Read-only nodes with debug settings
 */
USTRUCT(BlueprintType, meta = (Abstract))
struct REALITIESANIMATION_API FTGORRigUnit_ReadOnly : public FRigUnit
{
	GENERATED_BODY()

		UPROPERTY(meta = (Input, DetailsOnly))
		FRigUnit_DebugSettings DebugSettings;
};

////////////////////////////////////////////////////////////////////////////////////////////////////


USTRUCT()
struct REALITIESANIMATION_API FRigUnit_BoneCustomisation
{
	GENERATED_BODY()

		FRigUnit_BoneCustomisation()
	{
	}

	/**
	 */
	UPROPERTY(EditAnywhere, meta = (Input))
		FRigElementKey Key;

	/**
	 */
	UPROPERTY(EditAnywhere, meta = (Input))
		float Scale = 1.0f;
};

/**
 * C
 */
USTRUCT(meta = (DisplayName = "Customise initial", Keywords = "TGOR,Utility", PrototypeName = "CustomiseInitial", NodeColor = "1.0 0.44 0.0"))
struct REALITIESANIMATION_API FTGORRigUnit_CustomiseInitial : public FTGORRigUnit_Mutable
{
	GENERATED_BODY()

		FTGORRigUnit_CustomiseInitial() {}

	RIGVM_METHOD()
		virtual void Execute(const FRigUnitContext& Context) override;

	virtual FString ProcessPinLabelForInjection(const FString& InLabel) const override;

	/**
	 */
	UPROPERTY(meta = (Input, ExpandByDefault))
		TEnumAsByte<EAxis::Type> ScaleAxis = EAxis::Y;

	/**
	 */
	UPROPERTY(meta = (Input, ExpandByDefault))
		TArray<FRigUnit_BoneCustomisation> Items;

	UPROPERTY(transient)
		TArray<FCachedRigElement> CachedItems;

	UPROPERTY(transient)
		TArray<FTransform> Transforms;
};


////////////////////////////////////////////////////////////////////////////////////////////////////


/**
 * Copy transforms from bones to controls
 */
USTRUCT(meta = (DisplayName = "Clone Transforms", Keywords = "TGOR,Utility", PrototypeName = "CloneTransforms", NodeColor = "1.0 0.44 0.0"))
struct REALITIESANIMATION_API FTGORRigUnit_Propagate : public FTGORRigUnit_Mutable
{
	GENERATED_BODY()

		FTGORRigUnit_Propagate() {}

	RIGVM_METHOD()
		virtual void Execute(const FRigUnitContext& Context) override;

	virtual FString ProcessPinLabelForInjection(const FString& InLabel) const override;

	static void PropagateChainTorwards(const FRigElementKey& Current, const FRigElementKey& Next, const FVector& Target, FRigHierarchyContainer* Hierarchy, bool bPropagateToChildren);

	/**
	 */
	UPROPERTY(meta = (Input, ExpandByDefault))
		FRigElementKey Key;

	/**
	 */
	UPROPERTY(meta = (Input, ExpandByDefault))
		FRigElementKey NextKey;

	/**
	 */
	UPROPERTY(meta = (Input, ExpandByDefault))
		FVector TargetLocation = FVector::ZeroVector;

};

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * Copy transforms from bones to controls
 */
USTRUCT(meta = (DisplayName = "Clone Transforms", Keywords = "TGOR,Utility", PrototypeName = "CloneTransforms", NodeColor = "1.0 0.44 0.0"))
struct REALITIESANIMATION_API FTGORRigUnit_CloneTransforms : public FRigUnitMutable
{
	GENERATED_BODY()

		FTGORRigUnit_CloneTransforms() {}

	RIGVM_METHOD()
		virtual void Execute(const FRigUnitContext& Context) override;

	virtual FString ProcessPinLabelForInjection(const FString& InLabel) const override;

	UPROPERTY(meta = (Input))
		FRigElementKeyCollection Items;

	UPROPERTY(meta = (Input))
		ERigElementType TargetType = ERigElementType::Bone;
};

////////////////////////////////////////////////////////////////////////////////////////////////////


/**
 * Estimate forward ray from a triangle formation
 */
USTRUCT(meta = (DisplayName = "Transform to plane", Keywords = "TGOR,Utility", PrototypeName = "TransformToPlane", NodeColor = "1.0 0.44 0.0"))
struct REALITIESANIMATION_API FTGORRigUnit_TriangleEstimateDirection : public FRigUnit
{
	GENERATED_BODY()

		FTGORRigUnit_TriangleEstimateDirection() {}

	RIGVM_METHOD()
		virtual void Execute(const FRigUnitContext& Context) override;

	virtual FString ProcessPinLabelForInjection(const FString& InLabel) const override;


	/**
	 * Base bone where the direction is located
	 */
	UPROPERTY(meta = (Input, ExpandByDefault))
		FRigElementKey BaseKey = FRigElementKey(FName(), ERigElementType::Bone);

	/**
	 * Pivot bone to project along left/right
	 */
	UPROPERTY(meta = (Input, ExpandByDefault))
		FRigElementKey PivotKey = FRigElementKey(FName(), ERigElementType::Bone);

	/**
	 * Left vertex of triangle to project forward direction
	 */
	UPROPERTY(meta = (Input, ExpandByDefault))
		FRigElementKey LeftKey = FRigElementKey(FName(), ERigElementType::Bone);

	/**
	 * Right vertex of triangle to project forward direction
	 */
	UPROPERTY(meta = (Input, ExpandByDefault))
		FRigElementKey RightKey = FRigElementKey(FName(), ERigElementType::Bone);

	/**
	* Base bone local forward axis
	 */
	UPROPERTY(meta = (Input))
		FVector Axis = FVector::LeftVector;

	/**
	* Whether to plane project output direction on the vector spanned by pivot/base
	 */
	UPROPERTY(meta = (Input))
		bool Project = false;

	/**
	 * Projected forward ray
	 */
	UPROPERTY(meta = (Output))
		FVector Origin = FVector::ZeroVector;;
	UPROPERTY(meta = (Output))
		FVector Direction = FVector::ForwardVector;;

	// Cache
	UPROPERTY(Transient)
		FCachedRigElement BaseCache;
	UPROPERTY(Transient)
		FCachedRigElement PivotCache;
	UPROPERTY(Transient)
		FCachedRigElement LeftCache;
	UPROPERTY(Transient)
		FCachedRigElement RightCache;
};

////////////////////////////////////////////////////////////////////////////////////////////////////


/**
 * Apply means testing to match a direction to a chain of bones
 */
USTRUCT(meta = (DisplayName = "Transform to plane", Keywords = "TGOR,Utility", PrototypeName = "TransformToPlane", NodeColor = "1.0 0.44 0.0"))
struct REALITIESANIMATION_API FTGORRigUnit_TransformToPlane : public FRigUnit
{
	GENERATED_BODY()

		FTGORRigUnit_TransformToPlane() {}

	RIGVM_METHOD()
		virtual void Execute(const FRigUnitContext& Context) override;

	virtual FString ProcessPinLabelForInjection(const FString& InLabel) const override;

	/**
	 */
	UPROPERTY(meta = (Input))
		FVector Axis = FVector::UpVector;

	/**
	 */
	UPROPERTY(meta = (Input))
		FTransform Transform = FTransform::Identity;

	/**
	 * Aligned rotation
	 */
	UPROPERTY(meta = (Output))
		FPlane Output = FPlane(FVector::ZeroVector, FVector::UpVector);
};

////////////////////////////////////////////////////////////////////////////////////////////////////


/**
 * Apply means testing to match a direction to a chain of bones
 */
USTRUCT(meta = (DisplayName = "Mean Direction", Keywords = "TGOR,Utility", PrototypeName = "MeanDirection", NodeColor = "1.0 0.44 0.0"))
struct REALITIESANIMATION_API FTGORRigUnit_MeanDirection : public FRigUnit
{
	GENERATED_BODY()

		FTGORRigUnit_MeanDirection() {}

	RIGVM_METHOD()
		virtual void Execute(const FRigUnitContext& Context) override;

	virtual FString ProcessPinLabelForInjection(const FString& InLabel) const override;


	/**
	 * The chain to apply the power method to
	 */
	UPROPERTY(meta = (Input, ExpandByDefault))
		FRigElementKeyCollection SourceChain;

	/**
	 * Weight according to distance given by MeanKey
	 */
	UPROPERTY(meta = (Input, ExpandByDefault))
		FRigElementKey MeanKey;

	/**
	 * Aligned rotation
	 */
	UPROPERTY(meta = (Output))
		FVector Output = FVector::ForwardVector;


	// Cache
	UPROPERTY(Transient)
		FCachedRigElement MeanCache;
};


////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * Apply power iterations to match a direction to a chain of bones
 */
USTRUCT(meta = (DisplayName = "Power Direction", Keywords = "TGOR,Utility", PrototypeName = "PowerDirection", NodeColor = "1.0 0.44 0.0"))
struct REALITIESANIMATION_API FTGORRigUnit_PowerDirection : public FRigUnit
{
	GENERATED_BODY()

		FTGORRigUnit_PowerDirection() {}

	RIGVM_METHOD()
		virtual void Execute(const FRigUnitContext& Context) override;

	virtual FString ProcessPinLabelForInjection(const FString& InLabel) const override;


	/**
	 * The chain to apply the power method to
	 */
	UPROPERTY(meta = (Input, ExpandByDefault))
		FRigElementKeyCollection SourceChain;

	/**
	 * Number of power iterations
	 */
	UPROPERTY(meta = (Input))
		int32 Iterations = 8;

	/**
	 * Aligned rotation
	 */
	UPROPERTY(meta = (Output))
		FVector Output = FVector::ForwardVector;
};


////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * Compute maximum chain length from initial pose
 */
USTRUCT(meta = (DisplayName = "Chain Length", Keywords = "TGOR,Utility", PrototypeName = "ChainLength", NodeColor = "1.0 0.44 0.0"))
struct REALITIESANIMATION_API FTGORRigUnit_ChainLength : public FRigUnit
{
	GENERATED_BODY()

		FTGORRigUnit_ChainLength() {}

	RIGVM_METHOD()
		virtual void Execute(const FRigUnitContext& Context) override;

	virtual FString ProcessPinLabelForInjection(const FString& InLabel) const override;

public:
	static float ComputeInitialChainLength(const FRigElementKeyCollection& Chain, const FRigHierarchyContainer* Hierarchy);

	/**
	 * The chain to apply the power method to
	 */
	UPROPERTY(meta = (Input, ExpandByDefault))
		FRigElementKeyCollection SourceChain;

	/**
	 * Chain Length Multiplier (Multiplier of 1.0 assumes that maximum length of the Source Chain is when it's perfectly straight, but most organic things can not straighten all their bones like that, thus it might be needed to have this value lower than 1.0)
	 */
	UPROPERTY(meta = (Input))
		float ChainLengthMultiplier = 1.0f;

	/**
	 * Chain's Max Length in cm, determined from measuring length of each bone in the chain and summing it up, then multiplying it by the length multiplier.
	 */
	UPROPERTY(meta = (Output))
		float ChainMaxLength = 0.0f;
};


/**
 * Analyse chain properties
 */
USTRUCT(meta = (DisplayName = "Chain Analysis", Keywords = "TGOR,Utility", PrototypeName = "ChainAnalysis", NodeColor = "1.0 0.44 0.0"))
struct REALITIESANIMATION_API FTGORRigUnit_ChainAnalysis : public FRigUnit
{
	GENERATED_BODY()

		FTGORRigUnit_ChainAnalysis() {}

	RIGVM_METHOD()
		virtual void Execute(const FRigUnitContext& Context) override;

	virtual FString ProcessPinLabelForInjection(const FString& InLabel) const override;

public:
	static void ChainAnalysis(const FRigElementKeyCollection& Chain, const FRigHierarchyContainer* Hierarchy, float Multiplier, float& ChainMaxLength, float& CurrentLength, float& InitialLength);


	/**
	 * The chain to apply the power method to
	 */
	UPROPERTY(meta = (Input, ExpandByDefault))
		FRigElementKeyCollection SourceChain;

	/**
	 * Chain Length Multiplier (Multiplier of 1.0 assumes that maximum length of the Source Chain is when it's perfectly straight, but most organic things can not straighten all their bones like that, thus it might be needed to have this value lower than 1.0)
	 */
	UPROPERTY(meta = (Input))
		float ChainLengthMultiplier = 1.0f;

	/**
	 * Chain's Max Length in cm, determined from measuring length of each bone in the chain and summing it up, then multiplying it by the length multiplier.
	 */
	UPROPERTY(meta = (Output))
		float ChainMaxLength = 0.0f;

	/**
	 * What is the current length of a straight line from the start of the chain to the end, in cm.
	 */
	UPROPERTY(meta = (Output))
		float CurrentLength = 0.0f;

	/**
	 * What is the initial length of a straight line from the start of the chain to the end, in cm.
	 */
	UPROPERTY(meta = (Output))
		float InitialLength = 0.0f;

	/**
	 * The ratio between max and current chain length.
	 */
	UPROPERTY(meta = (Output))
		float LengthRatio = 0.0f;
};

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * Convert space from one space to the other
 */
USTRUCT(meta = (DisplayName = "Rebase", Keywords = "TGOR,Utility", PrototypeName = "Rebase", NodeColor = "1.0 0.44 0.0"))
struct REALITIESANIMATION_API FTGORRigUnit_Rebase : public FRigUnit
{
	GENERATED_BODY()

		FTGORRigUnit_Rebase() {}

	RIGVM_METHOD()
		virtual void Execute(const FRigUnitContext& Context) override;

	virtual FString ProcessPinLabelForInjection(const FString& InLabel) const override;

public:
	static void Rebase(FTransform& Transform, const FTransform& Current, const FTransform& From, const FTransform& To, float Unit);

	/**
	 * Transform to convert
	 */
	UPROPERTY(meta = (Input))
		FTransform Transform = FTransform::Identity;

	/**
	 * Space transform comes from
	 */
	UPROPERTY(meta = (Input))
		FTransform FromSpace = FTransform::Identity;

	/**
	 * Space we transform to
	 */
	UPROPERTY(meta = (Input))
		FTransform ToSpace = FTransform::Identity;

	/**
	 * Conversion factor applied to translation
	 */
	UPROPERTY(meta = (Input))
		float UnitScale = 0.0f;

	/**
	 * Output transform
	 */
	UPROPERTY(meta = (Output))
		FTransform Output = FTransform::Identity;
};

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * Convert translation and rotation independently from one transform to a target
 */
USTRUCT(meta = (DisplayName = "Convert Space", Keywords = "TGOR,Utility", PrototypeName = "ConvertSpace", NodeColor = "1.0 0.44 0.0"))
struct REALITIESANIMATION_API FTGORRigUnit_ConvertSpace : public FRigUnit
{
	GENERATED_BODY()

		FTGORRigUnit_ConvertSpace() {}

	RIGVM_METHOD()
		virtual void Execute(const FRigUnitContext& Context) override;

	virtual FString ProcessPinLabelForInjection(const FString& InLabel) const override;

public:
	static void ConvertSpace(FTransform& Transform, const FTransform& Current, const FTransform& From, const FTransform& To, float Unit);

	/**
	 * Transform to convert
	 */
	UPROPERTY(meta = (Input))
		FTransform Transform = FTransform::Identity;

	/**
	 * Space transform comes from
	 */
	UPROPERTY(meta = (Input))
		FTransform FromSpace = FTransform::Identity;

	/**
	 * Space we transform to
	 */
	UPROPERTY(meta = (Input))
		FTransform ToSpace = FTransform::Identity;

	/**
	 * Conversion factor applied to translation
	 */
	UPROPERTY(meta = (Input))
		float UnitScale = 0.0f;

	/**
	 * Output transform
	 */
	UPROPERTY(meta = (Output))
		FTransform Output = FTransform::Identity;
};


////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * Rotation to align forward and up direction
 */
USTRUCT(meta = (DisplayName = "Rotate Towards", Keywords = "TGOR,Utility", PrototypeName = "RotateTowards", NodeColor = "1.0 0.44 0.0"))
struct REALITIESANIMATION_API FTGORRigUnit_RotateToward : public FRigUnit
{
	GENERATED_BODY()

		FTGORRigUnit_RotateToward() {}

	RIGVM_METHOD()
		virtual void Execute(const FRigUnitContext& Context) override;

	virtual FString ProcessPinLabelForInjection(const FString& InLabel) const override;

public:
	static FQuat ComputeHeadingRotation(const FVector& SourceForward, const FVector& TargetForward, const FVector& SourceUp, const FVector& TargetUp);

	/**
	 * Rotation forward axis to align with target forward
	 */
	UPROPERTY(meta = (Input))
		FVector SourceForward = FVector::ForwardVector;

	/**
	 * Direction to align rotation forward axis with
	 */
	UPROPERTY(meta = (Input))
		FVector TargetForward = FVector::ForwardVector;

	/**
	 * Rotation up axis to align with target up (projected around TargetForward)
	 */
	UPROPERTY(meta = (Input))
		FVector SourceUp = FVector::UpVector;

	/**
	 * Direction to align rotation up axis with
	 */
	UPROPERTY(meta = (Input))
		FVector TargetUp = FVector::UpVector;

	/**
	 * Aligned rotation
	 */
	UPROPERTY(meta = (Output))
		FQuat Output = FQuat::Identity;
};

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * Rotation to align forward and up direction
 */
USTRUCT(meta = (DisplayName = "Get Scale Length", Keywords = "TGOR,Utility", PrototypeName = "GetScaleLength", NodeColor = "1.0 0.44 0.0"))
struct REALITIESANIMATION_API FTGORRigUnit_GetScaleLength : public FRigUnit
{
	GENERATED_BODY()

		FTGORRigUnit_GetScaleLength() {}

	RIGVM_METHOD()
		virtual void Execute(const FRigUnitContext& Context) override;

	virtual FString ProcessPinLabelForInjection(const FString& InLabel) const override;

public:

	/**
	 * Cone key
	 */
	UPROPERTY(meta = (Input, ExpandByDefault))
		FRigElementKey ConeKey = FRigElementKey(FName(), ERigElementType::Control);

	/**
	 * Scale axis
	 */
	UPROPERTY(meta = (Input, DetailsOnly))
		TEnumAsByte<EAxis::Type> Axis = EAxis::X;

	/**
	 * Output length
	 */
	UPROPERTY(meta = (Output))
		float Output = 0.0f;

	// Cache
	UPROPERTY(Transient)
		FCachedRigElement ConeCache;
};

