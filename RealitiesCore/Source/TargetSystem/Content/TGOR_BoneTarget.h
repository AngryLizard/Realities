// The Gateway of Realities: Planes of Existence. By Salireths.

#pragma once

#include "../TGOR_AimInstance.h"

#include "TGOR_Target.h"
#include "TGOR_BoneTarget.generated.h"

/**
 * Target content, defines a target that can be aimed at. By default targets interactable component itself.
 */
UCLASS(Blueprintable)
class TARGETSYSTEM_API UTGOR_BoneTarget : public UTGOR_Target
{
	GENERATED_BODY()
		
public:
	UTGOR_BoneTarget();

	virtual bool OverlapTarget(UTGOR_AimTargetComponent* Component, const FVector& Origin, FTGOR_AimPoint& Point) const override;
	virtual bool ComputeTarget(UTGOR_AimTargetComponent* Component, const FVector& Origin, const FVector& Direction, FTGOR_AimPoint& Point) const override;
	virtual FVector QueryAimLocation(const FTGOR_AimInstance& Instance) const override;
	virtual FVector QueryStickyLocation(const FTGOR_AimInstance& Instance) const override;

	////////////////////////////////////////////////////////////////////////////////////////////////////
protected:

	/** Name of the bone to be targeted */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "!TGOR Target", Meta = (Keywords = "C++"))
		FName BoneName;

	/** Bone local offset */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "!TGOR Target", Meta = (Keywords = "C++"))
		FVector Offset;

	/** If the trace hits at all, make sure it hit the right bone */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "!TGOR Target", Meta = (Keywords = "C++"))
		bool CheckBoneMatch;

	/** Distance threshold on camera plane for a valid target */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "!TGOR Target", Meta = (Keywords = "C++"))
		float DistanceThreshold;
};
