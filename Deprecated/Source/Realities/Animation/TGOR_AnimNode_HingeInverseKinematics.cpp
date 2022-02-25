// The Gateway of Realities: Planes of Existence.


#include "TGOR_AnimNode_HingeInverseKinematics.h"
#include "AnimationRuntime.h"
#include "DrawDebugHelpers.h"
#include "Engine/Engine.h"
#include "EngineGlobals.h"
#include "Animation/AnimInstanceProxy.h"
#include "Math/UnrealMathSSE.h"

DECLARE_CYCLE_STAT(TEXT("Compute finite difference"), STAT_ComputeFiniteDifference, STATGROUP_GD_IK);
DECLARE_CYCLE_STAT(TEXT("Compute newton"), STAT_ComputeNewton, STATGROUP_GD_IK);
DECLARE_CYCLE_STAT(TEXT("Compute armature energy"), STAT_ComputeArmature, STATGROUP_GD_IK);
DECLARE_DWORD_COUNTER_STAT(TEXT("Newton iterations"), STAT_NewtonIterations, STATGROUP_GD_IK);
DECLARE_FLOAT_COUNTER_STAT(TEXT("Newton gradient error"), STAT_NewtonGradientError, STATGROUP_GD_IK);
DECLARE_FLOAT_COUNTER_STAT(TEXT("Newton hessian error"), STAT_NewtonHessianError, STATGROUP_GD_IK);

TAutoConsoleVariable<int32> CVarAnimHingeIKFiniteEnable(TEXT("a.AnimNode.HingeIK.Finite"), 0, TEXT("Toggle finite difference check."));
TAutoConsoleVariable<int32> CVarAnimHingeIKDrawEnable(TEXT("a.AnimNode.HingeIK.Draw"), 0, TEXT("Toggle draw debug mode."));


FTGOR_IKBoneReference::FTGOR_IKBoneReference()
	: Index(INDEX_NONE)
{
}

void FTGOR_IKBoneReference::Initialise(const FBoneContainer& RequiredBones)
{
	Bone.Initialize(RequiredBones);
	Index = Bone.GetCompactPoseIndex(RequiredBones);
}

FTGOR_IKEffectorReference::FTGOR_IKEffectorReference()
	: FTGOR_IKBoneReference(), Offset(FVector::ZeroVector), Normal(EAxis::Z)
{
}

FTGOR_IKHingeReference::FTGOR_IKHingeReference()
	: FTGOR_IKBoneReference(), Axis(EAxis::X), Stiffness(1.0f)
{
}

FTGOR_IKHinge::FTGOR_IKHinge()
	: Axis(FVector::ZeroVector), Index(-1)
{
}


FTGOR_AnimNode_HingeInverseKinematics::FTGOR_AnimNode_HingeInverseKinematics()
	: Super(), Rotational(500.0f), Stiffness(1000.0f), Epsilon(5.0f), MaxIterations(16), CachedConfig(FVector::ZeroVector)
{
}

void FTGOR_AnimNode_HingeInverseKinematics::Initialize_AnyThread(const FAnimationInitializeContext& Context)
{
	Super::Initialize_AnyThread(Context);
}

void FTGOR_AnimNode_HingeInverseKinematics::CacheBones_AnyThread(const FAnimationCacheBonesContext& Context)
{
	Super::CacheBones_AnyThread(Context);
}

void FTGOR_AnimNode_HingeInverseKinematics::UpdateInternal(const FAnimationUpdateContext& Context)
{
	Super::UpdateInternal(Context);
}



void FTGOR_AnimNode_HingeInverseKinematics::GatherDebugData(FNodeDebugData& DebugData)
{
	FString DebugLine = DebugData.GetNodeName(this);
	DebugData.AddDebugItem(DebugLine);
	ComponentPose.GatherDebugData(DebugData);
}

