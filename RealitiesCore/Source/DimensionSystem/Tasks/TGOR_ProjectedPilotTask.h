// TGOR (C) // CHECKED //
#pragma once

#include "CoreMinimal.h"

#include "TGOR_EuclideanPilotTask.h"
#include "TGOR_ProjectedPilotTask.generated.h"


/**
* Y-Axis projected position, velocity and acceleration
*/
UCLASS(Blueprintable)
class DIMENSIONSYSTEM_API UTGOR_ProjectedPilotTask : public UTGOR_EuclideanPilotTask
{
	GENERATED_BODY()

public:

	////////////////////////////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////// ENGINE ////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////

	UTGOR_ProjectedPilotTask();
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual bool Initialise() override;

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	virtual FTGOR_MovementPosition ComputePosition() const override;
	virtual FTGOR_MovementSpace ComputeSpace() const override;
	virtual void InitDynamic(const FTGOR_MovementDynamic& Dynamic) override;
	virtual void SimulateDynamic(const FTGOR_MovementDynamic& Dynamic) override;
	virtual void SimulatePosition(const FTGOR_MovementPosition& Position) override;

	////////////////////////////////////////////////////////////////////////////////////////////////////
private:

	/** Dynamic relative to parent */
	UPROPERTY(ReplicatedUsing = RepNotifyLocal)
		FTGOR_MovementDynamic Local;

	/** Do network smoothing */
	UFUNCTION()
		void RepNotifyLocal(const FTGOR_MovementDynamic& Old);

	// Axis we're projecting along
	EAxis::Type ProjectionAxis = EAxis::X;
};
