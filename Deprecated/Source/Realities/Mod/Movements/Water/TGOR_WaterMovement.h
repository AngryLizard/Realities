// TGOR (C) // CHECKED //
#pragma once

#include "Realities/Mod/Movements/TGOR_Movement.h"
#include "TGOR_WaterMovement.generated.h"

////////////////////////////////////////////// DECL //////////////////////////////////////////////////////

class UTGOR_MovementComponent;

/**
* TGOR_GroundMovement is a movement mode on the ground
*/
UCLASS()
class REALITIES_API UTGOR_WaterMovement : public UTGOR_Movement
{
	GENERATED_BODY()

public:

	UTGOR_WaterMovement();

	virtual void Tick(UTGOR_MovementComponent* Component, const FTGOR_MovementTick& Tick, const FTGOR_MovementSpace& Space, const FTGOR_MovementExternal& External, bool Replay, FTGOR_MovementOutput& Out) override;
	virtual bool Invariant(const UTGOR_MovementComponent* Component, const FTGOR_MovementTick& Tick, const FTGOR_MovementSpace& Space, const FTGOR_MovementExternal& External) const override;

	////////////////////////////////////////////////////////////////////////////////////////////////////
protected:
	
	/** Water control coeffiecient for ground movement */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TGOR Movement")
		float WaterControl;

	/** Rotation for upright rotation */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TGOR Movement")
		float UprightRotation;

	/** Torque damping coefficient */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TGOR Movement")
		float AngularDamping;

	/** Strength of capsule repel */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TGOR Movement")
		float WallRepelStrength;

	/** Threshold on bouyancy ratio at which water movement starts */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TGOR Movement")
		float BouyancyThreshold;


	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Whether we can swim in the current volume */
	UFUNCTION(BlueprintPure, Category = "TGOR Movement", Meta = (Keywords = "C++"))
		bool CanSwimInCurrentSurroundings(const UTGOR_MovementComponent* Component) const;

	/** Called every simulation tick */
	UFUNCTION(BlueprintImplementableEvent, Category = "TGOR Animation", Meta = (Keywords = "C++"))
		void SwimmingAnimationTick(UTGOR_MovementComponent* Component, const FTGOR_MovementSpace& Space);

	////////////////////////////////////////////////////////////////////////////////////////////////////

protected:

	/** Computes forces depending on input, return speed ratio [0, 1] */
	virtual float GetInputForce(UTGOR_MovementComponent* Component, const FTGOR_MovementTick& Tick, const FTGOR_MovementSpace& Space, const FTGOR_MovementExternal& External, FTGOR_MovementOutput& Out) const;

private:

};
