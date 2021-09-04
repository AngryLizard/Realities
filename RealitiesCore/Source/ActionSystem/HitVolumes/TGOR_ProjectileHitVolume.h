#pragma once

#include "CoreMinimal.h"

#include "MovementSystem/TGOR_MovementInstance.h"

#include "TGOR_HitVolume.h"
#include "TGOR_ProjectileHitVolume.generated.h"

class ATGOR_PhysicsVolume;

////////////////////////////////////////////// STRUCT //////////////////////////////////////////////////////

USTRUCT(BlueprintType)
struct FTGOR_ProjectileDataInstance
{
	GENERATED_USTRUCT_BODY()

		FTGOR_ProjectileDataInstance();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "!TGOR Combat")
		FTGOR_Time Time;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "!TGOR Combat")
		int32 Hits;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "!TGOR Combat")
		FVector Target;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "!TGOR Combat")
		FTGOR_MovementDynamic Dynamic;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "!TGOR Combat")
		ATGOR_PhysicsVolume* Volume;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "!TGOR Combat")
		FTGOR_ForceInstance Forces;
};

/**
 * HitVolume consisting of one or multiple projectiles, by default does standard Rigid euler with gravity and air resistance
 */
UCLASS()
class ACTIONSYSTEM_API ATGOR_ProjectileHitVolume : public ATGOR_HitVolume
{
	GENERATED_BODY()

public:
	ATGOR_ProjectileHitVolume(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	
	virtual bool Update(float DeltaTime) override;
	virtual void Shoot(const FTGOR_AimInstance& Target, const FTGOR_ForceInstance& Forces) override;


	////////////////////////////////////////////////////////////////////////////////////////////////////
protected:

	/**  */
	UFUNCTION(BlueprintImplementableEvent, Category = "!TGOR Combat", Meta = (Keywords = "C++"))
		void OnHit(const FTGOR_ProjectileDataInstance& Projectile);

	/**  */
	UFUNCTION(BlueprintImplementableEvent, Category = "!TGOR Combat", Meta = (Keywords = "C++"))
		void OnDisplay(const FTGOR_ProjectileDataInstance& Projectile);

	////////////////////////////////////////////////////////////////////////////////////////////////////
protected:

	/** Body used for physical simulation */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "!TGOR Combat")
		FTGOR_MovementBody ProjectileBody;

	////////////////////////////////////////////////////////////////////////////////////////////////////
protected:

	/** Initial velocity projectiles spawn with */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "!TGOR Combat")
		float InitialVelocity;

	/** Projectile timeout in seconds */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "!TGOR Combat")
		float Timeout;

	/** Elasticity on bounces */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "!TGOR Combat")
		float Elasticity;

	/** Whether to do an actual hit on bounce */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "!TGOR Combat")
		bool HitOnBounce;

	/** Projectile hits before projectile dies */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "!TGOR Combat")
		int32 MaxHits;

	/** Up to which hit the instigator is to be ignored (-1 for always) */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "!TGOR Combat")
		int32 IgnoreInstigatorThreshold;

	/** Currently active projectiles */
	UPROPERTY(BlueprintReadOnly, Category = "!TGOR Combat")
		TArray<FTGOR_ProjectileDataInstance> Projectiles;

	////////////////////////////////////////////////////////////////////////////////////////////////////
protected:

	/** Channel used for collision */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "!TGOR Combat")
		TEnumAsByte<ECollisionChannel> CollisionChannel;

	/** Compute force applied to a given projectile and time ratio needed for damping */
	UFUNCTION(BlueprintPure, Category = "!TGOR Combat|Internal", Meta = (Keywords = "C++"))
		virtual FVector ComputeForce(const FTGOR_ProjectileDataInstance& Projectile) const;

	/** Trace between two points */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Combat|Internal", Meta = (Keywords = "C++"))
		bool Trace(const FVector& Start, const FVector& End, bool IgnoreInstigator, FHitResult& Hit);

	////////////////////////////////////////////////////////////////////////////////////////////////////
private:
};
