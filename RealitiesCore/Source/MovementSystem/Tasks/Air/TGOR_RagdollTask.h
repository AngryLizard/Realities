// TGOR (C) // CHECKED //
#pragma once

#include "CoreMinimal.h"

#include "MovementSystem/Tasks/TGOR_EuclideanMovementTask.h"
#include "TGOR_RagdollTask.generated.h"

////////////////////////////////////////////// DECL //////////////////////////////////////////////////////


/**
*
*/
UCLASS(Blueprintable)
class MOVEMENTSYSTEM_API UTGOR_RagdollTask : public UTGOR_EuclideanMovementTask
{
	GENERATED_BODY()

public:

	////////////////////////////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////// ENGINE ////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////

	UTGOR_RagdollTask();
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual void Initialise() override;
	virtual bool Invariant(const FTGOR_MovementSpace& Space, const FTGOR_MovementExternal& External) const override;
	virtual void Reset(const FTGOR_MovementSpace& Space, const FTGOR_MovementExternal& External);
	virtual void QueryInput(FVector& OutInput, FVector& OutView) const override;
	virtual void Update(FTGOR_MovementSpace& Space, const FTGOR_MovementExternal& External, const FTGOR_MovementTick& Tick, FTGOR_MovementOutput& Out) override;

	////////////////////////////////////////////////////////////////////////////////////////////////////
protected:

	/** Torque damping coefficient */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "!TGOR Movement")
		float AngularDamping = 1000.0f;

	/** Braking coeffiecient when touching ground */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "!TGOR Movement")
		float BrakeCoefficient = 6000.0f;

	/** Detector capsule size multiplier */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "!TGOR Movement")
		float DetectionMultiplier = 1.2f;

	/** Force applied trying to stand up */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "!TGOR Movement")
		float StandupForce = 80000.0f;

	/** Torque applied trying to stand up */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "!TGOR Movement")
		float StandupTorque = 3500.0f;


	/** Maximum achievable force when on ground */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "!TGOR Movement")
		float MaxOnGroundForce = 240000.0f;

	/** Maximum achievable torque when on ground */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "!TGOR Movement")
		float MaxOnGroundTorque = 8000.0f;

	/** Maximum achievable force/torque multiplier when in mid air */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "!TGOR Movement")
		float InAirMultiplier = 0.1f;

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Called every simulation tick when currently in mid-air */
	UFUNCTION(BlueprintImplementableEvent, Category = "!TGOR Animation", Meta = (Keywords = "C++"))
		void RagdollAnimationTickInAir(const FTGOR_MovementSpace& Space, const FTGOR_MovementExternal& External);

	/** Called every simulation tick when currently hitting a surface */
	UFUNCTION(BlueprintImplementableEvent, Category = "!TGOR Animation", Meta = (Keywords = "C++"))
		void RagdollAnimationTickOnGround(const FTGOR_MovementSpace& Space, const FTGOR_MovementExternal& External, const FVector& SurfaceNormal, const FVector& SurfacePoint);

};
