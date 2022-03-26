// The Gateway of Realities: Planes of Existence.

#include "TGORRigUnit_ForwardIK.h"

#include "RealitiesUtility/Structures/TGOR_Matrix3x3.h"
#include "RealitiesUtility/Utility/TGOR_Math.h"
#include "Units/RigUnitContext.h"

FTransform ComputeObjectiveTransform(URigHierarchy* Hierarchy, const FRigUnit_ObjectiveSettings& Settings, const FRigElementKey& Element, const FTransform& Objective)
{
	const FVector EEForwardTarget = Objective.TransformVectorNoScale(Settings.ForwardAxis);
	const FVector EEUpTarget = Objective.TransformVectorNoScale(Settings.UpAxis);
	const FVector EELocation = Objective.TransformPosition(Settings.Offset);

	FTransform EE;
	EE.SetScale3D(Hierarchy->GetInitialGlobalTransform(Element).GetScale3D());
	EE.SetRotation(FTGORRigUnit_RotateToward::ComputeHeadingRotation(Settings.EffectorForwardAxis, EEForwardTarget, Settings.EffectorUpAxis, EEUpTarget));
	EE.SetLocation(EELocation);
	return EE;
}

FQuat SoftRotate(const FTransform& Local, const FTransform& Transform, const FTransform& Anchor, float MaxAngle)
{
	const FQuat Rotation = FQuat::FindBetweenVectors(Local.GetLocation(), Transform.InverseTransformPosition(Anchor.GetLocation()));
	const float Angle = FTGORRigUnit_SoftBoundaries::SoftBoundaries(Rotation.GetAngle(), MaxAngle);
	return (FQuat(Rotation.GetRotationAxis(), Angle));
}

FTransform Fabrik(TArray<FTransform>& Transforms, const TArray<FTransform>& Rest, int32 Index, const FTransform& Forward, const FTransform& Backward, float MaxAngle)
{
	if (Index == Rest.Num() - 1)
	{
		return Backward;
	}

	const FTransform Regular = Rest[Index + 1];

	Transforms[Index] = Forward;
	const FQuat ForwardRotation = SoftRotate(Regular, Forward, Transforms[Index + 1], MaxAngle);

	const FTransform Transform = Regular * ForwardRotation * Forward;
	const FTransform Objective = Fabrik(Transforms, Rest, Index + 1, Transform, Backward, MaxAngle);

	Transforms[Index + 1] = Objective;
	const FQuat BackwardRotation = SoftRotate(Regular.Inverse(), Objective, Transforms[Index], MaxAngle);

	return Regular.Inverse() * BackwardRotation * Objective;
}

