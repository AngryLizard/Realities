// TGOR (C) // CHECKED //
#pragma once

#include "CoreMinimal.h"

#include "MovementSystem/Tasks/TGOR_MontageMovementTask.h"
#include "TGOR_ActionMontageMovementTask.generated.h"

////////////////////////////////////////////// DECL //////////////////////////////////////////////////////

class UTGOR_ActionComponent;

/**
*/
UCLASS(Blueprintable)
class ACTIONSYSTEM_API UTGOR_ActionMontageMovementTask : public UTGOR_MontageMovementTask
{
	GENERATED_BODY()

public:

	////////////////////////////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////// ENGINE ////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////

	UTGOR_ActionMontageMovementTask();
	virtual bool Initialise() override;
	virtual bool Invariant(const FTGOR_MovementSpace& Space, const FTGOR_MovementExternal& External) const override;
	virtual FTGOR_MovementPosition TickAnimationRootMotion(FTGOR_MovementSpace& Space, float DeltaTime) override;

	////////////////////////////////////////////////////////////////////////////////////////////////////
private:

	/** Connected action component */
	UPROPERTY()
		TWeakObjectPtr<UTGOR_ActionComponent> ActionComponent;
};
