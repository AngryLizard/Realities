// The Gateway of Realities: Planes of Existence.

#pragma once

#include "CoreMinimal.h"
#include "Navigation/PathFollowingComponent.h"
#include "TGOR_PathFollowingComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FPathFollowingCompletedSignature, FAIRequestID, RequestID, EPathFollowingResult::Type, Result);

DECLARE_LOG_CATEGORY_EXTERN(TGOR_LogPlayerNavigation, Warning, All);

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class MOVEMENTSYSTEM_API UTGOR_PathFollowingComponent : public UPathFollowingComponent
{
	GENERATED_UCLASS_BODY()

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	UFUNCTION(BlueprintCallable, Category = "!TGOR System", Meta = (AdvancedDisplay = "bStopOnOverlap,bCanStrafe,bAllowPartialPath"))
		EPathFollowingRequestResult::Type PathFollowingToActor(AActor* InGoal, float InAcceptanceRadius = -1, bool bInStopOnOverlap = true, bool bInUsePathfinding = true, bool bInCanStrafe = true, TSubclassOf<UNavigationQueryFilter> InFilterClass = NULL, bool bInAllowPartialPath = true);

	UFUNCTION(BlueprintCallable, Category = "!TGOR System", Meta = (AdvancedDisplay = "bStopOnOverlap,bCanStrafe,bAllowPartialPath"))
		EPathFollowingRequestResult::Type PathFollowingToLocation(const FVector& InDest, float InAcceptanceRadius, bool bInStopOnOverlap, bool bInUsePathfinding, bool bInProjectDestinationToNavigation, bool bInCanStrafe, TSubclassOf<UNavigationQueryFilter> InFilterClass, bool bInAllowPartialPaths);

	/** Blueprint notification that we've completed the current path following request */
	UPROPERTY(BlueprintAssignable)
		FPathFollowingCompletedSignature OnPathFollowingCompleted;

	////////////////////////////////////////////////////////////////////////////////////////////////////
private:

	FPathFollowingRequestResult MoveTo(AController* Controller, const FAIMoveRequest& MoveRequest, FNavPathSharedPtr* OutPath = nullptr);
	bool BuildPathfindingQuery(AController* Controller, const FAIMoveRequest& MoveRequest, FPathFindingQuery& Query);
	virtual void FindPathForMoveRequest(AController* Controller, const FAIMoveRequest& MoveRequest, FPathFindingQuery& Query, FNavPathSharedPtr& OutPath) const;
	virtual void OnMoveCompleted(FAIRequestID RequestID, const FPathFollowingResult& Result);

	/** Component used for moving along a path. */
	UPROPERTY(VisibleDefaultsOnly, Category = AI)
		TObjectPtr<UPathFollowingComponent> PathFollowingComponent;

	////////////////////////////////////////////////////////////////////////////////////////////////////
protected:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AI)
		TSubclassOf<UNavigationQueryFilter> DefaultNavigationFilterClass;

	/** Is strafing allowed during movement? */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AI)
		uint32 bAllowStrafe : 1;
};