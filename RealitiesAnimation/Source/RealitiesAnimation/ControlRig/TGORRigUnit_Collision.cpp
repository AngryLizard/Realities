// The Gateway of Realities: Planes of Existence.

#include "TGORRigUnit_Collision.h"
#include "TGORRigUnit_TriangleIK.h"
#include "RealitiesUtility/Utility/TGOR_Math.h"
#include "RealitiesUtility/Utility/TGOR_Error.h"
#include "Units/RigUnitContext.h"


void FTGORRigUnit_EllipsoidProjection::ComputeEllispoidProjection(const FTransform& Transform, float Radius, const FVector& Point, FVector& Closest, FVector& Normal)
{
	const FVector Scale = Transform.GetScale3D();
	if (FMath::IsNearlyZero(Scale.X * Scale.Y * Scale.Z))
	{
		Closest = Transform.GetLocation();
		Normal = (Point - Closest).GetSafeNormal();
		return;
	}

	// Warp the input along scale to get an appropriate ray direction that will alyways intersect the ellipsoid again
	const FVector ScaledNormal = (Transform.InverseTransformPosition(Point) * Scale).GetSafeNormal();
	const FVector ProjectedNormal = Transform.TransformVectorNoScale((ScaledNormal / Scale)).GetSafeNormal();
	const float RayDistance = Scale.GetAbsMax() * Radius;

	// Reproject for ellipsoid intersection
	const FVector RayStart = Transform.InverseTransformPosition(Point + ProjectedNormal * RayDistance);
	const FVector RayEnd = Transform.InverseTransformPosition(Point - ProjectedNormal * RayDistance);
	const FVector RayDelta = RayEnd - RayStart;
	const float RayLength = RayDelta.Size();
	const FVector RayDir = RayDelta / RayLength;

	// Compute ellipsoid intersection
	float Value = -(RayDir | RayStart);

	const float Rd = RayStart.SizeSquared() - FMath::Square(Radius);
	const float Discr = FMath::Square(Value) - Rd;
	if (Discr >= 0.0f)
	{
		Value -= FMath::Sqrt(Discr);
	}

	// Cast location on the sphere, restrict by raylength
	const FVector IntersectionNormal = (RayStart + RayDir * FMath::Clamp(Value, 0.0f, RayLength)).GetSafeNormal();

	// Transform back to world
	Normal = Transform.TransformVectorNoScale((IntersectionNormal / Scale)).GetSafeNormal();
	Closest = Transform.TransformPosition(IntersectionNormal * Radius);
}

FTGORRigUnit_EllipsoidProjection_Execute()
{
	DECLARE_SCOPE_HIERARCHICAL_COUNTER_RIGUNIT()
	const URigHierarchy* Hierarchy = Context.Hierarchy;

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
			const FTransform Transform = Hierarchy->GetGlobalTransform(Cache);
			ComputeEllispoidProjection(Transform, Radius, Point, Closest, Normal);

			if (DebugSettings.bEnabled)
			{
				Context.DrawInterface->DrawLine(FTransform::Identity, Closest, Closest + Normal * 25.0f, FLinearColor::Red, DebugSettings.Scale * 0.1f);
			}
		}
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////

float FTGORRigUnit_EllipsoidRaycast::ComputeEllispoidRaycast(const FTransform& Transform, float Radius, const FVector& Start, const FVector& End, FVector& Impact, FVector& Normal)
{
	const FVector RayStart = Transform.InverseTransformPosition(Start);
	const FVector RayEnd = Transform.InverseTransformPosition(End);
	const FVector RayDelta = RayEnd - RayStart;
	const float RayLength = RayDelta.Size();
	const FVector RayDir = RayDelta / RayLength;

	const float Sq = RayStart.SizeSquared();
	const float Rd = Sq - Radius * Radius;

	float Value = -(RayDir | RayStart);

	const float Discr = FMath::Square(Value) - Rd;
	if (Discr >= 0.0f)
	{
		Value -= FMath::Sqrt(Discr);
	}

	// Cast location on the sphere, restrict by raylength
	const FVector RaySpherePoint = RayStart + RayDir * FMath::Clamp(Value, 0.0f, RayLength);
	const float RaySphereDistance = RaySpherePoint.Size();

	Normal = RaySpherePoint / RaySphereDistance;
	Impact = Normal * Radius;

	// Transform back to world
	Normal = Transform.TransformVectorNoScale((Normal / Transform.GetScale3D())).GetSafeNormal();
	Impact = Transform.TransformPosition(Impact);

	return RaySphereDistance - Radius;
}

