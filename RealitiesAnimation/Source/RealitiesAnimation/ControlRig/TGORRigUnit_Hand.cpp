// The Gateway of Realities: Planes of Existence.

#include "TGORRigUnit_Hand.h"
#include "RealitiesUtility/Utility/TGOR_Math.h"
#include "RealitiesUtility/Utility/TGOR_Error.h"
#include "Units/RigUnitContext.h"


////////////////////////////////////////////////////////////////////////////////////////////////////

FTGORRigUnit_FingerTransform_Execute()
{
	DECLARE_SCOPE_HIERARCHICAL_COUNTER_RIGUNIT()
	URigHierarchy* Hierarchy = ExecuteContext.Hierarchy;

	if (Context.State == EControlRigState::Init)
	{
		BaseCache.Reset();
		FingerCache.Reset();
		return;
	}

	if (Context.State == EControlRigState::Update)
	{
		if (!BaseCache.UpdateCache(BaseKey, Hierarchy))
		{
			UE_CONTROLRIG_RIGUNIT_REPORT_WARNING(TEXT("Base key '%s' is not valid."), *BaseKey.ToString());
		}
		else if (!FingerCache.UpdateCache(FingerKey, Hierarchy))
		{
			UE_CONTROLRIG_RIGUNIT_REPORT_WARNING(TEXT("Finger key '%s' is not valid."), *FingerKey.ToString());
		}
		else
		{
			FTransform BaseTransform = Hierarchy->GetLocalTransform(BaseCache);
			const float BaseSpreadRadians = FMath::DegreesToRadians(BaseSpreadRange.X + Spread * (BaseSpreadRange.Y - BaseSpreadRange.X));
			BaseTransform.SetRotation(FQuat(BaseSpreadAxis, BaseSpreadRadians) * BaseTransform.GetRotation());
			Hierarchy->SetLocalTransform(BaseCache, BaseTransform, false, true);

			FTransform FingerTransform = Hierarchy->GetLocalTransform(FingerCache);
			const float FingerSpreadRadians = FMath::DegreesToRadians(FingerSpreadRange.X + Spread * (FingerSpreadRange.Y - FingerSpreadRange.X));
			FingerTransform.SetRotation(FQuat(FingerSpreadAxis, FingerSpreadRadians) * FingerTransform.GetRotation());
			Hierarchy->SetLocalTransform(FingerCache, FingerTransform, false, true);

			if (DebugSettings.bEnabled)
			{
				const FTransform BaseWorldTransform = Hierarchy->GetGlobalTransform(BaseCache);
				const FVector BaseWorldAxis = BaseWorldTransform.TransformVectorNoScale(BaseSpreadAxis);
				Context.DrawInterface->DrawLine(FTransform::Identity, BaseWorldTransform.GetLocation() - BaseWorldAxis * 10.0f, BaseWorldTransform.GetLocation() + BaseWorldAxis * 10.0f, FLinearColor::Red, DebugSettings.Scale * 0.2f);

				const FTransform FingerWorldTransform = Hierarchy->GetGlobalTransform(FingerCache);
				const FVector FingerWorldAxis = FingerWorldTransform.TransformVectorNoScale(FingerSpreadAxis);
				Context.DrawInterface->DrawLine(FTransform::Identity, FingerWorldTransform.GetLocation() - FingerWorldAxis * 10.0f, FingerWorldTransform.GetLocation() + FingerWorldAxis * 10.0f, FLinearColor::Blue, DebugSettings.Scale * 0.2f);
			}

			Angle = GrabRange.X + Grab * (GrabRange.Y - GrabRange.X);
		}
	}
}