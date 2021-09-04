// The Gateway of Realities: Planes of Existence.

#include "TGORRigUnit_IK.h"

#include "RealitiesUtility/Structures/TGOR_Matrix3x3.h"
#include "RealitiesUtility/Utility/TGOR_Math.h"
#include "Units/RigUnitContext.h"

////////////////////////////////////////////////////////////////////////////////////////////////////

FTGORRigUnit_SplineChainIK_Execute()
{
	DECLARE_SCOPE_HIERARCHICAL_COUNTER_RIGUNIT()
	FRigHierarchyContainer* Hierarchy = ExecuteContext.Hierarchy;

	if (Context.State == EControlRigState::Init)
	{
	}
	else
	{
		const int32 ChainCount = Chain.Num();
		if (ChainCount < 2)
		{
			UE_CONTROLRIG_RIGUNIT_REPORT_WARNING(TEXT("Chain has to have length at least 2."));
		}
		else
		{
			const FVector EEForwardTarget = Objective.TransformVectorNoScale(ObjectiveForwardAxis);
			const FVector EEUpTarget = Objective.TransformVectorNoScale(ObjectiveUpAxis);

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
				const float InvTotalDistance = 1.0f / TotalDistance;

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

					// Compute transform and prepare next iteration
					FTransform Local = Hierarchy->GetLocalTransform(Chain[Index]);
					FTGORRigUnit_Propagate::PropagateChainTorwards(Chain[Index-1], Chain[Index], NextLocation, Hierarchy, PropagateToChildren == ETGOR_Propagation::All);
				}
				
				// Set last member of chain
				FTransform EE = Hierarchy->GetGlobalTransform(Chain.Last());
				EE.SetRotation(FTGORRigUnit_RotateToward::ComputeHeadingRotation(EffectorForwardAxis, EEForwardTarget, EffectorUpAxis, EEUpTarget));
				EE.SetScale3D(Objective.GetScale3D());
				EE.SetLocation(Objective.GetLocation());
				Hierarchy->SetGlobalTransform(Chain.Last(), EE, PropagateToChildren != ETGOR_Propagation::Off);
			}
		}
	}
}

