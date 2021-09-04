// The Gateway of Realities: Planes of Existence.

#pragma once

#include "CoreMinimal.h"
#include "RealitiesUtility/Structures/TGOR_Matrix3x3.h"

#include "BoneIndices.h"
#include "BoneContainer.h"
#include "BonePose.h"

#include "BoneControllers/AnimNode_SkeletalControlBase.h"
#include "TGOR_AnimNode_HingeInverseKinematics.generated.h"

DECLARE_STATS_GROUP(TEXT("Gradient Descent IK"), STATGROUP_GD_IK, STATCAT_Advanced);

/** */
USTRUCT(BlueprintInternalUseOnly)
struct REALITIESANIMATION_API FTGOR_IKBoneReference
{
	GENERATED_USTRUCT_BODY()

public:
	FTGOR_IKBoneReference();
	void Initialise(const FBoneContainer& RequiredBones);

	UPROPERTY(EditAnywhere)
		FBoneReference Bone;
		FCompactPoseBoneIndex Index;
};

/** */
USTRUCT(BlueprintInternalUseOnly)
struct REALITIESANIMATION_API FTGOR_IKEffectorReference : public FTGOR_IKBoneReference
{
	GENERATED_USTRUCT_BODY()

public:
	FTGOR_IKEffectorReference();

	/** Endeffector offset in local space */
	UPROPERTY(EditAnywhere)
		FVector Offset;

	/** Axis to match with */
	UPROPERTY(EditAnywhere)
		TEnumAsByte<EAxis::Type> Normal;
};


/** */
USTRUCT(BlueprintInternalUseOnly)
struct REALITIESANIMATION_API FTGOR_IKHingeReference : public FTGOR_IKBoneReference
{
	GENERATED_USTRUCT_BODY()

public:
	FTGOR_IKHingeReference();

	/** Rotation axis */
	UPROPERTY(EditAnywhere)
		TEnumAsByte<EAxis::Type> Axis;

	/** Stiffness relative to general stiffness */
	UPROPERTY(EditAnywhere)
		float Stiffness;
};

/** */
USTRUCT(BlueprintInternalUseOnly)
struct REALITIESANIMATION_API FTGOR_IKHinge
{
	GENERATED_USTRUCT_BODY()

public:
	FTGOR_IKHinge();

	FVector Axis;
	float Regulariser;

	FTransform Default;
	FTransform Component;
	FCompactPoseBoneIndex Index;
	TArray<FCompactPoseBoneIndex> Children;
};


/** */
USTRUCT(BlueprintInternalUseOnly)
struct REALITIESANIMATION_API FTGOR_AnimNode_HingeInverseKinematics : public FAnimNode_SkeletalControlBase
{
	GENERATED_USTRUCT_BODY()

public:

	/** Cache identifier */
	UPROPERTY(EditAnywhere)
		FName Identifier;

	/** Objective transform if EffectorTarget is not defined */
	UPROPERTY(EditAnywhere, meta = (PinShownByDefault))
		FTransform Objective;

	/** objective Bone to match with */
	UPROPERTY(EditAnywhere)
		FTGOR_IKEffectorReference EffectorTarget;

	/** Hinge bone, need to be in sequence on one chain */
	UPROPERTY(EditAnywhere)
		FTGOR_IKHingeReference HingeX;

	/** Hinge bone, need to be in sequence on one chain */
	UPROPERTY(EditAnywhere)
		FTGOR_IKHingeReference HingeY;

	/** Hinge bone, need to be in sequence on one chain */
	UPROPERTY(EditAnywhere)
		FTGOR_IKHingeReference HingeZ;

	/** Bone to match with objective */
	UPROPERTY(EditAnywhere)
		FTGOR_IKEffectorReference Endeffector;

	/** Rotation target weight */
	UPROPERTY(EditAnywhere)
		float Rotational;

	/** General stiffness parameter */
	UPROPERTY(EditAnywhere)
		float Stiffness;

	/** Max error before terminating */
	UPROPERTY(EditAnywhere)
		float Epsilon;

	/** Max iterations before terminating */
	UPROPERTY(EditAnywhere)
		int32 MaxIterations;

	/**  */
	UPROPERTY(Transient)
		FTGOR_IKHinge CacheX;

	/**  */
	UPROPERTY(Transient)
		FTGOR_IKHinge CacheY;

	/**  */
	UPROPERTY(Transient)
		FTGOR_IKHinge CacheZ;

	/** Current hinge configuration */
	UPROPERTY(Transient)
		FVector CachedConfig;

public:
	FTGOR_AnimNode_HingeInverseKinematics();

	virtual void Initialize_AnyThread(const FAnimationInitializeContext& Context) override;
	virtual void CacheBones_AnyThread(const FAnimationCacheBonesContext& Context)  override;

protected:
	virtual void UpdateInternal(const FAnimationUpdateContext& Context) override;

	virtual void GatherDebugData(FNodeDebugData& DebugData) override;
	virtual void EvaluateSkeletalControl_AnyThread(FComponentSpacePoseContext& Output, TArray<FBoneTransform>& OutBoneTransforms) override;
	virtual bool IsValidToEvaluate(const USkeleton* Skeleton, const FBoneContainer& RequiredBones) override;

	FQuat GetQuat(const FVector& Axis, float Angle);
	FTransform InitialiseSpace(FCSPose<FCompactPose>& Pose, int32 Index);
	void ShowDebugJoints(FAnimInstanceProxy* Proxy, const FTransform& Tail, const FVector& Config);

	// Finite difference
	float ComputeEnergy(const FVector& Config, const FTransform& Tail);
	float FiniteDifference(const FVector& Config, const FTransform& Tail, EAxis::Type Primary, EAxis::Type Secondary, float Step);
	void FiniteDifference(const FVector& Config, const FTransform& Tail, const FVector& Gradient, const FTGOR_Matrix3x3& Hessian);

	// Newtons
	FTransform ComputeEETransform(const FTransform& Tail, const FVector& Config);
	float ComputeEEEnergy(const FTransform& Transform, const FVector& Config);
	bool NewtonsMethod3x3(FVector& Config, const FTransform& Tail, float Rate);
	
	FTGOR_IKHinge InitialiseHinge(const FBoneContainer& RequiredBones, const FTGOR_IKHingeReference& Reference, FCompactPoseBoneIndex Index);
	virtual void InitializeBoneReferences(const FBoneContainer& RequiredBones) override;

};