void FTGOR_AnimNode_HingeInverseKinematics::EvaluateSkeletalControl_AnyThread(FComponentSpacePoseContext& Output, TArray<FBoneTransform>& OutBoneTransforms)
{
	// Initialise objective if bone is defined
	if (EffectorTarget.Index != INDEX_NONE)
	{
		Objective = Output.Pose.GetComponentSpaceTransform(EffectorTarget.Index);
		Objective.AddToTranslation(EffectorTarget.Offset);
	}


	// Initialise joints
	FTransform Tail = InitialiseSpace(Output.Pose, 2);

	// Compute with solver appropriate for problem size
	FVector Config = CachedConfig;
	NewtonsMethod3x3(Config, Tail, FLT_MAX, 1.0f, 0);
	CachedConfig = Config;

	// Copy transforms to output, make sure to not set bundles multiple times
	if (CacheX.Index != CacheY.Index) OutBoneTransforms.Add(FBoneTransform(CacheX.Index, CacheX.Component));
	if (CacheY.Index != CacheZ.Index) OutBoneTransforms.Add(FBoneTransform(CacheY.Index, CacheY.Component));
	OutBoneTransforms.Add(FBoneTransform(CacheZ.Index, CacheZ.Component));

	// Sort bones
	OutBoneTransforms.Sort(FCompareBoneTransformIndex());

	if (Output.AnimInstanceProxy)
	{

		if (CVarAnimHingeIKDrawEnable.GetValueOnAnyThread() == 1)
		{
			ShowDebugJoints(Output.AnimInstanceProxy, Tail, CachedConfig);
		}
	}
}

bool FTGOR_AnimNode_HingeInverseKinematics::IsValidToEvaluate(const USkeleton* Skeleton, const FBoneContainer& RequiredBones)
{
	return(CacheX.Index != INDEX_NONE && CacheY.Index != INDEX_NONE && CacheZ.Index != INDEX_NONE);
}


FQuat FTGOR_AnimNode_HingeInverseKinematics::GetQuat(const FVector& Axis, float Angle)
{
	return(FQuat(Axis, Angle * Axis.SizeSquared()));
}

FTransform FTGOR_AnimNode_HingeInverseKinematics::InitialiseSpace(FCSPose<FCompactPose>& Pose, int32 Index)
{
	// Compute root component transform
	CacheX.Default = Pose.GetComponentSpaceTransform(CacheX.Index);

	// Compute second hinge offset to root
	CacheY.Default = FTransform::Identity;
	for (const FCompactPoseBoneIndex& Child : CacheX.Children)
	{
		CacheY.Default *= Pose.GetLocalSpaceTransform(Child);
	}

	// Compute thrid hinge offset to second hinge
	CacheZ.Default = FTransform::Identity;
	for (const FCompactPoseBoneIndex& Child : CacheY.Children)
	{
		CacheZ.Default *= Pose.GetLocalSpaceTransform(Child);
	}

	// Compute endeffector transform
	FTransform Tail = FTransform::Identity;
	for (const FCompactPoseBoneIndex& Child : CacheZ.Children)
	{
		Tail *= Pose.GetLocalSpaceTransform(Child);
	}
	Tail.AddToTranslation(Endeffector.Offset);
	return(Tail);
}

