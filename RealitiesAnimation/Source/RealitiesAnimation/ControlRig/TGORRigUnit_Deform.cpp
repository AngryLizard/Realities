// The Gateway of Realities: Planes of Existence.


#include "TGORRigUnit_Deform.h"
#include "TGORRigUnit_Utility.h"

#include "RealitiesUtility/Structures/TGOR_Matrix3x3.h"
#include "RealitiesUtility/Utility/TGOR_Math.h"
#include "Units/RigUnitContext.h"

#define DEFORM_RESET AbsBaseCache.Reset();
#define DEFORM_UPDATE \
	if (!AbsBaseCache.UpdateCache(AbsBaseKey, Context.Hierarchy)) \
	{ UE_CONTROLRIG_RIGUNIT_REPORT_WARNING(TEXT("Abstract base '%s' is not valid."), *AbsBaseKey.ToString()); } else \

////////////////////////////////////////////////////////////////////////////////////////////////////

FTGORRigUnit_DeformOrient_Execute()
{
	DECLARE_SCOPE_HIERARCHICAL_COUNTER_RIGUNIT()
	FRigHierarchyContainer* Hierarchy = ExecuteContext.Hierarchy;

	if (Context.State == EControlRigState::Init)
	{
		DEFORM_RESET;
	}
	else
	{
		DEFORM_UPDATE
		{
			const FTransform Transform = Hierarchy->GetGlobalTransform(AbsBaseCache);
			Tangent = OffsetRotation.RotateVector(Transform.TransformVectorNoScale(AbsBoneAimAxis));
		}
	}
}

FString FTGORRigUnit_DeformOrient::ProcessPinLabelForInjection(const FString& InLabel) const
{
	FString Formula;
	return FString::Printf(TEXT("%s: TODO"), *InLabel);
}



////////////////////////////////////////////////////////////////////////////////////////////////////

FTGORRigUnit_DeformChain_Execute()
{
	DECLARE_SCOPE_HIERARCHICAL_COUNTER_RIGUNIT()
	FRigHierarchyContainer* Hierarchy = ExecuteContext.Hierarchy;

	if (Context.State == EControlRigState::Init)
	{
		DEFORM_RESET;
		DeformCache.Reset();
		AbsTipCache.Reset();
		ObjBaseCache.Reset();
	}
	else
	{
		if (!DeformCache.UpdateCache(DeformBaseKey, Hierarchy))
		{
			UE_CONTROLRIG_RIGUNIT_REPORT_WARNING(TEXT("Deform '%s' is not valid."), *DeformBaseKey.ToString());
		}
		else if (!AbsTipCache.UpdateCache(AbsTipKey, Hierarchy))
		{
			UE_CONTROLRIG_RIGUNIT_REPORT_WARNING(TEXT("Tip '%s' is not valid."), *AbsTipKey.ToString());
		}
		else if (!ObjBaseCache.UpdateCache(ObjBaseKey, Hierarchy))
		{
			UE_CONTROLRIG_RIGUNIT_REPORT_WARNING(TEXT("Objective '%s' is not valid."), *ObjBaseKey.ToString());
		}
		else DEFORM_UPDATE
		{
			const FTransform BaseTransform = Hierarchy->GetGlobalTransform(AbsBaseCache);
			const FTransform TipTransform = Hierarchy->GetGlobalTransform(AbsTipCache);

			Direction = BaseTransform.TransformVectorNoScale(AbsBoneUpAxis);

			const FVector TargetDelta = (TipTransform.GetLocation() - BaseTransform.GetLocation());
			const float TargetNorm = TargetDelta.Size();

			if (FMath::IsNearlyZero(TargetNorm))
			{
				UE_CONTROLRIG_RIGUNIT_REPORT_WARNING(TEXT("Length is null."));
			}
			else
			{
				FVector TargetDirection = TargetDelta / TargetNorm;

				const FVector InitialFirst = Hierarchy->GetInitialGlobalTransform(AbsBaseCache).GetLocation();
				const FVector InitialLast = Hierarchy->GetInitialGlobalTransform(AbsTipCache).GetLocation();
				const float InitialLength = (InitialLast - InitialFirst).Size();

				// Extract source location from deform skeleton
				const FTransform Deform = Hierarchy->GetGlobalTransform(DeformCache);
				if (DebugSettings.bEnabled)
				{
					Context.DrawInterface->DrawPoint(FTransform::Identity, Deform.GetLocation(), DebugSettings.Scale * 1.5f, FLinearColor::Red);
				}

				// Set tip transform
				CurrentLength = TargetNorm / InitialLength * ChainMaxLength;
				FTransform Transform = Hierarchy->GetGlobalTransform(ObjBaseCache);
				Transform.SetLocation(Deform.GetLocation() + TargetDirection * CurrentLength);
				Transform.SetRotation(TipTransform.GetRotation() * OffsetRotation.Quaternion());
				Hierarchy->SetGlobalTransform(ObjBaseCache, Transform, PropagateToChildren != ETGOR_Propagation::Off);
			}
		}
	}
}

