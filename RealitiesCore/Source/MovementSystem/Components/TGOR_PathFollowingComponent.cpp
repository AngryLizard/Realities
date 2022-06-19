// The Gateway of Realities: Planes of Existence.


#include "TGOR_PathFollowingComponent.h"

#include "NavigationSystem.h"
#include "VisualLogger/VisualLoggerTypes.h"
#include "VisualLogger/VisualLogger.h"

DEFINE_LOG_CATEGORY(TGOR_LogPlayerNavigation);

UTGOR_PathFollowingComponent::UTGOR_PathFollowingComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	OnRequestFinished.AddUObject(this, &UTGOR_PathFollowingComponent::OnMoveCompleted);

	// TODO: Factor 3 is going to work for most pawns to properly "reach" the desitionation even if it's on the floot, but there might be better ways of doing this
	SetPreciseReachThreshold(1.f, 3.f);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
// Most of this is copied from AIController

EPathFollowingRequestResult::Type UTGOR_PathFollowingComponent::PathFollowingToActor(AActor* InGoal, float InAcceptanceRadius, bool bInStopOnOverlap, bool bInUsePathfinding, bool bInCanStrafe, TSubclassOf<UNavigationQueryFilter> InFilterClass, bool bInAllowPartialPaths)
{
	AController* Controller = Cast<AController>(GetOwner());
	if (!IsValid(Controller))
	{
		UE_VLOG(this, TGOR_LogPlayerNavigation, Log, TEXT("PathFollowing is only enabled on controllers"));
	}

	// abort active movement to keep only one request running
	if (GetStatus() != EPathFollowingStatus::Idle)
	{
		AbortMove(*Controller, FPathFollowingResultFlags::ForcedScript | FPathFollowingResultFlags::NewRequest, FAIRequestID::CurrentRequest, EPathFollowingVelocityMode::Keep);
	}

	FAIMoveRequest MoveReq(InGoal);
	MoveReq.SetUsePathfinding(bInUsePathfinding);
	MoveReq.SetAllowPartialPath(bInAllowPartialPaths);
	MoveReq.SetNavigationFilter(*InFilterClass ? InFilterClass : DefaultNavigationFilterClass);
	MoveReq.SetAcceptanceRadius(InAcceptanceRadius);
	MoveReq.SetReachTestIncludesAgentRadius(bInStopOnOverlap);
	MoveReq.SetCanStrafe(bInCanStrafe);

	return MoveTo(Controller, MoveReq);
}

EPathFollowingRequestResult::Type UTGOR_PathFollowingComponent::PathFollowingToLocation(const FVector& InDest, float InAcceptanceRadius, bool bInStopOnOverlap, bool bInUsePathfinding, bool bInProjectDestinationToNavigation, bool bInCanStrafe, TSubclassOf<UNavigationQueryFilter> InFilterClass, bool bInAllowPartialPaths)
{
	AController* Controller = Cast<AController>(GetOwner());
	if (!IsValid(Controller))
	{
		UE_VLOG(this, TGOR_LogPlayerNavigation, Log, TEXT("PathFollowing is only enabled on controllers"));
	}

	// abort active movement to keep only one request running
	if (GetStatus() != EPathFollowingStatus::Idle)
	{
		AbortMove(*Controller, FPathFollowingResultFlags::ForcedScript | FPathFollowingResultFlags::NewRequest, FAIRequestID::CurrentRequest, EPathFollowingVelocityMode::Keep);
	}

	FAIMoveRequest MoveReq(InDest);
	MoveReq.SetUsePathfinding(bInUsePathfinding);
	MoveReq.SetAllowPartialPath(bInAllowPartialPaths);
	MoveReq.SetProjectGoalLocation(bInProjectDestinationToNavigation);
	MoveReq.SetNavigationFilter(*InFilterClass ? InFilterClass : DefaultNavigationFilterClass);
	MoveReq.SetAcceptanceRadius(InAcceptanceRadius);
	MoveReq.SetReachTestIncludesAgentRadius(bInStopOnOverlap);
	MoveReq.SetCanStrafe(bInCanStrafe);

	return MoveTo(Controller, MoveReq);
}

