// The Gateway of Realities: Planes of Existence.

#pragma once

#include <functional>
#include "CoreMinimal.h"

#include "Realities/Components/Movement/TGOR_PhysicsComponent.h"
#include "TGOR_ColliderComponent.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FImpactDelegate, const FTGOR_MovementDynamic&, Dynamic, const FVector&, Point, const FVector&, Impulse);

/**
 * UTGOR_ColliderComponent keeps track of owning primitive collision
 */
UCLASS(ClassGroup = (Custom), Blueprintable, meta = (BlueprintSpawnableComponent))
class REALITIES_API UTGOR_ColliderComponent : public UTGOR_PhysicsComponent
{
	GENERATED_BODY()
	
public:
	UTGOR_ColliderComponent();

	virtual void SetBaseComponent(USceneComponent* Component) override;
	virtual float ComputeInertial(const FVector& Point, const FVector& Normal) const override;

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Whether to ignore a detected trace hit component */
	UFUNCTION(BlueprintPure, Category = "TGOR Movement", Meta = (Keywords = "C++"))
		bool IgnoreTracedComponent(UPrimitiveComponent* Component) const;

	/** Performs spherical overlap to expand ignored actors list */
	UFUNCTION(BlueprintCallable, Category = "TGOR Movement", Meta = (Keywords = "C++"))
		void OverlapIgnoreList();

	/** We check for which components to ignore if they are smaller then given threshold (relative to this component) */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TGOR Movement")
		float IgnoreThreshold;

	/** We check for which components to ignore in a radius that depends on height */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TGOR Movement")
		float IgnoreRadius;

	////////////////////////////////////////////////////////////////////////////////////////////////////
protected:

	/** Does a trace according to movement collision settings */
	UFUNCTION(BlueprintCallable, Category = "TGOR Movement", Meta = (Keywords = "C++"))
		bool MovementTraceSweep(float Radius, const FVector& Location, const FVector& Translation, FHitResult& Hit) const;
		bool MovementTraceSweep(const FCollisionShape& CollisionShape, const FTGOR_MovementPosition& Position, const FVector& Translation, FHitResult& Hit) const;

	/** Probe for components to be ignored in hit detection, adds them to the moveignore list */
	UFUNCTION(BlueprintPure, Category = "TGOR Movement", Meta = (Keywords = "C++"))
		bool ProbeSweep(float Radius, const FVector& Location, const FVector& Translation) const;
		bool ProbeSweep(const FCollisionShape& CollisionShape, FTGOR_MovementPosition& Position, const FVector& Translation) const;

	/** Move with collision detection without penetration */
	//UFUNCTION(BlueprintCallable, Category = "TGOR Movement", Meta = (Keywords = "C++"))
	bool SimulateSweep(FTGOR_MovementDynamic& Dynamic, const FVector& Translation, FHitResult& Hit);
	bool FilterSweepResults(const TArray<FHitResult>& HitResults, const FVector& Translation, FHitResult& Hit) const;
	void CollectSweepResults(const TArray<FHitResult>& HitResults, const FVector& Translation) const;

	/** Does an overlap according to movement collision settings */
	//UFUNCTION(BlueprintCallable, Category = "TGOR Movement", Meta = (Keywords = "C++"))
	bool MovementTraceOverlap(float Radius, const FVector& Location, FOverlapResult& Overlap) const;
	bool MovementTraceOverlap(const FCollisionShape& CollisionShape, const FTGOR_MovementPosition& Position, FOverlapResult& Overlap) const;

	/** Probe for components to be ignored in overlap detection, adds them to the moveignore list */
	//UFUNCTION(BlueprintPure, Category = "TGOR Movement", Meta = (Keywords = "C++"))
	bool ProbeOverlap(float Radius, const FVector& Location) const;
	bool ProbeOverlap(const FCollisionShape& CollisionShape, FTGOR_MovementPosition& Position) const;

	/** Overlap with collision detection without penetration */
	//UFUNCTION(BlueprintCallable, Category = "TGOR Movement", Meta = (Keywords = "C++"))
	bool SimulateOverlap(FTGOR_MovementDynamic& Dynamic, const FVector& Translation, FOverlapResult& Overlap);
	bool FilterOverlapResults(const TArray<FOverlapResult>& OverlapResults, FOverlapResult& Overlap) const;
	void CollectOverlapResults(const TArray<FOverlapResult>& OverlapResults) const;

	/** Compute penetration adjustment */
	UFUNCTION(BlueprintCallable, Category = "TGOR Movement", Meta = (Keywords = "C++"))
		FVector GetPenetrationAdjustment(const FHitResult& Hit) const;


	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Called on impact */
	UFUNCTION(BlueprintCallable, Category = "TGOR Movement", Meta = (Keywords = "C++"))
		virtual void Impact(const FTGOR_MovementDynamic& Dynamic, const FVector& Point, const FVector& Impact);

	/** True if this object can rotate on collision impact or point infliction */
	UFUNCTION(BlueprintPure, Category = "TGOR Movement", Meta = (Keywords = "C++"))
		virtual bool CanRotateOnImpact() const;

	////////////////////////////////////////////////////////////////////////////////////////////////////
protected:

	/** Max velocity clamp to prevent NaN errors */
	UPROPERTY(BlueprintAssignable, Category = "TGOR Movement")
		FImpactDelegate OnImpact;

	/** Core component elasticity on collision response [0, 1] */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TGOR Movement")
		float Elasticity;

	/** Core component friction coefficient [0, 1] */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TGOR Movement")
		float Friction;

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Compute collision with a given relative velocity, return impulse */
	UFUNCTION(BlueprintCallable, Category = "TGOR Movement", Meta = (Keywords = "C++"))
		FVector ComputeCollisionResponse(FTGOR_MovementSpace& Space, const FVector& Point, const FVector& RelativeVelocity, const FVector& Normal, float InvInertial);

	/** Translate over a given time, return impulse */
	UFUNCTION(BlueprintCallable, Category = "TGOR Movement", Meta = (Keywords = "C++"))
		FVector ComputeFrictionResponse(FTGOR_MovementSpace& Space, const FVector& Point, const FVector& RelativeVelocity, const FVector& Normal, float InvInertial);

	/** Computes impact resolution from a given hit result, returns whether a collision actually happened */
	UFUNCTION(BlueprintCallable, Category = "TGOR Movement", Meta = (Keywords = "C++"))
		bool Collide(FTGOR_MovementSpace& Space, const FHitResult& Hit, bool Replay);


};
