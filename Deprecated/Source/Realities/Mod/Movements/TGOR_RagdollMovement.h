// TGOR (C) // CHECKED //
#pragma once

#include "Realities/Mod/Movements/TGOR_Movement.h"
#include "TGOR_RagdollMovement.generated.h"

////////////////////////////////////////////// DECL //////////////////////////////////////////////////////

/**
* 
*/
UCLASS()
class REALITIES_API UTGOR_RagdollMovement : public UTGOR_Movement
{
	GENERATED_BODY()

public:
	
	UTGOR_RagdollMovement();

	virtual void QueryInputVector(const UTGOR_MovementComponent* Component, FVector& OutInput, FVector& OutView) const override;
	virtual void Tick(UTGOR_MovementComponent* Component, const FTGOR_MovementTick& Tick, const FTGOR_MovementSpace& Space, const FTGOR_MovementExternal& External, bool Replay, FTGOR_MovementOutput& Out) override;
	virtual bool Invariant(const UTGOR_MovementComponent* Component, const FTGOR_MovementTick& Tick, const FTGOR_MovementSpace& Space, const FTGOR_MovementExternal& External) const override;

	////////////////////////////////////////////////////////////////////////////////////////////////////
protected:

	/** Torque damping coefficient */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TGOR Movement")
		float AngularDamping;

	/** Braking coeffiecient when touching ground */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TGOR Movement")
		float BrakeCoefficient;

	/** Detector capsule size multiplier */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TGOR Movement")
		float DetectionMultiplier;

	/** Force applied trying to stand up */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TGOR Movement")
		float StandupForce;

	/** Torque applied trying to stand up */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TGOR Movement")
		float StandupTorque;


	/** Maximum achievable force when on ground */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TGOR Movement")
		float MaxOnGroundForce;

	/** Maximum achievable torque when on ground */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TGOR Movement")
		float MaxOnGroundTorque;

	/** Maximum achievable force/torque multiplier when in mid air */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TGOR Movement")
		float InAirMultiplier;

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Called every simulation tick when currently in mid-air */
	UFUNCTION(BlueprintImplementableEvent, Category = "TGOR Animation", Meta = (Keywords = "C++"))
		void RagdollAnimationTickInAir(UTGOR_MovementComponent* Component, const FTGOR_MovementSpace& Space, const FTGOR_MovementExternal& External);

	/** Called every simulation tick when currently hitting a surface */
	UFUNCTION(BlueprintImplementableEvent, Category = "TGOR Animation", Meta = (Keywords = "C++"))
		void RagdollAnimationTickOnGround(UTGOR_MovementComponent* Component, const FTGOR_MovementSpace& Space, const FTGOR_MovementExternal& External, const FVector& SurfaceNormal, const FVector& SurfacePoint);
};
