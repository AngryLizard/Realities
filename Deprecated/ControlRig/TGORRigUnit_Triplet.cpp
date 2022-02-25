// The Gateway of Realities: Planes of Existence.


#include "TGORRigUnit_Triplet.h"

#include "RealitiesUtility/Structures/TGOR_Matrix3x3.h"
#include "Units/RigUnitContext.h"

#define CONE_RESET BaseCache.Reset(); LeftCache.Reset(); RightCache.Reset();
#define CONE_UPDATE \
	if (!BaseCache.UpdateCache(BaseKey, Context.Hierarchy) || !LeftCache.UpdateCache(LeftKey, Context.Hierarchy) || !RightCache.UpdateCache(RightKey, Context.Hierarchy)) \
	 {UE_CONTROLRIG_RIGUNIT_REPORT_WARNING(TEXT("Base, left or right '%s' is not valid."), *BaseKey.ToString()); } else

////////////////////////////////////////////////////////////////////////////////////////////////////

FTGOR_TripletAnalysis::FTGOR_TripletAnalysis()
:	Normal(FVector::UpVector),
	Center(FVector::ZeroVector),
	Direction(FVector::ForwardVector),
	Tangent(FVector::RightVector),
	Base(0.0f)
{
}

void FTGORRigUnit_TripletAnalysis::ComputeTripletPartial(const FVector& Base, const FVector& Left, const FVector& Right, FTGOR_TripletAnalysis& Analysis)
{
	const FVector LeftDelta = Left - Base;
	const FVector RightDelta = Right - Base;
	Analysis.Direction = (RightDelta ^ LeftDelta).GetSafeNormal();
	Analysis.Center = (LeftDelta + RightDelta) / 2;
	Analysis.Tangent = Right - Left;
}

void FTGORRigUnit_TripletAnalysis::ComputeTripletFinish(const FVector& Base, const FVector& Normal, FTGOR_TripletAnalysis& Analysis)
{
	Analysis.Base = Analysis.Center | Normal;
	Analysis.Center = Base + Normal * Analysis.Base;
	Analysis.Tangent = FVector::VectorPlaneProject(Analysis.Tangent, Normal).GetSafeNormal();
	Analysis.Normal = Normal;

}

void FTGORRigUnit_TripletAnalysis::ComputeTripletAnalysis(const FVector& Base, const FVector& Left, const FVector& Right, const FVector& Normal, FTGOR_TripletAnalysis& Analysis)
{
	ComputeTripletPartial(Base, Left, Right, Analysis);
	ComputeTripletFinish(Base, Normal, Analysis);
}


FTGORRigUnit_TripletAnalysis_Execute()
{
	DECLARE_SCOPE_HIERARCHICAL_COUNTER_RIGUNIT()
	const FRigHierarchyContainer* Hierarchy = Context.Hierarchy;

	if (Context.State == EControlRigState::Init)
	{
		CONE_RESET;
	}
	else
	{
		CONE_UPDATE
		{
			// Cone triangle
			const FTransform Base = Hierarchy->GetGlobalTransform(BaseCache);
			const FTransform Left = Hierarchy->GetGlobalTransform(LeftCache);
			const FTransform Right = Hierarchy->GetGlobalTransform(RightCache);

			const FVector Normal = Base.TransformVectorNoScale(AimAxis);
			ComputeTripletAnalysis(Base.GetLocation(), Left.GetLocation(), Right.GetLocation(), Normal, Output);
		}
	}
}

