// The Gateway of Realities: Planes of Existence.


#include "TGORRigUnit_Abstract.h"

#include "RealitiesUtility/Structures/TGOR_Matrix3x3.h"
#include "Units/RigUnitContext.h"


FTGORRigUnit_AbstractOrient_Execute()
{
	DECLARE_SCOPE_HIERARCHICAL_COUNTER_RIGUNIT()
	FRigHierarchyContainer* Hierarchy = ExecuteContext.Hierarchy;

	if (Context.State == EControlRigState::Init)
	{
		AbsBaseCache.Reset();
		ConCache.Reset();
		AbsOrientCache.Reset();
	}
	else
	{
		if (SourceChain.Num() < 2)
		{
			UE_CONTROLRIG_RIGUNIT_REPORT_WARNING(TEXT("Chain has to have length at least 2."));
		}
		else
		{
			if (!AbsBaseCache.UpdateCache(AbsBaseKey, Hierarchy))
			{
				UE_CONTROLRIG_RIGUNIT_REPORT_WARNING(TEXT("Base '%s' is not valid."), *AbsBaseKey.ToString());
			}
			else if (!AbsOrientCache.UpdateCache(AbsOrientKey, Hierarchy))
			{
				UE_CONTROLRIG_RIGUNIT_REPORT_WARNING(TEXT("Orient '%s' is not valid."), *AbsOrientKey.ToString());
			}
			else
			{
				const FVector First = Hierarchy->GetGlobalTransform(SourceChain.First()).GetLocation();
				const FVector Last = Hierarchy->GetGlobalTransform(SourceChain.Last()).GetLocation();
				const FQuat Rotation = Hierarchy->GetGlobalTransform(AbsBaseCache).GetRotation();

				FVector ForwardTarget = Last - First;

				if (ConCache.UpdateCache(ConKey, Hierarchy))
				{
					const FTransform Initial = Hierarchy->GetInitialGlobalTransform(AbsBaseCache);
					const FVector InitialNormal = Initial.TransformVectorNoScale(AbsBoneAimAxis);
					const FTransform Cone = Hierarchy->GetGlobalTransform(ConCache);
					const FVector ConeNormal = Cone.TransformVectorNoScale(ConeAimAxis);
					const FQuat ConeOffset = FQuat::FindBetweenNormals(ConeNormal, InitialNormal);

					ForwardTarget = ConeOffset.RotateVector(ForwardTarget);
				}

				ForwardTarget.Normalize();
				const FVector UpTarget = Rotation * AbsBoneUpAxis;

				FTransform Transform = Hierarchy->GetGlobalTransform(AbsOrientCache);
				const FQuat Heading = FTGORRigUnit_RotateToward::ComputeHeadingRotation(AbsBoneAimAxis, ForwardTarget, AbsBoneUpAxis, UpTarget);
				Transform.SetRotation(Heading * OffsetRotation.Quaternion());
				Hierarchy->SetGlobalTransform(AbsOrientCache, Transform, PropagateToChildren != ETGOR_Propagation::Off);
			}
		}
	}
}

FString FTGORRigUnit_AbstractOrient::ProcessPinLabelForInjection(const FString& InLabel) const
{
	FString Formula;
	return FString::Printf(TEXT("%s: TODO"), *InLabel);
}


////////////////////////////////////////////////////////////////////////////////////////////////////

