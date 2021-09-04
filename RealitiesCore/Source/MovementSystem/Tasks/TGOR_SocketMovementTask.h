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

	virtual void Initialise() override;
	virtual bool Invariant(const FTGOR_MovementSpace& Space, const FTGOR_MovementExternal& External) const override;
	virtual void Update(FTGOR_MovementSpace& Space, const FTGOR_MovementExternal& External, const FTGOR_MovementTick& Tick, FTGOR_MovementOutput& Output) override;

protected:

	UPROPERTY(Transient)
		UTGOR_RigidComponent* RigidComponent;

	UPROPERTY(Transient)
		UTGOR_SocketPilotTask* SocketTask;
};
