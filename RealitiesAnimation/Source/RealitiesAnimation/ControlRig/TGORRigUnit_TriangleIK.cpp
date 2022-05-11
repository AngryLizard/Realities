// The Gateway of Realities: Planes of Existence.

#include "TGORRigUnit_TriangleIK.h"

#include "RealitiesUtility/Structures/TGOR_Matrix3x3.h"
#include "RealitiesUtility/Utility/TGOR_Math.h"
#include "Units/RigUnitContext.h"

////////////////////////////////////////////////////////////////////////////////////////////////////

// Given two triangle sides a,b,s, return length and distance along s of the triangle altitude/apex
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
	URigHierarchy* Hierarchy = ExecuteContext.Hierarchy;

	if (Context.State == EControlRigState::Init)
	{
		return;
	}

	if (Context.State == EControlRigState::Update)
	{
		if (Chain.Num() < 3)
		{
			UE_CONTROLRIG_RIGUNIT_REPORT_WARNING(TEXT("Chain has to have length at least 3."));
		}
		else
		{
			// Objective properties
			const FVector EEForwardTarget = Objective.TransformVectorNoScale(ObjectiveSettings.ForwardAxis);
			const FVector EEUpTarget = Objective.TransformVectorNoScale(ObjectiveSettings.UpAxis);

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
			FTGORRigUnit_Propagate::PropagateChainTowardsFixed(Chain[0], Chain[1], HingeLocation, Hierarchy, PropagateToChildren == ETGOR_Propagation::All);
			FTGORRigUnit_Propagate::PropagateChainTowardsFixed(Chain[1], Chain[2], Objective.GetLocation(), Hierarchy, PropagateToChildren == ETGOR_Propagation::All);

			// Set foot transform
			EE.SetRotation(FTGORRigUnit_RotateToward::ComputeHeadingRotation(ObjectiveSettings.EffectorForwardAxis, EEForwardTarget, ObjectiveSettings.EffectorUpAxis, EEUpTarget));
			EE.SetScale3D(InitialEE.GetScale3D());
			EE.SetLocation(Objective.GetLocation());
			Hierarchy->SetGlobalTransform(Chain[2], EE, false, PropagateToChildren != ETGOR_Propagation::Off);

			if (DebugSettings.bEnabled)
			{
				Context.DrawInterface->DrawLine(FTransform::Identity, Objective.GetLocation(), Objective.TransformPositionNoScale(EEForwardTarget * 20.0f), FLinearColor::Red, DebugSettings.Scale * 0.5f);
				Context.DrawInterface->DrawLine(FTransform::Identity, Objective.GetLocation(), Objective.TransformPositionNoScale(EEForwardTarget * 20.0f), FLinearColor::Blue, DebugSettings.Scale * 0.5f);
				Context.DrawInterface->DrawLine(FTransform::Identity, Objective.GetLocation(), Objective.TransformPositionNoScale(ObjectiveSettings.EffectorForwardAxis * 20.0f), FLinearColor::Black, DebugSettings.Scale * 0.5f);
				Context.DrawInterface->DrawLine(FTransform::Identity, Objective.GetLocation(), Objective.TransformPositionNoScale(ObjectiveSettings.EffectorUpAxis * 20.0f), FLinearColor::White, DebugSettings.Scale * 0.5f);
			}
		}
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////

FTGORRigUnit_DigitigradeIK_Execute()
{
	DECLARE_SCOPE_HIERARCHICAL_COUNTER_RIGUNIT()
	URigHierarchy* Hierarchy = ExecuteContext.Hierarchy;

	if (Context.State == EControlRigState::Init)
	{
		return;
	}

	if (Context.State == EControlRigState::Update)
	{
		if (Chain.Num() < 4)
		{
			UE_CONTROLRIG_RIGUNIT_REPORT_WARNING(TEXT("Chain has to have length at least 4."));
		}
		else
		{
			// Objective properties
			const FVector EEForwardTarget = Objective.TransformVectorNoScale(ObjectiveSettings.ForwardAxis);
			const FVector EEUpTarget = Objective.TransformVectorNoScale(ObjectiveSettings.UpAxis);
			const FVector EELocation = Objective.TransformPosition(ObjectiveSettings.Offset);

			// Compute initial leg properties
			const FTransform InitialUpperLeg = Hierarchy->GetInitialGlobalTransform(Chain[0]);
			const FTransform InitialLowerLeg = Hierarchy->GetInitialGlobalTransform(Chain[1]);
			const FTransform InitialAnkle = Hierarchy->GetInitialGlobalTransform(Chain[2]);
			const FTransform InitialFoot = Hierarchy->GetInitialGlobalTransform(Chain[3]);

			// Compute objective deltas
			const FTransform UpperLeg = Hierarchy->GetGlobalTransform(Chain[0]);
			const FVector PelvisLocation = UpperLeg.GetLocation();
			const FVector ObjectiveDelta = EELocation - PelvisLocation;
			const float ObjectiveNorm = ObjectiveDelta.Size();
			const FVector ObjectiveNormal = ObjectiveDelta / ObjectiveNorm;

			FVector Lengths;
			Lengths.X = (InitialUpperLeg.GetLocation() - InitialLowerLeg.GetLocation()).Size() * Customisation.X;
			Lengths.Y = (InitialLowerLeg.GetLocation() - InitialAnkle.GetLocation()).Size() * Customisation.Y;
			Lengths.Z = (InitialAnkle.GetLocation() - InitialFoot.GetLocation()).Size() * Customisation.Z;

			const FVector InitialAnkleDelta = InitialAnkle.GetLocation() - InitialUpperLeg.GetLocation();
			const FVector InitialKneeDelta = InitialLowerLeg.GetLocation() - InitialUpperLeg.GetLocation();
			const FVector InitialFootDelta = InitialFoot.GetLocation() - InitialUpperLeg.GetLocation();

			// Compute minimal distance between pelvis and ankle
			const float CosMinKneeAngle = FMath::Cos(FMath::DegreesToRadians(MinKneeAngle));
			const float HypothSqr = Lengths.X * Lengths.X + Lengths.Y * Lengths.Y - 2.0f * Lengths.X * Lengths.Y * CosMinKneeAngle;
			const float Hypoth = FMath::Sqrt(HypothSqr);

			// Compute minimal objective distance to not break angle constraints
			const float MinHeelRadians = FMath::Acos((HypothSqr + Lengths.Y * Lengths.Y - Lengths.X * Lengths.X) / (2.0f * Hypoth * Lengths.Y));
			const float CosMinHeelAngle = FMath::Cos(MinHeelRadians + FMath::DegreesToRadians(MinAnkleAngle));
			const float MinLegDistance = FMath::Sqrt(HypothSqr + Lengths.Z * Lengths.Z - 2.0f * Hypoth * Lengths.Z * CosMinHeelAngle);

			// Compute projected objective
			const float ObjectiveDistance = FMath::Max(ObjectiveNorm, MinLegDistance);
			const FVector ObjectiveLocation = PelvisLocation + ObjectiveNormal * ObjectiveDistance;

			// Compute proper rotation invariant direction according to initial pose
			const FQuat LegRotation = FQuat::FindBetweenNormals(InitialFootDelta.GetSafeNormal(), ObjectiveNormal);
			const FVector InitialKneeDirection = FVector::VectorPlaneProject(InitialKneeDelta - InitialAnkleDelta, InitialFootDelta.GetSafeNormal());
			const FVector LegDirection = (LegRotation * InitialKneeDirection).GetSafeNormal();

			if (DebugSettings.bEnabled)
			{
				Context.DrawInterface->DrawLine(FTransform::Identity, PelvisLocation, PelvisLocation + LegDirection * 50.0f, FLinearColor::White, DebugSettings.Scale * 0.5f);
			}

			// Compute directions
			const FVector FootDirection = EEForwardTarget * (EEUpTarget | ObjectiveNormal) - EEUpTarget * (EEForwardTarget | ObjectiveNormal);
			const FVector LowerDirection = FVector::VectorPlaneProject(FootDirection * (Lengths.X + Lengths.Y) + LegDirection * Lengths.Z * AnkleKneeDirectionWeight, ObjectiveNormal).GetSafeNormal();

			if (DebugSettings.bEnabled)
			{
				Context.DrawInterface->DrawLine(FTransform::Identity, ObjectiveLocation, ObjectiveLocation + FootDirection * 50.0f, FLinearColor::Black, DebugSettings.Scale * 0.5f);
			}

			// Interpolate ankle height
			float AnkleKath = 0.0f;
			float AnkleHeight = 0.0f;
			ComputeTriangle(Lengths.Z, Hypoth, MinLegDistance, AnkleHeight, AnkleKath);
			const float MaxChangLength = Lengths.X + Lengths.Y + Lengths.Z;
			const float DistanceRatio = FMath::Min((ObjectiveDistance - MinLegDistance) / (MaxChangLength - MinLegDistance), 1.0f);
			const float FinalHeight = AnkleHeight * (1.0f - FMath::Pow(DistanceRatio, StandingBend));

			// Compute ankle location
			const FVector AnkleDelta = LowerDirection * FinalHeight + ObjectiveNormal * AnkleKath;
			const FVector AnkleLocation = ObjectiveLocation - AnkleDelta.GetSafeNormal() * Lengths.Z;
			const FVector UpperDelta = AnkleLocation - PelvisLocation;
			const float UpperDistance = UpperDelta.Size();
			const FVector UpperNormal = UpperDelta / UpperDistance;

			if (DebugSettings.bEnabled)
			{
				Context.DrawInterface->DrawLine(FTransform::Identity, ObjectiveLocation, ObjectiveLocation + AnkleDelta, FLinearColor::Blue, DebugSettings.Scale * 0.5f);
			}

			// Compute knee location
			float KneeKath = 0.0f;
			float KneeHeight = 0.0f;
			ComputeTriangle(Lengths.X, Lengths.Y, UpperDistance, KneeHeight, KneeKath);
			const FVector UpperDirection = FVector::VectorPlaneProject(LowerDirection * Lengths.X * KneeHipDirectionWeight + LegDirection * Lengths.Y, UpperNormal).GetSafeNormal();

			const FVector KneeDelta = UpperNormal * KneeKath + UpperDirection * KneeHeight;
			const FVector KneeLocation = UpperLeg.GetLocation() + KneeDelta;

			if (DebugSettings.bEnabled)
			{
				Context.DrawInterface->DrawLine(FTransform::Identity, KneeLocation, KneeLocation + UpperDirection * 20.0f, FLinearColor::Green, DebugSettings.Scale * 0.5f);
				Context.DrawInterface->DrawLine(FTransform::Identity, AnkleLocation, AnkleLocation - LowerDirection * 20.0f, FLinearColor::Green, DebugSettings.Scale * 0.5f);

				Context.DrawInterface->DrawLine(FTransform::Identity, PelvisLocation, AnkleLocation, FLinearColor::Red, DebugSettings.Scale);
				Context.DrawInterface->DrawLine(FTransform::Identity, PelvisLocation, ObjectiveLocation, FLinearColor::Yellow, DebugSettings.Scale);
				Context.DrawInterface->DrawPoint(FTransform::Identity, KneeLocation, DebugSettings.Scale * 3.0f, FLinearColor::White);
			}

			// Transform to propagate along the leg
			FTGORRigUnit_Propagate::PropagateChainTowardsFixed(Chain[0], Chain[1], KneeLocation, Hierarchy, PropagateToChildren == ETGOR_Propagation::All);
			FTGORRigUnit_Propagate::PropagateChainTowardsFixed(Chain[1], Chain[2], AnkleLocation, Hierarchy, PropagateToChildren == ETGOR_Propagation::All);
			FTGORRigUnit_Propagate::PropagateChainTowardsFixed(Chain[2], Chain[3], ObjectiveLocation, Hierarchy, PropagateToChildren == ETGOR_Propagation::All);

			// Set foot transform
			FTransform Foot;
			Foot.SetScale3D(Hierarchy->GetInitialGlobalTransform(Chain[3]).GetScale3D());
			Foot.SetRotation(FTGORRigUnit_RotateToward::ComputeHeadingRotation(ObjectiveSettings.EffectorForwardAxis, EEForwardTarget, ObjectiveSettings.EffectorUpAxis, EEUpTarget));
			Foot.SetLocation(ObjectiveLocation);
			Hierarchy->SetGlobalTransform(Chain[3], Foot, false, PropagateToChildren != ETGOR_Propagation::Off);

			if (DebugSettings.bEnabled)
			{
				Context.DrawInterface->DrawLine(FTransform::Identity, ObjectiveLocation, ObjectiveLocation + EEForwardTarget * 20.0f, FLinearColor::Red, DebugSettings.Scale * 0.5f);
				Context.DrawInterface->DrawLine(FTransform::Identity, ObjectiveLocation, ObjectiveLocation + EEUpTarget * 20.0f, FLinearColor::Blue, DebugSettings.Scale * 0.5f);
				Context.DrawInterface->DrawLine(FTransform::Identity, ObjectiveLocation, ObjectiveLocation + Foot.TransformVectorNoScale(ObjectiveSettings.EffectorForwardAxis) * 20.0f, FLinearColor::Black, DebugSettings.Scale * 0.5f);
				Context.DrawInterface->DrawLine(FTransform::Identity, ObjectiveLocation, ObjectiveLocation + Foot.TransformVectorNoScale(ObjectiveSettings.EffectorUpAxis) * 20.0f, FLinearColor::White, DebugSettings.Scale * 0.5f);
			}
		}
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////

FTGORRigUnit_ClavicleIK_Execute()
{
	DECLARE_SCOPE_HIERARCHICAL_COUNTER_RIGUNIT()
	URigHierarchy* Hierarchy = ExecuteContext.Hierarchy;

	if (Context.State == EControlRigState::Init)
	{
		return;
	}

	if (Context.State == EControlRigState::Update)
	{
		if (Chain.Num() < 4)
		{
			UE_CONTROLRIG_RIGUNIT_REPORT_WARNING(TEXT("Chain has to have length at least 4."));
		}
		else
		{
			// Objective properties
			const FVector EEForwardTarget = Objective.TransformVectorNoScale(ObjectiveSettings.ForwardAxis);
			const FVector EEUpTarget = Objective.TransformVectorNoScale(ObjectiveSettings.UpAxis);
			const FVector EELocation = Objective.TransformPosition(ObjectiveSettings.Offset);

			// Compute initial leg properties
			const FTransform InitialClavicle = Hierarchy->GetInitialGlobalTransform(Chain[0]);
			const FTransform InitialUpperArm = Hierarchy->GetInitialGlobalTransform(Chain[1]);
			const FTransform InitialLowerArm = Hierarchy->GetInitialGlobalTransform(Chain[2]);
			const FTransform InitialHand = Hierarchy->GetInitialGlobalTransform(Chain[3]);

			FVector Lengths;
			Lengths.X = (InitialClavicle.GetLocation() - InitialUpperArm.GetLocation()).Size() * Customisation.X;
			Lengths.Y = (InitialUpperArm.GetLocation() - InitialLowerArm.GetLocation()).Size() * Customisation.Y;
			Lengths.Z = (InitialLowerArm.GetLocation() - InitialHand.GetLocation()).Size() * Customisation.Z;

			const FVector InitialEllbowDelta = InitialLowerArm.GetLocation() - InitialUpperArm.GetLocation();
			const FVector InitialHandDelta = InitialHand.GetLocation() - InitialUpperArm.GetLocation();

			// Compute clavicle offset
			const FTransform Clavicle = Hierarchy->GetGlobalTransform(Chain[0]);
			const FTransform Shoulder = Hierarchy->GetGlobalTransform(Chain[1]);
			const FVector ClavicleShoulderDelta = Shoulder.GetLocation() - Clavicle.GetLocation();
			const FVector ClavicleShoulderNormal = ClavicleShoulderDelta.GetSafeNormal();
			const FVector ClavicleObjectiveDelta = EELocation - Clavicle.GetLocation();
			const float ClavicleObjectiveDistance = ClavicleObjectiveDelta.Size();
			const FVector ClavicleObjectiveNormal = ClavicleObjectiveDelta / ClavicleObjectiveDistance;

			// Compute clavicle location
			const float MaxClavicleRadians = FMath::DegreesToRadians(MaxClavicleAngle);
			const FVector ClavicleOrthogonal = FVector::VectorPlaneProject(ClavicleObjectiveNormal, ClavicleShoulderNormal).GetSafeNormal();

			const float MaxArmLength = Lengths.Y + Lengths.Z;
			const float ClavicleIntensity = FMath::Min(1.0f - (ClavicleShoulderDelta.GetSafeNormal() | ClavicleObjectiveNormal), 1.0f);
			const float ClavicleDistancity = FMath::Min((Shoulder.GetLocation() - EELocation).Size() / MaxArmLength, 1.0f);
			const FVector ClavicleBiasVector = Clavicle.TransformVectorNoScale(ClavicleBias);
			const float BiasIntensity = FMath::Exp(ClavicleBiasVector | ClavicleObjectiveNormal);
			const float ClavicleSin = FMath::Sin(MaxClavicleRadians * BiasIntensity) * ClavicleIntensity * ClavicleDistancity;
			const FVector FinalClavicleNormal = ClavicleShoulderNormal * FMath::Sqrt(1.0f - ClavicleSin * ClavicleSin) + ClavicleOrthogonal * ClavicleSin;
			const FVector ClavicleLocation = Clavicle.GetLocation() + FinalClavicleNormal * Lengths.X;

			if (DebugSettings.bEnabled)
			{
				Context.DrawInterface->DrawLine(FTransform::Identity, Clavicle.GetLocation(), Clavicle.GetLocation() + ClavicleBiasVector * 30.0f, FLinearColor::White, DebugSettings.Scale * 0.5f);
				Context.DrawInterface->DrawLine(FTransform::Identity, Clavicle.GetLocation(), Clavicle.GetLocation() + ClavicleOrthogonal * 30.0f, FLinearColor::Blue, DebugSettings.Scale * 0.3f);
				Context.DrawInterface->DrawLine(FTransform::Identity, Clavicle.GetLocation(), Clavicle.GetLocation() + ClavicleShoulderNormal * 30.0f, FLinearColor::Green, DebugSettings.Scale * 0.3f);
			}

			// Compute minimal distance between pelvis and ankle
			const float CosMinEllbowAngle = FMath::Cos(FMath::DegreesToRadians(MinEllbowAngle));
			const float MinArmDistance = FMath::Sqrt(Lengths.X * Lengths.X + Lengths.Y * Lengths.Y - 2.0f * Lengths.X * Lengths.Y * CosMinEllbowAngle);

			// Compute objective deltas
			const FVector ObjectiveDelta = EELocation - ClavicleLocation;
			const float ObjectiveNorm = ObjectiveDelta.Size();
			const FVector ObjectiveNormal = ObjectiveDelta / ObjectiveNorm;
			const float ObjectiveDistance = FMath::Max(ObjectiveNorm, MinArmDistance);
			const FVector ObjectiveLocation = ClavicleLocation + ObjectiveNormal * ObjectiveDistance;

			// Compute proper rotation invariant direction according to initial pose
			const FVector InitialHandNormal = InitialHandDelta.GetSafeNormal();
			const FQuat ArmRotation = FQuat::FindBetweenNormals(InitialHandNormal, ObjectiveNormal);
			const FVector InitialArmDelta = FVector::VectorPlaneProject(InitialEllbowDelta, InitialHandNormal);
			const FVector ArmDirection = (ArmRotation * InitialArmDelta).GetSafeNormal();

			if (DebugSettings.bEnabled)
			{
				Context.DrawInterface->DrawLine(FTransform::Identity, ClavicleLocation, ClavicleLocation + ArmDirection * 50.0f, FLinearColor::White, DebugSettings.Scale * 0.5f);
			}

			// Compute directions
			const FVector EEAlignment = Objective.TransformVectorNoScale(EllbowAlignmentAxis);
			const FVector HandAlignment = EEForwardTarget * (EEAlignment | ObjectiveNormal) - EEAlignment * (EEForwardTarget | ObjectiveNormal);
			const FVector LowerDirection = FVector::VectorPlaneProject(HandAlignment * Lengths.Z + ArmDirection * Lengths.Y, ObjectiveNormal).GetSafeNormal();

			if (DebugSettings.bEnabled)
			{
				Context.DrawInterface->DrawLine(FTransform::Identity, ObjectiveLocation, ObjectiveLocation + HandAlignment * 30.0f, FLinearColor::Black, DebugSettings.Scale * 0.5f);
			}

			// Compute ellbow location
			float EllbowKath = 0.0f;
			float EllbowHeight = 0.0f;
			ComputeTriangle(Lengths.Y, Lengths.Z, ObjectiveDistance, EllbowHeight, EllbowKath);

			const FVector EllbowDelta = ObjectiveNormal * EllbowKath + LowerDirection * EllbowHeight;
			const FVector EllbowLocation = ClavicleLocation + EllbowDelta;

			if (DebugSettings.bEnabled)
			{
				Context.DrawInterface->DrawLine(FTransform::Identity, EllbowLocation, EllbowLocation + LowerDirection * 20.0f, FLinearColor::Green, DebugSettings.Scale * 0.5f);

				Context.DrawInterface->DrawLine(FTransform::Identity, ClavicleLocation, EllbowLocation, FLinearColor::Red, DebugSettings.Scale);
				Context.DrawInterface->DrawLine(FTransform::Identity, EllbowLocation, ObjectiveLocation, FLinearColor::Yellow, DebugSettings.Scale);
				Context.DrawInterface->DrawPoint(FTransform::Identity, EllbowLocation, DebugSettings.Scale * 3.0f, FLinearColor::White);
			}

			// Transform to propagate along the leg
			FTGORRigUnit_Propagate::PropagateChainTowardsFixed(Chain[0], Chain[1], ClavicleLocation, Hierarchy, PropagateToChildren == ETGOR_Propagation::All);
			FTGORRigUnit_Propagate::PropagateChainTowardsFixed(Chain[1], Chain[2], EllbowLocation, Hierarchy, PropagateToChildren == ETGOR_Propagation::All);
			FTGORRigUnit_Propagate::PropagateChainTowardsFixed(Chain[2], Chain[3], ObjectiveLocation, Hierarchy, PropagateToChildren == ETGOR_Propagation::All);

			// Set foot transform
			FTransform EE;
			EE.SetScale3D(Hierarchy->GetInitialGlobalTransform(Chain[3]).GetScale3D());
			EE.SetRotation(FTGORRigUnit_RotateToward::ComputeHeadingRotation(ObjectiveSettings.EffectorForwardAxis, EEForwardTarget, ObjectiveSettings.EffectorUpAxis, EEUpTarget));
			EE.SetLocation(ObjectiveLocation);
			Hierarchy->SetGlobalTransform(Chain[3], EE, false, PropagateToChildren != ETGOR_Propagation::Off);

			if (DebugSettings.bEnabled)
			{
				Context.DrawInterface->DrawLine(FTransform::Identity, ObjectiveLocation, ObjectiveLocation + EEForwardTarget * 20.0f, FLinearColor::Red, DebugSettings.Scale * 0.5f);
				Context.DrawInterface->DrawLine(FTransform::Identity, ObjectiveLocation, ObjectiveLocation + EEUpTarget * 20.0f, FLinearColor::Blue, DebugSettings.Scale * 0.5f);
				Context.DrawInterface->DrawLine(FTransform::Identity, ObjectiveLocation, ObjectiveLocation + EE.TransformVectorNoScale(ObjectiveSettings.EffectorForwardAxis) * 20.0f, FLinearColor::Black, DebugSettings.Scale * 0.5f);
				Context.DrawInterface->DrawLine(FTransform::Identity, ObjectiveLocation, ObjectiveLocation + EE.TransformVectorNoScale(ObjectiveSettings.EffectorUpAxis) * 20.0f, FLinearColor::White, DebugSettings.Scale * 0.5f);
			}
		}
	}
}
