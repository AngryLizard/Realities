// The Gateway of Realities: Planes of Existence.

#pragma once

#include "CoreMinimal.h"
#include "DimensionSystem/TGOR_PilotInstance.h"

#include "TGOR_MobilityComponent.h"
#include "TGOR_ColliderComponent.generated.h"

DECLARE_STATS_GROUP(TEXT("Collider Movement"), STATGROUP_COLLIDER, STATCAT_Advanced);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FImpactDelegate, const FTGOR_MovementDynamic&, Dynamic, const FVector&, Point, const FVector&, Impulse);

/**
 * UTGOR_ColliderComponent manages collisions with other colliders
 */
UCLASS(ClassGroup = (Custom), Blueprintable, meta = (BlueprintSpawnableComponent))
class DIMENSIONSYSTEM_API UTGOR_ColliderComponent : public UTGOR_MobilityComponent
{
	GENERATED_BODY()
	
public:
	UTGOR_ColliderComponent();

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Whether to ignore a detected trace hit component */
	UFUNCTION(BlueprintPure, Category = "!TGOR Collision|Internal", Meta = (Keywords = "C++"))
		bool IgnoreTracedComponent(UPrimitiveComponent* Component) const;

	/** Performs spherical overlap to expand ignored actors list */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Collision|Internal", Meta = (Keywords = "C++"))
		void OverlapIgnoreList();

	/** We check for which components to ignore if they are smaller then given threshold (relative to this component) */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "!TGOR Collision")
		float IgnoreThreshold;

	/** We check for which components to ignore in a radius that depends on height */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "!TGOR Collision")
		float IgnoreRadius;

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Does a sphere trace according to movement collision settings */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Collision", Meta = (Keywords = "C++"))
		bool MovementSphereTraceSweep(float Radius, const FVector& Location, const FVector& Translation, FHitResult& Hit) const;

	/** Does a trace according to movement collision settings */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Collision", Meta = (Keywords = "C++"))
		bool MovementTraceSweep(const FTGOR_MovementPosition& Position, const FVector& Translation, FHitResult& Hit) const;
		bool MovementTraceSweep(const FCollisionShape& CollisionShape, const FTGOR_MovementPosition& Position, const FVector& Translation, FHitResult& Hit) const;

	/** Probe for components to be ignored in hit detection, adds them to the moveignore list */
	UFUNCTION(BlueprintPure, Category = "!TGOR Collision", Meta = (Keywords = "C++"))
		bool ProbeSweep(float Radius, const FVector& Location, const FVector& Translation);
		bool ProbeSweep(const FCollisionShape& CollisionShape, FTGOR_MovementPosition& Position, const FVector& Translation);

	/** Move with collision detection without penetration */
	//UFUNCTION(BlueprintCallable, Category = "!TGOR Movement", Meta = (Keywords = "C++"))
	bool SimulateSweep(FTGOR_MovementDynamic& Dynamic, const FVector& Translation, FHitResult& Hit);
	bool FilterSweepResults(const TArray<FHitResult>& HitResults, const FVector& Translation, FHitResult& Hit) const;
	void CollectSweepResults(const TArray<FHitResult>& HitResults, const FVector& Translation);

	/** Does an overlap according to movement collision settings */
	//UFUNCTION(BlueprintCallable, Category = "!TGOR Movement", Meta = (Keywords = "C++"))
	bool MovementTraceOverlap(float Radius, const FVector& Location, FOverlapResult& Overlap) const;
	bool MovementTraceOverlap(const FCollisionShape& CollisionShape, const FTGOR_MovementPosition& Position, FOverlapResult& Overlap) const;

	/** Probe for components to be ignored in overlap detection, adds them to the moveignore list */
	//UFUNCTION(BlueprintPure, Category = "!TGOR Movement", Meta = (Keywords = "C++"))
	bool ProbeOverlap(float Radius, const FVector& Location);
	bool ProbeOverlap(const FCollisionShape& CollisionShape, const FTGOR_MovementPosition& Position);

	/** Overlap with collision detection without penetration */
	//UFUNCTION(BlueprintCallable, Category = "!TGOR Movement", Meta = (Keywords = "C++"))
	bool SimulateOverlap(FTGOR_MovementDynamic& Dynamic, const FVector& Translation, FOverlapResult& Overlap);
	bool FilterOverlapResults(const TArray<FOverlapResult>& OverlapResults, FOverlapResult& Overlap) const;
	void CollectOverlapResults(const TArray<FOverlapResult>& OverlapResults);

	/** Compute penetration adjustment */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Collision|Internal", Meta = (Keywords = "C++"))
		FVector GetPenetrationAdjustment(const FHitResult& Hit) const;

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Called on impact */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Collision|Internal", Meta = (Keywords = "C++"))
		virtual void Impact(const FTGOR_MovementDynamic& Dynamic, const FVector& Point, const FVector& Impact);

	/** True if this object can rotate on collision impact or point infliction */
	UFUNCTION(BlueprintPure, Category = "!TGOR Collision|Internal", Meta = (Keywords = "C++"))
		virtual bool CanRotateOnImpact() const;

	////////////////////////////////////////////////////////////////////////////////////////////////////
