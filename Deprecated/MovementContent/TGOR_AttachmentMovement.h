// TGOR (C) // CHECKED //
#pragma once

#include "TGOR_Movement.h"
#include "TGOR_AttachmentMovement.generated.h"

////////////////////////////////////////////// DECL //////////////////////////////////////////////////////

/**
* 
*/
UCLASS()
class REALITIES_API UTGOR_AttachmentMovement : public UTGOR_Movement
{
	GENERATED_BODY()

public:
	
	UTGOR_AttachmentMovement();

	virtual void QueryInputVector(const UTGOR_MovementComponent* Component, FVector& OutInput, FVector& OutView) const override;
	virtual void Tick(UTGOR_MovementComponent* Component, const FTGOR_MovementTick& Tick, const FTGOR_MovementSpace& Space, const FTGOR_MovementExternal& External, FTGOR_MovementOutput& Out) override;
	virtual bool Invariant(const UTGOR_MovementComponent* Component, const FTGOR_MovementTick& Tick, const FTGOR_MovementSpace& Space, const FTGOR_MovementExternal& External) const override;

	////////////////////////////////////////////////////////////////////////////////////////////////////
protected:

	/** Maximum achievable input force */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "!TGOR Movement")
		float InputForce;

	/** Maximum achievable external force */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "!TGOR Movement")
		float MaxForce;

	/** Maximum achievable external torque */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "!TGOR Movement")
		float MaxTorque;

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Called every simulation tick when attached */
	UFUNCTION(BlueprintImplementableEvent, Category = "!TGOR Animation", Meta = (Keywords = "C++"))
		void AttachmentAnimationTick(UTGOR_MovementComponent* Component, const FTGOR_MovementSpace& Space, const FTGOR_MovementExternal& External);
};
