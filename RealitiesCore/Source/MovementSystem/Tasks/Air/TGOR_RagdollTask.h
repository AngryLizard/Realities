// TGOR (C) // CHECKED //
#pragma once

#include "CoreMinimal.h"

#include "MovementSystem/Tasks/TGOR_OrientableTask.h"
#include "TGOR_RagdollTask.generated.h"

////////////////////////////////////////////// DECL //////////////////////////////////////////////////////

class UTGOR_Attribute;

/**
*
*/
UCLASS(Blueprintable)
class MOVEMENTSYSTEM_API UTGOR_RagdollTask : public UTGOR_OrientableTask
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
	virtual void Update(const FTGOR_MovementSpace& Space, const FTGOR_MovementExternal& External, const FTGOR_MovementTick& Tick, FTGOR_MovementOutput& Out) override;

	virtual float GetMaxSpeed() const;

	////////////////////////////////////////////////////////////////////////////////////////////////////
protected:

	/** Braking coeffiecient when touching ground */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "!TGOR Movement")
		float LinearDamping = 600.0f;

	/** Torque damping coefficient */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "!TGOR Movement")
		float AngularDamping = 100.0f;

	/** Upright torque coefficient */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "!TGOR Movement")
		float UprightTorque = 200.0f;

	/** Break force coefficient */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "!TGOR Movement")
		float BreakForce = 200.0f;


	/** Collision detection distance */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "!TGOR Movement")
		float DetectionDistance = 350.0f;

	/** How much external force direction is taken into consideration */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "!TGOR Movement")
		float ExternalBiasMultiplier = 10.0f;

	/** Maximum linear speed we detect for movement applications */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "!TGOR Movement")
		float MaxSpeed = 1200.0f;

	/** Relative lateral movement angle threshold for when we start rotating upright */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "!TGOR Movement")
		float UprightThreshold = 0.5f;

	/** Attribute for how much we try to stand upright in ]0,1] for 1 if always */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "!TGOR Movement")
		TSubclassOf<UTGOR_Attribute> AutonomyAttribute;

	////////////////////////////////////////////////////////////////////////////////////////////////////
protected:

	/** Impact location */
	UPROPERTY(Transient, BlueprintReadOnly, Category = "!TGOR Movement")
		FVector ImpactLocation;

	/** Impact normal */
	UPROPERTY(Transient, BlueprintReadOnly, Category = "!TGOR Movement")
		FVector ImpactNormal;

	/** Impact time */
	UPROPERTY(Transient, BlueprintReadOnly, Category = "!TGOR Movement")
		float ImpactTime;

};
