// The Gateway of Realities: Planes of Existence.


#include "TGOR_AimComponent.h"

#include "GameFramework/PlayerController.h"
#include "Engine/LocalPlayer.h"

#include "RealitiesUtility/Utility/TGOR_Math.h"

#include "TargetSystem/Components/TGOR_WorldInteractableComponent.h"
#include "DimensionSystem/Volumes/TGOR_LevelVolume.h"
#include "DimensionSystem/Data/TGOR_DimensionData.h"

#include "TargetSystem/Components/TGOR_AimTargetComponent.h"
#include "TargetSystem/Content/TGOR_Target.h"

#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "EngineUtils.h"

////////////////////////////////////////////////////////////////////////////////////////////////////

UTGOR_AimComponent::UTGOR_AimComponent()
:	Super()
{
}

void UTGOR_AimComponent::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

bool UTGOR_AimComponent::UpdateCandidatesNearby(TScriptInterface<ITGOR_AimReceiverInterface> Receiver, const FVector& Center, TEnumAsByte<ECollisionChannel> TraceChannel, float Radius /*= 10000.0f*/, bool bIgnoreSelf /*= true*/)
{
	if (!ensureMsgf(Receiver, TEXT("Invalid aim receiver")))
	{
		return false;
	}

	Candidates.Empty();
	FCollisionShape Shape;
	Shape.SetSphere(Radius);

	TArray<FOverlapResult> Results;
	FCollisionQueryParams QueryParams = FCollisionQueryParams::DefaultQueryParam;

	// Use LevelVolume as default target (and ignore it for the trace) if available
	ETGOR_FetchEnumeration State;
	UTGOR_DimensionData* Dimension = GetDimension(State);
	if (State == ETGOR_FetchEnumeration::Found)
	{
		ATGOR_LevelVolume* LevelVolume = Dimension->GetLevelVolume();
		if (IsValid(LevelVolume))
		{
			UTGOR_AimTargetComponent* Interactable = LevelVolume->FindComponentByClass<UTGOR_AimTargetComponent>();
			if (IsValid(Interactable) && Receiver->Execute_TestAimCandidate(Receiver.GetObject(), Interactable))
			{
				QueryParams.AddIgnoredActor(LevelVolume);
				Candidates.Emplace(Interactable);
			}
		}
	}

	UWorld* World = GetWorld();
	AActor* Actor = GetOwner();

	// Ignore own if selfaim is disabled
	if (bIgnoreSelf)
	{
		QueryParams.AddIgnoredActor(Actor);
	}

	// Find nearby interactables
	FCollisionResponseParams ResponseParams = FCollisionResponseParams::DefaultResponseParam;
	if (World->OverlapMultiByChannel(Results, Center, FQuat::Identity, TraceChannel, Shape, QueryParams, ResponseParams))
	{
		for (const FOverlapResult& Result : Results)
		{
			UTGOR_AimTargetComponent* Interactable = Cast<UTGOR_AimTargetComponent>(Result.GetComponent());
			if (IsValid(Interactable) && Result.Component.IsValid() && Receiver->Execute_TestAimCandidate(Receiver.GetObject(), Interactable))
			{
				// Ignoring attached actors
				if (!bIgnoreSelf || !Result.Component->GetOwner()->IsAttachedTo(Actor))
				{
					Candidates.Emplace(Interactable);
				}
			}
		}
		return true;
	}
	return false;
}

