// TGOR (C) // CHECKED //
#pragma once

#include "CoreMinimal.h"

#include "TGOR_MovementTask.h"
#include "TGOR_EuclideanMovementTask.generated.h"

////////////////////////////////////////////// DECL //////////////////////////////////////////////////////

class UTGOR_EuclideanPilotTask;
class UTGOR_PilotComponent;

/**
*
*/
UCLASS(Blueprintable)
class MOVEMENTSYSTEM_API UTGOR_EuclideanMovementTask : public UTGOR_MovementTask
{
	GENERATED_BODY()

public:

	////////////////////////////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////// ENGINE ////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////

	UTGOR_EuclideanMovementTask();

	virtual void Initialise() override;
	virtual bool Invariant(const FTGOR_MovementSpace& Space, const FTGOR_MovementExternal& External) const override;
	virtual void Update(const FTGOR_MovementSpace& Space, const FTGOR_MovementExternal& External, const FTGOR_MovementTick& Tick, FTGOR_MovementOutput& Output) override;

	virtual void PrepareStart() override;
	virtual void Interrupt() override;

	/** Simulate symplectic integration for a given time */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Collision|Internal", Meta = (Keywords = "C++"))
		void SimulateSymplectic(const FTGOR_MovementSpace& Space, const FTGOR_MovementForce& Force, const FTGOR_MovementExternal& External, float Timestep, bool Sweep);

	////////////////////////////////////////////////////////////////////////////////////////////////////
protected:

	UPROPERTY(Transient)
		TWeakObjectPtr<UTGOR_PilotComponent> RootComponent;

	UPROPERTY(Transient)
		TWeakObjectPtr<UTGOR_EuclideanPilotTask> EuclideanTask;
};
