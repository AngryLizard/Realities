// The Gateway of Realities: Planes of Existence.

#pragma once

#include "TGOR_SwimmingMovementComponent.h"
#include "TGOR_GroundMovementComponent.generated.h"


/**
 * TGOR_GroundMovementComponent adds movement functionality to an actor
 */
UCLASS()
class REALITIES_API UTGOR_GroundMovementComponent : public UTGOR_SwimmingMovementComponent
{
	GENERATED_BODY()
	

public:
	UTGOR_GroundMovementComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction) override;
	virtual void InitialiseMovementmode(EMovementMode CurrentMode, ETGOR_CustomMovementMode CustomMode) override;
	virtual bool CanCrouchInCurrentState() const override;

	virtual float CalcCustomWalkingVelocity(float DeltaTime, const FVector& InputAcceleration) override;

	////////////////////////////////////////////////////////////////////////////////////////////////////

	/** Walking speed */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TGOR|GroundMovement", Meta = (Keywords = "C++"))
		float GroundWalkSpeed;

	/** Backwards walking speed */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TGOR|GroundMovement", Meta = (Keywords = "C++"))
		float GroundBackSpeed;

	/** Running speed */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TGOR|GroundMovement", Meta = (Keywords = "C++"))
		float GroundRunSpeed;

	/** Crouching speed */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TGOR|GroundMovement", Meta = (Keywords = "C++"))
		float GroundCrouchSpeed;

	/** Sphere collision enabled in ground mode */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TGOR|GroundMovement", Meta = (Keywords = "C++"))
		bool GroundSphereCollisionEnabled;


	/** Walking angle allowed when running */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TGOR|GroundMovement", Meta = (Keywords = "C++", ClampMin = "0", ClampMax = "90"))
		float RunningWalkAngle;

	/** Walking angle allowed when walking */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TGOR|GroundMovement", Meta = (Keywords = "C++", ClampMin = "0", ClampMax = "90"))
		float WalkingWalkAngle;

	/** Slowdown multiplier when running on a slope */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TGOR|GroundMovement", Meta = (Keywords = "C++", ClampMin = "0", ClampMax = "1"))
		float AngleSlowdown;

	/** Lerp for limit applied on steep angles */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TGOR|GroundMovement", Meta = (Keywords = "C++"))
		float AngleSpeedLerp;

	
protected:

	// Rotate towards either cursor or input vector while walking depending on direction (Go towards cursor on 180° turn first when aggressive)
	void RelativeDirection(float ForwardDot, float RightDot, float CursorForwardDot, float CursorRightDot, float Turn);

private:

	void SetWalkAngle(float Speed, float Limit);
	float GetWalkAngleSpeed(float Limit, const FVector2D& Direction);
	float GetRunAngleSpeed(float DeltaTime, float Speed, float Limit, float MinLimit, const FVector2D& Direction);

	// Switch state functions
	void SwitchGroundOutside(float Input, float Speed);
	void SwitchGroundIdle(float Relative, float Input, float ForwardDot, float AimDot);
	void SwitchGroundWalking(float Relative, float Input, float Speed, float Limit, const FVector2D& Direction, float AimDot);
	void SwitchGroundBackwards(float Relative, float Input, float Speed, float Limit, const FVector2D& Direction, float AimDot);
	void SwitchGroundRunning(float Relative, float Input, float ForwardDot, float Speed, float Limit, const FVector2D& Direction);
	void SwitchGroundCrouching(float Input);

	// Rotate towards in steps
	void GridDirection(float ForwardDot, float RightDot, float Turn);
	
	// Rotate towards either cursor or input vector while standing
	void CursorDirection(float Input, float ForwardDot, float RightDot, float CursorForwardDot, float CursorRightDot);


	// Move in an arch if needed
	void ArchDirection(float DeltaTime, float Input, const FVector2D& Forward, const FVector2D& Direction);

	// Move Forward only when not too fast
	void RestrictDirection(float DeltaTime, float Input, float Speed, float Limit, const FVector2D& Forward);
};