void FTGOR_AnimNode_HingeInverseKinematics::ShowDebugJoints(FAnimInstanceProxy* Proxy, const FTransform& Tail, const FVector& Config)
{

	// Transform
	USkeletalMeshComponent* Component = Proxy->GetSkelMeshComponent();
	if (IsValid(Component))
	{
		const FTransform Transform = Component->GetComponentTransform();

		const FTransform CacheXComponent = FTransform(GetQuat(CacheX.Axis, Config.X)) * CacheX.Default;
		const FTransform GlobalX = CacheXComponent * Transform;
		const FVector LocationX = GlobalX.GetLocation();
		const FVector AxisX = GlobalX.TransformVector(CacheX.Axis);
		DrawDebugLine(Component->GetWorld(), LocationX, LocationX + AxisX * 60.0f, FColor::Red, false, -1.f, 0, 3.f);
		
		const FTransform CacheYComponent = FTransform(GetQuat(CacheY.Axis, Config.Y)) * CacheY.Default * CacheXComponent;
		const FTransform GlobalY = CacheYComponent * Transform;
		const FVector LocationY = GlobalY.GetLocation();
		const FVector AxisY = GlobalY.TransformVector(CacheY.Axis);
		DrawDebugLine(Component->GetWorld(), LocationY, LocationY + AxisY * 60.0f, FColor::Green, false, -1.f, 0, 3.f);

		const FTransform CacheZComponent = FTransform(GetQuat(CacheZ.Axis, Config.Z)) * CacheZ.Default * CacheYComponent;
		const FTransform GlobalZ = CacheZComponent * Transform;
		const FVector LocationZ = GlobalZ.GetLocation();
		const FVector AxisZ = GlobalZ.TransformVector(CacheZ.Axis);
		DrawDebugLine(Component->GetWorld(), LocationZ, LocationZ + AxisZ * 60.0f, FColor::Blue, false, -1.f, 0, 3.f);

		const FTransform ComponentTail = Tail * CacheZComponent;
		const FTransform GlobalTail = ComponentTail * Transform;
		const FVector LocationTail = GlobalTail.GetLocation();
		DrawDebugPoint(Component->GetWorld(), LocationTail, 5.f, FColor::White, false, -1.f, 0);

		const FVector Normal = GlobalTail.GetUnitAxis(Endeffector.Normal);
		DrawDebugLine(Component->GetWorld(), LocationTail, LocationTail + Normal * 60.0f, FColor::Cyan, false, -1.f, 0, 3.f);

		const FVector TargetNormal = Objective.GetUnitAxis(EffectorTarget.Normal);
		DrawDebugLine(Component->GetWorld(), LocationTail, LocationTail + TargetNormal * 60.0f, FColor::Magenta, false, -1.f, 0, 3.f);

		const FVector TargetPosition = Transform.TransformPosition(Objective.GetLocation());
		DrawDebugLine(Component->GetWorld(), LocationTail, TargetPosition, FColor::Yellow, false, -1.f, 0, 3.f);
	}
}

float FTGOR_AnimNode_HingeInverseKinematics::ComputeEnergy(const FVector& Config, const FTransform& Tail)
{
	const FTransform CacheXComponent = FTransform(GetQuat(CacheX.Axis, Config.X)) * CacheX.Default;
	const FTransform CacheYComponent = FTransform(GetQuat(CacheY.Axis, Config.Y)) * CacheY.Default * CacheXComponent;
	const FTransform CacheZComponent = FTransform(GetQuat(CacheZ.Axis, Config.Z)) * CacheZ.Default * CacheYComponent;
	const FTransform Transform = Tail * CacheZComponent;

	const FVector ErrorAfter = Transform.GetLocation() - Objective.GetLocation();
	const float NormError = 1.0f - (Transform.GetUnitAxis(Endeffector.Normal) | Objective.GetUnitAxis(EffectorTarget.Normal));
	return(0.5f * (Config.X * Config.X * CacheX.Regulariser + Config.Y * Config.Y * CacheY.Regulariser + Config.Z * Config.Z * CacheZ.Regulariser + ErrorAfter.SizeSquared()) + NormError * Rotational);
}

float FTGOR_AnimNode_HingeInverseKinematics::FiniteDifference(const FVector& Config, const FTransform& Tail, EAxis::Type Primary, EAxis::Type Secondary, float Step)
{
	FVector PrimaryStep = FVector::ZeroVector, SecondaryStep = FVector::ZeroVector;
	PrimaryStep.SetComponentForAxis(Primary, Step / 2);
	SecondaryStep.SetComponentForAxis(Secondary, Step / 2);

	const float LeftLeft = ComputeEnergy(Config - PrimaryStep - SecondaryStep, Tail);
	const float LeftRight = ComputeEnergy(Config - PrimaryStep + SecondaryStep, Tail);
	const float Left = (LeftRight - LeftLeft) / Step;

	const float RightLeft = ComputeEnergy(Config + PrimaryStep - SecondaryStep, Tail);
	const float RightRight = ComputeEnergy(Config + PrimaryStep + SecondaryStep, Tail);
	const float Right = (RightRight - RightLeft) / Step;

	return((Right - Left) / Step);
}

