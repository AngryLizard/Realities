// The Gateway of Realities: Planes of Existence.

#include "TGORRigUnit_Dynamics.h"

#include "RealitiesUtility/Structures/TGOR_Matrix3x3.h"
#include "RealitiesUtility/Utility/TGOR_Math.h"
#include "Units/RigUnitContext.h"


FTGORRigUnit_SplineChainDynamics_Execute()
{
	DECLARE_SCOPE_HIERARCHICAL_COUNTER_RIGUNIT()
	URigHierarchy* Hierarchy = ExecuteContext.Hierarchy;

	if (Context.State == EControlRigState::Init)
	{
		WorkData.CachedItems.Reset();
		WorkData.Positions.Reset();
		WorkData.Velocities.Reset();
		return;
	}

	if (Context.State == EControlRigState::Update)
	{

		const int32 ChainCount = Chain.Num();
		if (ChainCount < 2)
		{
			UE_CONTROLRIG_RIGUNIT_REPORT_WARNING(TEXT("Chain has to have length at least 2."));
		}
		else
		{
			if (WorkData.CachedItems.Num() == 0 && ChainCount > 0)
			{
				for (FRigElementKey Item : Chain)
				{
					FCachedRigElement Element = FCachedRigElement(Item, Hierarchy);
					WorkData.CachedItems.Add(Element);

					WorkData.Positions.Add(Hierarchy->GetGlobalTransform(Element).GetLocation());
					WorkData.Velocities.Add(FVector::ZeroVector);
				}
			}

			const float ChainMaxLength = FTGORRigUnit_ChainLength::ComputeInitialChainLength(Chain, Hierarchy);
			if (FMath::IsNearlyZero(ChainMaxLength))
			{
				UE_CONTROLRIG_RIGUNIT_REPORT_WARNING(TEXT("Total chain length is null."));
			}
			else
			{

				float Distance = 0.0f;
				FTransform Current = Hierarchy->GetGlobalTransform(WorkData.CachedItems[0]);
				FVector CurrentInitial = Hierarchy->GetInitialGlobalTransform(WorkData.CachedItems[0]).GetLocation();
				for (int32 Index = 1; Index < ChainCount; Index++)
				{
					const FCachedRigElement& Element = WorkData.CachedItems[Index];
					const FTransform Next = Hierarchy->GetGlobalTransform(Element);
					const FVector NextInitial = Hierarchy->GetInitialGlobalTransform(Element).GetLocation();

					const float Length = (NextInitial - CurrentInitial).Size();
					Distance += Length / ChainMaxLength;

					FVector TargetDelta = FVector::ZeroVector;
					if (Distance < 0.5f)
					{
						TargetDelta = FMath::Lerp(StartTangent, MiddleTangent, Distance / 0.5f);
					}
					else
					{
						TargetDelta = FMath::Lerp(MiddleTangent, EndTangent, (Distance - 0.5f) / 0.5f);
					}

					const float TargetDeltaNorm = TargetDelta.Size();
					if (FMath::IsNearlyZero(TargetDeltaNorm))
					{
						UE_CONTROLRIG_RIGUNIT_REPORT_WARNING(TEXT("Target tangent is null."));
					}
					else
					{
						const FVector TargetTangent = TargetDelta / TargetDeltaNorm;
						const FVector Target = Current.GetLocation() + TargetTangent * Length;


						FVector& Position = WorkData.Positions[Index];
						FVector& Velocity = WorkData.Velocities[Index];

						const FVector Acc = (Target - Position) * (Spring / FMath::Max(Context.DeltaTime, 1.0f)) - Velocity * Damping + External;

						Position += Velocity * Context.DeltaTime;// +0.5f * Acc * Context.DeltaTime * Context.DeltaTime;
						Velocity += Acc * Context.DeltaTime;

						const FVector Normal = (Position - Current.GetLocation()).GetSafeNormal();
						Velocity = FVector::VectorPlaneProject(Velocity, Normal);
						Position = Current.GetLocation() + Normal * Length;

						Position = FMath::Lerp(Target, Position, Intensity);

						// Project onto collider
						const float Dist = Collider.PlaneDot(Position);
						if (Dist < 0.0f)
						{
							Position -= Dist * Collider.GetNormal();
						}

						if (DebugSettings.bEnabled)
						{
							Context.DrawInterface->DrawPoint(FTransform::Identity, Position, DebugSettings.Scale * 1.5f, FLinearColor::Red);
							Context.DrawInterface->DrawLine(FTransform::Identity, Current.GetLocation(), Position, FLinearColor::Blue, DebugSettings.Scale * 0.5f);
						}

						FTGORRigUnit_Propagate::PropagateChainTowards(WorkData.CachedItems[Index - 1].GetKey(), Element.GetKey(), Position, Hierarchy, PropagateToChildren == ETGOR_Propagation::All);
					}

					Current = Hierarchy->GetGlobalTransform(Element);
					CurrentInitial = NextInitial;
				}
			}
		}
	}
}