FTGORRigUnit_AbstractChain_Execute()
{
	DECLARE_SCOPE_HIERARCHICAL_COUNTER_RIGUNIT()
	FRigHierarchyContainer* Hierarchy = ExecuteContext.Hierarchy;

	if (Context.State == EControlRigState::Init)
	{
		AbsBaseCache.Reset();
		ConCache.Reset();
		AbsTipCache.Reset();
	}
	else
	{
		if (SourceChain.Num() < 2)
		{
			UE_CONTROLRIG_RIGUNIT_REPORT_WARNING(TEXT("Chain has to have length at least 2."));
		}
		else
		{
			const float ChainMaxLength = FTGORRigUnit_ChainLength::ComputeInitialChainLength(SourceChain, Hierarchy) * LengthMultiplier;

			const FTransform First = Hierarchy->GetGlobalTransform(SourceChain.First());
			const FTransform Last = Hierarchy->GetGlobalTransform(SourceChain.Last());
			const FVector TargetDelta = Last.GetLocation() - First.GetLocation();
			CurrentLength = TargetDelta.Size();

			if (FMath::IsNearlyZero(CurrentLength))
			{
				UE_CONTROLRIG_RIGUNIT_REPORT_WARNING(TEXT("Length is null."));
			}
			else if (!AbsBaseCache.UpdateCache(AbsBaseKey, Hierarchy))
			{
				UE_CONTROLRIG_RIGUNIT_REPORT_WARNING(TEXT("Base '%s' is not valid."), *AbsBaseKey.ToString());
			}
			else
			{
				FVector TargetDirection = TargetDelta / CurrentLength;

				if (ConCache.UpdateCache(ConKey, Hierarchy))
				{
					const FTransform Initial = Hierarchy->GetInitialGlobalTransform(AbsBaseCache);
					const FVector InitialNormal = Initial.TransformVectorNoScale(AbsBoneAimAxis);
					const FTransform Cone = Hierarchy->GetGlobalTransform(ConCache);
					const FVector ConeNormal = Cone.TransformVectorNoScale(ConeAimAxis);
					const FQuat ConeOffset = FQuat::FindBetweenNormals(ConeNormal, InitialNormal);

					TargetDirection = ConeOffset.RotateVector(TargetDirection);
				}

				TargetDirection.Normalize();
				const FVector UpTarget = Hierarchy->GetGlobalTransform(SourceChain[ChainUpBoneIdx]).TransformVectorNoScale(ChainUpAxis);

				FTransform BaseTransform = Hierarchy->GetGlobalTransform(AbsBaseCache);
				BaseTransform.SetRotation(FTGORRigUnit_RotateToward::ComputeHeadingRotation(AbsBoneAimAxis, TargetDirection, AbsBoneUpAxis, UpTarget));
				Hierarchy->SetGlobalTransform(AbsBaseCache, BaseTransform, PropagateToChildren == ETGOR_Propagation::All);

				if (!AbsTipCache.UpdateCache(AbsTipKey, Hierarchy))
				{
					UE_CONTROLRIG_RIGUNIT_REPORT_WARNING(TEXT("Tip '%s' is not valid."), *AbsTipKey.ToString());
				}
				else
				{
					const FVector InitialFirst = Hierarchy->GetInitialGlobalTransform(AbsBaseCache).GetLocation();
					const FVector InitialLast = Hierarchy->GetInitialGlobalTransform(AbsTipCache).GetLocation();
					const float InitialLength = (InitialLast - InitialFirst).Size();

					// Rotate and upscale the tip
					FTransform TipTransform = Hierarchy->GetGlobalTransform(AbsTipCache);
					TipTransform.SetLocation(BaseTransform.GetLocation() + TargetDirection * CurrentLength / ChainMaxLength * InitialLength);
					TipTransform.SetRotation(Last.GetRotation() * OffsetRotation.Quaternion());
					Hierarchy->SetGlobalTransform(AbsTipCache, TipTransform, PropagateToChildren != ETGOR_Propagation::Off);
				}
			}
		}
	}
}

FString FTGORRigUnit_AbstractChain::ProcessPinLabelForInjection(const FString& InLabel) const
{
	FString Formula;
	return FString::Printf(TEXT("%s: TODO"), *InLabel);
}


////////////////////////////////////////////////////////////////////////////////////////////////////