void FTGOR_AnimNode_HingeInverseKinematics::FiniteDifference(const FVector& Config, const FTransform& Tail, const FVector& Gradient, const FTGOR_Matrix3x3& Hessian)
{
	SCOPE_CYCLE_COUNTER(STAT_ComputeFiniteDifference);

	const float H = 0.001f;
	
	const float GX = ComputeEnergy(Config + FVector(H, 0.0f, 0.0f), Tail);
	const float GY = ComputeEnergy(Config + FVector(0.0f, H, 0.0f), Tail);
	const float GZ = ComputeEnergy(Config + FVector(0.0f, 0.0f, H), Tail);
	const float G = ComputeEnergy(Config, Tail);
	const FVector Grad = FVector(GX - G, GY - G, GZ - G) / H;

	const float HH = 0.001f;

	const float Hessian_X_X = FiniteDifference(Config, Tail, EAxis::X, EAxis::X, HH);
	const float Hessian_X_Y = FiniteDifference(Config, Tail, EAxis::X, EAxis::Y, HH);
	const float Hessian_X_Z = FiniteDifference(Config, Tail, EAxis::X, EAxis::Z, HH);
	const float Hessian_Y_Y = FiniteDifference(Config, Tail, EAxis::Y, EAxis::Y, HH);
	const float Hessian_Y_Z = FiniteDifference(Config, Tail, EAxis::Y, EAxis::Z, HH);
	const float Hessian_Z_Z = FiniteDifference(Config, Tail, EAxis::Z, EAxis::Z, HH);

	const float GradientError = (Grad - Gradient).Size();
	INC_FLOAT_STAT_BY(STAT_NewtonGradientError, GradientError);

	const float HessianError = FMath::Square(Hessian_X_X - Hessian.X.X) + FMath::Square(Hessian_X_Y - Hessian.X.Y) + FMath::Square(Hessian_X_Z - Hessian.X.Z) + FMath::Square(Hessian_Y_Y - Hessian.Y.Y) + FMath::Square(Hessian_Y_Z - Hessian.Y.Z) + FMath::Square(Hessian_Z_Z - Hessian.Z.Z);
	INC_FLOAT_STAT_BY(STAT_NewtonHessianError, FMath::Sqrt(HessianError));
}

