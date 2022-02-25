// The Gateway of Realities: Planes of Existence.

#pragma once

#include "CoreMinimal.h"
#include "Utility/Error/TGOR_Error.h"

#include "Components/SceneComponent.h"
#include "TGOR_CollisionSpheresComponent.generated.h"

class UTGOR_CollisionSphereComponent;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class REALITIES_API UTGOR_CollisionSpheresComponent : public USceneComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UTGOR_CollisionSpheresComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	// Probes all spheres
	bool Probe(const FVector& Delta, const FQuat& NewRotation, bool bSweep, FHitResult& OutHit, EMoveComponentFlags MoveFlags, ETeleportType Teleport);

	/** Probes for collision from the center */
	FVector ProbeCenter();

	/** Finds spherecomponents in this component */
	UFUNCTION(BlueprintCallable, Category = "TGOR|Movement", Meta = (Keywords = "C++"))
		void Build();

	/** currently loaded sphere components */
	UPROPERTY(BlueprintReadOnly, Category = "TGOR|Movement")
		TArray<UTGOR_CollisionSphereComponent*> SphereComponents;
};
