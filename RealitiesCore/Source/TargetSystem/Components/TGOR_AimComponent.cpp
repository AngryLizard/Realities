// The Gateway of Realities: Planes of Existence.


#include "TGOR_AimComponent.h"

#include "GameFramework/PlayerController.h"
#include "Engine/LocalPlayer.h"

#include "RealitiesUtility/Utility/TGOR_Math.h"

#include "TargetSystem/Components/TGOR_WorldInteractableComponent.h"
#include "DimensionSystem/Volumes/TGOR_LevelVolume.h"
#include "DimensionSystem/Data/TGOR_DimensionData.h"

#include "TargetSystem/Components/TGOR_InteractableComponent.h"
#include "TargetSystem/Content/TGOR_Target.h"

#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "EngineUtils.h"


FTGOR_WeightedPoint::FTGOR_WeightedPoint()
: Weight(0.0f),
Direction(FVector::OneVector)
{
}

////////////////////////////////////////////////////////////////////////////////////////////////////


UTGOR_AimComponent::UTGOR_AimComponent()
:	Super(),
	AngleThreshold(0.05f),
	SelfAimWeight(0.0f),
	MaxAimDistance(10'000.0f),
	ForwardAimDistance(250.0f),
	ConsiderOutOfRange(false),
	CameraTrace(ECC_GameTraceChannel1)
{
	PrimaryComponentTick.bCanEverTick = true;
	SetIsReplicatedByDefault(true);
}

void UTGOR_AimComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// Poll for inactive handles to be removed
	for (auto It = Handles.CreateIterator(); It; ++It)
	{
		const TSet<UTGOR_CoreContent*> ContentContext = It->Value.Register->Execute_GetActiveContent(It->Value.Register.GetObject());
		if (!ContentContext.Contains(It->Key))
		{
			It.RemoveCurrent();
		}
	}
}

void UTGOR_AimComponent::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// Replicate to everyone
	DOREPLIFETIME_CONDITION(UTGOR_AimComponent, AimInstance, COND_SkipOwner);
}


////////////////////////////////////////////////////////////////////////////////////////////////////


bool UTGOR_AimComponent::UpdateCandidatesNearby()
{
	Candidates.Empty();
	FCollisionShape Shape;
	Shape.SetSphere(MaxAimDistance);

	TArray<FOverlapResult> Results;
	FCollisionQueryParams QueryParams;

	// Use LevelVolume as default target (and ignore it for the trace) if available
	ETGOR_FetchEnumeration State;
	UTGOR_DimensionData* Dimension = GetDimension(State);
	if (State == ETGOR_FetchEnumeration::Found)
	{
		ATGOR_LevelVolume* LevelVolume = Dimension->GetLevelVolume();
		if (IsValid(LevelVolume))
		{
			UTGOR_InteractableComponent* Interactable = LevelVolume->FindComponentByClass<UTGOR_InteractableComponent>();
			if (IsValid(Interactable))
			{
				QueryParams.AddIgnoredActor(LevelVolume);
				Candidates.Emplace(Interactable);
			}
		}
	}

	UWorld* World = GetWorld();
	AActor* Actor = GetOwner();

	// Ignore own if selfaim is disabled (performance)
	const bool IgnoreSelf = SelfAimWeight < SMALL_NUMBER;
	if (IgnoreSelf)
	{
		QueryParams.AddIgnoredActor(Actor);
	}

	// Find nearby interactables
	FCollisionResponseParams ResponseParams;
	if (World->OverlapMultiByChannel(Results, GetComponentLocation(), FQuat::Identity, CameraTrace, Shape, QueryParams, ResponseParams))
	{
		for (const FOverlapResult& Result : Results)
		{
			UTGOR_InteractableComponent* Interactable = Cast<UTGOR_InteractableComponent>(Result.GetComponent());
			if (IsValid(Interactable))
			{
				// Ignoring attached actors
				if (!IgnoreSelf || !Result.Actor->IsAttachedTo(Actor))
				{
					Candidates.Emplace(Interactable);
				}
			}
		}
		return true;
	}
	return false;
}

