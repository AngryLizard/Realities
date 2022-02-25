// The Gateway of Realities: Planes of Existence.

#pragma once

#include "Realities/Mod/Matters/TGOR_Matter.h"
#include "Realities/Mod/Processes/TGOR_Process.h"
#include "TGOR_MatterProcess.generated.h"

/**
 * 
 */
UCLASS()
class REALITIES_API UTGOR_MatterProcess : public UTGOR_Process
{
	GENERATED_BODY()
	
public:
	UTGOR_MatterProcess();

	////////////////////////////////////////////////////////////////////////////////////////////////////
protected:

	/** Computes mass of a given composition */
	UFUNCTION(BlueprintPure, Category = "TGOR Inventory", Meta = (Keywords = "C++"))
		static float ComputeCompositionMass(const TMap<UTGOR_Matter*, int32>& Composition);

	/** Computer intersection between two matter compositions. */
	UFUNCTION(BlueprintPure, Category = "TGOR Inventory", Meta = (Keywords = "C++"))
		static TMap<UTGOR_Matter*, int32> ComputeCompositionFilter(const TMap<UTGOR_Matter*, int32>& Composition, const TMap<UTGOR_Matter*, int32>& Filter);

	/** Whether a matter composition is compatible with a given filter (as in, FilterMatter returns a non-empty result). */
	UFUNCTION(BlueprintPure, Category = "TGOR Inventory", Meta = (Keywords = "C++"))
		static bool IsCompositionCompatible(const TMap<UTGOR_Matter*, int32>& Composition, const TMap<UTGOR_Matter*, int32>& Filter);


	////////////////////////////////////////////////////////////////////////////////////////////////////
protected:

	/** Pulls out a specific mass of matter from a composition. Chooses matter in order of the composition. */
	UFUNCTION(BlueprintPure, Category = "TGOR Inventory", Meta = (Keywords = "C++"))
		TMap<UTGOR_Matter*, int32> MatterRationing(const TMap<UTGOR_Matter*, int32>& Composition, float Antimatter);

	////////////////////////////////////////////////////////////////////////////////////////////////////
private:
	UPROPERTY()
		FRandomStream MatterPermutator;

};
