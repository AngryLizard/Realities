// The Gateway of Realities: Planes of Existence.


#include "TGORRigUnit_Objective.h"

#include "RealitiesUtility/Structures/TGOR_Matrix3x3.h"
#include "RealitiesUtility/Utility/TGOR_Math.h"
#include "Units/RigUnitContext.h"

#define OBJECTIVE_RESET ObjBaseCache.Reset()
#define OBJECTIVE_UPDATE \
	if (!ObjBaseCache.UpdateCache(ObjBaseKey, Context.Hierarchy)) \
	{ UE_CONTROLRIG_RIGUNIT_REPORT_WARNING(TEXT("Base '%s' is not valid."), *ObjBaseKey.ToString()); } else \



////////////////////////////////////////////////////////////////////////////////////////////////////

FTGORRigUnit_ObjectivePlanarProject_Execute()
{
	DECLARE_SCOPE_HIERARCHICAL_COUNTER_RIGUNIT()
	URigHierarchy* Hierarchy = ExecuteContext.Hierarchy;

	if (Context.State == EControlRigState::Init)
	{
		OBJECTIVE_RESET;
		ObjPlaneCache.Reset();
	}
	else
	{
		if (!ObjPlaneCache.UpdateCache(PlaneKey, Hierarchy))
		{
			UE_CONTROLRIG_RIGUNIT_REPORT_WARNING(TEXT("Plane '%s' is not valid."), *PlaneKey.ToString());
		}
		else OBJECTIVE_UPDATE
		{
			FTransform Objective = Hierarchy->GetGlobalTransform(ObjBaseCache);
			const FVector ObjectiveNormal = Objective.TransformVectorNoScale(ObjectiveUpAxis);

			const FTransform Plane = Hierarchy->GetGlobalTransform(ObjPlaneCache);
			const FVector PlaneNormal = Plane.TransformVectorNoScale(PlaneAxis);
			const FVector PlaneOrigin = Plane.GetLocation();

			const FVector Delta = Objective.GetLocation() - Pivot;
			const float DeltaNorm = Delta.Size();
			if (!FMath::IsNearlyZero(DeltaNorm))
			{
				const FVector DeltaNormal = Delta / DeltaNorm;

				const float Distance = ((PlaneOrigin - Pivot) | PlaneNormal) / (DeltaNormal | PlaneNormal);

				if (DebugSettings.bEnabled)
				{
					Context.DrawInterface->DrawLine(FTransform::Identity, Pivot, Pivot + DeltaNormal * Distance, FLinearColor::Yellow, DebugSettings.Scale);
				}

				const float Closeness = FMath::Max(Distance / DeltaNorm, MaxProject);
				if (!FMath::IsNearlyZero(RotationThreshold))
				{
					const float Clearance = FMath::Max((Closeness - 1.0f) / RotationThreshold, 0.0f);
					if (Clearance < 1.0f)
					{
						const FVector TargetNormal = FMath::Lerp(PlaneNormal, ObjectiveNormal, Clearance).GetSafeNormal();
						Objective.SetRotation(FQuat::FindBetweenNormals(ObjectiveNormal, TargetNormal) * Objective.GetRotation());
					}
				}

				if (Distance < DeltaNorm)
				{
					Objective.SetLocation(Pivot + Delta * Closeness);
				}

				Hierarchy->SetGlobalTransform(ObjBaseCache, Objective, PropagateToChildren != ETGOR_Propagation::Off);
			}
		}
	}
}

FString FTGORRigUnit_ObjectivePlanarProject::ProcessPinLabelForInjection(const FString& InLabel) const
{
	FString Formula;
	return FString::Printf(TEXT("%s: TODO"), *InLabel);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

FTGORRigUnit_BulgeBellCurve_Execute()
{
	DECLARE_SCOPE_HIERARCHICAL_COUNTER_RIGUNIT()
	URigHierarchy* Hierarchy = ExecuteContext.Hierarchy;

	if (Context.State == EControlRigState::Init)
	{
		ObjectCache.Reset();
		BulgeCache.Reset();
	}
	else
	{
		if (!ObjectCache.UpdateCache(ObjectKey, Hierarchy))
		{
			UE_CONTROLRIG_RIGUNIT_REPORT_WARNING(TEXT("Object '%s' is not valid."), *ObjectKey.ToString());
		}
		else if(!BulgeCache.UpdateCache(BulgeKey, Hierarchy))
		{
			UE_CONTROLRIG_RIGUNIT_REPORT_WARNING(TEXT("Bulge '%s' is not valid."), *BulgeKey.ToString());
		}
		else
		{
			const FTransform Object = Hierarchy->GetGlobalTransform(ObjectCache);
			const FTransform Bulge = Hierarchy->GetGlobalTransform(BulgeCache);

			const FVector Reference = Bulge.TransformPosition(Offset);
			if (DebugSettings.bEnabled)
			{
				Context.DrawInterface->DrawPoint(FTransform::Identity, Reference, 3.0f * DebugSettings.Scale, FLinearColor::Red);
			}

			const float Scale = 1.0f + FMath::Exp((Reference - Object.GetLocation()).SizeSquared() / -(Variance * Radius)) * Radius;

			FTransform Transform = Bulge;
			Transform.SetScale3D(Transform.GetScale3D() * FVector(FMath::Lerp(1.0f, Scale, ScaleMask.X), FMath::Lerp(1.0f, Scale, ScaleMask.Y), FMath::Lerp(1.0f, Scale, ScaleMask.Z)));
			Hierarchy->SetGlobalTransform(BulgeCache, Transform, false);
		}
	}
}

FString FTGORRigUnit_BulgeBellCurve::ProcessPinLabelForInjection(const FString& InLabel) const
{
	FString Formula;
	return FString::Printf(TEXT("%s: TODO"), *InLabel);
}
