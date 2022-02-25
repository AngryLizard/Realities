// The Gateway of Realities: Planes of Existence.

#pragma once

#include "Kismet/KismetSystemLibrary.h"

#include "TGOR_GroundMovementComponent.h"
#include "TGOR_FallingMovementComponent.generated.h"

/**
 * TGOR_FallingMovementComponent adds falling functionality to an actor
 */
UCLASS()
class REALITIES_API UTGOR_FallingMovementComponent : public UTGOR_GroundMovementComponent
{
	GENERATED_BODY()

public:
	UTGOR_FallingMovementComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction) override;
	void InitialiseMovementmode(EMovementMode CurrentMode, ETGOR_CustomMovementMode CustomMode) override;
	virtual bool CanCrouchInCurrentState() const override;

	virtual float CalcCustomControlledFallingVelocity(float DeltaTime, const FVector& InputAcceleration) override;
	virtual float CalcCustomFreeFallingVelocity(float DeltaTime, const FVector& InputAcceleration) override;

	////////////////////////////////////////////////////////////////////////////////////////////////////

	/** Max angle to slide on (0.0 for 90°, 1.0 for 0°) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TGOR|FallingMovement", Meta = (Keywords = "C++", ClampMin = "0.0", ClampMax = "1.0"))
		float MaxSlideAngle;

	/** Sphere collision enabled in ground mode */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TGOR|FallingMovement", Meta = (Keywords = "C++"))
		bool FallingSphereCollisionEnabled;

	/** Sliding enabled */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TGOR|FallingMovement", Meta = (Keywords = "C++"))
		bool Sliding;

private:

	// Retreives current groundspeed max
	float GetGroundSpeedMax();

};
