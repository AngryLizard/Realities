// The Gateway of Realities: Planes of Existence. By Salireths.

#pragma once

#include "../TGOR_AimInstance.h"

#include "TGOR_Target.h"
#include "TGOR_SkeletalTarget.generated.h"

/**
 * Target content, defines a target that can be aimed at. By default targets interactable component itself.
 */
UCLASS(Blueprintable)
class TARGETSYSTEM_API UTGOR_SkeletalTarget : public UTGOR_Target
{
	GENERATED_BODY()
		
public:
	UTGOR_SkeletalTarget();

	virtual bool OverlapTarget(UTGOR_InteractableComponent* Component, const FVector& Origin, float MaxDistance, FTGOR_AimPoint& Point) const override;
	virtual bool SearchTarget(UTGOR_InteractableComponent* Component, const FVector& Origin, const FVector& Direction, float MaxDistance, FTGOR_AimPoint& Point) const override;
	virtual bool ComputeTarget(const FTGOR_AimPoint& Point, const FVector& Origin, const FVector& Direction, float MaxDistance, FTGOR_AimInstance& Instance) const override;
	virtual FVector QueryAimLocation(const FTGOR_AimInstance& Instance) const override;
	virtual FVector QueryStickyLocation(const FTGOR_AimInstance& Instance) const override;

	////////////////////////////////////////////////////////////////////////////////////////////////////
protected:
	
	/** Whether the center in the SearchTarget output snaps to the nearest bone or to the trace hit */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "!TGOR Target", Meta = (Keywords = "C++"))
		bool SnapCenterToBone;

	/** Normalized distance percentage applied when this skeleton is hit */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "!TGOR Target", Meta = (Keywords = "C++"))
		float HitDistancePercentage;
};
