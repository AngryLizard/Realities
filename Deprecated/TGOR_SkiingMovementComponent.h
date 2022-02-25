// The Gateway of Realities: Planes of Existence.

#pragma once

#include "TGOR_FlyingMovementComponent.h"
#include "TGOR_SkiingMovementComponent.generated.h"

/**
 * TGOR_SkiingMovementComponent adds skiing funcionality to an actor
 */
UCLASS()
class REALITIES_API UTGOR_SkiingMovementComponent : public UTGOR_FlyingMovementComponent
{
	GENERATED_BODY()
public:
	UTGOR_SkiingMovementComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction) override;
	void InitialiseMovementmode(EMovementMode CurrentMode, ETGOR_CustomMovementMode CustomMode) override;
	virtual bool CanCrouchInCurrentState() const override;

	////////////////////////////////////////////////////////////////////////////////////////////////////

	/** True if currently skiing */
	UFUNCTION(BlueprintPure, Category = "TGOR|SkiingMovement", Meta = (Keywords = "C++"))
		bool IsSkiing() const;

	virtual float CalcCustomSkiingVelocity(float DeltaTime, const FVector& InputAcceleration) override;
	

private:
	
};