FString FTGORRigUnit_TripletAnalysis::ProcessPinLabelForInjection(const FString& InLabel) const
{
	FString Formula;
	return FString::Printf(TEXT("%s: TODO"), *InLabel);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

FTGORRigUnit_TripletReproject_Execute()
{
	DECLARE_SCOPE_HIERARCHICAL_COUNTER_RIGUNIT()
	const FRigHierarchyContainer* Hierarchy = Context.Hierarchy;

	if (Context.State == EControlRigState::Init)
	{
		CONE_RESET;
		RefCache.Reset();
		PivotCache.Reset();
	}
	else
	{
		if (!RefCache.UpdateCache(RefKey, Hierarchy))
		{
			UE_CONTROLRIG_RIGUNIT_REPORT_WARNING(TEXT("Ref '%s' is not valid."), *RefKey.ToString());
		}
		else if (!PivotCache.UpdateCache(PivotKey, Hierarchy))
		{
			UE_CONTROLRIG_RIGUNIT_REPORT_WARNING(TEXT("Pivot '%s' is not valid."), *PivotKey.ToString());
		}
		else CONE_UPDATE
		{
			const FTransform Base = Hierarchy->GetGlobalTransform(BaseCache);
			Output = Hierarchy->GetGlobalTransform(RefCache);
			const FTransform Left = Hierarchy->GetGlobalTransform(LeftCache);
			const FTransform Right = Hierarchy->GetGlobalTransform(RightCache);

			const FTransform Pivot = Hierarchy->GetGlobalTransform(PivotCache);
			const FVector ConeCenter = Pivot.GetLocation();
			const FVector ConeNormal = Pivot.TransformVectorNoScale(Analysis.Normal);
			const FVector ConeTangent = Pivot.TransformVectorNoScale(Analysis.Tangent);

			if (DebugSettings.bEnabled)
			{
				Context.DrawInterface->DrawPoint(FTransform::Identity, ConeCenter, DebugSettings.Scale * 1.5f, FLinearColor::Red);
				Context.DrawInterface->DrawLine(FTransform::Identity, ConeCenter, ConeCenter + ConeNormal * 20.0f, FLinearColor::Blue, DebugSettings.Scale);
				Context.DrawInterface->DrawLine(FTransform::Identity, ConeCenter, ConeCenter + ConeTangent * 20.0f, FLinearColor::Green, DebugSettings.Scale);
			}

			// Compute location so we match the base distance-wise
			const FVector Project = Base.GetLocation() - Output.GetLocation();
			const float ProjectNorm = Project.Size();
			Output.SetLocation(ConeCenter - ConeNormal * (ProjectNorm + Analysis.Base));

			if (!FMath::IsNearlyZero(ProjectNorm))
			{
				// Compute tilt
				const FVector ProjectNormal = Project / ProjectNorm;
				const FQuat UpQuat = FQuat::FindBetweenNormals(ProjectNormal, ConeNormal);

				// Compute turn
				const FVector Delta = Right.GetLocation() - Left.GetLocation();
				const FVector ProjectTangent = FVector::VectorPlaneProject(Delta, ConeNormal).GetSafeNormal();
				const FQuat Rotation = FQuat::FindBetweenNormals(ProjectTangent, ConeTangent) * UpQuat;

				// Compute rotation so we match normal and tangent
				Output.SetRotation(Rotation * Output.GetRotation());


				if (DebugSettings.bEnabled)
				{
					Context.DrawInterface->DrawPoint(FTransform::Identity, Output.GetLocation(), DebugSettings.Scale * 1.5f, FLinearColor::Yellow);
					Context.DrawInterface->DrawLine(FTransform::Identity, Output.GetLocation(), Output.GetLocation() + ProjectNormal * 20.0f, FLinearColor::White, DebugSettings.Scale);
					Context.DrawInterface->DrawLine(FTransform::Identity, Output.GetLocation(), Output.GetLocation() + ProjectTangent * 20.0f, FLinearColor::Black, DebugSettings.Scale);
				}
			}
		}
	}
}

FString FTGORRigUnit_TripletReproject::ProcessPinLabelForInjection(const FString& InLabel) const
{
	FString Formula;
	return FString::Printf(TEXT("%s: TODO"), *InLabel);
}


////////////////////////////////////////////////////////////////////////////////////////////////////

FTGORRigUnit_TripletTransform_Execute()
{
	DECLARE_SCOPE_HIERARCHICAL_COUNTER_RIGUNIT()
	const FRigHierarchyContainer* Hierarchy = Context.Hierarchy;

	if (Context.State == EControlRigState::Init)
	{
		RefCache.Reset();
		PivotCache.Reset();
	}
	else
	{
		if (!RefCache.UpdateCache(RefKey, Hierarchy))
		{
			UE_CONTROLRIG_RIGUNIT_REPORT_WARNING(TEXT("Ref '%s' is not valid."), *RefKey.ToString());
		}
		else if (!PivotCache.UpdateCache(PivotKey, Hierarchy))
		{
			UE_CONTROLRIG_RIGUNIT_REPORT_WARNING(TEXT("Pivot '%s' is not valid."), *PivotKey.ToString());
		}
		else
		{
			const FVector Ref = Hierarchy->GetGlobalTransform(RefCache).GetLocation();
			const FTransform Pivot = Hierarchy->GetGlobalTransform(PivotCache);

			Output = Pivot.TransformPosition(Ref - Analysis.Center);
		}
	}
}

FString FTGORRigUnit_TripletTransform::ProcessPinLabelForInjection(const FString& InLabel) const
{
	FString Formula;
	return FString::Printf(TEXT("%s: TODO"), *InLabel);
}
