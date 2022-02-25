// The Gateway of Realities: Planes of Existence.


#include "TGOR_AnimNode_AnimationContentInstance.h"
#include "AnimationRuntime.h"
#include "DrawDebugHelpers.h"
#include "Engine/Engine.h"
#include "EngineGlobals.h"
#include "Animation/AnimClassInterface.h"
#include "Animation/AnimNode_SubInput.h"
#include "Animation/AnimInstanceProxy.h"

FTGOR_AnimNode_AnimationContentInstance::FTGOR_AnimNode_AnimationContentInstance()
:	Super(), 
	Blend(0.0f),
	PrimaryInstanceToRun(nullptr),
	SecondaryInstanceToRun(nullptr),
	DefaultInstanceToRun(nullptr)
{
}

void FTGOR_AnimNode_AnimationContentInstance::Initialize_AnyThread(const FAnimationInitializeContext& Context)
{
	FAnimNode_Base::Initialize_AnyThread(Context);

	InPose.Initialize(Context);
}

void FTGOR_AnimNode_AnimationContentInstance::CacheBones_AnyThread(const FAnimationCacheBonesContext& Context)
{
	InPose.CacheBones(Context);
}

void FTGOR_AnimNode_AnimationContentInstance::Update_AnyThread(const FAnimationUpdateContext& Context)
{
	InPose.Update(Context);
	//EvaluateGraphExposedInputs.Execute(Context);

	if (PrimaryInstanceToRun)
	{
		// Only update if we've not had a single-threaded update already
		FTGOR_SubAnimInstanceProxy& Proxy = PrimaryInstanceToRun->GetSubProxyOnAnyThread();
		if (PrimaryInstanceToRun->NeedsUpdate())
		{
			Proxy.UpdateSubAnimation();
		}
	}

	if (SecondaryInstanceToRun)
	{
		// Only update if we've not had a single-threaded update already
		FTGOR_SubAnimInstanceProxy& Proxy = SecondaryInstanceToRun->GetSubProxyOnAnyThread();
		if (SecondaryInstanceToRun->NeedsUpdate())
		{
			Proxy.UpdateSubAnimation();
		}
	}

	if (DefaultInstanceToRun)
	{
		// Only update if we've not had a single-threaded update already
		FTGOR_SubAnimInstanceProxy& Proxy = DefaultInstanceToRun->GetSubProxyOnAnyThread();
		if (DefaultInstanceToRun->NeedsUpdate())
		{
			Proxy.UpdateSubAnimation();
		}
	}
}

void FTGOR_AnimNode_AnimationContentInstance::Evaluate_AnyThread(FPoseContext& Output)
{
	if (Blend < 0.0f)
	{
		// Only run primary if there is blending
		ScheduleInstance(Output, PrimaryInstanceToRun);
	}
	else
	{
		// Run primary animation
		FPoseContext PrimaryContext(Output);
		ScheduleInstance(PrimaryContext, PrimaryInstanceToRun);

		// Run secondary animation
		FPoseContext SecondaryContext(Output);
		ScheduleInstance(SecondaryContext, SecondaryInstanceToRun);
			
		// Blend between them
		FAnimationRuntime::BlendTwoPosesTogether(PrimaryContext.Pose, SecondaryContext.Pose, PrimaryContext.Curve, SecondaryContext.Curve, Blend, Output.Pose, Output.Curve);
	}
}

void FTGOR_AnimNode_AnimationContentInstance::GatherDebugData(FNodeDebugData& DebugData)
{
	// Add our entry
	FString DebugLine = DebugData.GetNodeName(this);
	FString SlotName = AnimationContentQueue.GetPlainNameString();
	DebugLine += "Target: " + SlotName;

	DebugData.AddDebugItem(DebugLine);

	// Gather data from the sub instances
	if (PrimaryInstanceToRun)
	{
		FAnimInstanceProxy& Proxy = PrimaryInstanceToRun->GetSubProxyOnAnyThread();
		Proxy.GatherDebugData(DebugData.BranchFlow(1.0f));
	}
	if (SecondaryInstanceToRun)
	{
		FAnimInstanceProxy& Proxy = SecondaryInstanceToRun->GetSubProxyOnAnyThread();
		Proxy.GatherDebugData(DebugData.BranchFlow(1.0f));
	}
	if (DefaultInstanceToRun)
	{
		FAnimInstanceProxy& Proxy = DefaultInstanceToRun->GetSubProxyOnAnyThread();
		Proxy.GatherDebugData(DebugData.BranchFlow(1.0f));
	}

	// Pass to next
	InPose.GatherDebugData(DebugData.BranchFlow(1.0f));
}

bool FTGOR_AnimNode_AnimationContentInstance::HasPreUpdate() const
{
	return true;
}

