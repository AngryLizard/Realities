// TGOR (C) // CHECKED //
#pragma once

#include "CoreMinimal.h"
#include "MovementSystem/TGOR_IPC.h"

#include "TGOR_UprightTask.h"
#include "TGOR_WalkingTask.generated.h"

////////////////////////////////////////////// DECL //////////////////////////////////////////////////////

/**
*
*/
UCLASS(Blueprintable)
class MOVEMENTSYSTEM_API UTGOR_WalkingTask : public UTGOR_UprightTask
{
	GENERATED_BODY()

public:

	////////////////////////////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////// ENGINE ////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////

	UTGOR_WalkingTask();
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual float GetMaxSpeed() const override;
	virtual bool Initialise() override;

	////////////////////////////////////////////////////////////////////////////////////////////////////
protected:

	/** Max speed for ground movement */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "!TGOR Movement")
		float MaximumSpeed = 500.0f;

	/** Ratio when character starts moving after turning ([0,..], 0 for off) */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "!TGOR Movement")
		float LockMovementWithTurning = 4.0f;

	/** Ratio for how much character moves in input direction opposed to forward ([0, 1], 1 for movement in input direction) */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "!TGOR Movement")
		float DirectMovement = 0.0f;


	/** Rotational slowdown with speed ([0, 1], 1 for off) */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "!TGOR Movement")
		float TorqueSpeedSlowdown = 0.4f;

	/** Input torque coefficient */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "!TGOR Movement")
		float TurnTorque = 90.0f;

	////////////////////////////////////////////////////////////////////////////////////////////////////
protected:

	virtual float GetInputForce(const FTGOR_MovementTick& Tick, const FTGOR_MovementSpace& Space, const FTGOR_MovementExternal& External, const FTGOR_MovementRepel& Repel, FTGOR_MovementOutput& Out) const override;

/*
	////////////////////////////////////////////////////////////////////////////////////////////////////
protected:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "!TGOR Movement")
		FTGOR_PendulumProperties Pendulum;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "!TGOR Movement")
		FTGOR_RiccatiProperties Riccati;

	////////////////////////////////////////////////////////////////////////////////////////////////////
private:

	UPROPERTY()
		FTGOR_IPCProperties IPC;
*/
};