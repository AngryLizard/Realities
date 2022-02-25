// The Gateway of Realities: Planes of Existence.

#pragma once

#include "TGOR_MontageMovementComponent.h"
#include "TGOR_ClimbingMovementComponent.generated.h"

/**
* TGOR_ClimbingMovementComponent adds climbing functionality to an actor
*/
UCLASS()
class REALITIES_API UTGOR_ClimbingMovementComponent : public UTGOR_MontageMovementComponent
{
	GENERATED_BODY()

public:
	UTGOR_ClimbingMovementComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction) override;
	virtual void InitialiseMovementmode(EMovementMode CurrentMode, ETGOR_CustomMovementMode CustomMode) override;
	virtual bool CanCrouchInCurrentState() const override;
	virtual bool DoJump(bool bReplayingMoves) override;

	////////////////////////////////////////////////////////////////////////////////////////////////////

	/** True if currently skiing */
	UFUNCTION(BlueprintPure, Category = "TGOR|ClimbMovement", Meta = (Keywords = "C++"))
		bool IsClimbing() const;


	virtual float CalcCustomClimbingVelocity(float DeltaTime, const FVector& InputAcceleration) override;

	////////////////////////////////////////////////////////////////////////////////////////////////////
	
	/** Multiple of capsule half height for height to ledge */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TGOR|ClimbMovement")
		float LedgeHeightRatio;

	/** Multiple of capsule radius for distance to ledge */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TGOR|ClimbMovement")
		float LedgeDistanceRatio;


	/** Ratio of speed to turn around edges */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TGOR|ClimbMovement")
		float LedgeTurnRatio;

	/** Stamina points needed to be able to climb */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TGOR|Movement", Meta = (Keywords = "C++"))
		float ClimbStaminaThreshold;

	/** Ratio of default capsule height for climbing trace span */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TGOR|ClimbMovement")
		float ClimbVerticalSpan;

	/** Ratio of default capsule height for climbing trace span */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TGOR|ClimbMovement")
		float ClimbHorizontalSpan;

	/** vertical speed for climbing */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TGOR|ClimbMovement")
		float ClimbVerticalSpeed;

	/** horizontal speed for climbing */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TGOR|ClimbMovement")
		float ClimbHorizontalSpeed;

	/** Exp of how much climbing over edges speeds climbing down (0 for no slowdown) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TGOR|ClimbMovement")
		float ClimbEdgeSlowdown;

	/** Min speed ratio when going over edge */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TGOR|ClimbMovement")
		float ClimbEdgeMinSpeed;


	/** Exp of how much climbing over inverse edges speeds climbing down (0 for no slowdown) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TGOR|ClimbMovement")
		float ClimbInverseEdgeSlowdown;

	/** Min speed ratio when going over inverse edge */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TGOR|ClimbMovement")
		float ClimbInverseEdgeMinSpeed;


	/** Angle at which character standsup from climbing or falls down */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TGOR|ClimbMovement")
		float StandupAngle;

	////////////////////////////////////////////////////////////////////////////////////////////////////

	/** Trace type for climbing. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TGOR|ClimbMovement")
		TEnumAsByte<ETraceTypeQuery> ClimbingTraceType;
	
	/** Current Surface right */
	UPROPERTY(BlueprintReadOnly, Category = "TGOR|ClimbMovement")
		FVector WallSurfaceRight;

	/** Current Surface up */
	UPROPERTY(BlueprintReadOnly, Category = "TGOR|ClimbMovement")
		FVector WallSurfaceUp;

	/** Current Surface pitch (average trace) */
	UPROPERTY(BlueprintReadOnly, Category = "TGOR|ClimbMovement")
		float WallSurfacePitch;

	/** Bendness for the wall. (0 for flat, negative for bent outward) */
	UPROPERTY(BlueprintReadOnly, Category = "TGOR|ClimbMovement")
		float WallEdgeBendness;

	/** Bendness for the wall. (0 for flat, negative for bent outward) */
	UPROPERTY(BlueprintReadOnly, Category = "TGOR|ClimbMovement")
		float WallCornerBendness;

	/** Difference of surface normals to find out if character is currently on an edge ( 0.0 for flat surface, 1.0 for 90° edge) */
	UPROPERTY(BlueprintReadOnly, Category = "TGOR|ClimbMovement")
		float SurfacePitchDifference;

	/** Difference of bottom and top normals to find out if character is currently on an edge ( 0.0 for flat surface, 1.0 for 90° edge) */
	UPROPERTY(BlueprintReadOnly, Category = "TGOR|ClimbMovement")
		float LedgePitchDifference;

	/** Difference of left and right normals to find out if character is currently on a corner ( 0.0 for flat surface, 1.0 for 90° edge) */
	UPROPERTY(BlueprintReadOnly, Category = "TGOR|ClimbMovement")
		float CornerPitchDifference;

	////////////////////////////////////////////////////////////////////////////////////////////////////

	/** Trace result of the top trace */
	UPROPERTY(BlueprintReadOnly, Category = "TGOR|ClimbMovement")
		FHitResult TopTraceResult;

	/** Trace result of the bottom trace */
	UPROPERTY(BlueprintReadOnly, Category = "TGOR|ClimbMovement")
		FHitResult BottomTraceResult;

	/** Trace result of the right trace */
	UPROPERTY(BlueprintReadOnly, Category = "TGOR|ClimbMovement")
		FHitResult RightTraceResult;

	/** Trace result of the left trace */
	UPROPERTY(BlueprintReadOnly, Category = "TGOR|ClimbMovement")
		FHitResult LeftTraceResult;

	/** Trace result of the center trace */
	UPROPERTY(BlueprintReadOnly, Category = "TGOR|ClimbMovement")
		FHitResult CenterTraceResult;


protected:
	// Tries to climb on surface currently in front of the character
	bool TryEnterClimb(const FVector& InputAcceperation);

	// Leave climbing mode
	void LeaveClimb(EMovementMode GroundMode);
	

private:
	
	// Compute surface normale based on four traces and rotate the character
	bool ComputeSurfaceNormal(float DeltaTime);

	// Move character to the right position relative to the surface
	bool GrappleWall(float DeltaTime);
	
	// Do a linetrace forward using an offset in localspace
	bool LineTraceForward(float Length, const FVector& Offset, FHitResult& Hit);

	// Move on the wall depending in input
	void MoveOnWall(float DeltaTime, const FVector& InputAcceleration);
	
};