FTGORRigUnit_EllipsoidRaycast_Execute()
{
	DECLARE_SCOPE_HIERARCHICAL_COUNTER_RIGUNIT()
	const URigHierarchy* Hierarchy = Context.Hierarchy;

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
			const FTransform Transform = Hierarchy->GetGlobalTransform(Cache);
			Distance = ComputeEllispoidRaycast(Transform, Radius, Start, End, Impact, Normal);

			if (DebugSettings.bEnabled)
			{
				Context.DrawInterface->DrawLine(FTransform::Identity, Impact, Impact + Normal * 25.0f, FLinearColor::Red, DebugSettings.Scale * 0.1f);
			}
		}
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////

FVector FTGORRigUnit_EllipsoidRayCollide::ComputeEllispoidRayCollide(const FTransform& Transform, float Radius, const FVector& Start, const FVector& End, const FVector& Direction, float Adapt)
{
	const FVector RayStart = Transform.InverseTransformPosition(Start);
	const FVector RayEnd = Transform.InverseTransformPosition(End);
	const FVector RayDelta = RayEnd - RayStart;
	const float RayLength = RayDelta.Size();
	const FVector RayDir = RayDelta / RayLength;

	const FVector RayMove = Transform.InverseTransformVector(Direction);
	const FVector RayAxis = (RayMove ^ RayDir).GetSafeNormal();

	const float Ad = RayAxis | RayStart; // Distance between ricle center to sphere
	const float Rq = FMath::Square(Radius) - FMath::Square(Ad); // Pythagoras to compute square circle radius

	// Check if we're in reach
	if (!FMath::IsNearlyZero(Adapt) || Rq > 0.0f)
	{
		const FVector PC = RayAxis * Ad; // Circle center

		// Check if where moving through the sphere at all
		const FVector CenterDelta = RayStart - RayDir * (RayDir | RayStart) - PC;
		if (CenterDelta.SizeSquared() < Rq || (CenterDelta | RayMove) > 0.0f)
		{
			const FVector SD = PC - RayStart;
			const float Sd = SD.Size(); // Distance between line start and circle center

			const float Pq = FMath::Max(Rq, 0.0f);
			const float Dq = FMath::Square(Sd) - Pq; // Square distance from line start to tangent point
			if (Dq > 0.0f)
			{
				const FVector SN = SD / Sd;
				const FVector HN = FVector::VectorPlaneProject(-RayMove, SN).GetSafeNormal();

				const float Height = FMath::Sqrt(Dq * Pq) / Sd; // Triangle height (area / hypoth)
				const float Kath = FMath::Sqrt(Dq - FMath::Square(Height));
				const FVector Tangent = HN * Height + SN * Kath; // Tangent

				const FVector TangentPoint = Transform.TransformPosition(RayStart + Tangent);
				const FVector WorldDelta = End - Start;
				const float WorldLength = WorldDelta.Size();
				const FVector TangentDelta = TangentPoint - Start;
				//const float TangentLength = TangentDelta.Size();

				const float Intensity = FMath::Clamp(-Rq / (Adapt * Adapt), 0.0f, 1.0f);
				const FVector Delta = FMath::Lerp(TangentDelta, WorldDelta, Intensity);
				return Start + Delta.GetSafeNormal() * WorldLength;
			}
		}
	}

	// Don't move by default
	return End;
}

FTGORRigUnit_EllipsoidRayCollide_Execute()
{
	DECLARE_SCOPE_HIERARCHICAL_COUNTER_RIGUNIT()
	const URigHierarchy* Hierarchy = Context.Hierarchy;

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
			const FTransform Transform = Hierarchy->GetGlobalTransform(Cache);
			Deflect = ComputeEllispoidRayCollide(Transform, Radius, Start, End, Direction, Adapt);

			if (DebugSettings.bEnabled)
			{
				Context.DrawInterface->DrawPoint(FTransform::Identity, Deflect, DebugSettings.Scale * 5.0f, FLinearColor::White);
			}
		}
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////

