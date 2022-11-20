// TGOR (C) // CHECKED //
#pragma once

#include "CoreMinimal.h"

#include "TGOR_MovementTask.h"
#include "TGOR_SocketMovementTask.generated.h"

////////////////////////////////////////////// DECL //////////////////////////////////////////////////////
class UTGOR_SocketPilotTask;

/**
*
*/
UCLASS(Blueprintable)
class MOVEMENTSYSTEM_API UTGOR_SocketMovementTask : public UTGOR_MovementTask
{
	GENERATED_BODY()

public:

	////////////////////////////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////// ENGINE ////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////

	UTGOR_SocketMovementTask();

	virtual bool Initialise() override;
	virtual bool Invariant(const FTGOR_MovementSpace& Space, const FTGOR_MovementExternal& External) const override;
	virtual void Update(const FTGOR_MovementSpace& Space, const FTGOR_MovementExternal& External, const FTGOR_MovementTick& Tick, FTGOR_MovementOutput& Output) override;

	virtual void PrepareStart() override;
	virtual void Interrupt() override;

protected:

	UPROPERTY(Transient)
		TWeakObjectPtr<UTGOR_PilotComponent> RootComponent;

	UPROPERTY(Transient)
		TWeakObjectPtr<UTGOR_SocketPilotTask> SocketTask;
};