bool UTGOR_AimComponent::UpdatePointsFromCamera(TScriptInterface<ITGOR_AimReceiverInterface> Receiver, const FVector& Location, const FVector& Direction, TSubclassOf<UTGOR_Target> Filter /*= nullptr*/, float AngleThreshold /*= 0.05f*/, float SelfAimWeight /*= 1.0f*/, bool bIgnoreFront /*= true*/, float frontAimDistance /*= 0.0f*/)
{
	if (!ensureMsgf(Receiver, TEXT("Invalid aim receiver")))
	{
		return false;
	}

	// Query targets from input view
	FTGOR_AimQueryParams Params;
	FVector Origin = Location;

	if (bIgnoreFront)
	{
		// Project aim origin onto AimComponent if we aren't focusing self so we don't catch things behind the character
		const FVector Pivot = GetComponentLocation();
		Origin += Direction * (((Pivot - Location) | Direction) + frontAimDistance);
	}

	// Query all candidates for matches
	Points.Empty();
	for (UTGOR_AimTargetComponent* Candidate : Candidates)
	{
		// Adapt starting weight depending on whether target is owner
		const AActor* SelfOwner = GetOwner();
		const AActor* OtherOwner = Candidate->GetOwner();
		const float Weight = (OtherOwner == SelfOwner || OtherOwner->IsAttachedTo(SelfOwner)) ? SelfAimWeight : 1.0f;
		if (Weight >= SMALL_NUMBER)
		{
			// Grab all target points
			for (UTGOR_Target* Target : Candidate->Targets)
			{
				if (!IsValid(Target) || 
					(*Filter && !Target->IsA(Filter)) ||
					!Target->TargetCondition(Candidate, this))
				{
					continue;
				}

				FTGOR_WeightedAimPoint WeightedPoint;
				if (!Target->ComputeTarget(Candidate, Origin, Direction, WeightedPoint.Point))
				{
					continue;
				}

				WeightedPoint.Point.Instance.Target = Target;
				if (Receiver->Execute_TestAimTarget(Receiver.GetObject(), WeightedPoint.Point.Instance))
				{
					WeightedPoint.Weight = Weight * Target->Importance;
					WeightedPoint.Direction = UTGOR_Math::Normalize(WeightedPoint.Point.Center - Origin);
					Points.Add(WeightedPoint);
				}
			}
		}
	}

	// Filter targets that are too close to each other
	for (auto It = Points.CreateIterator(); It; ++It)
	{
		const FVector& Reference = It->Direction;

		for (const FTGOR_WeightedAimPoint& Point : Points)
		{
			const float Angle = Point.Direction | Reference;
			if (!IsValid(It->Point.Instance.Target) || 
				(Angle < AngleThreshold && It->Point.Instance.Target->Importance < Point.Point.Instance.Target->Importance))
			{
				It.RemoveCurrent();
				break;
			}
		}
	}

	// Make sure any points have been found
	if (Points.Num() > 0)
	{
		// Compute weights
		float Total = 0.0f;
		for (int32 i = 0; i < Points.Num(); i++)
		{
			FTGOR_WeightedAimPoint& Own = Points[i];
			for (int32 j = 0; j < Points.Num(); j++)
			{
				FTGOR_AimPoint& Their = Points[j].Point;
				if (i != j)
				{
					// Accumulate weights, make sure result never grows to infinity
					Own.Weight *= FMath::Min(Their.Distance, 1.0f);
				}
			}
			Total += Own.Weight;
		}

		// Normalize and find best point (Default to first found)
		if (Total >= SMALL_NUMBER)
		{
			for (FTGOR_WeightedAimPoint& Point : Points)
			{
				Point.Weight /= Total;
			}
		}
		else
		{
			Points[0].Weight = 1.0f;
		}

		// Sort points by weight, highest first
		Points.Sort([](const FTGOR_WeightedAimPoint& A, const FTGOR_WeightedAimPoint& B)->bool {return(A.Weight > B.Weight); });
	}
	return true;
}

bool UTGOR_AimComponent::UpdateAimFromPoints(TScriptInterface<ITGOR_AimReceiverInterface> Receiver, bool bConsiderOutOfRange /*= false*/)
{
	if (!ensureMsgf(Receiver, TEXT("Invalid aim receiver")))
	{
		return false;
	}

	if (Points.Num() > 0)
	{
		// Create actual AimInstance
		const FTGOR_AimPoint& BestPoint = Points[0].Point;
		if (bConsiderOutOfRange || BestPoint.Distance < 1.0f)
		{
			Receiver->Execute_ApplyAimTarget(Receiver.GetObject(), BestPoint.Instance);
			return true;
		}
	}
	return false;
}

void UTGOR_AimComponent::ResetAim()
{
	Candidates.Reset();
	Points.Reset();
}

FVector UTGOR_AimComponent::ComputeAimLocation(const FTGOR_AimInstance& Instance, bool Sticky)
{
	// Make sure component is either valid or undefined
	if (IsValid(Instance.Target) && Instance.Component.IsValid())
	{
		if (Sticky)
		{
			return(Instance.Target->QueryStickyLocation(Instance));
		}
		return(Instance.Target->QueryAimLocation(Instance));
	}

	// Default to worldspace if there is no target
	return(Instance.Offset);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

TArray<UTGOR_AimTargetComponent*> UTGOR_AimComponent::GetNearbyCandidates(const FVector& Center, TSubclassOf<UTGOR_Target> Filter, float MaxDistance) const
{
	// TODO: Use heap structure for better performance
	// Find matching candidates
	TArray<TPair<UTGOR_AimTargetComponent*, float>> CandidateDistances;
	for (UTGOR_AimTargetComponent* Candidate : Candidates)
	{
		const float DistanceSqr = (Candidate->GetComponentLocation() - Center).SizeSquared();
		if (DistanceSqr < MaxDistance * MaxDistance && Candidate->Targets.ContainsByPredicate([Filter](UTGOR_Target* Target) { return Target->IsA(Filter); }))
		{
			CandidateDistances.Emplace(Candidate, DistanceSqr);
		}
	}

	// Sort by distance
	CandidateDistances.Sort([](const auto& A, const auto& B) { return A.Value < B.Value; });

	// Compile output interactions
	TArray<UTGOR_AimTargetComponent*> Output;
	for (const auto& Pair : CandidateDistances)
	{
		Output.Emplace(Pair.Key);
	}
	return Output;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
