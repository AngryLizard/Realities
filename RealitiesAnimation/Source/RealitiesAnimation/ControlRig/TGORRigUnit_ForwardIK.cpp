// The Gateway of Realities: Planes of Existence.

#include "TGORRigUnit_ForwardIK.h"

#include "RealitiesUtility/Structures/TGOR_Matrix3x3.h"
#include "RealitiesUtility/Utility/TGOR_Math.h"
#include "Units/RigUnitContext.h"

FTransform ComputeObjectiveTransform(FRigHierarchyContainer* Hierarchy, const FRigUnit_ObjectiveSettings& Settings, const FRigElementKey& Element, const FTransform& Objective)
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
	FRigHierarchyContainer* Hierarchy = ExecuteContext.Hierarchy;

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
				Rest.Emplace(Hierarchy->GetInitialTransform(Chain[Index]));
			}

			// Objective properties
			const float MaxRadians = FMath::DegreesToRadians(MaxAngle);
			for (int32 Iteration = 0; Iteration < Iterations; Iteration++)
			{
				Transforms[0] = Fabrik(Transforms, Rest, 0, StartEE, EndEE, MaxRadians);
			}

			// Set bones to transforms
			Hierarchy->SetGlobalTransform(Chain.First(), Transforms[0], PropagateToChildren != ETGOR_Propagation::Off);
			for (int32 Index = 1; Index < ChainNum - 1; Index++)
			{
				Hierarchy->SetGlobalTransform(Chain[Index], Transforms[Index], PropagateToChildren == ETGOR_Propagation::All);
			}
			Hierarchy->SetGlobalTransform(Chain.Last(), Transforms.Last(), PropagateToChildren != ETGOR_Propagation::Off);
		}
	}
}

FString FTGORRigUnit_AnchorIK::ProcessPinLabelForInjection(const FString& InLabel) const
{
	FString Formula;
	return FString::Printf(TEXT("%s: TODO"), *InLabel);
}


////////////////////////////////////////////////////////////////////////////////////////////////////

void WeightedMean(TArray<FTransform>& Transforms, const TArray<FTransform>& A, const TArray<FTransform>& B, float Bias)
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

FTGORRigUnit_BendIK_Execute()
{
	DECLARE_SCOPE_HIERARCHICAL_COUNTER_RIGUNIT()
	FRigHierarchyContainer* Hierarchy = ExecuteContext.Hierarchy;

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
			TArray<FTransform> Rest;
			Rest.Reserve(ChainNum);
			TArray<FTransform> StartChain;
			StartChain.Reserve(ChainNum);
			TArray<FTransform> EndChain;
			EndChain.Reserve(ChainNum);
			TArray<FTransform> Transforms;
			Transforms.Reserve(ChainNum);
			for (int32 Index = 0; Index < ChainNum; Index++)
			{
				Rest.Emplace(Hierarchy->GetInitialTransform(Chain[Index]));
				Transforms.Emplace(Hierarchy->GetGlobalTransform(Chain[Index]));
				StartChain.Emplace(StartEE);
				EndChain.Emplace(EndEE);
			}

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
			
			// Collapse start and end chain into one
			WeightedMean(Transforms, StartChain, EndChain, 0.0f);

			for (int32 Iteration = 0; Iteration < Iterations; Iteration++)
			{
				// Apply one FABRIK iteration to both directions
				const float MaxAnchorRadians = FMath::DegreesToRadians(MaxAnchorAngle);
				for (int32 Index = 1; Index < ChainNum; Index++)
				{
					const FTransform Regular = Rest[Index];
					const FQuat Rotation = SoftRotate(Regular, StartChain[Index - 1], Transforms[Index], MaxAnchorRadians);
					StartChain[Index] = Regular * Rotation * StartChain[Index - 1];

					if (DebugSettings.bEnabled)
					{
						Context.DrawInterface->DrawPoint(FTransform::Identity, StartChain[Index].GetLocation(), DebugSettings.Scale * 7.5f, FLinearColor::White);
					}
				}

				const float MaxObjectiveRadians = FMath::DegreesToRadians(MaxObjectiveAngle);
				for (int32 Index = ChainNum - 1; Index >= 1; Index--)
				{
					const FTransform Regular = Rest[Index];
					const FQuat Rotation = SoftRotate(Regular.Inverse(), EndChain[Index], Transforms[Index - 1], MaxObjectiveRadians);
					EndChain[Index - 1] = Regular.Inverse() * Rotation * EndChain[Index];

					if (DebugSettings.bEnabled)
					{
						Context.DrawInterface->DrawPoint(FTransform::Identity, EndChain[Index - 1].GetLocation(), DebugSettings.Scale * 7.5f, FLinearColor::Black);
					}
				}

				// Collapse both FABRIK iterations into one
				WeightedMean(Transforms, StartChain, EndChain, 1.0f);
			}

			// Make sure all bones are properly rotated
			Straighten(Transforms, Rest);

			// Set bones to transforms
			Hierarchy->SetGlobalTransform(Chain.First(), Transforms[0], PropagateToChildren != ETGOR_Propagation::Off);
			for (int32 Index = 1; Index < ChainNum - 1; Index++)
			{
				Hierarchy->SetGlobalTransform(Chain[Index], Transforms[Index], PropagateToChildren == ETGOR_Propagation::All);
			}
			Hierarchy->SetGlobalTransform(Chain.Last(), Transforms.Last(), PropagateToChildren != ETGOR_Propagation::Off);
		}
	}
}

FString FTGORRigUnit_BendIK::ProcessPinLabelForInjection(const FString& InLabel) const
{
	FString Formula;
	return FString::Printf(TEXT("%s: TODO"), *InLabel);
}
