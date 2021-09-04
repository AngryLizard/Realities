// The Gateway of Realities: Planes of Existence.

#pragma once

#include "TGOR_Behaviour.h"
#include "TGOR_FollowBehaviour.generated.h"

class UTGOR_ColliderComponent;

/**
 * 
 */
UCLASS(Blueprintable)
class AISYSTEM_API UTGOR_FollowBehaviour : public UTGOR_Behaviour
{
	GENERATED_BODY()
	
public:
	UTGOR_FollowBehaviour();

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Predict location of given space (time in seconds) */
	UFUNCTION(BlueprintPure, Category = "!TGOR NPC", Meta = (Keywords = "C++"))
		FVector PredictLocation(const FTGOR_MovementDynamic& Dynamic, float Time) const;

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Compute gradient on own velocity for an object to follow another at a given distance */
	UFUNCTION(BlueprintPure, Category = "!TGOR NPC", Meta = (Keywords = "C++"))
		FVector ComputeFollowGradient(const FTGOR_MovementDynamic& Dynamic, const FTGOR_MovementDynamic& Other, float Distance) const;

	/** Compute gradient on own velocity for an object to avoid a given collision result */
	UFUNCTION(BlueprintPure, Category = "!TGOR NPC", Meta = (Keywords = "C++"))
		FVector ComputeCollisionGradient(const FTGOR_MovementDynamic& Dynamic, UTGOR_ColliderComponent* Collision, const FTGOR_MovementDynamic& Other, float Distance) const;

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Compute time when two objects are going to be closest assuming linear movement */
	UFUNCTION(BlueprintPure, Category = "!TGOR NPC", Meta = (Keywords = "C++"))
		float ComputeClosestTime(const FTGOR_MovementDynamic& Dynamic, const FTGOR_MovementDynamic& Other) const;

	/** Compute error (half square distance) for two objects being closest */
	UFUNCTION(BlueprintPure, Category = "!TGOR NPC", Meta = (Keywords = "C++"))
		float ComputeClosestError(const FTGOR_MovementDynamic& Dynamic, const FTGOR_MovementObstacle& Other) const;

	/** Compute gradient on own velocity for two objects being closest */
	UFUNCTION(BlueprintPure, Category = "!TGOR NPC", Meta = (Keywords = "C++"))
		FVector ComputeClosestGradient(const FTGOR_MovementDynamic& Dynamic, const FTGOR_MovementObstacle& Other) const;

	/** Tests finite difference for closest gradient, returns error */
	UFUNCTION(BlueprintPure, Category = "!TGOR NPC", Meta = (Keywords = "C++"))
		FVector ComputeClosestGradientFd(const FTGOR_MovementDynamic& Dynamic, const FTGOR_MovementObstacle& Other) const;

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Compute collision avoidance for all tracked colliders. */
	UFUNCTION(BlueprintCallable, Category = "!TGOR NPC", Meta = (Keywords = "C++"))
		FVector ComputeColliderAvoidanceGradient(const FTGOR_MovementDynamic& Dynamic, const TArray<FTGOR_MovementObstacle>& Colliders, float MaxTime) const;
};
