// The Gateway of Realities: Planes of Existence.

#pragma once

#include "RealitiesUtility/Utility/TGOR_Error.h"

#include "CoreMinimal.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "TGOR_ProjectileMovementComponent.generated.h"

/**
* TGOR_ProjectileState stores the current state of the projectile
*/
USTRUCT(BlueprintType)
struct FTGOR_ProjectileState
{
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FRotator Rotator;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FVector Location;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FVector Velocity;
};



/**
* TGOR_LogRow stores all log information of one tick
*/
UCLASS()
class REALITIES_API UTGOR_ProjectileMovementComponent : public UProjectileMovementComponent
{
	GENERATED_BODY()
	
public:
	UTGOR_ProjectileMovementComponent();

	/** UE4 function. Called from the engine to allow specifying of variables to be replicated. */
	virtual void GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const override;

	// Called when the game starts
	virtual void BeginPlay() override;

	/** UE4 function. Called from the engine each frame to allow objects to update based on elapsed time since the previous frame. */
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	////////////////////////////////////////////////////////////////////////////////////////////////////
protected:

	/** Seconds between updates to clients. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "!TGOR Projectile")
		float ReplicationRate;

	/** Threshold when velocity gets replicated. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "!TGOR Projectile")
		float VelocityThreshold;

	UPROPERTY(ReplicatedUsing = OnReplicateState)
	FTGOR_ProjectileState State;

	/** Called on each client on replication of current state. */
	UFUNCTION()
		void OnReplicateState();

private:
	float _timer;
};
