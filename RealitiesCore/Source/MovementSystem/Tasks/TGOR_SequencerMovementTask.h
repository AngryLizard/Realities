// TGOR (C) // CHECKED //
#pragma once

#include "CoreMinimal.h"

#include "TGOR_MovementTask.h"
#include "TGOR_SequencerMovementTask.generated.h"

////////////////////////////////////////////// DECL //////////////////////////////////////////////////////

class UTGOR_SequencerPilotTask;

/**
*
*/
UCLASS(Blueprintable)
class MOVEMENTSYSTEM_API UTGOR_SequencerMovementTask : public UTGOR_MovementTask
{
	GENERATED_BODY()

		friend struct FTGOR_RootMotionModifier;

public:

	////////////////////////////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////// ENGINE ////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////

	UTGOR_SequencerMovementTask();

	virtual bool Initialise() override;
	virtual bool Invariant(const FTGOR_MovementSpace& Space, const FTGOR_MovementExternal& External) const override;
	virtual void Update(const FTGOR_MovementSpace& Space, const FTGOR_MovementExternal& External, const FTGOR_MovementTick& Tick, FTGOR_MovementOutput& Output) override;

	virtual void PrepareStart() override;
	virtual void Interrupt() override;

	////////////////////////////////////////////////////////////////////////////////////////////////////
protected:

	/** Get parented pose */
	UFUNCTION(BlueprintPure, Category = "!TGOR Animation", Meta = (Keywords = "C++"))
		USkeletalMeshComponent* GetPose() const;

	////////////////////////////////////////////////////////////////////////////////////////////////////
protected:

	UPROPERTY(Transient)
		TWeakObjectPtr<UTGOR_PilotComponent> RootComponent;

	UPROPERTY(Transient)
		TWeakObjectPtr<UTGOR_SequencerPilotTask> SequencerTask;
};
