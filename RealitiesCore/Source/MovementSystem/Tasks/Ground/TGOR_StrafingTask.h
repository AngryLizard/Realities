// TGOR (C) // CHECKED //
#pragma once

#include "CoreMinimal.h"

#include "TGOR_UprightTask.h"
#include "TGOR_StrafingTask.generated.h"

////////////////////////////////////////////// DECL //////////////////////////////////////////////////////

class UTGOR_Attribute;

/**
*
*/
UCLASS(Blueprintable)
class MOVEMENTSYSTEM_API UTGOR_StrafingTask : public UTGOR_UprightTask
{
	GENERATED_BODY()

public:

	////////////////////////////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////// ENGINE ////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////

	UTGOR_StrafingTask();
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual bool Invariant(const FTGOR_MovementSpace& Space, const FTGOR_MovementExternal& External) const override;
	virtual float GetInputForce(const FTGOR_MovementTick& Tick, const FTGOR_MovementSpace& Space, const FVector& Orientation, const FTGOR_MovementExternal& External, const FTGOR_MovementRepel& Repel, FTGOR_MovementOutput& Out) const override;

	////////////////////////////////////////////////////////////////////////////////////////////////////
protected:

	/** Max speed for ground movement backward */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "!TGOR Movement")
		float MaximumSpeedForward = 600.0f;

	/** Max speed for ground movement sideways */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "!TGOR Movement")
		float MaximumSpeedSideways = 300.0f;

	/** Max speed for ground movement backward */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "!TGOR Movement")
		float MaximumSpeedBackward = 300.0f;


	/** Ratio when character starts moving after turning ([0,..], 0 for off) */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "!TGOR Movement")
		float LockMovementWithTurning = 4.0f;

	/** Rotational slowdown with speed ([0, 1], 1 for off) */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "!TGOR Movement")
		float TorqueSpeedSlowdown = 0.4f;

	/** Input torque coefficient */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "!TGOR Movement")
		float TurnTorque = 1000.0f;


	/** This movement requires this attribute to be greater than 0.5 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "!TGOR Movement")
		TSubclassOf<UTGOR_Attribute> InvariantAttribute;

	////////////////////////////////////////////////////////////////////////////////////////////////////
protected:

	/** Computes maximum speed */
	UFUNCTION(BlueprintPure, Category = "!TGOR Movement", Meta = (Keywords = "C++"))
		float ComputeMaxSpeed(FVector& View) const;

};
