// The Gateway of Realities: Planes of Existence.

#pragma once

#include "CoreMinimal.h"
#include "Animation/TGOR_AnimInstance.h"
#include "Animation/TGOR_SubAnimInstance.h"
#include "Utility/Error/TGOR_Error.h"

#include "Animation/AnimNodeBase.h"
#include "TGOR_AnimNode_AnimationContentInstance.generated.h"




/** */
USTRUCT(BlueprintInternalUseOnly)
struct REALITIES_API FTGOR_AnimNode_AnimationContentInstance : public FAnimNode_Base
{
	GENERATED_USTRUCT_BODY()

public:

	/**
	*  Input pose for the node, intentionally not accessible because if there's no input
	*  Node in the target class we don't want to show this as a pin
	*/
	UPROPERTY()
		FPoseLink InPose;

	/** Objective bone */
	UPROPERTY(EditAnywhere)
		FName AnimationContentQueue;

	/** Default animinstance class (overrides input-pose if valid) */
	UPROPERTY(EditAnywhere)
		TSubclassOf<UTGOR_SubAnimInstance> Default;

	/** Which syncgroups are inherited from parent */
	UPROPERTY(EditAnywhere)
		TArray<FName> SyncGroupInheritance;

	UPROPERTY()
		float Blend;

	UPROPERTY(transient)
		UTGOR_SubAnimInstance* PrimaryInstanceToRun;

	UPROPERTY(transient)
		UTGOR_SubAnimInstance* SecondaryInstanceToRun;

	UPROPERTY(transient)
		UTGOR_SubAnimInstance* DefaultInstanceToRun;

	// Temporary storage for the output of the subinstance, will be copied into output pose.
	// Declared at member level to avoid reallocating the objects
	TArray<FTransform> BoneTransforms;
	FBlendedHeapCurve BlendedCurve;

public:
	FTGOR_AnimNode_AnimationContentInstance();

	// FAnimNode_Base interface
	virtual void Initialize_AnyThread(const FAnimationInitializeContext& Context) override;
	virtual void CacheBones_AnyThread(const FAnimationCacheBonesContext& Context) override;
	virtual void Update_AnyThread(const FAnimationUpdateContext& Context) override;
	virtual void Evaluate_AnyThread(FPoseContext& Output) override;
	virtual void GatherDebugData(FNodeDebugData& DebugData) override;

	virtual bool HasPreUpdate() const override;
	virtual void PreUpdate(const UAnimInstance* InAnimInstance) override;

	virtual void OnInitializeAnimInstance(const FAnimInstanceProxy* InProxy, const UAnimInstance* InAnimInstance) override;

protected:

	// M
	void InheritTickRecords(const UTGOR_AnimInstance* AnimInstance, UTGOR_SubAnimInstance* SubAnimInstance);

	// Makes sure the bone transforms array can contain the pose information from the provided anim instance
	void AllocateBoneTransforms(const UAnimInstance* InAnimInstance);

	// Schedules the given instance if available, default otherwise, input otherwise, refpose otherwise
	void ScheduleInstance(FPoseContext& Output, UTGOR_SubAnimInstance* Instance);

	// Evaluates one given instance
	void EvaluateInstance(FPoseContext& Output, UTGOR_SubAnimInstance* Instance);
};
