// The Gateway of Realities: Planes of Existence.

#pragma once

#include "CoreMinimal.h"
#include "Utility/Error/TGOR_Error.h"

#include "Components/SphereComponent.h"
#include "TGOR_CollisionSphereComponent.generated.h"

/**
 * 
 */
UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class REALITIES_API UTGOR_CollisionSphereComponent : public USphereComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UTGOR_CollisionSphereComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:

	/** Probes for collision from cached to new location */
		bool Probe(const FVector& Delta, FHitResult& Hit, EMoveComponentFlags MoveFlags, ETeleportType Teleport);

	/** Probes for collision from the center */
		FVector ProbeCenter();

	/** Stores current state for later probe */
		void PrepareProbe();

	/** Last cached location */
	UPROPERTY(BlueprintReadOnly, Category = "TGOR|Movement")
		FVector LocationCache;
};
