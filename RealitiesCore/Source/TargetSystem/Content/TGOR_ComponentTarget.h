// The Gateway of Realities: Planes of Existence. By Salireths.

#pragma once

#include "../TGOR_AimInstance.h"

#include "TGOR_Target.h"
#include "TGOR_ComponentTarget.generated.h"

/**
 * Target content, defines a target that can be aimed at. By default targets interactable component itself.
 */
UCLASS(Blueprintable)
class TARGETSYSTEM_API UTGOR_ComponentTarget : public UTGOR_Target
{
	GENERATED_BODY()
		
public:
	UTGOR_ComponentTarget();
	virtual bool OverlapTarget(UTGOR_InteractableComponent* Component, const FVector& Origin, float MaxDistance, FTGOR_AimPoint& Point) const override;
	virtual bool SearchTarget(UTGOR_InteractableComponent* Component, const FVector& Origin, const FVector& Direction, float MaxDistance, FTGOR_AimPoint& Point) const override;
	virtual bool ComputeTarget(const FTGOR_AimPoint& Point, const FVector& Origin, const FVector& Direction, float MaxDistance, FTGOR_AimInstance& Instance) const override;
	virtual FVector QueryAimLocation(const FTGOR_AimInstance& Instance) const override;
	virtual FVector QueryStickyLocation(const FTGOR_AimInstance& Instance) const override;
	virtual UTGOR_InteractableComponent* QueryInteractable(const FTGOR_AimInstance& Instance) const override;

	////////////////////////////////////////////////////////////////////////////////////////////////////
protected:

	/** Offset from the center in component local space */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "!TGOR Target", Meta = (Keywords = "C++"))
		FVector Offset;

	/** Distance threshold on camera plane for a valid target */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "!TGOR Target", Meta = (Keywords = "C++"))
		float DistanceThreshold;
};