bool FTGOR_AnimNode_HingeInverseKinematics::NewtonsMethod3x3(FVector& Config, const FTransform& Tail, float Threshold, float Rate, int32 Iteration)
{
	SCOPE_CYCLE_COUNTER(STAT_ComputeNewton);

	// Stop on too many iterations
	if (Iteration > MaxIterations)
	{
		return true;
	}
	
	// Transform all hinges and compute endeffector location
	CacheX.Component = FTransform(GetQuat(CacheX.Axis, Config.X)) * CacheX.Default;
	CacheY.Component = FTransform(GetQuat(CacheY.Axis, Config.Y)) * CacheY.Default * CacheX.Component;
	CacheZ.Component = FTransform(GetQuat(CacheZ.Axis, Config.Z)) * CacheZ.Default * CacheY.Component;
	const FTransform Transform = Tail * CacheZ.Component;

	// Compute error vector
	const FVector End = Transform.GetLocation();
	const FVector Error = End - Objective.GetLocation();

	// Compute rotational error
	const FVector TargetNormal = Transform.GetUnitAxis(Endeffector.Normal);
	const FVector ObjectiveNormal = Objective.GetUnitAxis(EffectorTarget.Normal);
	const float RotError = 1.0f - (TargetNormal | ObjectiveNormal);

	// Compute energy (Error vector + Regulariser, 2E(q) = e'e + q'q)
#define ENERGY(I) Config.I * Config.I * Cache##I.Regulariser
	const float Energy = 0.5f * (ENERGY(X) + ENERGY(Y) + ENERGY(Z) + Error.SizeSquared()) + RotError * Rotational;
	if (Energy >= Threshold)
	{
		return false;
	}

	INC_DWORD_STAT_BY(STAT_NewtonIterations, Iteration)

	// Compute gradient (g = dE/dq = J'e with Jacobian J) and hessian (H = dE^2/dq^2 = Te + J'J for 3D Tensor T)
	FVector Gradient;
	FVector Stabiliser;
	FTGOR_Matrix3x3 Hessian;

	// Jacobian can be computed analytically by rotating the vector between endeffector and hinge around the hinge axis
#define GRADIENT(I) \
	const FVector Axis##I = Cache##I.Component.TransformVectorNoScale(Cache##I.Axis); \
	const FVector Jacobian##I = Axis##I ^ (End - Cache##I.Component.GetLocation()); \
	const FVector Unary##I = Axis##I ^ TargetNormal; \
	Stabiliser.I = Cache##I.Regulariser; \
	Gradient.I = (Jacobian##I | Error) - (Unary##I | ObjectiveNormal) * Rotational + Stabiliser.I * Config.I;

	// Hessian can be computed analytically by rotating the jacobians around the hinge axis
#define HESSIAN(I, J) Hessian.I.J = ((Axis##I ^ Jacobian##J) | Error) - ((Axis##I ^ Unary##J) | ObjectiveNormal) * Rotational + (Jacobian##I | Jacobian##J)
#define HESSIAN_DIAG(I) HESSIAN(I, I) + Stabiliser.I

	GRADIENT(X);
	HESSIAN_DIAG(X);

	GRADIENT(Y);
	HESSIAN_DIAG(Y);
	HESSIAN(X, Y);

	GRADIENT(Z);
	HESSIAN_DIAG(Z);
	HESSIAN(X, Z);
	HESSIAN(Y, Z);

	// Finite difference check
	if (CVarAnimHingeIKFiniteEnable.GetValueOnAnyThread() == 1)
	{
		FiniteDifference(Config, Tail, Gradient, Hessian);
	}

	if (Gradient.SizeSquared() < Epsilon)
	{
		return(true);
	}

	// Compute final direction
	FVector Delta = Hessian.CholeskyInvert(Gradient);

	// Make sure hessian is s.p.d (delta needs to point in the same direction as gradient)
	int32 Count = 0;
	while ((Delta | Gradient) < 0.0f && Count++ < MaxIterations)
	{
		Gradient += Config * Stabiliser * Count;
		Hessian.X.X += Stabiliser.X * Count;
		Hessian.Y.Y += Stabiliser.Y * Count;
		Hessian.Z.Z += Stabiliser.Z * Count;
		Delta = Hessian.CholeskyInvert(Gradient);
	}


	// Compute using newtons method q = q - (H^-1)g and bisection
	while (Rate > SMALL_NUMBER)
	{
		FVector Result = Config - Delta * Rate;
		if (NewtonsMethod3x3(Result, Tail, Energy, Rate, Iteration + 1))
		{
			Config = Result;
			return true;
		}

		Rate = Rate / 2;
	}

	return false;
}



FTGOR_IKHinge FTGOR_AnimNode_HingeInverseKinematics::InitialiseHinge(const FBoneContainer& RequiredBones, const FTGOR_IKHingeReference& Reference, FCompactPoseBoneIndex Index)
{
	FTGOR_IKHinge Hinge;

	// Add children to cache until hinge itself reached
	while (Index != INDEX_NONE)
	{
		// Add children until parent is reached
		if (Index != Reference.Index)
		{
			Hinge.Children.Add(Index);
		}
		else break;

		Index = RequiredBones.GetParentBoneIndex(Index);
	}

	// Initialise hinge
	Hinge.Index = Index;
	Hinge.Axis.SetComponentForAxis(Reference.Axis, 1.0f);
	Hinge.Regulariser = Stiffness * Reference.Stiffness;
	return(Hinge);
}

void FTGOR_AnimNode_HingeInverseKinematics::InitializeBoneReferences(const FBoneContainer& RequiredBones)
{
	// Assume endeffector to be root
	EffectorTarget.Initialise(RequiredBones);
	Endeffector.Initialise(RequiredBones);
	HingeZ.Initialise(RequiredBones);
	HingeY.Initialise(RequiredBones);
	HingeX.Initialise(RequiredBones);

	// Look for root starting at endeffector
	CacheZ = InitialiseHinge(RequiredBones, HingeZ, Endeffector.Index);
	CacheY = InitialiseHinge(RequiredBones, HingeY, CacheZ.Index);
	CacheX = InitialiseHinge(RequiredBones, HingeX, CacheY.Index);
}