FString FTGORRigUnit_DeformChain::ProcessPinLabelForInjection(const FString& InLabel) const
{
	FString Formula;
	return FString::Printf(TEXT("%s: TODO"), *InLabel);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

#define PIVOT_RESET DEFORM_RESET ObjBaseCache.Reset();
#define PIVOT_UPDATE DEFORM_UPDATE \
	if (!ObjBaseCache.UpdateCache(ObjBaseKey, Context.Hierarchy)) \
	{ UE_CONTROLRIG_RIGUNIT_REPORT_WARNING(TEXT("Objective '%s' is not valid."), *ObjBaseKey.ToString()); } else

////////////////////////////////////////////////////////////////////////////////////////////////////

FTGORRigUnit_DeformTranslate_Execute()
{
	DECLARE_SCOPE_HIERARCHICAL_COUNTER_RIGUNIT()
	FRigHierarchyContainer* Hierarchy = ExecuteContext.Hierarchy;

	if (Context.State == EControlRigState::Init)
	{
		PIVOT_RESET;
		DeformCache.Reset();
		AbsPivotCache.Reset();
		AbsTargetCache.Reset();
	}
	else
	{
		if (!DeformCache.UpdateCache(DeformKey, Hierarchy))
		{
			UE_CONTROLRIG_RIGUNIT_REPORT_WARNING(TEXT("Deform '%s' is not valid."), *DeformKey.ToString());
		}
		else if (!ObjBaseCache.UpdateCache(ObjBaseKey, Hierarchy))
		{
			UE_CONTROLRIG_RIGUNIT_REPORT_WARNING(TEXT("Objective '%s' is not valid."), *ObjBaseKey.ToString());
		}
		else
		{
			const FTransform InitialFirst = Hierarchy->GetInitialGlobalTransform(DeformCache);
			const FTransform First = Hierarchy->GetGlobalTransform(DeformCache);
			
			if (!AbsPivotCache.UpdateCache(AbsPivotKey, Hierarchy))
			{
				UE_CONTROLRIG_RIGUNIT_REPORT_WARNING(TEXT("Pivot '%s' is not valid."), *AbsPivotKey.ToString());
			}
			else if (!AbsTargetCache.UpdateCache(AbsTargetKey, Hierarchy))
			{
				UE_CONTROLRIG_RIGUNIT_REPORT_WARNING(TEXT("Target '%s' is not valid."), *AbsTargetKey.ToString());
			}
			else PIVOT_UPDATE
			{
				const FTransform BaseTransform = Hierarchy->GetGlobalTransform(AbsBaseCache);
				AbsPivot = Hierarchy->GetGlobalTransform(AbsPivotCache);

				const FVector TargetDelta = AbsPivot.GetLocation() - BaseTransform.GetLocation();
				const float TargetNorm = TargetDelta.Size();
				if (!FMath::IsNearlyZero(TargetNorm))
				{
					const FVector InitialBase = Hierarchy->GetInitialGlobalTransform(AbsBaseCache).GetLocation();
					const FVector InitialPivot = Hierarchy->GetInitialGlobalTransform(AbsPivotCache).GetLocation();
					const float InitialLength = (InitialPivot - InitialBase).Size();

					if (!FMath::IsNearlyZero(InitialLength))
					{
						const float AbsDeformTranslate = ChainMaxLength / InitialLength;

						const FVector TargetDirection = TargetDelta / TargetNorm;
						const FVector DeformBaseLocation = FMath::Lerp(First.GetLocation(), InitialFirst.GetLocation(), PivotInitialLerp);
						const FVector PivotLocation = DeformBaseLocation + TargetDirection * TargetNorm * AbsDeformTranslate;

						const FTransform TargetTransform = Hierarchy->GetGlobalTransform(AbsTargetCache);
						DeformPivot = FTransform(AbsPivot.GetRotation() * OffsetRotation.Quaternion(), PivotLocation, FVector::OneVector);

						FTransform Transform = Hierarchy->GetGlobalTransform(ObjBaseCache);
						FTGORRigUnit_ConvertSpace::ConvertSpace(Transform, TargetTransform, AbsPivot, DeformPivot, AbsDeformTranslate);
						Hierarchy->SetGlobalTransform(ObjBaseCache, Transform, PropagateToChildren != ETGOR_Propagation::Off);
					}
				}
			}
		}
	}
}

FString FTGORRigUnit_DeformTranslate::ProcessPinLabelForInjection(const FString& InLabel) const
{
	FString Formula;
	return FString::Printf(TEXT("%s: TODO"), *InLabel);
}



////////////////////////////////////////////////////////////////////////////////////////////////////

FTGORRigUnit_DeformInitial_Execute()
{
	DECLARE_SCOPE_HIERARCHICAL_COUNTER_RIGUNIT()
	FRigHierarchyContainer* Hierarchy = ExecuteContext.Hierarchy;

	if (Context.State == EControlRigState::Init)
	{
		PIVOT_RESET;
		DeformBaseCache.Reset();
		AbsRefCache.Reset();
		DeformRefCache.Reset();
	}
	else
	{
		if (!DeformBaseCache.UpdateCache(DeformBaseKey, Hierarchy))
		{
			UE_CONTROLRIG_RIGUNIT_REPORT_WARNING(TEXT("Deform '%s' is not valid."), *DeformBaseKey.ToString());
		}
		else if (!AbsRefCache.UpdateCache(AbsRefKey, Hierarchy))
		{
			UE_CONTROLRIG_RIGUNIT_REPORT_WARNING(TEXT("Abs reference '%s' is not valid."), *AbsRefKey.ToString());
		}
		else if (!DeformRefCache.UpdateCache(DeformRefKey, Hierarchy))
		{
			UE_CONTROLRIG_RIGUNIT_REPORT_WARNING(TEXT("Deform reference '%s' is not valid."), *DeformRefKey.ToString());
		}
		else PIVOT_UPDATE
		{
			const FVector InitAbsRefLocation = Hierarchy->GetInitialGlobalTransform(AbsRefCache).GetLocation();
			AbsPivot = Hierarchy->GetInitialGlobalTransform(AbsBaseCache);

			const FVector InitDeformRefLocation = Hierarchy->GetInitialGlobalTransform(DeformRefCache).GetLocation();
			DeformPivot = Hierarchy->GetInitialGlobalTransform(DeformBaseCache);
			DeformPivot.SetRotation(AbsPivot.GetRotation() * OffsetRotation.Quaternion());
			
			const float AbsDeformTranslate = FMath::Sqrt((InitDeformRefLocation - DeformPivot.GetLocation()).SizeSquared() / (InitAbsRefLocation - AbsPivot.GetLocation()).SizeSquared());

			const FTransform AbsTransform = Hierarchy->GetGlobalTransform(AbsBaseCache);

			FTransform Transform = Hierarchy->GetGlobalTransform(ObjBaseCache);
			FTGORRigUnit_ConvertSpace::ConvertSpace(Transform, AbsTransform, AbsPivot, DeformPivot, AbsDeformTranslate);
			Hierarchy->SetGlobalTransform(ObjBaseCache, Transform, PropagateToChildren != ETGOR_Propagation::Off);
		}
	}
}

FString FTGORRigUnit_DeformInitial::ProcessPinLabelForInjection(const FString& InLabel) const
{
	FString Formula;
	return FString::Printf(TEXT("%s: TODO"), *InLabel);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

FTGORRigUnit_DeformRoot_Execute()
{
	DECLARE_SCOPE_HIERARCHICAL_COUNTER_RIGUNIT()
	FRigHierarchyContainer* Hierarchy = ExecuteContext.Hierarchy;

	if (Context.State == EControlRigState::Init)
	{
		PIVOT_RESET;
		AbsRootCache.Reset();
		DeformRootCache.Reset();
	}
	else
	{
		if (!DeformBaseCache.UpdateCache(DeformBaseKey, Hierarchy))
		{
			UE_CONTROLRIG_RIGUNIT_REPORT_WARNING(TEXT("Deform base '%s' is not valid."), *DeformBaseKey.ToString());
		}
		else PIVOT_UPDATE
		{
			// Compute source root transform
			AbsPivot = Hierarchy->GetGlobalTransform(AbsBaseCache);

			const FTransform InitialAbsBase = Hierarchy->GetInitialGlobalTransform(AbsBaseCache);

			FTransform InitialAbsRoot = FTransform::Identity;
			if (AbsRootCache.UpdateCache(AbsRootKey, Hierarchy))
			{
				InitialAbsRoot = Hierarchy->GetInitialGlobalTransform(AbsRootCache);
			}

			const float AbsHeight = (InitialAbsBase.GetLocation() - InitialAbsRoot.GetLocation()).Size();
			if (!FMath::IsNearlyZero(AbsHeight))
			{
				const FTransform InitialDeformBase = Hierarchy->GetInitialGlobalTransform(DeformBaseCache);

				FTransform InitialDeformRoot = FTransform::Identity;
				if (DeformRootCache.UpdateCache(DeformRootKey, Hierarchy))
				{
					InitialDeformRoot = Hierarchy->GetInitialGlobalTransform(DeformRootCache);
				}

				const float DeformHeight = (InitialDeformBase.GetLocation() - InitialDeformRoot.GetLocation()).Size();
				const FTransform AbsBase = Hierarchy->GetGlobalTransform(AbsBaseCache);
				DeformPivot.SetRotation(AbsPivot.GetRotation() * OffsetRotation.Quaternion());
				DeformPivot.SetLocation(InitialDeformRoot.GetLocation() + (AbsBase.GetLocation() - InitialAbsRoot.GetLocation()) * (DeformHeight / AbsHeight * RootHeight));
				DeformPivot.SetScale3D(FVector::OneVector);

				// TODO: Possibly include offset computation
				Hierarchy->SetGlobalTransform(ObjBaseCache, DeformPivot, PropagateToChildren != ETGOR_Propagation::Off);
			}
		}
	}
}

FString FTGORRigUnit_DeformRoot::ProcessPinLabelForInjection(const FString& InLabel) const
{
	FString Formula;
	return FString::Printf(TEXT("%s: TODO"), *InLabel);
}
