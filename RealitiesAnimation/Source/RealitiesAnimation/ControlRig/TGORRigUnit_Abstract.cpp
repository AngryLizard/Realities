// The Gateway of Realities: Planes of Existence.


#include "TGORRigUnit_Abstract.h"

#include "RealitiesUtility/Structures/TGOR_Matrix3x3.h"
#include "RealitiesUtility/Utility/TGOR_Error.h"
#include "Units/RigUnitContext.h"

#define SOURCE_RESET AbsBaseCache.Reset();
#define SOURCE_UPDATE \
	if (!AbsBaseCache.UpdateCache(AbsBaseKey, Context.Hierarchy)) \
	{ UE_CONTROLRIG_RIGUNIT_REPORT_WARNING(TEXT("Base '%s' is not valid."), *AbsBaseKey.ToString()); } else \



FTGORRigUnit_AbstractOrient_Execute()
{
	DECLARE_SCOPE_HIERARCHICAL_COUNTER_RIGUNIT()
	FRigHierarchyContainer* Hierarchy = ExecuteContext.Hierarchy;

	if (Context.State == EControlRigState::Init)
	{
		SOURCE_RESET;
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
			if (!AbsOrientCache.UpdateCache(AbsOrientKey, Hierarchy))
			{
				UE_CONTROLRIG_RIGUNIT_REPORT_WARNING(TEXT("Orient '%s' is not valid."), *AbsOrientKey.ToString());
			}
			else SOURCE_UPDATE
			{
				const FVector First = Hierarchy->GetGlobalTransform(SourceChain.First()).GetLocation();
				const FVector Last = Hierarchy->GetGlobalTransform(SourceChain.Last()).GetLocation();
				const FQuat Rotation = Hierarchy->GetGlobalTransform(AbsBaseCache).GetRotation();

				const FVector ForwardTarget = (Last - First).GetSafeNormal();
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
		SOURCE_RESET;
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
			else SOURCE_UPDATE
			{
				const FVector TargetDirection = TargetDelta / CurrentLength;
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
		SOURCE_RESET;
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
				else if (!AbsPivotCache.UpdateCache(AbsPivotKey, Hierarchy))
				{
					UE_CONTROLRIG_RIGUNIT_REPORT_WARNING(TEXT("Pivot '%s' is not valid."), *AbsPivotKey.ToString());
				}
				else if (!AbsTargetCache.UpdateCache(AbsTargetKey, Hierarchy))
				{
					UE_CONTROLRIG_RIGUNIT_REPORT_WARNING(TEXT("Target '%s' is not valid."), *AbsTargetKey.ToString());
				}
				else SOURCE_UPDATE
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
		SOURCE_RESET;
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

		else if (!SourceBaseCache.UpdateCache(SourceBaseKey, Hierarchy))
		{
			UE_CONTROLRIG_RIGUNIT_REPORT_WARNING(TEXT("Source '%s' is not valid."), *SourceBaseKey.ToString());
		}
		else SOURCE_UPDATE
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

FTGORRigUnit_AbstractRoot_Execute()
{
	DECLARE_SCOPE_HIERARCHICAL_COUNTER_RIGUNIT()
	FRigHierarchyContainer* Hierarchy = ExecuteContext.Hierarchy;

	if (Context.State == EControlRigState::Init)
	{
		SOURCE_RESET;
		SourceBaseCache.Reset();
		AbsRootCache.Reset();
		SourceRootCache.Reset();
	}
	else
	{
		if (!SourceBaseCache.UpdateCache(SourceBaseKey, Hierarchy))
		{
			UE_CONTROLRIG_RIGUNIT_REPORT_WARNING(TEXT("Source base '%s' is not valid."), *SourceBaseKey.ToString());
		}
		else SOURCE_UPDATE
		{
			// Compute source root transform
			SourcePivot = Hierarchy->GetGlobalTransform(SourceBaseCache);

			FTransform InitialSourceRoot = FTransform::Identity;
			if (SourceRootCache.UpdateCache(SourceRootKey, Hierarchy))
			{
				InitialSourceRoot = Hierarchy->GetInitialGlobalTransform(SourceRootCache);
			}

			const FTransform InitialSourceBase = Hierarchy->GetInitialGlobalTransform(SourceBaseCache);
			const float SourceHeight = (InitialSourceBase.GetLocation() - InitialSourceRoot.GetLocation()).Size();
			if (!FMath::IsNearlyZero(SourceHeight))
			{
				FTransform InitialAbsRoot = FTransform::Identity;
				if (AbsRootCache.UpdateCache(AbsRootKey, Hierarchy))
				{
					InitialAbsRoot = Hierarchy->GetInitialGlobalTransform(AbsRootCache);
				}

				const FTransform InitialAbsBase = Hierarchy->GetInitialGlobalTransform(AbsBaseCache);
				const float AbsHeight = (InitialAbsBase.GetLocation() - InitialAbsRoot.GetLocation()).Size();

				const FTransform SourceBase = Hierarchy->GetGlobalTransform(SourceBaseCache);

				AbsPivot.SetRotation(SourcePivot.GetRotation() * OffsetRotation.Quaternion());
				AbsPivot.SetLocation(InitialAbsRoot.GetLocation() + (SourceBase.GetLocation() - InitialSourceRoot.GetLocation()) * (AbsHeight / SourceHeight * RootHeight));
				AbsPivot.SetScale3D(FVector::OneVector);

				// TODO: Possibly include offset computation
				Hierarchy->SetGlobalTransform(AbsBaseCache, AbsPivot, PropagateToChildren != ETGOR_Propagation::Off);
			}
		}
	}
}

FString FTGORRigUnit_AbstractRoot::ProcessPinLabelForInjection(const FString& InLabel) const
{
	FString Formula;
	return FString::Printf(TEXT("%s: TODO"), *InLabel);
}
