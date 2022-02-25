// TGOR (C) // CHECKED //
#pragma once

#include "Realities/Mod/Movements/TGOR_Movement.h"
#include "TGOR_PilotMovement.generated.h"

////////////////////////////////////////////// DECL //////////////////////////////////////////////////////

/**
* TGOR_PilotMovement is used when pawn is moved by a guide
*/
UCLASS()
class REALITIES_API UTGOR_PilotMovement : public UTGOR_Movement
{
	GENERATED_BODY()

public:

	UTGOR_PilotMovement();
	virtual bool Invariant(const UTGOR_MovementComponent* Component, const FTGOR_MovementTick& Tick, const FTGOR_MovementSpace& Space) const override;
	virtual TSubclassOf<UTGOR_Animation> QueryAnimation(const UTGOR_MovementComponent* Component, const FTGOR_MovementTick& Tick, const FTGOR_MovementSpace& Space) const override;

	virtual void Tick(UTGOR_MovementComponent* Component, const FTGOR_MovementTick& Tick, const FTGOR_MovementSpace& Space, bool Replay, FTGOR_MovementOutput& Out) override;

	////////////////////////////////////////////////////////////////////////////////////////////////////
protected:

	/** Damping against linear velocity */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TGOR Movement")
		float LinearDamping;

	/** Damping against angular velocity */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TGOR Movement")
		float AngularDamping;

	/** Force towards attachment */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TGOR Movement")
		float AttachForce;

	/** Torque towards attachment */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TGOR Movement")
		float AttachTorque;

	/** Called every tick */
	UFUNCTION(BlueprintImplementableEvent, Category = "TGOR Animation", Meta = (Keywords = "C++"))
		void PilotAnimationTick(UTGOR_MovementComponent* Component, const FTGOR_MovementSpace& Space, bool HasBase);
private:
};