protected:

	/** Max velocity clamp to prevent NaN errors */
	UPROPERTY(BlueprintAssignable, Category = "!TGOR Collision")
		FImpactDelegate OnImpact;

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Inflicts an impulse to a given location and part index (does nothing by default) */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Collision", Meta = (Keywords = "C++"))
		virtual void InflictPointImpact(const FVector& Point, const FVector& Impulse);

	/** Inflicts a force to a given location (does nothing by default) */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Collision", Meta = (Keywords = "C++"))
		virtual void InflictPointForce(const FVector& Point, const FVector& Force, float DeltaTime);

	/** Computes inertial torque factor along a normal (used by collision response) */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Collision|Internal", Meta = (Keywords = "C++"))
		float ComputeInertial(const FVector& Point, const FVector& Normal) const;

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:
	
	/** Compute collision with a given relative velocity, return impulse */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Collision|Internal", Meta = (Keywords = "C++"))
		FVector ComputeCollisionResponse(FTGOR_MovementSpace& Space, const FVector& Point, const FVector& RelativeVelocity, const FVector& Normal, float InvInertial, float Elasticity);

	/** Translate over a given time, return impulse */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Collision|Internal", Meta = (Keywords = "C++"))
		FVector ComputeFrictionResponse(FTGOR_MovementSpace& Space, const FVector& Point, const FVector& RelativeVelocity, const FVector& Normal, float InvInertial, float Friction);

	/** Computes impact resolution from a given hit result, returns whether a collision actually happened */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Collision|Internal", Meta = (Keywords = "C++"))
		bool Collide(FTGOR_MovementSpace& Space, const FHitResult& Hit, float Elasticity, float Friction);

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Max collision iterations */
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		int32 MaxCollisionIterations;

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:
	/** Simulate move for a given time */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Collision|Internal", Meta = (Keywords = "C++"))
		void SimulateMove(UPARAM(Ref)FTGOR_MovementSpace& Space, const FTGOR_MovementPosition& Offset, float Timestep, bool Sweep, float Elasticity, float Friction, FTGOR_MovementImpact& ImpactResult);

	/** Translate over a given time, returns hit form last iteration */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Collision|Internal", Meta = (Keywords = "C++"))
		int32 SimulateTranslate(UPARAM(Ref)FTGOR_MovementSpace& Space, const FVector& Offset, float Timestep, bool Sweep, float Elasticity, float Friction, FTGOR_MovementImpact& ImpactResult, float Ratio, int32 Iteration);

	/** Tries to resolve a penetration */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Collision|Internal", Meta = (Keywords = "C++"))
		bool PullBack(UPARAM(Ref)FTGOR_MovementSpace& Space, const FHitResult& OutHit, const FVector& Translation);

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Performs a sphere trace from origin in any direction, parameters given in world space */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Movement", Meta = (Keywords = "C++"))
		bool CenteredTrace(const FVector& Location, const FVector& Offset, const FVector& Direction, float Distance, float Radius, FHitResult& HitOut) const;

	/** Performs four traces for smoother ground detection, uses principal to offset samples.
	 *	The triangulated normal is given in HitOut.ImpactNormal, the averaged weight is given in HitOut.Normal.
	 *	Parameters given in local space (state) */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Movement", Meta = (Keywords = "C++"))
		bool SampleTrace(const FVector& Location, const FVector& Direction, const FVector& Principal, float Distance, float Radius, FHitResult& HitOut) const;

	/** Performs a capsule overlap from origin */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Movement", Meta = (Keywords = "C++"))
		bool Overlap(const FVector& Location, const FQuat& Rotation, float Radius, float HalfSize, FHitResult& HitOut) const;

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Performs a sphere trace from origin in any direction, parameters given in visual local space */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Movement", Meta = (Keywords = "C++"))
		void VisualTrace(USceneComponent* VisualComponent, const FVector& Offset, const FVector& Direction, float Distance, float Radius, FTGOR_VisualContact& Contact) const;
};
