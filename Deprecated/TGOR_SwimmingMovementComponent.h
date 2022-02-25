// The Gateway of Realities: Planes of Existence.

#pragma once

#include "TGOR_ClimbingMovementComponent.h"
#include "TGOR_SwimmingMovementComponent.generated.h"

/**
 * TGOR_SwimmingMovementComponent adds swimming functionality to an actor
 */
UCLASS()
class REALITIES_API UTGOR_SwimmingMovementComponent : public UTGOR_ClimbingMovementComponent
{
	GENERATED_BODY()

public:
	UTGOR_SwimmingMovementComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction) override;
	void InitialiseMovementmode(EMovementMode CurrentMode, ETGOR_CustomMovementMode CustomMode) override;
	virtual bool CanCrouchInCurrentState() const override;

	virtual float CalcCustomSwimmingVelocity(float DeltaTime, const FVector& InputAcceleration) override;

	////////////////////////////////////////////////////////////////////////////////////////////////////

	/** Swimming speed */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TGOR|SwimmingMovement", Meta = (Keywords = "C++", ClampMin = "-1", ClampMax = "1"))
		float SwimmingSwimSpeed;

	/** Backwards swimming speed */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TGOR|SwimmingMovement", Meta = (Keywords = "C++", ClampMin = "-1", ClampMax = "1"))
		float SwimmingBackSpeed;

	/** Swimming fast speed */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TGOR|SwimmingMovement", Meta = (Keywords = "C++", ClampMin = "-1", ClampMax = "1"))
		float SwimmingSwimFastSpeed;

	/** Swimming downed upward speed */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TGOR|SwimmingMovement", Meta = (Keywords = "C++", ClampMin = "-1", ClampMax = "1"))
		float SwimmingDownedDriftSpeed;

	/** Swimming downed upward acceleration gravity factor (0.0 for no upward drift, 1.0 for gravity) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TGOR|SwimmingMovement", Meta = (Keywords = "C++", ClampMin = "-1", ClampMax = "1"))
		float SwimmingDownedDriftGravityFactor;


	/** Depth at which the character starts rising to surface */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TGOR|SwimmingMovement", Meta = (Keywords = "C++", ClampMin = "0"))
		float BuoyancyDepth;

	/** Submerged under water */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TGOR|SwimmingMovement", Meta = (Keywords = "C++"))
		bool Submerged;

	/** Distance to the water surface */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TGOR|SwimmingMovement", Meta = (Keywords = "C++", ClampMin = "0"))
		float WaterSurfaceDist;

protected:

	// Updates the submerged variable
	float UpdateSubmerged();

	// Checks surroundings and changes mode to swimming if necessary (true if changed)
	bool DetectWater();

	// Compute force applied to Z (drag etc)
	float UpwardForce(float Submerged);
	
	// Move up or down
	void Dive(float DeltaTime, float Drift, float Submerged);

	// Accelerate only if under limit, manage drag
	void Swim(float DeltaTime, float Input, float Speed, float Limit, const FVector& Direction, const FVector& Forward, bool DriftFriction = true);


	// General downed switch
	void SwitchDowned();

private:

	// Swimming state switches
	void SwitchSwimmingOutside(float Input);
	void SwitchSwimmingIdle(float Relative, float Input, float ForwardDot, float AimDot);
	void SwitchSwimmingSwimming(float Relative, float Input, float Speed, float Limit, const FVector2D& Direction, float AimDot);
	void SwitchSwimmingBackwards(float Relative, float Input, float Speed, float Limit, const FVector2D& Direction, float AimDot);
	void SwitchSwimmingSwimfast(float Relative, float Input, float ForwardDot, float Speed, float Limit, const FVector2D& Direction);

};