FPathFollowingRequestResult UTGOR_PathFollowingComponent::MoveTo(AController* Controller, const FAIMoveRequest& MoveRequest, FNavPathSharedPtr* OutPath)
{
	// both MoveToActor and MoveToLocation can be called from blueprints/script and should keep only single movement request at the same time.
	// this function is entry point of all movement mechanics - do NOT abort in here, since movement may be handled by AITasks, which support stacking 

	UE_VLOG(this, TGOR_LogPlayerNavigation, Log, TEXT("MoveTo: %s"), *MoveRequest.ToString());

	FPathFollowingRequestResult ResultData;
	ResultData.Code = EPathFollowingRequestResult::Failed;

	if (MoveRequest.IsValid() == false)
	{
		UE_VLOG(this, TGOR_LogPlayerNavigation, Error, TEXT("MoveTo request failed due MoveRequest not being valid. Most probably desired Goal Actor not longer exists. MoveRequest: '%s'"), *MoveRequest.ToString());
		return ResultData;
	}

	ensure(MoveRequest.GetNavigationFilter() || !DefaultNavigationFilterClass);

	bool bCanRequestMove = true;
	bool bAlreadyAtGoal = false;

	if (!MoveRequest.IsMoveToActorRequest())
	{
		if (MoveRequest.GetGoalLocation().ContainsNaN() || FAISystem::IsValidLocation(MoveRequest.GetGoalLocation()) == false)
		{
			UE_VLOG(this, TGOR_LogPlayerNavigation, Error, TEXT("AAIController::MoveTo: Destination is not valid! Goal(%s)"), TEXT_AI_LOCATION(MoveRequest.GetGoalLocation()));
			bCanRequestMove = false;
		}

		// fail if projection to navigation is required but it failed
		if (bCanRequestMove && MoveRequest.IsProjectingGoal())
		{
			UNavigationSystemV1* NavSys = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld());
			const FNavAgentProperties& AgentProps = Controller->GetNavAgentPropertiesRef();
			FNavLocation ProjectedLocation;

			if (NavSys && !NavSys->ProjectPointToNavigation(MoveRequest.GetGoalLocation(), ProjectedLocation, INVALID_NAVEXTENT, &AgentProps))
			{
				if (MoveRequest.IsUsingPathfinding())
				{
					UE_VLOG_LOCATION(this, TGOR_LogPlayerNavigation, Error, MoveRequest.GetGoalLocation(), 30.f, FColor::Red, TEXT("AAIController::MoveTo failed to project destination location to navmesh"));
				}
				else
				{
					UE_VLOG_LOCATION(this, TGOR_LogPlayerNavigation, Error, MoveRequest.GetGoalLocation(), 30.f, FColor::Red, TEXT("AAIController::MoveTo failed to project destination location to navmesh, path finding is disabled perhaps disable goal projection ?"));
				}

				bCanRequestMove = false;
			}

			MoveRequest.UpdateGoalLocation(ProjectedLocation.Location);
		}

		bAlreadyAtGoal = bCanRequestMove && HasReached(MoveRequest);
	}
	else
	{
		bAlreadyAtGoal = bCanRequestMove && HasReached(MoveRequest);
	}

	if (bAlreadyAtGoal)
	{
		UE_VLOG(this, TGOR_LogPlayerNavigation, Log, TEXT("MoveTo: already at goal!"));
		ResultData.MoveId = RequestMoveWithImmediateFinish(EPathFollowingResult::Success);
		ResultData.Code = EPathFollowingRequestResult::AlreadyAtGoal;
	}
	else if (bCanRequestMove)
	{
		FPathFindingQuery PFQuery;

		const bool bValidQuery = BuildPathfindingQuery(Controller, MoveRequest, PFQuery);
		if (bValidQuery)
		{
			FNavPathSharedPtr QueryPath;
			FindPathForMoveRequest(Controller, MoveRequest, PFQuery, QueryPath);

			const FAIRequestID RequestID = QueryPath.IsValid() ? RequestMove(MoveRequest, QueryPath) : FAIRequestID::InvalidRequest;
			if (RequestID.IsValid())
			{
				bAllowStrafe = MoveRequest.CanStrafe();
				ResultData.MoveId = RequestID;
				ResultData.Code = EPathFollowingRequestResult::RequestSuccessful;

				if (OutPath)
				{
					*OutPath = QueryPath;
				}
			}
		}
	}

	if (ResultData.Code == EPathFollowingRequestResult::Failed)
	{
		ResultData.MoveId = RequestMoveWithImmediateFinish(EPathFollowingResult::Invalid);
	}

	return ResultData;
}

