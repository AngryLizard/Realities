// TGOR (C) // CHECKED //
#pragma once

#include "CoreMinimal.h"

#include "MovementSystem/Tasks/TGOR_EuclideanMovementTask.h"
#include "TGOR_SlidingTask.generated.h"

////////////////////////////////////////////// DECL //////////////////////////////////////////////////////

class UTGOR_HandleComponent;
class UTGOR_ArmatureComponent;
class UTGOR_Primitive;

/**
*
*/
UCLASS(Blueprintable)
class MOVEMENTSYSTEM_API UTGOR_SlidingTask : public UTGOR_EuclideanMovementTask
{
	GENERATED_BODY()

public:

	////////////////////////////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////// ENGINE ////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////

	UTGOR_SlidingTask();

	virtual bool Initialise() override;
	virtual bool Invariant(const FTGOR_MovementSpace& Space, const FTGOR_MovementExternal& External) const override;
	virtual void Reset(const FTGOR_MovementSpace& Space, const FTGOR_MovementExternal& External);
	virtual void QueryInput(FVector& OutInput, FVector& OutView) const override;
	virtual void Context(const FTGOR_MovementSpace& Space, const FTGOR_MovementExternal& External, const FTGOR_MovementTick& Tick) override;
	virtual void Update(const FTGOR_MovementSpace& Space, const FTGOR_MovementExternal& External, const FTGOR_MovementTick& Tick, FTGOR_MovementOutput& Out) override;

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Max sliding speed */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "!TGOR Movement")
		double MaxSlidingSpeed = 900.0;

	/** Maximum spread of the sliding collision query */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "!TGOR Movement")
		double SlidingQuerySpread = 100.0;

	/** Distance we query walls for */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "!TGOR Movement")
		double SlidingQueryDistance = 250.0;

	////////////////////////////////////////////////////////////////////////////////////////////////////
protected:

	/** Direction we're querying for sliding right now */
	UPROPERTY(Transient, BlueprintReadOnly, Category = "!TGOR Movement")
		FVector SlidingDirection;

	/** Distance from wall */
	UPROPERTY(Transient, BlueprintReadOnly, Category = "!TGOR Movement")
		double SlidingDistance;

};
