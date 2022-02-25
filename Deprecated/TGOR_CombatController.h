// The Gateway of Realities: Planes of Existence.

#pragma once

#include "TGOR_OnlineController.h"
#include "TGOR_CombatController.generated.h"

////////////////////////////////////////////// DECL //////////////////////////////////////////////////////

class ATGOR_Battleable;

/**
* TGOR_CombatController allows a playercontroller to filter input and relay it to a controllable
*/
UCLASS()
class REALITIES_API ATGOR_CombatController : public ATGOR_OnlineController
{
	GENERATED_BODY()
	
public:

	////////////////////////////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////// ENGINE ////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////

	ATGOR_CombatController();

	////////////////////////////////////////////////////////////////////////////////////////////////////

	/** Ensures battleable */
	UFUNCTION(BlueprintCallable, Category = "TGOR|Game", Meta = (ExpandEnumAsExecs = "Branches", Keywords = "C++"))
		ATGOR_Battleable* EnsureBattleable(ETGOR_FetchEnumeration& Branches);

	/** Find any region component in currently controlled */
	UFUNCTION(BlueprintCallable, Category = "TGOR|Game", Meta = (ExpandEnumAsExecs = "Branches", Keywords = "C++"))
		UTGOR_RegionComponent* FindRegionComponent(ETGOR_FetchEnumeration& Branches);

	////////////////////////////////////////////////////////////////////////////////////////////////////

	/** Enter battlemode */
	UFUNCTION(BlueprintCallable, Category = "TGOR|Interaction", Meta = (ExpandEnumAsExecs = "Branches", Keywords = "C++"))
		void EnterBattleMode(ETGOR_ComputeEnumeration& Branches);

	/** Leave battlemode */
	UFUNCTION(BlueprintCallable, Category = "TGOR|Interaction", Meta = (ExpandEnumAsExecs = "Branches", Keywords = "C++"))
		void LeaveBattleMode(ETGOR_ComputeEnumeration& Branches);

	////////////////////////////////////////////////////////////////////////////////////////////////////

	/** Battleable that is currently controlled */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TGOR|Game", Meta = (Keywords = "C++", ClampMin = "0.1", ClampMax = "0.9"))
		ATGOR_Battleable* Battleable;

	/** Lock radius */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TGOR|Game", Meta = (Keywords = "C++", ClampMin = "0.1", ClampMax = "0.9"))
		float LockRadius;

	/////////////////////////////////////////////// INTERNAL ///////////////////////////////////////////

protected:

private:

};