FString FTGORRigUnit_SplineChainDynamics::ProcessPinLabelForInjection(const FString& InLabel) const
{
	FString Formula;
	return FString::Printf(TEXT("%s: TODO"), *InLabel);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

FTGORRigUnit_RetractGripDynamics_Execute()
{
	DECLARE_SCOPE_HIERARCHICAL_COUNTER_RIGUNIT()
	URigHierarchy* Hierarchy = ExecuteContext.Hierarchy;

	if (Context.State == EControlRigState::Init)
	{
		WorkData.RetractCache.Reset();
		WorkData.bInitialized = false;
		return;
	}

	if (Context.State == EControlRigState::Update)
	{
		if (!WorkData.RetractCache.UpdateCache(RetractKey, Hierarchy))
		{
			UE_CONTROLRIG_RIGUNIT_REPORT_WARNING(TEXT("Retract key '%s' is not valid."), *RetractKey.ToString());
		}
		else if (SqrtSpring < SMALL_NUMBER)
		{
			UE_CONTROLRIG_RIGUNIT_REPORT_WARNING(TEXT("Spring constant %.0f must be greater than 0"), SqrtSpring);
		}
		else
		{
			const FVector4 Objective = FVector4(Target, 0.0f);

			if (!WorkData.bInitialized)
			{
				WorkData.Position = Objective;
				WorkData.Velocity = FVector4(0, 0, 0, 0);
				WorkData.Compensation = 0.0f;
				WorkData.bInitialized = true;
			}

			// Set default to current position
			Output = FVector(WorkData.Position);

			const FVector4 Delta = (Objective - WorkData.Position);
			const float Distance = Delta.Size();
			if (!FMath::IsNearlyZero(Context.DeltaTime))
			{
				const float Sampled = Distance / Context.DeltaTime;

				/*
				if (Sampled < WorkData.Compensation + AccThreshold * Context.DeltaTime)
				{
					// Move with target and remember compensation
					WorkData.Position = Objective;
					WorkData.Velocity = Delta / Context.DeltaTime;
					WorkData.Compensation = Sampled;
				}
				else
				*/
				{
					const float DistanceRatio = Distance / MaxDistance;

					// Spring motion
					const FVector4 Accel = SqrtSpring * (Delta * SqrtSpring - 2.0f * WorkData.Velocity);
					const FVector4 External = FVector4(0, 0, 0, DistanceRatio * RetractStrength);
					WorkData.Velocity += (Accel + External) * Context.DeltaTime;
					WorkData.Position += WorkData.Velocity * Context.DeltaTime;
				}

				// Limit speed
				const float MaxSpeed = MaxDistance * SqrtSpring * INV_E;
				float Speed = FMath::Max(FVector(WorkData.Velocity).Size() - WorkData.Compensation, 0.0f);
				if (Speed > MaxSpeed)
				{
					const float SpeedRatio = MaxSpeed / Speed;
					WorkData.Velocity.X = WorkData.Velocity.X * SpeedRatio;
					WorkData.Velocity.Y = WorkData.Velocity.Y * SpeedRatio;
					WorkData.Velocity.Z = WorkData.Velocity.Z * SpeedRatio;
				}

				// Retract according to speed
				const FVector Retract = Hierarchy->GetGlobalTransform(WorkData.RetractCache).GetLocation();
				const float PullRatio = (1.0f - FMath::Exp(-WorkData.Position.W)) * RetractRatio;
				Output = FMath::Lerp(FVector(WorkData.Position), Retract, PullRatio);
			}
		}
	}
}

FString FTGORRigUnit_RetractGripDynamics::ProcessPinLabelForInjection(const FString& InLabel) const
{
	FString Formula;
	return FString::Printf(TEXT("%s: TODO"), *InLabel);
}
