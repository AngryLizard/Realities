// The Gateway of Realities: Planes of Existence. By Salireths.

#pragma once

#include "TargetSystem/TGOR_AimInstance.h"

#include "TargetSystem/Content/TGOR_Target.h"
#include "TGOR_TouchableTarget.generated.h"

/**
 * 
 */
UCLASS(Blueprintable)
class DIALOGUESYSTEM_API UTGOR_TouchableTarget : public UTGOR_Target
{
	GENERATED_BODY()
		
public:
	UTGOR_TouchableTarget();
	virtual bool OverlapTarget(UTGOR_AimTargetComponent* Component, const FVector& Origin, float MaxDistance, FTGOR_AimPoint& Point) const override;
	virtual bool SearchTarget(UTGOR_AimTargetComponent* Component, const FVector& Origin, const FVector& Direction, float MaxDistance, FTGOR_AimPoint& Point) const override;
	virtual bool ComputeTarget(const FTGOR_AimPoint& Point, const FVector& Origin, const FVector& Direction, float MaxDistance, FTGOR_AimInstance& Instance) const override;
	virtual FVector QueryAimLocation(const FTGOR_AimInstance& Instance) const override;
	virtual FVector QueryStickyLocation(const FTGOR_AimInstance& Instance) const override;
	virtual UTGOR_AimTargetComponent* QueryInteractable(const FTGOR_AimInstance& Instance) const override;

};
