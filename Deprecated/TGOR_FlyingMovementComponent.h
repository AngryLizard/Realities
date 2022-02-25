// The Gateway of Realities: Planes of Existence.

#pragma once

#include "TGOR_FallingMovementComponent.h"
#include "TGOR_FlyingMovementComponent.generated.h"

/**
 * TGOR_FlyingMovementComponent adds flying functionality to an actor
 */
UCLASS()
class REALITIES_API UTGOR_FlyingMovementComponent : public UTGOR_FallingMovementComponent
{
	GENERATED_BODY()

public:
	UTGOR_FlyingMovementComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction) override;
	void InitialiseMovementmode(EMovementMode CurrentMode, ETGOR_CustomMovementMode CustomMode) override;
	virtual bool CanCrouchInCurrentState() const override;
	
	virtual float CalcCustomFlyingVelocity(float DeltaTime, const FVector& InputAcceleration) override;

	////////////////////////////////////////////////////////////////////////////////////////////////////
	
	
};