FTGORRigUnit_AbstractTranslate_Execute()
{
	DECLARE_SCOPE_HIERARCHICAL_COUNTER_RIGUNIT()
	FRigHierarchyContainer* Hierarchy = ExecuteContext.Hierarchy;

	if (Context.State == EControlRigState::Init)
	{
		AbsBaseCache.Reset();
		AbsPivotCache.Reset();
		AbsTargetCache.Reset();
	}
	else
	{
		if (SourceChain.Num() < 2)
		{
			UE_CONTROLRIG_RIGUNIT_REPORT_WARNING(TEXT("Chain has to have length at least 2."));
		}
		else
		{
			const float ChainMaxLength = FTGORRigUnit_ChainLength::ComputeInitialChainLength(SourceChain, Hierarchy) * LengthMultiplier;

			const FTransform First = Hierarchy->GetGlobalTransform(SourceChain.First());
			const FTransform Last = Hierarchy->GetGlobalTransform(SourceChain.Last());
			const float CurrentLength = (Last.GetLocation() - First.GetLocation()).Size();

			const FTransform InitialFirst = Hierarchy->GetInitialGlobalTransform(SourceChain.First());
			const FTransform InitialLast = Hierarchy->GetInitialGlobalTransform(SourceChain.Last());
			const float InitialLength = (InitialLast.GetLocation() - InitialFirst.GetLocation()).Size();

			if (TargetDirection.IsNearlyZero())
			{
				UE_CONTROLRIG_RIGUNIT_REPORT_WARNING(TEXT("Target direction is null."));
			}
			else if (SourceForwardAxis.IsNearlyZero())
			{
				UE_CONTROLRIG_RIGUNIT_REPORT_WARNING(TEXT("Source forward axis is null."));
			}
			else if (SourceUpAxis.IsNearlyZero())
			{
				UE_CONTROLRIG_RIGUNIT_REPORT_WARNING(TEXT("Source up axis is null."));
			}
			else
			{
				const FVector TargetNormal = TargetDirection.GetSafeNormal();

				const FVector TargetUp = First.TransformVectorNoScale(SourceUpAxis);
				const FQuat SourceRotation = FTGORRigUnit_RotateToward::ComputeHeadingRotation(SourceForwardAxis, TargetNormal, SourceUpAxis, TargetUp);
				const FVector SourceBaseLocation = FMath::Lerp(First.GetLocation(), InitialFirst.GetLocation(), PivotInitialLerp);
				const FVector SourceLocation = SourceBaseLocation + TargetNormal * FMath::Lerp(CurrentLength, InitialLength, LengthInitialLerp);
				SourcePivot = FTransform(SourceRotation, SourceLocation, FVector::OneVector);

				if (AbsBoneAimAxis.IsNearlyZero())
				{
				UE_CONTROLRIG_RIGUNIT_REPORT_WARNING(TEXT("Abstractor forward axis is null."));
				}
				else if (AbsBoneUpAxis.IsNearlyZero())
				{
					UE_CONTROLRIG_RIGUNIT_REPORT_WARNING(TEXT("Abstractor up axis is null."));
				}
				else if (!AbsBaseCache.UpdateCache(AbsBaseKey, Hierarchy))
				{
					UE_CONTROLRIG_RIGUNIT_REPORT_WARNING(TEXT("Base '%s' is not valid."), *AbsBaseKey.ToString());
				}
				else if (!AbsPivotCache.UpdateCache(AbsPivotKey, Hierarchy))
				{
					UE_CONTROLRIG_RIGUNIT_REPORT_WARNING(TEXT("Pivot '%s' is not valid."), *AbsPivotKey.ToString());
				}
				else if (!AbsTargetCache.UpdateCache(AbsTargetKey, Hierarchy))
				{
					UE_CONTROLRIG_RIGUNIT_REPORT_WARNING(TEXT("Target '%s' is not valid."), *AbsTargetKey.ToString());
				}
				else
				{
					FTransform BaseTransform = Hierarchy->GetGlobalTransform(AbsBaseCache);

					const FVector InitialBase = Hierarchy->GetInitialGlobalTransform(AbsBaseCache).GetLocation();
					const FVector InitialPivot = Hierarchy->GetInitialGlobalTransform(AbsPivotCache).GetLocation();
					const float SourceAbsTranslate = (InitialPivot - InitialBase).Size() / ChainMaxLength;

					const FQuat AbsRotation = FTGORRigUnit_RotateToward::ComputeHeadingRotation(AbsBoneAimAxis, TargetNormal, AbsBoneUpAxis, TargetUp);
					const FVector AbsBaseLocation = FMath::Lerp(BaseTransform.GetLocation(), InitialBase, PivotInitialLerp);
					const FVector AbsLocation = AbsBaseLocation + TargetNormal * (FMath::Lerp(CurrentLength, InitialLength, LengthInitialLerp) * SourceAbsTranslate);

					// Rotate the base
					BaseTransform.SetRotation(AbsRotation);
					Hierarchy->SetGlobalTransform(AbsBaseCache, BaseTransform, false);

					// Translate and rotate the pivot
					AbsPivot = FTransform(AbsRotation * OffsetRotation.Quaternion(), AbsLocation, FVector::OneVector);
					Hierarchy->SetGlobalTransform(AbsPivotCache, AbsPivot, false);

					// Rotate and translate the pivot
					FTransform TargetTransform = Hierarchy->GetGlobalTransform(AbsTargetCache);
					FTGORRigUnit_ConvertSpace::ConvertSpace(TargetTransform, Last, SourcePivot, AbsPivot, SourceAbsTranslate);
					Hierarchy->SetGlobalTransform(AbsTargetCache, TargetTransform, PropagateToChildren != ETGOR_Propagation::Off);
				}
			}
		}
	}
}

FString FTGORRigUnit_AbstractTranslate::ProcessPinLabelForInjection(const FString& InLabel) const
{
	FString Formula;
	return FString::Printf(TEXT("%s: TODO"), *InLabel);
}


////////////////////////////////////////////////////////////////////////////////////////////////////

