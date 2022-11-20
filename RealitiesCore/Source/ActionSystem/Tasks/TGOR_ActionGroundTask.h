// TGOR (C) // CHECKED //
#pragma once

#include "CoreMinimal.h"

#include "MovementSystem/Tasks/Ground/TGOR_GroundTask.h"
#include "TGOR_ActionGroundTask.generated.h"

////////////////////////////////////////////// DECL //////////////////////////////////////////////////////

class UTGOR_ActionComponent;

/**
*/
UCLASS(Blueprintable)
class ACTIONSYSTEM_API UTGOR_ActionGroundTask : public UTGOR_GroundTask
{
	GENERATED_BODY()

public:

	////////////////////////////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////// ENGINE ////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////

	UTGOR_ActionGroundTask();
	virtual bool Initialise() override;
	virtual bool Invariant(const FTGOR_MovementSpace& Space, const FTGOR_MovementExternal& External) const override;
	virtual FTGOR_MovementPosition TickAnimationRootMotion(FTGOR_MovementSpace& Space, float DeltaTime) override;

	virtual float GetStretch(const FTGOR_MovementTick& Tick, const FTGOR_MovementSpace& Space, const FTGOR_MovementExternal& External) const override;

	////////////////////////////////////////////////////////////////////////////////////////////////////

	/** Stretch factor to be used */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "!TGOR Movement|Detection")
		float TargetStretch = 0.6f;

	////////////////////////////////////////////////////////////////////////////////////////////////////
private:

	/** Connected action component */
	UPROPERTY()
		TWeakObjectPtr<UTGOR_ActionComponent> ActionComponent;
};