void FTGOR_AnimNode_AnimationContentInstance::PreUpdate(const UAnimInstance* InAnimInstance)
{
	// Assign instances
	const UTGOR_AnimInstance* Instance = Cast<UTGOR_AnimInstance>(InAnimInstance);
	if (Instance && Instance->HasQueue(AnimationContentQueue))
	{
		const FTGOR_AnimationInstanceQueue& Queue = Instance->GetQueue(AnimationContentQueue);
		Blend = Queue.Blend;

		// No need to cast if already matching
		if (PrimaryInstanceToRun != Queue.Primary.Instance)
		{
			PrimaryInstanceToRun = Cast<UTGOR_SubAnimInstance>(Queue.Primary.Instance);
		}
		if (SecondaryInstanceToRun != Queue.Secondary.Instance)
		{
			SecondaryInstanceToRun = Cast<UTGOR_SubAnimInstance>(Queue.Secondary.Instance);
		}
	}
	else
	{
		// Reset if invalid blueprint or queue
		PrimaryInstanceToRun = nullptr;
		SecondaryInstanceToRun = nullptr;
	}

	if (USkeletalMeshComponent* SkelComp = InAnimInstance->GetSkelMeshComponent())
	{
		const int32 NumTransforms = SkelComp->GetComponentSpaceTransforms().Num();
		BoneTransforms.Empty(NumTransforms);
		BoneTransforms.AddZeroed(NumTransforms);

		// Retire default once initialised
		if (PrimaryInstanceToRun && DefaultInstanceToRun)
		{
			DefaultInstanceToRun->UninitializeAnimation();
			DefaultInstanceToRun = nullptr;
		}
	}

	if (IsValid(DefaultInstanceToRun))
	{
		InheritTickRecords(Instance, DefaultInstanceToRun);
	}

	if (IsValid(PrimaryInstanceToRun))
	{
		InheritTickRecords(Instance, PrimaryInstanceToRun);
	}

	if (IsValid(SecondaryInstanceToRun))
	{
		InheritTickRecords(Instance, SecondaryInstanceToRun);
	}
}

void FTGOR_AnimNode_AnimationContentInstance::InheritTickRecords(const UTGOR_AnimInstance* AnimInstance, UTGOR_SubAnimInstance* SubAnimInstance)
{
	// Inherit sync groups from parent proxy
	const FTGOR_AnimInstanceProxy& ParentProxy = AnimInstance->GetInstanceProxy();
	FTGOR_SubAnimInstanceProxy& SubProxy = SubAnimInstance->GetSubProxyOnAnyThread();
	
	SubProxy.MergeSyncGroupsFrom(ParentProxy, SyncGroupInheritance);
}

void FTGOR_AnimNode_AnimationContentInstance::OnInitializeAnimInstance(const FAnimInstanceProxy* InProxy, const UAnimInstance* InAnimInstance)
{
	// Implementation from FAnimNode_SubInstance
	if (*Default)
	{
		USkeletalMeshComponent* MeshComp = InAnimInstance->GetSkelMeshComponent();
		check(MeshComp);

		// Full reinit, kill old instances
		if (DefaultInstanceToRun)
		{
			MeshComp->SubInstances.Remove(DefaultInstanceToRun);
			DefaultInstanceToRun->MarkPendingKill();
			DefaultInstanceToRun = nullptr;
		}

		// Need an instance to run
		DefaultInstanceToRun = NewObject<UTGOR_SubAnimInstance>(MeshComp, Default);

		// Set up bone transform array
		const int32 NumTransforms = MeshComp->GetComponentSpaceTransforms().Num();
		BoneTransforms.Empty(NumTransforms);
		BoneTransforms.AddZeroed(NumTransforms);

		// Initialize the new instance
		DefaultInstanceToRun->InitializeAnimation();

		MeshComp->SubInstances.Add(DefaultInstanceToRun);
	}
	else if (DefaultInstanceToRun)
	{
		DefaultInstanceToRun->UninitializeAnimation();
		DefaultInstanceToRun = nullptr;
	}
}


void FTGOR_AnimNode_AnimationContentInstance::ScheduleInstance(FPoseContext& Output, UTGOR_SubAnimInstance* Instance)
{
	const bool HasConnectedInput = (InPose.GetLinkNode() != nullptr);

	if (Instance)
	{
		EvaluateInstance(Output, Instance);
	}
	else if (DefaultInstanceToRun)
	{
		EvaluateInstance(Output, DefaultInstanceToRun);
	}
	else if (HasConnectedInput)
	{
		InPose.Evaluate(Output);
	}
	else
	{
		Output.ResetToRefPose();
	}
}

void FTGOR_AnimNode_AnimationContentInstance::EvaluateInstance(FPoseContext& Output, UTGOR_SubAnimInstance* Instance)
{
	//FAnimNode_SubInstance

	InPose.Evaluate(Output);
	FTGOR_SubAnimInstanceProxy& Proxy = Instance->GetSubProxyOnAnyThread();
	FAnimNode_SubInput* InputNode = Instance->GetSubInputNode();

	if (InputNode)
	{
		//InputNode->InputPose.CopyBonesFrom(Output.Pose);
		//InputNode->InputCurve.CopyFrom(Output.Curve);
	}

	// Prevent crash on recompile
	if (Proxy.GetRequiredBones().IsValid())
	{
		Instance->ParallelEvaluateAnimation(false, nullptr, BlendedCurve, Output.Pose);
	}

	Output.Curve.CopyFrom(BlendedCurve);
}