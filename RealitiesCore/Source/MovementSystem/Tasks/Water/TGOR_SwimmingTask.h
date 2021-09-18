// TGOR (C) // CHECKED //
#pragma once

#include "CoreMinimal.h"

#include "TGOR_WaterTask.h"
#include "TGOR_SwimmingTask.generated.h"

////////////////////////////////////////////// DECL //////////////////////////////////////////////////////

/**
*
*/
UCLASS(Blueprintable)
class MOVEMENTSYSTEM_API UTGOR_SwimmingTask : public UTGOR_WaterTask
{
	GENERATED_BODY()

public:

	////////////////////////////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////// ENGINE ////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////

	UTGOR_SwimmingTask();
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void QueryInput(FVector& OutInput, FVector& OutView) const override;

	/** Max speed for swimming movement */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "!TGOR Movement")
		float MaximumSpeed = 900.0f;

	/** Max strength in swimming direction */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "!TGOR Movement")
		float SwimStrength = 100000.0f;

	/** Max rotation angular velocity */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "!TGOR Movement")
		float TurnTorque = 1000.0f;

	/** Max rotation angular velocity when rotating around input axis */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "!TGOR Movement")
		float SwivelRotation = 2200.0f;

	/** Distance to surface to slow down onto */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "!TGOR Movement")
		float SurfaceThreshold = 50.0f;


	/** Ratio when character starts moving after turning ([0,..], 0 for off) */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "!TGOR Movement")
		float LockMovementWithTurning = 4.0f;

	/** Amount of rotation slowing down with speed */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "!TGOR Movement")
		float TorqueSpeedSlowdown = 0.3f;

	////////////////////////////////////////////////////////////////////////////////////////////////////

protected:

	virtual float GetInputForce(const FTGOR_MovementTick& Tick, const FTGOR_MovementSpace& Space, const FTGOR_MovementExternal& External, FTGOR_MovementOutput& Out) const override;

};