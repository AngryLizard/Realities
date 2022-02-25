// The Gateway of Realities: Planes of Existence.

#pragma once

#include "TGOR_CustomMovementComponent.h"
#include "TGOR_CreatureMovementComponent.generated.h"

class ATGOR_MovementCharacter;

UENUM(BlueprintType)
enum class ETGOR_SubMovementMode : uint8
{
	Outside,
	Idle,
	Crouching,
	Move,
	Backwards,
	MoveFast,
	Downed,
	Dead
};

/**
 * TGOR_CreatureMovementComponent adds an interface to the holding TGOR_MovementCharacter
 */
UCLASS()
class REALITIES_API UTGOR_CreatureMovementComponent : public UTGOR_CustomMovementComponent
{
	GENERATED_BODY()
	
public:
	UTGOR_CreatureMovementComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction) override;
	
	virtual bool CanCrouchInCurrentState() const override;
	virtual bool DoJump(bool bReplayingMoves) override;
	
	virtual void OnMovementModeChanged(EMovementMode PreviousMovementMode, uint8 PreviousCustomMode) override;
	virtual void InitialiseMovementmode(EMovementMode CurrentMode, ETGOR_CustomMovementMode CustomMode);

	virtual void SimulateMovement(float DeltaTime) override;
	virtual void SecureCustomVelocity(float DeltaTime, int32 Iterations) override;

	////////////////////////////////////////////////////////////////////////////////////////////////////

	/** Owning Character */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TGOR|Movement")
		ATGOR_MovementCharacter* CreatureCharacter;

	/** Debug mode for all traces */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TGOR|Movement")
		TEnumAsByte<EDrawDebugTrace::Type> DebugTrace;

	////////////////////////////////////////////////////////////////////////////////////////////////////

	/** Sluggish turn speed */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TGOR|Movement", Meta = (Keywords = "C++", ClampMin = "0"))
		float TurnSlug;

	/** Slow turn speed */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TGOR|Movement", Meta = (Keywords = "C++", ClampMin = "0"))
		float TurnSlow;

	/** Medium turn speed */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TGOR|Movement", Meta = (Keywords = "C++", ClampMin = "0"))
		float TurnMedium;

	/** Fast turn speed */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TGOR|Movement", Meta = (Keywords = "C++", ClampMin = "0"))
		float TurnFast;

	////////////////////////////////////////////////////////////////////////////////////////////////////

	/** Threshold for speed limits */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TGOR|Movement", Meta = (Keywords = "C++"))
		float SpeedThreshold;

	/** Turn around threshold */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TGOR|Movement", Meta = (Keywords = "C++"))
		float Turnaround;

	/** General factor applied to all speed */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TGOR|Movement", Meta = (Keywords = "C++"))
		float SpeedFactor;
	
	/** Energy threshold to allow running */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TGOR|Movement", Meta = (Keywords = "C++"))
		float ExhaustStaminaThreshold;

	/** Energy rate when running */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TGOR|Movement", Meta = (Keywords = "C++"))
		float ExhaustStaminaRate;

	/** Water level ratio on capsule height (-1 for feet, 1 for head) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TGOR|Movement", Meta = (Keywords = "C++", ClampMin = "-1", ClampMax = "1"))
		float SurfaceLevelRatio;

	////////////////////////////////////////////////////////////////////////////////////////////////////

	/** Movement speed factor when downed */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TGOR|DownedMovement", Meta = (Keywords = "C++"))
		float DownedSlowdown;

	////////////////////////////////////////////////////////////////////////////////////////////////////

	/** Current movement submode */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TGOR|Movement", Meta = (Keywords = "C++"))
		ETGOR_SubMovementMode SubMovementMode;

	/** Current ground/sliding angle */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TGOR|Movement", Meta = (Keywords = "C++"))
		float FloorAngle;

	/** Current Surface normal */
	UPROPERTY(BlueprintReadOnly, Category = "TGOR|Movement")
		FVector SurfaceNormal;

	////////////////////////////////////////////////////////////////////////////////////////////////////

protected:
	// True if fast movement is possible
	bool CanFast(float Speed, const FVector2D& Direction);
	
	// Return input direction or forward vector if no input
	FVector2D GetDirection(float Input, const FVector2D& InputAcceleration, const FVector2D& Forward);

	// Update collision sphere parameters
	void UpdateCollisionSphereParams(bool Active = true, float Pitch = 0.0f);

	// Update floor angle
	void UpdateFloorAngle();

	// Compute distance to water
	float GetSubmerged();


	// Crouches character and sets IsCrouched on character
	void ForceCrouch();

	// Uncrouches character and sets IsCrouched on character
	void ForceUnCrouch();

	// Changes movement mode (Use instead of SetMovementMode)
	void ForceMode(EMovementMode Mode, uint8 Custom = 0);
	
	// Faces the given direction
	void FaceDirection(float ForwardDot, float RightDot, float Turn);
	

private:
};