FTGORRigUnit_AnchorIK_Execute()
{
	DECLARE_SCOPE_HIERARCHICAL_COUNTER_RIGUNIT()
	URigHierarchy* Hierarchy = ExecuteContext.Hierarchy;

	if (Context.State == EControlRigState::Init)
	{
	}
	else
	{
		const int32 ChainNum = Chain.Num();
		if (ChainNum < 2)
		{
			UE_CONTROLRIG_RIGUNIT_REPORT_WARNING(TEXT("Chain has to have length at least 2."));
		}
		else
		{
			// Objective properties
			const FTransform StartEE = ComputeObjectiveTransform(Hierarchy, AnchorSettings, Chain.First(), Anchor);
			const FTransform EndEE = ComputeObjectiveTransform(Hierarchy, ObjectiveSettings, Chain.Last(), Objective);

			// Populate transform lists
			TArray<FTransform> Transforms, Rest;
			Transforms.Reserve(ChainNum);
			Rest.Reserve(ChainNum);
			for (int32 Index = 0; Index < ChainNum; Index++)
			{
				Transforms.Emplace(Hierarchy->GetGlobalTransform(Chain[Index]));
				Rest.Emplace(Hierarchy->GetInitialGlobalTransform(Chain[Index]));
			}

			// Objective properties
			const float MaxRadians = FMath::DegreesToRadians(MaxAngle);
			for (int32 Iteration = 0; Iteration < Iterations; Iteration++)
			{
				Transforms[0] = Fabrik(Transforms, Rest, 0, StartEE, EndEE, MaxRadians);
			}

			// Set bones to transforms
			Hierarchy->SetGlobalTransform(Chain.First(), Transforms[0], false, PropagateToChildren != ETGOR_Propagation::Off);
			for (int32 Index = 1; Index < ChainNum - 1; Index++)
			{
				Hierarchy->SetGlobalTransform(Chain[Index], Transforms[Index], false, PropagateToChildren == ETGOR_Propagation::All);
			}
			Hierarchy->SetGlobalTransform(Chain.Last(), Transforms.Last(), false, PropagateToChildren != ETGOR_Propagation::Off);
		}
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void WeightedMean(const FRigUnitContext& Context, const FRigUnit_DebugSettings& DebugSettings, TArray<FTransform>& Transforms, const TArray<FTransform>& A, const TArray<FTransform>& B, float Bias)
{
	// Both ends always match
	Transforms[0] = A[0];
	Transforms.Last() = B.Last();

	// Compute weighted average
	const int32 Num = Transforms.Num();
	for (int32 Index = Num-1; Index > 0; Index--)
	{
		const float Ratio = ((float)Index) / (Num - 1);
		const float Weight = FMath::Lerp(FMath::Square(Ratio), 1.0f - FMath::Square(1.0f - Ratio), Bias);
		Transforms[Index].Blend(A[Index], B[Index], Weight);

		if (DebugSettings.bEnabled)
		{
			Context.DrawInterface->DrawPoint(FTransform::Identity, Transforms[Index].GetLocation(), DebugSettings.Scale * 5.0f, FLinearColor::Yellow);
		}
	}
}

void Straighten(TArray<FTransform>& Transforms, const TArray<FTransform>& Rest)
{
	// Rotate bones to align with given rest pose
	const int32 Num = Transforms.Num();
	for (int32 Index = 1; Index < Num-1; Index++)
	{
		const FVector Target = Transforms[Index].InverseTransformPosition(Transforms[Index + 1].GetLocation());
		const FQuat Rotation = FQuat::FindBetweenVectors(Rest[Index + 1].GetLocation(), Target);
		Transforms[Index].SetRotation(Transforms[Index].GetRotation() * Rotation);
	}
}

void InitialiseBendTransforms(
	const FRigUnitContext& Context, 
	const FRigUnit_DebugSettings& DebugSettings, 
	const TArrayView<const FRigElementKey>& Chain,
	const FTransform& StartEE, const FVector& StartOffset, float StartRadius,
	const FTransform& EndEE, const FVector& EndOffset, float EndRadius,
	TArray<FTransform>& Rest, 
	TArray<FTransform>& StartChain, 
	TArray<FTransform>& EndChain, 
	TArray<FTransform>& Transforms)
{
	const int32 ChainNum = Chain.Num();

	// Populate transform lists
	Rest.Reserve(ChainNum);
	StartChain.Reserve(ChainNum);
	EndChain.Reserve(ChainNum);
	Transforms.Reserve(ChainNum);
	for (int32 Index = 0; Index < ChainNum; Index++)
	{
		Rest.Emplace(Context.Hierarchy->GetInitialLocalTransform(Chain[Index]));
		Transforms.Emplace(Context.Hierarchy->GetGlobalTransform(Chain[Index]));
		StartChain.Emplace(StartEE);
		EndChain.Emplace(EndEE);
	}

	const FQuat StartOffsetRotation = FQuat(StartOffset.GetSafeNormal(), FMath::DegreesToRadians(StartOffset.Size()));
	const FQuat StartLimited = FTGORRigUnit_LimitRotation::SoftLimitRotation(StartOffsetRotation * StartEE.GetRotation().Inverse() * Transforms.Last().GetRotation(), FMath::DegreesToRadians(StartRadius));
	StartChain[0].SetRotation(StartEE.GetRotation()* StartOffsetRotation.Inverse()* StartLimited);

	const FQuat EndOffsetRotation = FQuat(EndOffset.GetSafeNormal(), FMath::DegreesToRadians(EndOffset.Size()));
	const FQuat EndLimited = FTGORRigUnit_LimitRotation::SoftLimitRotation(EndOffsetRotation * EndEE.GetRotation().Inverse() * Transforms.Last().GetRotation(), FMath::DegreesToRadians(EndRadius));
	EndChain.Last().SetRotation(EndEE.GetRotation() * EndOffsetRotation.Inverse() * EndLimited);

	// Compute separate global transforms assuming rest pose for both ends
	for (int32 Index = 1; Index < ChainNum; Index++)
	{
		StartChain[Index] = Rest[Index] * StartChain[Index - 1];
		EndChain[ChainNum - Index - 1] = Rest[ChainNum - Index].Inverse() * EndChain[ChainNum - Index];

		if (DebugSettings.bEnabled)
		{
			Context.DrawInterface->DrawPoint(FTransform::Identity, StartChain[Index].GetLocation(), DebugSettings.Scale * 5.0f, FLinearColor::Red);
			Context.DrawInterface->DrawPoint(FTransform::Identity, EndChain[ChainNum - Index - 1].GetLocation(), DebugSettings.Scale * 5.0f, FLinearColor::Blue);
		}
	}
}


void ChainForwardSolve(const FRigUnitContext& Context, const FRigUnit_DebugSettings& DebugSettings, const TArray<FTransform>& Rest, const TArray<FTransform>& Transforms, TArray<FTransform>& StartChain, float MaxAnchorRadians)
{
	const int32 ChainNum = Rest.Num();
	for (int32 Index = 1; Index < ChainNum; Index++)
	{
		FTransform Regular = Rest[Index];
		const float RegularDistance = (Transforms[Index].GetLocation() - Transforms[Index - 1].GetLocation()).Size();
		Regular.SetLocation(Regular.GetLocation().GetClampedToMaxSize(RegularDistance));

		const FQuat Rotation = SoftRotate(Regular, StartChain[Index - 1], Transforms[Index], MaxAnchorRadians);
		StartChain[Index] = Regular * Rotation * StartChain[Index - 1];

		if (DebugSettings.bEnabled)
		{
			Context.DrawInterface->DrawPoint(FTransform::Identity, StartChain[Index].GetLocation(), DebugSettings.Scale * 7.5f, FLinearColor::White);
		}
	}
}

void ChainBackwardSolve(const FRigUnitContext& Context, const FRigUnit_DebugSettings& DebugSettings, const TArray<FTransform>& Rest, const TArray<FTransform>& Transforms, TArray<FTransform>& EndChain, float MaxObjectiveRadians)
{
	const int32 ChainNum = Rest.Num();
	for (int32 Index = ChainNum - 1; Index >= 1; Index--)
	{
		FTransform Regular = Rest[Index];
		const float RegularDistance = (Transforms[Index].GetLocation() - Transforms[Index - 1].GetLocation()).Size();
		Regular.SetLocation(Regular.GetLocation().GetClampedToMaxSize(RegularDistance));

		const FTransform& RegularInv = Regular.Inverse();
		const FQuat Rotation = SoftRotate(RegularInv, EndChain[Index], Transforms[Index - 1], MaxObjectiveRadians);
		EndChain[Index - 1] = RegularInv * Rotation * EndChain[Index];

		if (DebugSettings.bEnabled)
		{
			Context.DrawInterface->DrawPoint(FTransform::Identity, EndChain[Index - 1].GetLocation(), DebugSettings.Scale * 7.5f, FLinearColor::Black);
		}
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////

FTGORRigUnit_BendIK_Execute()
{
	DECLARE_SCOPE_HIERARCHICAL_COUNTER_RIGUNIT()
	URigHierarchy* Hierarchy = ExecuteContext.Hierarchy;

	if (Context.State == EControlRigState::Init)
	{
		return;
	}

	if (Context.State == EControlRigState::Update)
	{
		const int32 ChainNum = Chain.Num();
		if (ChainNum < 2)
		{
			UE_CONTROLRIG_RIGUNIT_REPORT_WARNING(TEXT("Chain has to have length at least 2."));
		}
		else
		{
			// Objective properties
			const FTransform StartEE = ComputeObjectiveTransform(Hierarchy, AnchorSettings, Chain[0], Anchor);
			const FTransform EndEE = ComputeObjectiveTransform(Hierarchy, ObjectiveSettings, Chain.Last(), Objective);

			TArray<FTransform> Rest, StartChain, EndChain, Transforms;
			InitialiseBendTransforms(Context, DebugSettings, Chain, StartEE, ObjectiveLimitOffset, ObjectiveLimitRadius, EndEE, AnchorLimitOffset, AnchorLimitRadius, Rest, StartChain, EndChain, Transforms);

			// Collapse start and end chain into one
			WeightedMean(Context, DebugSettings, Transforms, StartChain, EndChain, 0.0f);

			const float MaxAnchorRadians = FMath::DegreesToRadians(MaxAnchorAngle);
			const float MaxObjectiveRadians = FMath::DegreesToRadians(MaxObjectiveAngle);
			for (int32 Iteration = 0; Iteration < Iterations; Iteration++)
			{
				// Apply one FABRIK iteration to both directions
				ChainForwardSolve(Context, DebugSettings, Rest, Transforms, StartChain, MaxAnchorRadians);
				ChainBackwardSolve(Context, DebugSettings, Rest, Transforms, EndChain, MaxObjectiveRadians);

				// Collapse both FABRIK iterations into one
				WeightedMean(Context, DebugSettings, Transforms, StartChain, EndChain, 1.0f);
			}

			// Make sure all bones are properly rotated
			Straighten(Transforms, Rest);

			Transforms[0] = StartEE;
			Transforms.Last() = EndEE;

			// Set bones to transforms
			Hierarchy->SetGlobalTransform(Chain[0], Transforms[0], false, PropagateToChildren != ETGOR_Propagation::Off);
			for (int32 Index = 1; Index < ChainNum - 1; Index++)
			{
				Hierarchy->SetGlobalTransform(Chain[Index], Transforms[Index], false, PropagateToChildren == ETGOR_Propagation::All);
			}
			Hierarchy->SetGlobalTransform(Chain.Last(), Transforms.Last(), false, PropagateToChildren != ETGOR_Propagation::Off);
		}
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////

FTGORRigUnit_SingleBendIK_Execute()
{
	DECLARE_SCOPE_HIERARCHICAL_COUNTER_RIGUNIT()
	URigHierarchy* Hierarchy = ExecuteContext.Hierarchy;

	if (Context.State == EControlRigState::Init)
	{
		return;
	}

	if (Context.State == EControlRigState::Update)
	{
		const int32 ChainNum = Chain.Num();
		if (ChainNum < 2)
		{
			UE_CONTROLRIG_RIGUNIT_REPORT_WARNING(TEXT("Chain has to have length at least 2."));
		}
		else
		{
			// Objective properties
			const FTransform StartEE = Hierarchy->GetGlobalTransform(Chain[0]);
			const FTransform EndEE = ComputeObjectiveTransform(Hierarchy, ObjectiveSettings, Chain.Last(), Objective);

			// Populate transform lists
			TArray<FTransform> Rest, StartChain, EndChain, Transforms;
			InitialiseBendTransforms(Context, DebugSettings, Chain, StartEE, ObjectiveLimitOffset, ObjectiveLimitRadius, EndEE, AnchorLimitOffset, AnchorLimitRadius, Rest, StartChain, EndChain, Transforms);

			// Collapse start and end chain into one
			WeightedMean(Context, DebugSettings, Transforms, StartChain, EndChain, 0.0f);

			const float MaxAnchorRadians = FMath::DegreesToRadians(MaxAnchorAngle);
			const float MaxObjectiveRadians = FMath::DegreesToRadians(MaxObjectiveAngle);
			for (int32 Iteration = 0; Iteration < Iterations; Iteration++)
			{
				ChainBackwardSolve(Context, DebugSettings, Rest, Transforms, EndChain, MaxObjectiveRadians);
				ChainForwardSolve(Context, DebugSettings, Rest, Transforms, StartChain, MaxAnchorRadians);

				// Collapse both FABRIK iterations into one
				WeightedMean(Context, DebugSettings, Transforms, StartChain, EndChain, 1.0f);
			}

			// Make sure all bones are properly rotated
			Straighten(Transforms, Rest);

			// Set bones to transforms
			for (int32 Index = 1; Index < ChainNum - 1; Index++)
			{
				Hierarchy->SetGlobalTransform(Chain[Index], Transforms[Index], false, PropagateToChildren == ETGOR_Propagation::All);
			}
			Hierarchy->SetGlobalTransform(Chain.Last(), Transforms.Last(), false, PropagateToChildren != ETGOR_Propagation::Off);
		}
	}
}
