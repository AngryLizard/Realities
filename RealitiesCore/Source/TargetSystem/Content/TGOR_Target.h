// The Gateway of Realities: Planes of Existence. By Salireths.

#pragma once

#include "../TGOR_AimInstance.h"

#include "DimensionSystem/Content/TGOR_SpawnModule.h"
#include "TGOR_Target.generated.h"

#define PARAMS_CHK check(Component && "Interactable Component invalid"); if (MaxDistance < SMALL_NUMBER) { return false; }

class UTGOR_AimTargetComponent;

/**
 * Target content, defines a target that can be aimed at. By default targets interactable component itself.
 */
UCLASS(Blueprintable)
class TARGETSYSTEM_API UTGOR_Target : public UTGOR_SpawnModule
{
	GENERATED_BODY()
		
public:
	UTGOR_Target();

	/** Intersects this target with a sphere overlap with given origin and radius (Used for hit detection) */
	UFUNCTION(BlueprintPure, Category = "!TGOR Target", Meta = (Keywords = "C++"))
		virtual bool OverlapTarget(UTGOR_AimTargetComponent* Component, const FVector& Origin, float Radius, FTGOR_AimPoint& Point) const;

	/** Fill AimPoint with custom target data from ray, used to store a potential target location (Used for targeting) */
	UFUNCTION(BlueprintPure, Category = "!TGOR Target", Meta = (Keywords = "C++"))
		virtual bool SearchTarget(UTGOR_AimTargetComponent* Component, const FVector& Origin, const FVector& Direction, float MaxDistance, FTGOR_AimPoint& Point) const;

	/** Fill AimInstance with custom target data from ray, used to store a relative target location */
	UFUNCTION(BlueprintPure, Category = "!TGOR Target", Meta = (Keywords = "C++"))
		virtual bool ComputeTarget(const FTGOR_AimPoint& Point, const FVector& Origin, const FVector& Direction, float MaxDistance, FTGOR_AimInstance& Instance) const;

	/** Return aim location in world space given an instance */
	UFUNCTION(BlueprintPure, Category = "!TGOR Target", Meta = (Keywords = "C++"))
		virtual FVector QueryAimLocation(const FTGOR_AimInstance& Instance) const;

	/** Return target location in world space given an instance */
	UFUNCTION(BlueprintPure, Category = "!TGOR Target", Meta = (Keywords = "C++"))
		virtual FVector QueryStickyLocation(const FTGOR_AimInstance& Instance) const;

	/** Return display struct for a given aim (defaults to this target content) */
	UFUNCTION(BlueprintPure, Category = "!TGOR Target", Meta = (Keywords = "C++"))
		virtual FTGOR_Display QueryDisplay(const FTGOR_AimInstance& Instance) const;

	/** Return interactable component for a given aim (defaults to the first Interactable component found on Component owner)*/
	UFUNCTION(BlueprintPure, Category = "!TGOR Target", Meta = (Keywords = "C++"))
		virtual UTGOR_AimTargetComponent* QueryInteractable(const FTGOR_AimInstance& Instance) const;

	////////////////////////////////////////////////////////////////////////////////////////////////////

	/** Computes local offset from ray to transform */
	UFUNCTION(BlueprintPure, Category = "!TGOR Target", Meta = (Keywords = "C++"))
		FVector ComputeOffset(const FTransform& Transform, const FVector& Origin, const FVector& Direction) const;

	/** Computes projection from ray to transform */
	UFUNCTION(BlueprintPure, Category = "!TGOR Target", Meta = (Keywords = "C++"))
		FVector ComputeProject(const FVector& Location, const FVector& Origin, const FVector& Direction) const;

	/** Computes distance ratio and returns whether location is in range */
	UFUNCTION(BlueprintPure, Category = "!TGOR Target", Meta = (Keywords = "C++"))
		bool ComputeDistance(const FVector& Location, const FVector& Origin, float Radius, FTGOR_AimPoint& Point) const;

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Importance of thit target in [0, 1]. Considered while weighting */
	UPROPERTY(EditAnywhere, Category = "!TGOR Inventory")
		float Importance;

};