FString FTGORRigUnit_SplineChainIK::ProcessPinLabelForInjection(const FString& InLabel) const
{
	FString Formula;
	return FString::Printf(TEXT("%s: TODO"), *InLabel);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

bool ComputeTriangle(float a, float b, float s, float& h, float& x)
{
	if (a + b < s)
	{
		x = s / 2;
		h = 0.0f;
		return false;
	}

	const float aa = a * a;
	const float bb = b * b;
	const float ss = s * s;
	const float hh = (2.0f * (aa * bb + aa * ss + bb * ss) - (aa * aa + bb * bb  + ss * ss)) / (4.0f * ss);

	if (hh < 0.0f)
	{
		x = s / 2;
		h = 0.0f;
		return false;
	}

	x = FMath::Sqrt(aa - hh);
	h = FMath::Sqrt(hh);
	return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

FTGORRigUnit_HingeIK_Execute()
{
	DECLARE_SCOPE_HIERARCHICAL_COUNTER_RIGUNIT()
	FRigHierarchyContainer* Hierarchy = ExecuteContext.Hierarchy;

	if (Context.State == EControlRigState::Init)
	{
	}
	else
	{
		if (Chain.Num() < 3)
		{
			UE_CONTROLRIG_RIGUNIT_REPORT_WARNING(TEXT("Chain has to have length at least 3."));
		}
		else
		{
			// Objective properties
			const FVector EEForwardTarget = Objective.TransformVectorNoScale(ObjectiveForwardAxis);
			const FVector EEUpTarget = Objective.TransformVectorNoScale(ObjectiveUpAxis);

			// Compute initial leg properties
			const FTransform InitialUpper = Hierarchy->GetInitialGlobalTransform(Chain[0]);
			const FTransform InitialLower = Hierarchy->GetInitialGlobalTransform(Chain[1]);
			const FTransform InitialEE = Hierarchy->GetInitialGlobalTransform(Chain[2]);

			FVector2D InitialLengths;
			InitialLengths.X = (InitialUpper.GetLocation() - InitialLower.GetLocation()).Size() * Customisation.X;
			InitialLengths.Y = (InitialLower.GetLocation() - InitialEE.GetLocation()).Size() * Customisation.Y;

			// Rescale to fit length
			float ChainMaxLength = InitialLengths.X + InitialLengths.Y;
			if (!FMath::IsNearlyZero(ChainMaxLength))
			{
				const float TargetRatio = TargetLength / ChainMaxLength;

				ChainMaxLength *= TargetRatio;
				InitialLengths *= TargetRatio;
			}

			// Compute objective deltas
			FTransform Upper = Hierarchy->GetGlobalTransform(Chain[0]);
			FTransform Lower = Hierarchy->GetGlobalTransform(Chain[1]);
			FTransform EE = Hierarchy->GetGlobalTransform(Chain[2]);
			const FVector ObjectiveDelta = Objective.GetLocation() - Upper.GetLocation();
			const float ObjectiveNorm = ObjectiveDelta.Size();
			const FVector ObjectiveNormal = ObjectiveDelta / ObjectiveNorm;

			const FVector HingeDirection = UTGOR_Math::Normalize(FVector::VectorPlaneProject(Direction, ObjectiveNormal));

			// Compute pivot location
			float Kath = 0.0f;
			float Height = 0.0f;
			ComputeTriangle(InitialLengths.X, InitialLengths.Y, ObjectiveNorm, Height, Kath);

			const FVector HingeLocation = Upper.GetLocation() + ObjectiveNormal * Kath + HingeDirection * Height;

			if (DebugSettings.bEnabled)
			{
				const FVector Location = Upper.GetLocation() + ObjectiveNormal * Kath;
				Context.DrawInterface->DrawLine(FTransform::Identity, Location, HingeLocation, FLinearColor::Yellow, DebugSettings.Scale);
				Context.DrawInterface->DrawLine(FTransform::Identity, Objective.GetLocation(), Upper.GetLocation(), FLinearColor::Yellow, DebugSettings.Scale * 0.5f);
			}

			// Transform to propagate along the hinge
			FTGORRigUnit_Propagate::PropagateChainTorwards(Chain[0], Chain[1], HingeLocation, Hierarchy, PropagateToChildren == ETGOR_Propagation::All);
			FTGORRigUnit_Propagate::PropagateChainTorwards(Chain[1], Chain[2], Objective.GetLocation(), Hierarchy, PropagateToChildren == ETGOR_Propagation::All);

			// Set foot transform
			EE.SetRotation(FTGORRigUnit_RotateToward::ComputeHeadingRotation(EffectorForwardAxis, EEForwardTarget, EffectorUpAxis, EEUpTarget));
			EE.SetScale3D(InitialEE.GetScale3D());
			EE.SetLocation(Objective.GetLocation());
			Hierarchy->SetGlobalTransform(Chain[2], EE, PropagateToChildren != ETGOR_Propagation::Off);

			if (DebugSettings.bEnabled)
			{
				Context.DrawInterface->DrawLine(FTransform::Identity, Objective.GetLocation(), Objective.TransformPositionNoScale(EEForwardTarget * 20.0f), FLinearColor::Red, DebugSettings.Scale * 0.5f);
				Context.DrawInterface->DrawLine(FTransform::Identity, Objective.GetLocation(), Objective.TransformPositionNoScale(EEForwardTarget * 20.0f), FLinearColor::Blue, DebugSettings.Scale * 0.5f);
				Context.DrawInterface->DrawLine(FTransform::Identity, Objective.GetLocation(), Objective.TransformPositionNoScale(EffectorForwardAxis * 20.0f), FLinearColor::Black, DebugSettings.Scale * 0.5f);
				Context.DrawInterface->DrawLine(FTransform::Identity, Objective.GetLocation(), Objective.TransformPositionNoScale(EffectorUpAxis * 20.0f), FLinearColor::White, DebugSettings.Scale * 0.5f);
			}
		}
	}
}

FString FTGORRigUnit_HingeIK::ProcessPinLabelForInjection(const FString& InLabel) const
{
	FString Formula;
	return FString::Printf(TEXT("%s: TODO"), *InLabel);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

FTGORRigUnit_DigitigradeIK_Execute()
{
	DECLARE_SCOPE_HIERARCHICAL_COUNTER_RIGUNIT()
	FRigHierarchyContainer* Hierarchy = ExecuteContext.Hierarchy;

	if (Context.State == EControlRigState::Init)
	{
	}
	else
	{
		if (Chain.Num() < 4)
		{
			UE_CONTROLRIG_RIGUNIT_REPORT_WARNING(TEXT("Chain has to have length at least 4."));
		}
		else
		{
			// Objective properties
			const FVector EEForwardTarget = Objective.TransformVectorNoScale(ObjectiveForwardAxis);
			const FVector EEUpTarget = Objective.TransformVectorNoScale(ObjectiveUpAxis);

			// Compute initial leg properties
			const FTransform InitialUpperLeg = Hierarchy->GetInitialGlobalTransform(Chain[0]);
			const FTransform InitialLowerLeg = Hierarchy->GetInitialGlobalTransform(Chain[1]);
			const FTransform InitialAnkle = Hierarchy->GetInitialGlobalTransform(Chain[2]);
			const FTransform InitialFoot = Hierarchy->GetInitialGlobalTransform(Chain[3]);

			FVector Lengths;
			Lengths.X = (InitialUpperLeg.GetLocation() - InitialLowerLeg.GetLocation()).Size() * Customisation.X;
			Lengths.Y = (InitialLowerLeg.GetLocation() - InitialAnkle.GetLocation()).Size() * Customisation.Y;
			Lengths.Z = (InitialAnkle.GetLocation() - InitialFoot.GetLocation()).Size() * Customisation.Z;

			// Rescale to fit length
			float ChainMaxLength = Lengths.X + Lengths.Y + Lengths.Z * Digitness;
			if (!FMath::IsNearlyZero(ChainMaxLength))
			{
				const float TargetRatio = TargetLength / ChainMaxLength;

				ChainMaxLength *= TargetRatio;
				Lengths *= TargetRatio;
			}

			// Compute objective deltas
			FTransform UpperLeg = Hierarchy->GetGlobalTransform(Chain[0]);
			const FVector ObjectiveDelta = Objective.GetLocation() - UpperLeg.GetLocation();
			const float ObjectiveNorm = ObjectiveDelta.Size();
			const FVector ObjectiveNormal = ObjectiveDelta / ObjectiveNorm;

			// Relative position of lower leg pivot along objective axis
			const float LowerRatio = Lengths.Z / (Lengths.X + Lengths.Z);
			

			// Use average direction for ankle direction
			const FVector AnkleDirection = UTGOR_Math::Normalize(FVector::VectorPlaneProject(Direction + EEForwardTarget, ObjectiveNormal));
			const FVector PlantiAnkle = Objective.GetLocation() - EEForwardTarget * Lengths.Z * FootPivot;

			const FVector AnkleDelta = PlantiAnkle - UpperLeg.GetLocation();
			const float AnkleNorm = AnkleDelta.Size();
			const FVector AnkleNormal = AnkleDelta / AnkleNorm;

			if (DebugSettings.bEnabled)
			{
				const FVector Location = Objective.GetLocation() - ObjectiveDelta * LowerRatio;
				Context.DrawInterface->DrawLine(FTransform::Identity, Location, Location + AnkleDirection * 20.0f, FLinearColor::Yellow, DebugSettings.Scale);
				Context.DrawInterface->DrawLine(FTransform::Identity, Objective.GetLocation(), UpperLeg.GetLocation(), FLinearColor::Yellow, DebugSettings.Scale * 0.5f);
			}


			// Compute ankle position
			float LowerLength = ObjectiveNorm * LowerRatio;

			float LowerKath = 0.0f;
			float LowerHeight = 0.0f;
			ComputeTriangle(Lengths.Z, Lengths.Y * LowerPivot, LowerLength, LowerHeight, LowerKath);

			const FVector DigiAnkle = Objective.GetLocation() - ObjectiveNormal * LowerKath - AnkleDirection * LowerHeight;
			const FVector AnkleLocation = FMath::Lerp(PlantiAnkle, DigiAnkle, Digitness);


			if (DebugSettings.bEnabled)
			{
				Context.DrawInterface->DrawPoint(FTransform::Identity, DigiAnkle, DebugSettings.Scale * 1.5f, FLinearColor::Red);
				Context.DrawInterface->DrawPoint(FTransform::Identity, AnkleLocation, DebugSettings.Scale * 3.0f, FLinearColor::Blue);
			}

			// Compute foot location

			const float HeelHeight = (AnkleLocation - Objective.GetLocation()) | EEUpTarget;
			const float SoleLength = FMath::Sqrt((Lengths.Z * Lengths.Z) - (HeelHeight * HeelHeight));
			const FVector Heel = AnkleLocation - EEUpTarget * HeelHeight;
			const FVector FootLocation = Heel + EEForwardTarget * SoleLength;

			if (DebugSettings.bEnabled)
			{
				Context.DrawInterface->DrawPoint(FTransform::Identity, Heel, DebugSettings.Scale * 3.0f, FLinearColor::Blue);
				Context.DrawInterface->DrawPoint(FTransform::Identity, FootLocation, DebugSettings.Scale * 3.0f, FLinearColor::Green);
			}


			// Compute knee location
			float UpperKath = 0.0f;
			float UpperHeight = 0.0f;
			const FVector UpperDelta = AnkleLocation - UpperLeg.GetLocation();
			const float UpperNorm = UpperDelta.Size();
			const FVector UpperNormal = UpperDelta / UpperNorm;
			ComputeTriangle(Lengths.X, Lengths.Y, UpperNorm, UpperHeight, UpperKath);

			const FVector UpperDirection = UTGOR_Math::Normalize(FVector::VectorPlaneProject(Direction, UpperNormal));
			const FVector LowerLegLocation = UpperLeg.GetLocation() + UpperNormal * UpperKath + UpperDirection * UpperHeight;

			if (DebugSettings.bEnabled)
			{
				Context.DrawInterface->DrawPoint(FTransform::Identity, LowerLegLocation, DebugSettings.Scale * 3.0f, FLinearColor::White);
			}

			// Transform to propagate along the leg
			FTGORRigUnit_Propagate::PropagateChainTorwards(Chain[0], Chain[1], LowerLegLocation, Hierarchy, PropagateToChildren == ETGOR_Propagation::All);
			FTGORRigUnit_Propagate::PropagateChainTorwards(Chain[1], Chain[2], AnkleLocation, Hierarchy, PropagateToChildren == ETGOR_Propagation::All);
			FTGORRigUnit_Propagate::PropagateChainTorwards(Chain[2], Chain[3], FootLocation, Hierarchy, PropagateToChildren == ETGOR_Propagation::All);

			// Set foot transform
			FTransform Foot;
			Foot.SetScale3D(Hierarchy->GetInitialGlobalTransform(Chain[3]).GetScale3D());
			Foot.SetRotation(FTGORRigUnit_RotateToward::ComputeHeadingRotation(EffectorForwardAxis, EEForwardTarget, EffectorUpAxis, EEUpTarget));
			Foot.SetLocation(FootLocation);
			Hierarchy->SetGlobalTransform(Chain[3], Foot, PropagateToChildren != ETGOR_Propagation::Off);

			if (DebugSettings.bEnabled)
			{
				Context.DrawInterface->DrawLine(FTransform::Identity, FootLocation, FootLocation + EEForwardTarget * 20.0f, FLinearColor::Red, DebugSettings.Scale * 0.5f);
				Context.DrawInterface->DrawLine(FTransform::Identity, FootLocation, FootLocation + EEUpTarget * 20.0f, FLinearColor::Blue, DebugSettings.Scale * 0.5f);
				Context.DrawInterface->DrawLine(FTransform::Identity, FootLocation, FootLocation + Foot.TransformVectorNoScale(EffectorForwardAxis) * 20.0f, FLinearColor::Black, DebugSettings.Scale * 0.5f);
				Context.DrawInterface->DrawLine(FTransform::Identity, FootLocation, FootLocation + Foot.TransformVectorNoScale(EffectorUpAxis) * 20.0f, FLinearColor::White, DebugSettings.Scale * 0.5f);
			}
		}
	}
}

FString FTGORRigUnit_DigitigradeIK::ProcessPinLabelForInjection(const FString& InLabel) const
{
	FString Formula;
	return FString::Printf(TEXT("%s: TODO"), *InLabel);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

FTGORRigUnit_ClavicleIK_Execute()
{
	DECLARE_SCOPE_HIERARCHICAL_COUNTER_RIGUNIT()
	FRigHierarchyContainer* Hierarchy = ExecuteContext.Hierarchy;

	if (Context.State == EControlRigState::Init)
	{
	}
	else
	{
		if (Chain.Num() < 2)
		{
			UE_CONTROLRIG_RIGUNIT_REPORT_WARNING(TEXT("Chain has to have length at least 2."));
		}
		else if (FMath::IsNearlyZero(MaxChainLength))
		{
			UE_CONTROLRIG_RIGUNIT_REPORT_WARNING(TEXT("MaxChainLength mustn't be 0."));
		}
		else
		{
			FTransform Base = Hierarchy->GetGlobalTransform(Chain[0]);

			const FVector ClavicleDelta = (Clavicle - Base.GetLocation());
			const float ClavicleNorm = ClavicleDelta.Size();
			if (!FMath::IsNearlyZero(ClavicleNorm))
			{
				const FVector ClavicleNormal = ClavicleDelta / ClavicleNorm;


				const FVector ObjectiveDelta = (Objective.GetLocation() - Clavicle);
				const float ObjectiveNorm = ObjectiveDelta.Size();
				if (!FMath::IsNearlyZero(ObjectiveNorm))
				{
					const FVector ObjectiveNormal = ObjectiveDelta / ObjectiveNorm;

					FQuat Rotation = FQuat::Identity;

					const float Intensity = 1.0f - (ObjectiveNormal | ClavicleNormal);
					if (!FMath::IsNearlyZero(Intensity))
					{
						const float Ratio = 1.0f - FMath::Exp(-(ObjectiveNorm / MaxChainLength) * Intensity * Strength);
						const FVector Axis = (ClavicleNormal ^ (ObjectiveNormal + ClavicleNormal)).GetSafeNormal();
						Rotation = FQuat(Axis, AngleLimit * Ratio);
					}


					Base.SetRotation(Rotation * Base.GetRotation());
					Hierarchy->SetGlobalTransform(Chain[0], Base, PropagateToChildren == ETGOR_Propagation::All);

					FTransform Tip = Hierarchy->GetGlobalTransform(Chain[1]);
					Tip.SetRotation(Rotation * Tip.GetRotation());
					Tip.SetLocation(Base.GetLocation() + Rotation * ClavicleDelta);
					Hierarchy->SetGlobalTransform(Chain[1], Tip, PropagateToChildren == ETGOR_Propagation::OnlyLast);
				}
			}
		}
	}
}

FString FTGORRigUnit_ClavicleIK::ProcessPinLabelForInjection(const FString& InLabel) const
{
	FString Formula;
	return FString::Printf(TEXT("%s: TODO"), *InLabel);
}