FTGORRigUnit_EllipsoidChainCollide_Execute()
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
		const int32 ChainNum = Chain.Num();
		if (ChainNum < 2)
		{
			UE_CONTROLRIG_RIGUNIT_REPORT_WARNING(TEXT("Chain has to have length at least 2."));
		}
		else if (!Cache.UpdateCache(Key, Hierarchy))
		{
			UE_CONTROLRIG_RIGUNIT_REPORT_WARNING(TEXT("key '%s' is not valid."), *Key.ToString());
		}
		else
		{
			// Apply rotation along the whole chain
			FTransform Transform = Hierarchy->GetGlobalTransform(Chain[0]);
			const FTransform Ellipsoid = Hierarchy->GetGlobalTransform(Cache);

			const float MaxChainLength = FTGORRigUnit_ChainLength::ComputeInitialChainLength(Chain, Hierarchy);

			// Intensity according to relative distance
			FVector Anchor, AnchorNormal;
			FTGORRigUnit_EllipsoidProjection::ComputeEllispoidProjection(Ellipsoid, Radius, Transform.GetLocation(), Anchor, AnchorNormal);

			// Diminish intensity with distance 
			const float Distance = (Transform.GetLocation() - Anchor).Size();// | AnchorNormal;
			const float Intensity = FMath::Clamp(1.0f + Discovery - Distance / MaxChainLength, 0.0f, 1.0f);

			// Rotate each segment
			const float MaxRadians = FMath::DegreesToRadians(MaxAngle);
			const float RotationRadians = FMath::DegreesToRadians(RotationAngle);
			const FVector WorldAxis = Transform.TransformVectorNoScale(RotationAxis);
			const FQuat Rotation = FQuat(WorldAxis, RotationRadians * (1.0f - Intensity));

			for (int32 Index = 1; Index < ChainNum; Index++)
			{
				const FTransform Local = Hierarchy->GetLocalTransform(Chain[Index]);

				// Collide future bone location with ellipsoid
				const FVector Start = Transform.GetLocation();
				const FVector Delta = Rotation * (Transform.TransformVector(Local.GetLocation()));
				const float DeltaSize = Delta.Size();
				const FVector DeltaNormal = Delta / DeltaSize;

				if (!FMath::IsNearlyZero(Intensity))
				{
					FVector Closest, Normal;
					const FVector Point = Start + Delta * NormalRatio;
					FTGORRigUnit_EllipsoidProjection::ComputeEllispoidProjection(Ellipsoid, Radius, Point, Closest, Normal);

					const FVector FinalPlane = FVector::VectorPlaneProject(FVector::VectorPlaneProject(DeltaNormal, Normal), WorldAxis);
					const FVector FinalDelta = FMath::Lerp(DeltaNormal, FinalPlane, Intensity).GetSafeNormal() * DeltaSize;

					/*
					const FVector CurrentUp = (WorldAxis ^ Delta).GetSafeNormal();
					const FVector TargetUp = FVector::VectorPlaneProject(Normal, WorldAxis);
					const FVector FinalUp = FMath::Lerp(CurrentUp, TargetUp, Intensity).GetSafeNormal();
					*/

					// Rotate to match
					const FVector CurrentDelta = Transform.TransformVectorNoScale(Local.GetLocation());
					const FQuat AlignRotation = FQuat::FindBetweenVectors(CurrentDelta, FinalDelta);
					
					const FQuat FinalRotation = FTGORRigUnit_LimitRotation::LimitRotation(AlignRotation, WorldAxis, MaxRadians, RotationRadians);
					Transform.SetRotation(FinalRotation * Transform.GetRotation());

					// Update chain element
					Hierarchy->SetGlobalTransform(Chain[Index - 1], Transform, false, PropagateToChildren == ETGOR_Propagation::All);

					// Propagate
					Transform = Local * Transform;

					if (DebugSettings.bEnabled)
					{
						Context.DrawInterface->DrawPoint(FTransform::Identity, Point, DebugSettings.Scale * 2.0f, FLinearColor::White);
						Context.DrawInterface->DrawPoint(FTransform::Identity, Closest, DebugSettings.Scale * 2.0f, FLinearColor::Black);
					}
				}
				else
				{
					FTGORRigUnit_Propagate::PropagateChainTowards(Chain[Index - 1], Chain[Index], Start + Delta, Hierarchy, PropagateToChildren == ETGOR_Propagation::All);
					Transform = Hierarchy->GetGlobalTransform(Chain[Index]);
				}


				if (DebugSettings.bEnabled)
				{
					Context.DrawInterface->DrawLine(FTransform::Identity, Start - WorldAxis * 5.0f, Start + WorldAxis * 5.0f, FLinearColor::Red, DebugSettings.Scale * 0.1f);
					Context.DrawInterface->DrawPoint(FTransform::Identity, Start, DebugSettings.Scale * 5.0f, FLinearColor::Blue);
				}
			}


			Hierarchy->SetGlobalTransform(Chain.Last(), Transform, false, PropagateToChildren != ETGOR_Propagation::Off);
		}
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////

