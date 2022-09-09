// The Gateway of Realities: Planes of Existence.


#include "TGOR_AnimNode_GroundTracer.h"
#include "Animation/AnimInstanceProxy.h"

#include "RealitiesUtility/Utility/TGOR_Math.h"

DECLARE_CYCLE_STAT(TEXT("Ground Trace"), STAT_GroundTrace, STATGROUP_GROUND_TRACE);


FTGOR_TraceBoneReference::FTGOR_TraceBoneReference()
:	BoneIndex(INDEX_NONE), 
	TargetIndex(INDEX_NONE), 
	ReferenceIndex(INDEX_NONE), 
	Offset(FVector::ZeroVector)
{

}

void FTGOR_TraceBoneReference::Initialise(const FBoneContainer& RequiredBones)
{
	Bone.Initialize(RequiredBones);
	BoneIndex = Bone.GetCompactPoseIndex(RequiredBones);

	Target.Initialize(RequiredBones);
	TargetIndex = Target.GetCompactPoseIndex(RequiredBones);

	Reference.Initialize(RequiredBones);
	ReferenceIndex = Reference.GetCompactPoseIndex(RequiredBones);
}

FTGOR_AnimNode_GroundTracer::FTGOR_AnimNode_GroundTracer()
:	MaxDistance(0.0f),
	TraceRadius(5.0f),
	Direction(FVector(0.0f, 0.0f, -1.0f)),
	Rotator(FRotator::ZeroRotator),
	Contact()
{
}


void FTGOR_AnimNode_GroundTracer::GatherDebugData(FNodeDebugData& DebugData)
{
	FString DebugLine = DebugData.GetNodeName(this);
	DebugData.AddDebugItem(DebugLine);
	ComponentPose.GatherDebugData(DebugData);
}

void FTGOR_AnimNode_GroundTracer::EvaluateSkeletalControl_AnyThread(FComponentSpacePoseContext& Output, TArray<FBoneTransform>& OutBoneTransforms)
{
	// Evaluate the input
	ComponentPose.EvaluateComponentSpace(Output);

	if (Output.AnimInstanceProxy)
	{
		if (Bone.BoneIndex != INDEX_NONE && Bone.TargetIndex != INDEX_NONE && Bone.ReferenceIndex != INDEX_NONE)
		{
			const FTransform& Component = Output.Pose.GetComponentSpaceTransform(Bone.BoneIndex);
			const FTransform& Reference = Output.Pose.GetComponentSpaceTransform(Bone.ReferenceIndex);
			const FVector Ray = UTGOR_Math::Normalize(Direction);

			// Project to origin plane for trace origin
			const FVector BoneLocation = Component.GetLocation();
			const FVector RefLocation = Reference.GetLocation();
			const FVector Diff = BoneLocation - RefLocation;
			const FVector TraceLocation = RefLocation + (Diff - Ray * (Ray | Diff));

			// Compute distance to reference bone
			FVector HitNormal = -Ray;
			float Distance = -MaxDistance;
			const float Nom = (Ray | Contact.Normal);
			if (Contact.Blocking && Nom <= -SMALL_NUMBER)
			{
				const float Dist = Contact.Location | Contact.Normal;
				const float Step = ((TraceLocation | Contact.Normal) - Dist) / Nom;
				Distance = FMath::Clamp(Step, -MaxDistance, MaxDistance);
				HitNormal = Contact.Normal;
			}

			/*
			USceneComponent* SceneComponent = Output.AnimInstanceProxy->GetSkelMeshComponent();

			const FVector WorldLocationA = SceneComponent->GetComponentTransform().TransformPosition(TraceLocation);
			DrawDebugPoint(SceneComponent->GetWorld(), WorldLocationA, 10.0f, FColor::Blue, false, -1.0f, 0);
			const FVector WorldLocationC = SceneComponent->GetComponentTransform().TransformPosition(BoneLocation);
			DrawDebugPoint(SceneComponent->GetWorld(), WorldLocationC, 10.0f, FColor::Green, false, -1.0f, 0);

			DrawDebugLine(SceneComponent->GetWorld(), WorldLocationA, WorldLocationC, FColor::Purple, false, -1.0f, 0, 2.0f);


			const FVector WorldLocationB = SceneComponent->GetComponentTransform().TransformPosition(TraceLocation - Ray * Distance);
			DrawDebugPoint(SceneComponent->GetWorld(), WorldLocationB, 10.0f, FColor::Red, false, -1.0f, 0);
			const FVector WorldLocationD = SceneComponent->GetComponentTransform().TransformPosition(BoneLocation - Ray * Distance + Bone.Offset);
			DrawDebugPoint(SceneComponent->GetWorld(), WorldLocationD, 10.0f, FColor::White, false, -1.0f, 0);
			*/

			// Compute final transform in component space
			const FVector Target = BoneLocation - Ray * Distance + Bone.Offset;
			const FQuat RotDelta = FQuat::FindBetweenNormals(FVector::UpVector, HitNormal);
			const FQuat Quat = RotDelta * Component.GetRotation();
			OutBoneTransforms.Add(FBoneTransform(Bone.TargetIndex, FTransform(Quat, Target, Component.GetScale3D())));
		}
	}
}

bool FTGOR_AnimNode_GroundTracer::IsValidToEvaluate(const USkeleton* Skeleton, const FBoneContainer& RequiredBones)
{
	// if all bones are valid
	return Bone.Bone.IsValidToEvaluate(RequiredBones) && Bone.Target.IsValidToEvaluate(RequiredBones) && Bone.Reference.IsValidToEvaluate(RequiredBones);
}

void FTGOR_AnimNode_GroundTracer::InitializeBoneReferences(const FBoneContainer& RequiredBones)
{
	DECLARE_SCOPE_HIERARCHICAL_COUNTER_ANIMNODE(InitializeBoneReferences)
	Bone.Initialise(RequiredBones);
}

