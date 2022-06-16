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
	UFUNCTION(BlueprintCallable, Category = "!TGOR Movement|Internal", Meta = (Keywords = "C++"))
		void SimulateSymplectic(const FTGOR_MovementSpace& Space, const FTGOR_MovementForce& Force, const FTGOR_MovementExternal& External, float DeltaTime, bool Sweep);

	////////////////////////////////////////////////////////////////////////////////////////////////////

	/** Core component elasticity on collision response [0, 1] */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "!TGOR Movement")
		FTGOR_Normal Elasticity = 0.1f;

	/** Core component friction coefficient [0, 1] */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "!TGOR Movement")
		FTGOR_Normal Friction = 0.1f;

	////////////////////////////////////////////////////////////////////////////////////////////////////

	/** Called if an impact was registered. */
	UFUNCTION(BlueprintImplementableEvent, Category = "!TGOR Movement", Meta = (Keywords = "C++"))
		void OnSymplecticCollision(const FTGOR_MovementSpace& Space, const FVector& Normal, const float Strength, float DeltaTime);

	////////////////////////////////////////////////////////////////////////////////////////////////////
protected:

	UPROPERTY(Transient, BlueprintReadOnly, Category = "!TGOR Movement")
		FTGOR_MovementImpact LastMovementImpact;

	UPROPERTY(Transient, BlueprintReadOnly, Category = "!TGOR Movement")
		TWeakObjectPtr<UTGOR_PilotComponent> RootComponent;

	UPROPERTY(Transient, BlueprintReadOnly, Category = "!TGOR Movement")
		TWeakObjectPtr<UTGOR_EuclideanPilotTask> EuclideanTask;
};