FTGORRigUnit_AbstractInitial_Execute()
{
	DECLARE_SCOPE_HIERARCHICAL_COUNTER_RIGUNIT()
	FRigHierarchyContainer* Hierarchy = ExecuteContext.Hierarchy;

	if (Context.State == EControlRigState::Init)
	{
		AbsBaseCache.Reset();
		SourceBaseCache.Reset();
		AbsRefCache.Reset();
		SourceRefCache.Reset();
	}
	else
	{
		// If no reference is present we assume origin
		FVector InitAbsRefLocation = FVector::ZeroVector;
		if (AbsRefCache.UpdateCache(AbsRefKey, Hierarchy))
		{
			InitAbsRefLocation = Hierarchy->GetInitialGlobalTransform(AbsRefCache).GetLocation();
		}

		// If no reference is present we assume origin
		FVector InitSourceRefLocation = FVector::ZeroVector;
		if (SourceRefCache.UpdateCache(SourceRefKey, Hierarchy))
		{
			InitSourceRefLocation = Hierarchy->GetInitialGlobalTransform(SourceRefCache).GetLocation();
		}

		if (!AbsBaseCache.UpdateCache(AbsBaseKey, Hierarchy))
		{
			UE_CONTROLRIG_RIGUNIT_REPORT_WARNING(TEXT("Abs '%s' is not valid."), *AbsBaseKey.ToString());
		}
		else if (!SourceBaseCache.UpdateCache(SourceBaseKey, Hierarchy))
		{
			UE_CONTROLRIG_RIGUNIT_REPORT_WARNING(TEXT("Source '%s' is not valid."), *SourceBaseKey.ToString());
		}
		else
		{
			SourcePivot = Hierarchy->GetInitialGlobalTransform(SourceBaseCache);

			AbsPivot = Hierarchy->GetInitialGlobalTransform(AbsBaseCache);
			AbsPivot.SetRotation(AbsPivot.GetRotation() * OffsetRotation.Quaternion());

			const float SourceAbsTranslate = FMath::Sqrt((InitAbsRefLocation - AbsPivot.GetLocation()).SizeSquared() / (InitSourceRefLocation - SourcePivot.GetLocation()).SizeSquared()) * LengthMultiplier;

			const FTransform SourceTransform = Hierarchy->GetGlobalTransform(SourceBaseCache);
			FTransform AbsTransform = Hierarchy->GetGlobalTransform(AbsBaseCache);
			FTGORRigUnit_ConvertSpace::ConvertSpace(AbsTransform, SourceTransform, SourcePivot, AbsPivot, SourceAbsTranslate);
			Hierarchy->SetGlobalTransform(AbsBaseCache, AbsTransform, PropagateToChildren != ETGOR_Propagation::Off);
		}
	}
}