bool UTGOR_PathFollowingComponent::BuildPathfindingQuery(AController* Controller, const FAIMoveRequest& MoveRequest, FPathFindingQuery& Query)
{
	bool bResult = false;

	UNavigationSystemV1* NavSys = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld());
	const ANavigationData* NavData = (NavSys == nullptr) ? nullptr :
		MoveRequest.IsUsingPathfinding() ? NavSys->GetNavDataForProps(Controller->GetNavAgentPropertiesRef(), Controller->GetNavAgentLocation()) :
		NavSys->GetAbstractNavData();

	if (NavData)
	{
		FVector GoalLocation = MoveRequest.GetGoalLocation();
		if (MoveRequest.IsMoveToActorRequest())
		{
			const INavAgentInterface* NavGoal = Cast<const INavAgentInterface>(MoveRequest.GetGoalActor());
			if (NavGoal)
			{
				const FVector Offset = NavGoal->GetMoveGoalOffset(Controller);
				GoalLocation = FQuatRotationTranslationMatrix(MoveRequest.GetGoalActor()->GetActorQuat(), NavGoal->GetNavAgentLocation()).TransformPosition(Offset);
			}
			else
			{
				GoalLocation = MoveRequest.GetGoalActor()->GetActorLocation();
			}
		}

		FSharedConstNavQueryFilter NavFilter = UNavigationQueryFilter::GetQueryFilter(*NavData, Controller, MoveRequest.GetNavigationFilter());
		Query = FPathFindingQuery(*Controller, *NavData, Controller->GetNavAgentLocation(), GoalLocation, NavFilter);
		Query.SetAllowPartialPaths(MoveRequest.IsUsingPartialPaths());

		OnPathfindingQuery(Query);
		bResult = true;
	}
	else
	{
		if (NavSys == nullptr)
		{
			UE_VLOG(this, TGOR_LogPlayerNavigation, Warning, TEXT("Unable AAIController::BuildPathfindingQuery due to no NavigationSystem present. Note that even pathfinding-less movement requires presence of NavigationSystem."));
		}
		else
		{
			UE_VLOG(this, TGOR_LogPlayerNavigation, Warning, TEXT("Unable to find NavigationData instance while calling AAIController::BuildPathfindingQuery"));
		}
	}

	return bResult;
}

void UTGOR_PathFollowingComponent::FindPathForMoveRequest(AController* Controller, const FAIMoveRequest& MoveRequest, FPathFindingQuery& Query, FNavPathSharedPtr& OutPath) const
{
	UNavigationSystemV1* NavSys = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld());
	if (NavSys)
	{
		FPathFindingResult PathResult = NavSys->FindPathSync(Query);
		if (PathResult.Result != ENavigationQueryResult::Error)
		{
			if (PathResult.IsSuccessful() && PathResult.Path.IsValid())
			{
				if (MoveRequest.IsMoveToActorRequest())
				{
					PathResult.Path->SetGoalActorObservation(*MoveRequest.GetGoalActor(), 100.0f);
				}

				PathResult.Path->EnableRecalculationOnInvalidation(true);
				OutPath = PathResult.Path;
			}
		}
		else
		{
			UE_VLOG(this, TGOR_LogPlayerNavigation, Error, TEXT("Trying to find path to %s resulted in Error")
				, MoveRequest.IsMoveToActorRequest() ? *GetNameSafe(MoveRequest.GetGoalActor()) : *MoveRequest.GetGoalLocation().ToString());
			UE_VLOG_SEGMENT(this, TGOR_LogPlayerNavigation, Error, Controller->GetPawn() ? Controller->GetPawn()->GetActorLocation() : FAISystem::InvalidLocation
				, MoveRequest.GetGoalLocation(), FColor::Red, TEXT("Failed move to %s"), *GetNameSafe(MoveRequest.GetGoalActor()));
		}
	}
}

void UTGOR_PathFollowingComponent::OnMoveCompleted(FAIRequestID RequestID, const FPathFollowingResult& Result)
{
	OnPathFollowingCompleted.Broadcast(RequestID, Result.Code);
}