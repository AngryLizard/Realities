// The Gateway of Realities: Planes of Existence. By Salireths.

#pragma once

#include "../TGOR_AimInstance.h"

#include "TGOR_ComponentTarget.h"
#include "TGOR_WorldTarget.generated.h"

/**
 * World target traces static world, usually good for default behaviour from a LevelVolume
 */
UCLASS(Blueprintable)
class TARGETSYSTEM_API UTGOR_WorldTarget : public UTGOR_Target
{
	GENERATED_BODY()
		
public:
	UTGOR_WorldTarget();
	virtual bool OverlapTarget(UTGOR_AimTargetComponent* Component, const FVector& Origin, FTGOR_AimPoint& Point) const override;
	virtual bool ComputeTarget(UTGOR_AimTargetComponent* Component, const FVector& Origin, const FVector& Direction, FTGOR_AimPoint& Point) const override;
	virtual FVector QueryAimLocation(const FTGOR_AimInstance& Instance) const override;
	virtual UTGOR_AimTargetComponent* QueryInteractable(const FTGOR_AimInstance& Instance) const override;

	////////////////////////////////////////////////////////////////////////////////////////////////////
protected:

};