FString FTGORRigUnit_AbstractInitial::ProcessPinLabelForInjection(const FString& InLabel) const
{
	FString Formula;
	return FString::Printf(TEXT("%s: TODO"), *InLabel);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

FTGORRigUnit_AbstractAnchor_Execute()
{
	DECLARE_SCOPE_HIERARCHICAL_COUNTER_RIGUNIT()
	FRigHierarchyContainer* Hierarchy = ExecuteContext.Hierarchy;

	if (Context.State == EControlRigState::Init)
	{
		AbsBaseCache.Reset();
		AbsRefCache.Reset();

		SourceBaseCache.Reset();
		SourceLeftCache.Reset();
		SourceRightCache.Reset();
	}
	else
	{
		if (!AbsBaseCache.UpdateCache(AbsBaseKey, Hierarchy))
		{
			UE_CONTROLRIG_RIGUNIT_REPORT_WARNING(TEXT("Abs base '%s' is not valid."), *AbsBaseKey.ToString());
		}
		else if (!AbsRefCache.UpdateCache(AbsRefKey, Hierarchy))
		{
			UE_CONTROLRIG_RIGUNIT_REPORT_WARNING(TEXT("Abs ref '%s' is not valid."), *AbsRefKey.ToString());
		}
		else if (!SourceBaseCache.UpdateCache(SourceBaseKey, Hierarchy) ||
			!SourceLeftCache.UpdateCache(SourceLeftKey, Hierarchy) ||
			!SourceRightCache.UpdateCache(SourceRightKey, Hierarchy))
		{
			UE_CONTROLRIG_RIGUNIT_REPORT_WARNING(TEXT("Source left or right '%s' is not valid."), *SourceBaseKey.ToString());
		}
		else
		{
			// Source triangle
			const FTransform SourceBase = Hierarchy->GetGlobalTransform(SourceBaseCache);
			const FTransform SourceLeft = Hierarchy->GetGlobalTransform(SourceLeftCache);
			const FTransform SourceRight = Hierarchy->GetGlobalTransform(SourceRightCache);

			// Compute source analysis using cone normal
			FTGOR_TripletAnalysis SourceAnalysis;
			FTGORRigUnit_TripletAnalysis::ComputeTripletPartial(SourceBase.GetLocation(), SourceLeft.GetLocation(), SourceRight.GetLocation(), SourceAnalysis);

			const FQuat UpQuat = FQuat::FindBetweenNormals(ConeAnalysis.Direction, SourceAnalysis.Direction);
			const FVector SourceNormal = UpQuat.RotateVector(ConeAnalysis.Normal);

			FTGORRigUnit_TripletAnalysis::ComputeTripletFinish(SourceBase.GetLocation(), SourceNormal, SourceAnalysis);

			// Compute final total rotation
			const FVector ConeProject = FVector::VectorPlaneProject(ConeAnalysis.Tangent, SourceNormal);
			const FVector SourceProject = FVector::VectorPlaneProject(SourceAnalysis.Tangent, SourceNormal);
			const FQuat Rotation = FQuat::FindBetweenVectors(ConeProject, SourceProject) * UpQuat;

			if (DebugSettings.bEnabled)
			{
				Context.DrawInterface->DrawLine(FTransform::Identity, ConeAnalysis.Center, ConeAnalysis.Center + ConeAnalysis.Direction * 20.0f, FLinearColor::Blue, DebugSettings.Scale * 0.5f);
				Context.DrawInterface->DrawLine(FTransform::Identity, ConeAnalysis.Center, ConeAnalysis.Center + ConeProject * 20.0f, FLinearColor::Blue, DebugSettings.Scale * 0.5f);
				Context.DrawInterface->DrawLine(FTransform::Identity, SourceAnalysis.Center, SourceAnalysis.Center + SourceAnalysis.Direction * 20.0f, FLinearColor::Red, DebugSettings.Scale * 0.5f);
				Context.DrawInterface->DrawLine(FTransform::Identity, SourceAnalysis.Center, SourceAnalysis.Center + SourceProject * 20.0f, FLinearColor::Red, DebugSettings.Scale * 0.5f);
			}

			SourcePivot.SetRotation(Rotation);
			SourcePivot.SetLocation(ConeAnalysis.Center);

			// Compute length conversion
			const FTransform InitialAbsBase = Hierarchy->GetInitialGlobalTransform(AbsBaseCache);
			const FTransform InitialAbsRef = Hierarchy->GetInitialGlobalTransform(AbsRefCache);
			const float InitialChainLength = (InitialAbsBase.GetLocation() - InitialAbsRef.GetLocation()).Size();
			const float ConeChainLength = (RefAnalysis.Center - ConeAnalysis.Center).Size();

			AbsPivot.SetRotation(InitialAbsBase.GetRotation());
			AbsPivot.SetLocation(InitialAbsBase.GetLocation());

			FTransform Transform = Hierarchy->GetGlobalTransform(AbsBaseCache);
			Transform.SetRotation(Rotation * InitialAbsBase.GetRotation());
			Transform.SetLocation(InitialAbsBase.GetLocation() + (SourceAnalysis.Center - ConeAnalysis.Center) / ConeChainLength * InitialChainLength);
			Hierarchy->SetGlobalTransform(AbsBaseCache, Transform, PropagateToChildren != ETGOR_Propagation::Off);
		}
	}
}

FString FTGORRigUnit_AbstractAnchor::ProcessPinLabelForInjection(const FString& InLabel) const
{
	FString Formula;
	/*
	if (bMapRange)
	{
		Formula += FString::Printf(TEXT(" Map(%.02f, %.02f, %.02f, %.02f)"), InRange.Min, InRange.Max, OutRange.Min, OutRange.Max);
	}
	if (bInterpResult)
	{
		Formula += FString::Printf(TEXT(" Interp(%.02f, %.02f)"), InterpSpeedIncreasing, InterpSpeedDecreasing);
	}
	if (bClampResult)
	{
		Formula += FString::Printf(TEXT(" Clamp(%.02f, %.02f)"), ClampMin, ClampMax);
	}

	if (Formula.IsEmpty())
	{
		return InLabel;
	}
	return FString::Printf(TEXT("%s: %s"), *InLabel, *Formula);
	*/
	return FString::Printf(TEXT("%s: TODO"), *InLabel);
}