void UTGOR_AimComponent::UpdateAimFromCamera(const FVector& Location, const FVector& Direction)
{
	if (IsSuspended())
	{
		return;
	}

	// Project aim origin onto AimComponent if we aren't focusing self so we don't catch things behind the character
	const FVector Pivot = GetComponentLocation();
	const FVector Offset = GetForwardVector() * ForwardAimDistance;
	const FVector Origin = Location + Direction * ((Pivot + Offset - Location) | Direction) * (1.0f - SelfAimWeight);

	// Get filter from current handles
	TSubclassOf<UTGOR_Target> Filter = GetAimFilter();

	// Query all candidates for matches
	Points.Empty();
	for (UTGOR_InteractableComponent* Candidate : Candidates)
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
				if (IsValid(Target) && Target->IsA(Filter))
				{
					FTGOR_AimPoint Point;
					Point.Target = Target;
					if (Target->SearchTarget(Candidate, Origin, Direction, MaxAimDistance, Point))
					{
						FTGOR_WeightedPoint WeightedPoint;
						WeightedPoint.Point = Point;
						WeightedPoint.Weight = Weight * Target->Importance;
						WeightedPoint.Direction = UTGOR_Math::Normalize(Point.Center - Location);
						Points.Add(WeightedPoint);
					}
				}
			}
		}
	}

	// Filter targets that are too close
	for (auto It = Points.CreateIterator(); It; ++It)
	{
		const FVector& Reference = It->Direction;

		for (const FTGOR_WeightedPoint& Point : Points)
		{
			const float Angle = Point.Direction | Reference;
			if (Angle < AngleThreshold && It->Point.Target->Importance < Point.Point.Target->Importance)
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
			FTGOR_WeightedPoint& Own = Points[i];
			for (int32 j = 0; j < Points.Num(); j++)
			{
				FTGOR_AimPoint& Their = Points[j].Point;
				if (i != j)
				{
					// Make sure result never grows to infinity, magic number 1000 to not have nearby targets intercept
					Own.Weight *= FMath::Min(Their.Distance, 1.0f);
				}
			}
			Total += Own.Weight;
		}

		// Normalize and find best point (Default to first found)
		if (Total >= SMALL_NUMBER)
		{
			for (FTGOR_WeightedPoint& Point : Points)
			{
				Point.Weight /= Total;
			}
		}
		else
		{
			Points[0].Weight = 1.0f;
		}

		// Sort points by weight, highest first
		Points.Sort([](const FTGOR_WeightedPoint& A, const FTGOR_WeightedPoint& B)->bool {return(A.Weight > B.Weight); });

		// Create actual AimInstance
		const FTGOR_AimPoint& BestPoint = Points[0].Point;
		if (ConsiderOutOfRange || BestPoint.Distance < 1.0f)
		{
			if (BestPoint.Target->ComputeTarget(BestPoint, Origin, Direction, MaxAimDistance, AimInstance))
			{
				AimInstance.Target = BestPoint.Target;
				ReplicateFocus(AimInstance);
				return;
			}
		}
	}

	// Default to worldspace
	AimInstance.Target = nullptr;
	AimInstance.Component = nullptr;
	AimInstance.Index = -1;
	AimInstance.Offset = Location + Direction * MaxAimDistance;
	ReplicateFocus(AimInstance);
}


FVector UTGOR_AimComponent::ComputeCurrentAimLocation(bool Sticky) const
{
	return ComputeAimLocation(AimInstance, Sticky);
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

void UTGOR_AimComponent::RegisterHandle(TScriptInterface<ITGOR_RegisterInterface> Register, UTGOR_CoreContent* Content)
{
	// Add handle
	FTGOR_AimSuspensionHandle& Handle = Handles.FindOrAdd(Content);
	Handle.Suspend = true;
	Handle.Register = Register;
}

void UTGOR_AimComponent::RegisterFilter(TScriptInterface<ITGOR_RegisterInterface> Register, UTGOR_CoreContent* Content, TSubclassOf<UTGOR_Target> Filter)
{
	// Add handle
	FTGOR_AimSuspensionHandle& Handle = Handles.FindOrAdd(Content);
	Handle.Filter = Filter;
	Handle.Suspend = false;
	Handle.Register = Register;
}

void UTGOR_AimComponent::UnregisterHandle(UTGOR_CoreContent* Content)
{
	// Remove handle
	//FTGOR_AimSuspensionHandle Handle;
	//Handles.RemoveAndCopyValue(Content, Handle);
	Handles.Remove(Content);
}

bool UTGOR_AimComponent::IsSuspended() const
{
	for (const auto& Pair : Handles)
	{
		if (Pair.Value.Suspend) return true;
	}
	return false;
}

TSubclassOf<UTGOR_Target> UTGOR_AimComponent::GetAimFilter() const
{
	TSubclassOf<UTGOR_Target> Filter = UTGOR_Target::StaticClass();
	for (const auto& Pair : Handles)
	{
		if (!*Pair.Value.Filter) return nullptr; // Can't intersect nothing
		if (Pair.Value.Filter->IsChildOf(Filter)) Filter = Pair.Value.Filter; // Can narrow filter
		if (!Filter->IsChildOf(Pair.Value.Filter)) return nullptr; // No intersection
	}
	return Filter;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void UTGOR_AimComponent::ReplicateFocus_Implementation(FTGOR_AimInstance Instance)
{
	AimInstance = Instance;
}

bool UTGOR_AimComponent::ReplicateFocus_Validate(FTGOR_AimInstance Instance)
{
	return true;
}


void UTGOR_AimComponent::OnRepNotifyAimInstance()
{
}

////////////////////////////////////////////////////////////////////////////////////////////////////

const FTGOR_AimInstance& UTGOR_AimComponent::GetCurrentAim() const
{
	return AimInstance;
}


bool UTGOR_AimComponent::HasTarget(TSubclassOf<UTGOR_Target> Type) const
{
	if (IsValid(AimInstance.Target))
	{
		return AimInstance.Target->IsA(Type);
	}
	return false;
}

bool UTGOR_AimComponent::IsAimingAt(AActor* Actor) const
{
	if (AimInstance.Component.IsValid())
	{
		return AimInstance.Component->GetOwner() == Actor;
	}
	return false;
}

bool UTGOR_AimComponent::IsAimingAtSelf() const
{
	return IsAimingAt(GetOwner());
}