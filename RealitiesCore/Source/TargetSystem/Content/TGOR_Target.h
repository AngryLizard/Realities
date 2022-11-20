// The Gateway of Realities: Planes of Existence. By Salireths.

#pragma once

#include "../TGOR_AimInstance.h"

#include "DimensionSystem/Content/TGOR_SpawnModule.h"
#include "TGOR_Target.generated.h"

#define PARAMS_CHK check(Component && "Interactable Component invalid"); if ( Component->TargetRadius < SMALL_NUMBER) { return false; }

class UTGOR_AimComponent;
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

	/** Whether this target is valid for a given source */
	UFUNCTION(BlueprintPure, Category = "!TGOR Target", Meta = (Keywords = "C++"))
		virtual bool TargetCondition(UTGOR_AimTargetComponent* Component, UTGOR_AimComponent* Source) const;

	/** Intersects this target with a sphere overlap with given origin and radius (Used for hit detection) */
	UFUNCTION(BlueprintPure, Category = "!TGOR Target", Meta = (Keywords = "C++"))
		virtual bool OverlapTarget(UTGOR_AimTargetComponent* Component, const FVector& Origin, FTGOR_AimPoint& Point) const;

	/** Fill AimPoint with custom target data from ray, used to store a potential target location (Used for targeting) */
	UFUNCTION(BlueprintPure, Category = "!TGOR Target", Meta = (Keywords = "C++"))
		virtual bool ComputeTarget(UTGOR_AimTargetComponent* Component, const FVector& Origin, const FVector& Direction, FTGOR_AimPoint& Point) const;

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
public:

	/** Importance of thit target in [0, 1]. Considered while weighting */
	UPROPERTY(EditAnywhere, Category = "!TGOR Inventory")
		float Importance;

};
