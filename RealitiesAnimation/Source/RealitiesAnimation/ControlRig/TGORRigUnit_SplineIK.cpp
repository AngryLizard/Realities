// The Gateway of Realities: Planes of Existence.

#include "TGORRigUnit_SplineIK.h"

#include "RealitiesUtility/Structures/TGOR_Matrix3x3.h"
#include "RealitiesUtility/Utility/TGOR_Math.h"
#include "Units/RigUnitContext.h"

////////////////////////////////////////////////////////////////////////////////////////////////////

FTGORRigUnit_SplineChainIK_Execute()
{
	DECLARE_SCOPE_HIERARCHICAL_COUNTER_RIGUNIT()
	URigHierarchy* Hierarchy = ExecuteContext.Hierarchy;

	if (Context.State == EControlRigState::Init)
	{
		return;
	}

	if (Context.State == EControlRigState::Update)
	{
		const int32 ChainCount = Chain.Num();
		if (ChainCount < 2)
		{
			UE_CONTROLRIG_RIGUNIT_REPORT_WARNING(TEXT("Chain has to have length at least 2."));
		}
		else
		{
			const FVector EEForwardTarget = Objective.TransformVectorNoScale(ObjectiveSettings.ForwardAxis);
			const FVector EEUpTarget = Objective.TransformVectorNoScale(ObjectiveSettings.UpAxis);

			// Extract length information from initial chain
			TArray<float> Lengths;
			Lengths.SetNumZeroed(ChainCount-1);
			float TotalDistance = 0.0f;

			FVector Current = Hierarchy->GetInitialGlobalTransform(Chain[0]).GetLocation();
			for (int32 Index = 1; Index < ChainCount; Index++)
			{
				const FVector Next = Hierarchy->GetInitialGlobalTransform(Chain[Index]).GetLocation();
				const float Length = (Current - Next).Size();
				Lengths[Index-1] = Length;
				TotalDistance += Length;
				Current = Next;
			}

			if (FMath::IsNearlyZero(TotalDistance))
			{
				UE_CONTROLRIG_RIGUNIT_REPORT_WARNING(TEXT("Total chain length is null."));
			}
			else
			{
				const float InvTotalDistance = PositionAlongSpline / TotalDistance;

				// Define B-Spline
				const FTransform Origin = Hierarchy->GetGlobalTransform(Chain.First());
				const float TargetDistance = (Origin.GetLocation() - Objective.GetLocation()).Size();

				const FVector StartAnchor = Origin.GetLocation() + TangentStart * (TargetDistance * Bend);
				const FVector EndAnchor = Objective.GetLocation() + TangentEnd * (TargetDistance * Bend);
				auto LerpSpline = [&](float Alpha) -> FVector {
					return FMath::Lerp(FMath::Lerp(Origin.GetLocation(), StartAnchor, Alpha), FMath::Lerp(EndAnchor, Objective.GetLocation(), Alpha), FMath::SmoothStep(0.f, 1.f, Alpha));
				};

				// Build spline
				float Distance = 0.0f;
				for (int32 Index = 1; Index < ChainCount; Index++)
				{
					// Compute distance and next target along spline
					const float NextDistance = Distance + Lengths[Index-1] * InvTotalDistance;
					const FVector NextLocation = LerpSpline(NextDistance);
					Distance = NextDistance;

					if (DebugSettings.bEnabled)
					{
						Context.DrawInterface->DrawPoint(FTransform::Identity, NextLocation, DebugSettings.Scale * 2.0f, FLinearColor::Red);
					}

					// Compute transform and prepare next iteration
					FTGORRigUnit_Propagate::PropagateChainTowardsWithScale(Chain[Index-1], Chain[Index], NextLocation, Hierarchy, PropagateToChildren == ETGOR_Propagation::All);
				}
				
				// Set last member of chain
				FTransform EE = Hierarchy->GetGlobalTransform(Chain.Last());
				const FQuat HeadingRotation = FTGORRigUnit_RotateToward::ComputeHeadingRotation(ObjectiveSettings.EffectorForwardAxis, EEForwardTarget, ObjectiveSettings.EffectorUpAxis, EEUpTarget);
				EE.SetRotation(FQuat::Slerp(HeadingRotation, EE.GetRotation(), RotateWithTangent));
				EE.SetScale3D(Objective.GetScale3D());
				EE.SetLocation(EE.GetLocation());
				Hierarchy->SetGlobalTransform(Chain.Last(), EE, false, PropagateToChildren != ETGOR_Propagation::Off);
			}
		}
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////

FTGORRigUnit_BendTargetIK_Execute()
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
			const FVector EELocation = Objective.TransformPosition(ObjectiveSettings.Offset);
			const FVector EEUpTarget = Objective.TransformVectorNoScale(ObjectiveSettings.UpAxis);

			const FTransform Last = Hierarchy->GetGlobalTransform(Chain.Last());
			const FVector TargetDirection = (EELocation - Last.GetLocation()).GetSafeNormal();
			const FVector CurrentForward = Last.TransformVectorNoScale(ObjectiveSettings.EffectorForwardAxis);
			const FVector CurrentUp = Last.TransformVectorNoScale(ObjectiveSettings.EffectorUpAxis);

			// Compute bending applied to each segment
			const FVector BendAxis = FVector::CrossProduct(CurrentForward, TargetDirection).GetSafeNormal();
			const float BendAngle = FTGORRigUnit_SoftBoundaries::SoftBoundaries(FMath::Acos(CurrentForward | TargetDirection), FMath::DegreesToRadians(MaxBendAngle));
			const FQuat BendSegment = FQuat(BendAxis, BendAngle * Intensity / (ChainNum - 1));
			const FQuat FullBend = FQuat(BendAxis, BendAngle * Intensity);

			// Compute twist applied to each segment
			const FVector TwistUpAxis = FullBend * CurrentUp;
			const FVector TwistAxis = FVector::CrossProduct(TwistUpAxis, EEUpTarget).GetSafeNormal().ProjectOnToNormal(TargetDirection);
			const float TwistAngle = FTGORRigUnit_SoftBoundaries::SoftBoundaries(FMath::Acos(TwistUpAxis | EEUpTarget), FMath::DegreesToRadians(MaxTwistAngle));
			const FQuat TwistSegment = FQuat(TwistAxis, TwistAngle * Intensity / (ChainNum - 1));

			if (DebugSettings.bEnabled)
			{
				const FTransform First = Hierarchy->GetGlobalTransform(Chain.First());
				Context.DrawInterface->DrawLine(FTransform::Identity, First.GetLocation(), First.GetLocation() + TwistUpAxis * 20.0f, FLinearColor::Red, DebugSettings.Scale * 0.2f);
				Context.DrawInterface->DrawLine(FTransform::Identity, First.GetLocation(), First.GetLocation() + EEUpTarget * 20.0f, FLinearColor::Green, DebugSettings.Scale * 0.2f);
				Context.DrawInterface->DrawLine(FTransform::Identity, First.GetLocation(), First.GetLocation() + TwistAxis * 20.0f, FLinearColor::Blue, DebugSettings.Scale * 0.5f);
			}

			// Apply rotation along the whole chain
			FTransform Transform = Hierarchy->GetGlobalTransform(Chain[0]);
			for (int32 Index = 1; Index < ChainNum; Index++)
			{
				const FTransform Local = Hierarchy->GetLocalTransform(Chain[Index]);

				// Rotate segment along forward axis according to how much they are aligned to roll axis
				Transform.SetRotation(TwistSegment * BendSegment * Transform.GetRotation());

				// Update chain element (no update needed since we propagate everything later)
				Hierarchy->SetGlobalTransform(Chain[Index-1], Transform, false, false);

				// Propagate to next in line and store delta for fabrik
				Transform = Local * Transform;
			}

			// Set EE transform
			Hierarchy->SetGlobalTransform(Chain.Last(), Transform, false, PropagateToChildren != ETGOR_Propagation::Off);

			if (DebugSettings.bEnabled)
			{
				Context.DrawInterface->DrawLine(FTransform::Identity, Transform.GetLocation(), Transform.GetLocation() + EEUpTarget * 20.0f, FLinearColor::Blue, DebugSettings.Scale * 0.5f);
				Context.DrawInterface->DrawLine(FTransform::Identity, Transform.GetLocation(), Transform.GetLocation() + Transform.TransformVectorNoScale(ObjectiveSettings.EffectorForwardAxis) * 20.0f, FLinearColor::Black, DebugSettings.Scale * 0.5f);
				Context.DrawInterface->DrawLine(FTransform::Identity, Transform.GetLocation(), Transform.GetLocation() + Transform.TransformVectorNoScale(ObjectiveSettings.EffectorUpAxis) * 20.0f, FLinearColor::White, DebugSettings.Scale * 0.5f);
			}
		}
	}
}
