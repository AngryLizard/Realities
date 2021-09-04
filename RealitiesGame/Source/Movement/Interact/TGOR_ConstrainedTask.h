// TGOR (C) // CHECKED //
#pragma once

#include "CoreMinimal.h"

#include "MovementSystem/Tasks/TGOR_SocketMovementTask.h"
#include "TGOR_ConstrainedTask.generated.h"

////////////////////////////////////////////// DECL //////////////////////////////////////////////////////

/**
*
*/
UCLASS(Blueprintable)
class MOVEMENT_API UTGOR_ConstrainedTask : public UTGOR_SocketMovementTask
{
	GENERATED_BODY()

public:

	////////////////////////////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////// ENGINE ////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////

	UTGOR_ConstrainedTask();
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual void Initialise() override;
	virtual bool Invariant(const FTGOR_MovementSpace& Space, const FTGOR_MovementExternal& External) const override;
	virtual void Reset(const FTGOR_MovementSpace& Space, const FTGOR_MovementExternal& External);
	virtual void QueryInput(FVector& OutInput, FVector& OutView) const override;
	virtual void Update(FTGOR_MovementSpace& Space, const FTGOR_MovementExternal& External, const FTGOR_MovementTick& Tick, FTGOR_MovementOutput& Out) override;

	////////////////////////////////////////////////////////////////////////////////////////////////////
protected:

	/** Maximum achievable input force */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "!TGOR Movement")
		float InputForce = 320000.0f;

	/** Maximum achievable external force */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "!TGOR Movement")
		float MaxForce = 240000.0f;

	/** Maximum achievable external torque */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "!TGOR Movement")
		float MaxTorque = 8000.0f;

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Called every simulation tick when attached */
	UFUNCTION(BlueprintImplementableEvent, Category = "!TGOR Animation", Meta = (Keywords = "C++"))
		void AttachmentAnimationTick(const FTGOR_MovementSpace& Space, const FTGOR_MovementExternal& External);
};