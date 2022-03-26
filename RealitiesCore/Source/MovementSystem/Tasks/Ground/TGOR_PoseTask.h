// TGOR (C) // CHECKED //
#pragma once

#include "CoreMinimal.h"

#include "TGOR_GroundTask.h"
#include "TGOR_PoseTask.generated.h"

////////////////////////////////////////////// DECL //////////////////////////////////////////////////////

/**
*
*/
UCLASS(Blueprintable)
class MOVEMENTSYSTEM_API UTGOR_PoseTask : public UTGOR_GroundTask
{
	GENERATED_BODY()

public:

	////////////////////////////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////// ENGINE ////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////

	UTGOR_PoseTask();
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual bool Invariant(const FTGOR_MovementSpace& Space, const FTGOR_MovementExternal& External) const override;
	virtual float GetStretch(const FTGOR_MovementTick& Tick, const FTGOR_MovementSpace& Space, const FTGOR_MovementExternal& External) const override;
	virtual float GetInputForce(const FTGOR_MovementTick& Tick, const FTGOR_MovementSpace& Space, const FTGOR_MovementExternal& External, const FTGOR_MovementRepel& Repel, FTGOR_MovementOutput& Out) const override;

	////////////////////////////////////////////////////////////////////////////////////////////////////
protected:

	/** Axis snap ratio in Z axis in positive (X) and negative (Y) */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "!TGOR Movement")
		FVector2D ZAxisSnap = FVector2D(1.0f, 0.35f);

	/** Torque applied on input */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "!TGOR Movement")
		float RotationTorque = 1200.0f;

	/** Applied torque to snap the axis towards durface normals */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "!TGOR Movement")
		float SnappingTorque = 800.0f;

	/** Applied torque around surface normal on input */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "!TGOR Movement")
		float SwayTorque = 0.0f;

	/** Whether character can only move in major axis */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "!TGOR Movement")
		bool RestrictsAxis = false;

	////////////////////////////////////////////////////////////////////////////////////////////////////

};