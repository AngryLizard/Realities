// The Gateway of Realities: Planes of Existence.

#include "TGORRigUnit_ExpressionIK.h"

#include "RealitiesUtility/Structures/TGOR_Matrix3x3.h"
#include "RealitiesUtility/Utility/TGOR_Math.h"
#include "Units/RigUnitContext.h"

////////////////////////////////////////////////////////////////////////////////////////////////////

FTGORRigUnit_LookAtIK_Execute()
{
	DECLARE_SCOPE_HIERARCHICAL_COUNTER_RIGUNIT()
	URigHierarchy* Hierarchy = ExecuteContext.Hierarchy;

	if (Context.State == EControlRigState::Init)
	{
		Cache.Reset();
		return;
	}

	if (Context.State == EControlRigState::Update)
	{
		if (!Cache.UpdateCache(Key, Hierarchy))
		{
			UE_CONTROLRIG_RIGUNIT_REPORT_WARNING(TEXT("Key '%s' is not valid."), *Key.ToString());
		}
		else
		{
			// Objective properties
			const FVector EELocation = Objective.TransformPosition(ObjectiveSettings.Offset);

			const FVector EffectorRightAxis = FVector::CrossProduct(ObjectiveSettings.EffectorForwardAxis, ObjectiveSettings.EffectorUpAxis);

			// Get current transform basis
			FTransform Transform = Hierarchy->GetGlobalTransform(Cache);
			const FVector CurrentUp = Transform.TransformVectorNoScale(ObjectiveSettings.EffectorUpAxis);
			const FVector CurrentForward = Transform.TransformVectorNoScale(ObjectiveSettings.EffectorForwardAxis);
			const FVector CurrentRight = Transform.TransformVectorNoScale(EffectorRightAxis);

			if (DebugSettings.bEnabled)
			{
				Context.DrawInterface->DrawLine(FTransform::Identity, Transform.GetLocation(), Transform.GetLocation() + CurrentForward * 10.0f, FLinearColor::Red, DebugSettings.Scale * 0.1f);
				Context.DrawInterface->DrawLine(FTransform::Identity, Transform.GetLocation(), Transform.GetLocation() + CurrentRight * 10.0f, FLinearColor::Green, DebugSettings.Scale * 0.1f);
				Context.DrawInterface->DrawLine(FTransform::Identity, Transform.GetLocation(), Transform.GetLocation() + CurrentUp * 10.0f, FLinearColor::Blue, DebugSettings.Scale * 0.1f);
			}

			// Compute center with input offsets
			const FQuat CenterRotation = FQuat(CurrentUp, FMath::DegreesToRadians(Bias.X)) * FQuat(CurrentRight, FMath::DegreesToRadians(Bias.Y)) * Transform.GetRotation();

			// Transform look-at to localspace so we can limit the angles efficiently
			const FVector TargetDirection = (EELocation - Transform.GetLocation()).GetSafeNormal();
			const FVector LocalDirection = CenterRotation.Inverse() * TargetDirection;

			FVector Intensities;
			Intensities.Y = FTGORRigUnit_SoftBoundaries::SoftBoundaries((LocalDirection | EffectorRightAxis) * Intensity, FMath::DegreesToRadians(Range.X));
			Intensities.Z = FTGORRigUnit_SoftBoundaries::SoftBoundaries((LocalDirection | ObjectiveSettings.EffectorUpAxis) * Intensity, FMath::DegreesToRadians(Range.Y));
			Intensities.X = FMath::Sqrt(1.0f - (FMath::Square(Intensities.Y) + FMath::Square(Intensities.Z)));

			const FVector Restricted = ObjectiveSettings.EffectorForwardAxis * Intensities.X + EffectorRightAxis * Intensities.Y + ObjectiveSettings.EffectorUpAxis * Intensities.Z;

			// Transform back to global space and set bone transform
			const FVector FinalDirection = CenterRotation * Restricted;
			const FQuat Offset = FQuat::FindBetween(CurrentForward, FinalDirection);

			Transform.SetRotation(Offset * Transform.GetRotation());

			Hierarchy->SetGlobalTransform(Cache, Transform, PropagateToChildren != ETGOR_Propagation::Off);

			if (DebugSettings.bEnabled)
			{
				Context.DrawInterface->DrawLine(FTransform::Identity, Transform.GetLocation(), Transform.GetLocation() + FinalDirection * 15.0f, FLinearColor::White, DebugSettings.Scale * 0.5f);
				Context.DrawInterface->DrawLine(FTransform::Identity, Transform.GetLocation(), Transform.GetLocation() + TargetDirection * 20.0f, FLinearColor::Black, DebugSettings.Scale * 0.25f);
			}
		}
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////

FTGORRigUnit_EyelidIK_Execute()
{
	DECLARE_SCOPE_HIERARCHICAL_COUNTER_RIGUNIT()
	URigHierarchy* Hierarchy = ExecuteContext.Hierarchy;

	if (Context.State == EControlRigState::Init)
	{
		Cache.Reset();
		return;
	}

	if (Context.State == EControlRigState::Update)
	{
		if (!Cache.UpdateCache(Key, Hierarchy))
		{
			UE_CONTROLRIG_RIGUNIT_REPORT_WARNING(TEXT("key '%s' is not valid."), *Key.ToString());
		}
		else
		{
			// Objective properties
			const FVector EEForward = Objective.TransformVectorNoScale(ObjectiveSettings.ForwardAxis);

			// Get current transform basis
			FTransform Transform = Hierarchy->GetGlobalTransform(Cache);
			const FVector CurrentUp = Transform.TransformVectorNoScale(ObjectiveSettings.EffectorUpAxis);
			const FVector CurrentForward = Transform.TransformVectorNoScale(ObjectiveSettings.EffectorForwardAxis);
			const FVector EffectorRightAxis = FVector::CrossProduct(ObjectiveSettings.EffectorForwardAxis, ObjectiveSettings.EffectorUpAxis);
			const FVector CurrentRight = Transform.TransformVectorNoScale(EffectorRightAxis);

			if (DebugSettings.bEnabled)
			{
				Context.DrawInterface->DrawLine(FTransform::Identity, Transform.GetLocation(), Transform.GetLocation() + CurrentForward * 10.0f, FLinearColor::Red, DebugSettings.Scale * 0.1f);
				Context.DrawInterface->DrawLine(FTransform::Identity, Transform.GetLocation(), Transform.GetLocation() + CurrentRight * 10.0f, FLinearColor::Green, DebugSettings.Scale * 0.1f);
				Context.DrawInterface->DrawLine(FTransform::Identity, Transform.GetLocation(), Transform.GetLocation() + CurrentUp * 10.0f, FLinearColor::Blue, DebugSettings.Scale * 0.1f);
			}

			// Range from the bottom (starting at 0) all the way to the top
			const float RangeRadians = FMath::DegreesToRadians(Range);
			const float IrisRadians = FMath::DegreesToRadians(Offset);

			// Compute bottom with input offsets
			const float CenterRadians = FMath::DegreesToRadians(Bias);
			const FQuat CenterRotation = FQuat(CurrentRight, CenterRadians);
			const FVector CenterDirection = CenterRotation * CurrentUp;

			// Compute eyelid angle
			const float EyeRadians = FTGORRigUnit_SoftBoundaries::SoftBoundaries(IrisRadians - FMath::Asin(CenterDirection | EEForward), RangeRadians);
			const float LidRadians = FMath::Lerp(FMath::Lerp(EyeRadians, RangeRadians, Openness), -RangeRadians, Closeness);
			const FQuat LidRotation = FQuat(CurrentRight, CenterRadians + LidRadians);

			Transform.SetRotation(LidRotation * Transform.GetRotation());

			Hierarchy->SetGlobalTransform(Cache, Transform, PropagateToChildren != ETGOR_Propagation::Off);

			if (DebugSettings.bEnabled)
			{
				Context.DrawInterface->DrawLine(FTransform::Identity, Transform.GetLocation(), Transform.GetLocation() + CenterRotation * CurrentForward * 15.0f, FLinearColor::White, DebugSettings.Scale * 0.5f);
			}
		}
	}
}
