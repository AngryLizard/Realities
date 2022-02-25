// The Gateway of Realities: Planes of Existence.

#pragma once

#include "CoreMinimal.h"
#include "Realities/Base/Instances/Creature/TGOR_MovementInstance.h"
#include "Realities/Animation/TGOR_AnimInstance.h"
#include "Realities/Utility/Error/TGOR_Error.h"

#include "BoneIndices.h"
#include "BoneContainer.h"
#include "BonePose.h"

#include "BoneControllers/AnimNode_SkeletalControlBase.h"
#include "TGOR_AnimNode_GroundTracer.generated.h"

DECLARE_STATS_GROUP(TEXT("Ground Trace"), STATGROUP_GROUND_TRACE, STATCAT_Advanced);

/** */
USTRUCT(BlueprintInternalUseOnly)
struct REALITIES_API FTGOR_TraceBoneReference
{
	GENERATED_USTRUCT_BODY()

public:
	FTGOR_TraceBoneReference();
	void Initialise(const FBoneContainer& RequiredBones);

	UPROPERTY(EditAnywhere)
		FBoneReference Bone;
		FCompactPoseBoneIndex BoneIndex;

	UPROPERTY(EditAnywhere)
		FBoneReference Target;
		FCompactPoseBoneIndex TargetIndex;

	UPROPERTY(EditAnywhere)
		FBoneReference Reference;
		FCompactPoseBoneIndex ReferenceIndex;

	/** Local offset */
	UPROPERTY(EditAnywhere)
		FVector Offset;
};


/** */
USTRUCT(BlueprintInternalUseOnly)
struct REALITIES_API FTGOR_AnimNode_GroundTracer : public FAnimNode_SkeletalControlBase
{
	GENERATED_USTRUCT_BODY()

public:

	/** Max distance away from origin plane for trace */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Translation)
		float MaxDistance;

	/** Trace radius */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Translation)
		float TraceRadius;

	/** Direction (in local space) to offset */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Translation, meta = (PinHiddenByDefault))
		FVector Direction;

	/** Rotation */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Translation, meta = (PinHiddenByDefault))
		FRotator Rotator;

	/** Contact to adhere to */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Translation, meta = (PinShownByDefault))
		FTGOR_VisualContact Contact;

	/** Bone to transform */
	UPROPERTY(EditAnywhere)
		FTGOR_TraceBoneReference Bone;
	
public:
	FTGOR_AnimNode_GroundTracer();

	// FAnimNode_Base interface
	virtual void GatherDebugData(FNodeDebugData& DebugData) override;
	// End of FAnimNode_Base interface

	// FAnimNode_SkeletalControlBase interface
	virtual void EvaluateSkeletalControl_AnyThread(FComponentSpacePoseContext& Output, TArray<FBoneTransform>& OutBoneTransforms) override;
	virtual bool IsValidToEvaluate(const USkeleton* Skeleton, const FBoneContainer& RequiredBones) override;
	// End of FAnimNode_SkeletalControlBase interface

private:
	// FAnimNode_SkeletalControlBase interface
	virtual void InitializeBoneReferences(const FBoneContainer& RequiredBones) override;
	// End of FAnimNode_SkeletalControlBase interface


};
