// The Gateway of Realities: Planes of Existence.

#pragma once

#include <functional>
#include "CoreMinimal.h"

#include "Realities/Components/Movement/TGOR_SymplecticComponent.h"
#include "TGOR_LinearComponent.generated.h"

/**
 * UTGOR_LinearComponent physically moves an object without rotation (making collisions more stable).
 */
UCLASS(ClassGroup = (Custom), Blueprintable, meta = (BlueprintSpawnableComponent))
class REALITIES_API UTGOR_LinearComponent : public UTGOR_SymplecticComponent
{
	GENERATED_BODY()

public:
	UTGOR_LinearComponent();

	////////////////////////////////////////////////////////////////////////////////////////////////////
protected:

	/** Applied torque for upright rotation */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TGOR Movement")
		float StandupTorque;

	/** Torque damping coefficient */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TGOR Movement")
		float AngularDamping;

	////////////////////////////////////////////////////////////////////////////////////////////////////
protected:

	virtual void ComputePhysics(const FTGOR_MovementTick& Tick, const FTGOR_MovementSpace& Space, const FTGOR_MovementExternal& External, bool Replay, FTGOR_MovementOutput& Output) override;
	virtual bool CanRotateOnImpact() const override;

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Distance this component should check for movement basis */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TGOR Movement")
		float ProbeDistance;

};
