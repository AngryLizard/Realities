// The Gateway of Realities: Planes of Existence.

#pragma once

#include "Utility/TGOR_CustomEnumerations.h"
#include "Utility/Error/TGOR_Error.h"
#include "Utility/TGOR_Math.h"

#include "GameFramework/CharacterMovementComponent.h"
#include "TGOR_CustomMovementComponent.generated.h"

class UTGOR_CustomMovementComponent;
class UTGOR_CollisionSpheresComponent;

typedef float (UTGOR_CustomMovementComponent::*InputAccelerationFunc)(float DeltaTime, const FVector& InputAcceleration);


/**
 * TGOR_CustomMovementComponent implements a CharacterMovementComponent with very custom behaviour
 */
UCLASS()
class REALITIES_API UTGOR_CustomMovementComponent : public UCharacterMovementComponent
{
	GENERATED_BODY()
	
public:
	UTGOR_CustomMovementComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual void BeginPlay() override;

	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction) override;

	/** Alter acceleration (friction, max acceleration etc) and calculate velocity */
	void CalcCustomVelocity(float DeltaTime, float Friction, bool bFluid, float BrakingDeceleration, InputAccelerationFunc func);

	/** Move character and add hit behaviour */
	virtual void SecureCustomVelocity(float DeltaTime, int32 Iterations);

	////////////////////////////////////////////////////////////////////////////////////////////////////
		
	virtual float CalcCustomControlledFallingVelocity(float DeltaTime, const FVector& InputAcceleration);
	virtual float CalcCustomFreeFallingVelocity(float DeltaTime, const FVector& InputAcceleration);
	virtual float CalcCustomWalkingVelocity(float DeltaTime, const FVector& InputAcceleration);
	virtual float CalcCustomSwimmingVelocity(float DeltaTime, const FVector& InputAcceleration);
	virtual float CalcCustomFlyingVelocity(float DeltaTime, const FVector& InputAcceleration);
	virtual float CalcCustomSkiingVelocity(float DeltaTime, const FVector& InputAcceleration);
	virtual float CalcCustomClimbingVelocity(float DeltaTime, const FVector& InputAcceleration);
	virtual void CalcCustomMontageVelocity(float DeltaTime);

	////////////////////////////////////////////////////////////////////////////////////////////////////

	virtual void PhysFalling(float DeltaTime, int32 Iterations) override;
	virtual void PhysWalking(float DeltaTime, int32 Iterations) override;
	virtual void PhysSwimming(float DeltaTime, int32 Iterations) override;
	virtual void PhysFlying(float DeltaTime, int32 Iterations) override;
	virtual void PhysCustom(float DeltaTime, int32 Iterations) override;

	virtual void PhysSkiing(float DeltaTime, int32 Iterations);
	virtual void PhysMontage(float DeltaTime, int32 Iterations);
	virtual void PhysClimb(float DeltaTime, int32 Iterations);

	////////////////////////////////////////////////////////////////////////////////////////////////////

	virtual float SlideAlongSurface(const FVector& Delta, float Time, const FVector& Normal, FHitResult& Hit, bool bHandleImpact) override;
	virtual bool IsHardlyWalkable(const FHitResult& Hit) const;

	////////////////////////////////////////////////////////////////////////////////////////////////////

	virtual float SwimCustom(FVector Delta, FHitResult& Hit);
	virtual void PhysicsRotation(float DeltaTime) override;
	virtual void CustomRotation(float DeltaTime, FRotator Delta);

	virtual bool CanCrouchInCurrentState() const override;

	////////////////////////////////////////////////////////////////////////////////////////////////////
	
	/** Applies curve friction towards acceleration vector instead of input vector */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TGOR|Movement", Meta = (Keywords = "C++"))
		bool CurveCorrection;


	/** What the character actually rotated */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TGOR|Movement", Meta = (Keywords = "C++"))
		FRotator TurnDelta;

	/** Used to compute character's rotation */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TGOR|Movement", Meta = (Keywords = "C++"))
		FRotator TurnAdditive;

	/** Last speed limit */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TGOR|Movement", Meta = (Keywords = "C++"))
		float LastSpeedLimit;

	/** Turn speed */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TGOR|Movement", Meta = (Keywords = "C++"))
		float TurnSpeed;

	/** Secondary Brakefactor */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TGOR|Movement", Meta = (Keywords = "C++"))
		float MovementBrakeFactor;

	/** Current max velocity */
	UPROPERTY(BlueprintReadOnly, Category = "TGOR|Movement", Meta = (Keywords = "C++"))
		float VelocityClamp;


	/** Pitch of correction on collision spheres  */
	UPROPERTY(BlueprintReadOnly, Category = "TGOR|Movement", Meta = (Keywords = "C++"))
		float CollisionSphereCorrectionPitch;

	/** Speed of correction on collision spheres  */
	UPROPERTY(BlueprintReadOnly, Category = "TGOR|Movement", Meta = (Keywords = "C++"))
		float CollisionSphereCorrectionSpeed;

	/** Correction on collision spheres enabled */
	UPROPERTY(BlueprintReadOnly, Category = "TGOR|Movement", Meta = (Keywords = "C++"))
		bool CollisionSphereCorrectionEnabled;

	/** Collision spheres */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TGOR|Movement", Meta = (Keywords = "C++"))
		UTGOR_CollisionSpheresComponent* CollisionSpheres;

protected:
	// Moves the given direction
	void MoveDirection(float DeltaTime, float Input, const FVector& Direction);
	void MoveDirection(float DeltaTime, float Input, const FVector2D& Direction);

	// Computes rotation using TurnAdditive
	void CalcTurnDelta(float DeltaTime);

	// Implement custom collision
	virtual bool MoveUpdatedComponentImpl(const FVector& Delta, const FQuat& NewRotation, bool bSweep, FHitResult* OutHit = NULL, ETeleportType Teleport = ETeleportType::None) override;
	bool RotateCustomComponent(float DeltaSeconds, float Multiplier, const FQuat& NewRotation, bool bSweep, ETeleportType Teleport = ETeleportType::None);


private:
	FVector _movementDirection;
};