FTGORRigUnit_EllipsoidTransformProject_Execute()
{
	DECLARE_SCOPE_HIERARCHICAL_COUNTER_RIGUNIT()
	const URigHierarchy* Hierarchy = Context.Hierarchy;

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
			const FVector EEForwardTarget = Objective.TransformVectorNoScale(ObjectiveSettings.ForwardAxis);
			const FVector EEUpTarget = Objective.TransformVectorNoScale(ObjectiveSettings.UpAxis);

			const FQuat EERotation = Objective.TransformRotation(ObjectiveSettings.RotationOffset.Quaternion());
			const FVector EELocation = Objective.TransformPosition(ObjectiveSettings.TranslationOffset);

			const FTransform Ellipsoid = Hierarchy->GetGlobalTransform(Cache);
			const FVector RayStart = EELocation + EEUpTarget * Discovery;

			// Get closest point on the ellipsoid to the ray
			FVector Closest, Normal;
			float HitDistance = FTGORRigUnit_EllipsoidRaycast::ComputeEllispoidRaycast(Ellipsoid, Radius, RayStart, EELocation, Closest, Normal);
			if (!FMath::IsNearlyZero(HitDistance))
			{
				const FVector RayDelta = EELocation - RayStart;
				const float RayLength = RayDelta.Size();
				const FVector RayNormal = RayDelta / RayLength;
				const FVector Point = RayStart + RayNormal * FMath::Clamp((Closest - RayStart) | RayNormal, 0.0f, RayLength);
				FTGORRigUnit_EllipsoidProjection::ComputeEllispoidProjection(Ellipsoid, Radius, Point, Closest, Normal);

				HitDistance = (Point - Closest).Size();

				if (DebugSettings.bEnabled)
				{
					Context.DrawInterface->DrawPoint(FTransform::Identity, Point, DebugSettings.Scale * 5.0f, FLinearColor::Blue);
				}
			}

			if (DebugSettings.bEnabled)
			{
				Context.DrawInterface->DrawLine(FTransform::Identity, RayStart, EELocation, FLinearColor::White, DebugSettings.Scale * 0.1f);
				Context.DrawInterface->DrawLine(FTransform::Identity, Closest, Closest + Normal * 10.0f, FLinearColor::Green, DebugSettings.Scale * 0.1f);
				Context.DrawInterface->DrawPoint(FTransform::Identity, Closest, DebugSettings.Scale * 5.0f, FLinearColor::Blue);
			}

			const float Intensity = FMath::Square(FMath::Max(1.0f - HitDistance / Distance, 0.0f));
			const FVector UpTarget = FMath::Lerp(EEUpTarget, Normal, Intensity).GetSafeNormal();
			const FVector LocationTarget = FMath::Lerp(EELocation, Closest, Intensity);
			
			const float MaxRadians = FMath::DegreesToRadians(MaxAngle);
			const FQuat AlignRotation = FQuat::FindBetweenNormals(EEUpTarget, UpTarget);
			const FQuat LimitRotation = FTGORRigUnit_LimitRotation::SoftLimitRotation(AlignRotation, MaxRadians);

			const FQuat Rotation = FTGORRigUnit_RotateToward::ComputeHeadingRotation(ObjectiveSettings.EffectorForwardAxis, EEForwardTarget, ObjectiveSettings.EffectorUpAxis, EEUpTarget);
			Projection.SetRotation(LimitRotation * EERotation);
			Projection.SetLocation(LocationTarget);
			Projection.SetScale3D(Objective.GetScale3D());

			if (DebugSettings.bEnabled)
			{
				Context.DrawInterface->DrawPoint(FTransform::Identity, LocationTarget, DebugSettings.Scale * 5.0f, FLinearColor::Red);
			}
		}
	}
}
