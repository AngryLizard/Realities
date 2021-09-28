// TGOR (C) // CHECKED //
#pragma once

#include "CoreMinimal.h"

#include "TGOR_LinearPilotTask.h"
#include "TGOR_EuclideanPilotTask.generated.h"


/**
* Position, velocity and acceleration
*/
UCLASS(Blueprintable)
class DIMENSIONSYSTEM_API UTGOR_EuclideanPilotTask : public UTGOR_LinearPilotTask
{
	GENERATED_BODY()

public:

	////////////////////////////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////// ENGINE ////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////

	UTGOR_EuclideanPilotTask();
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	virtual FTGOR_MovementPosition ComputePosition() const override;
	virtual FTGOR_MovementSpace ComputeSpace() const override;
	virtual void SimulateDynamic(const FTGOR_MovementDynamic& Dynamic) override;
	virtual void SimulatePosition(const FTGOR_MovementPosition& Position) override;

	////////////////////////////////////////////////////////////////////////////////////////////////////

	/** Reset base to current position of owner component */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Movement", Meta = (Keywords = "C++"))
		void ResetToComponent(UTGOR_MobilityComponent* Owner);

	////////////////////////////////////////////////////////////////////////////////////////////////////
private:

	/** Dynamic relative to parent */
	UPROPERTY(ReplicatedUsing = RepNotifyLocal)
		FTGOR_MovementDynamic Local;

	/** Do network smoothing */
	UFUNCTION()
		void RepNotifyLocal(const FTGOR_MovementDynamic& Old);

};
