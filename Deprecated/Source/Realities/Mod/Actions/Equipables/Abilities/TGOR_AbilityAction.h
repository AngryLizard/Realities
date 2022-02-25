// TGOR (C) // CHECKED //
#pragma once

#include "Realities/Utility/TGOR_CustomEnumerations.h"
#include "Realities/Base/Instances/Creature/TGOR_MovementInstance.h"

#include "Realities/Mod/Actions/Equipables/TGOR_EquipableAction.h"
#include "TGOR_AbilityAction.generated.h"

////////////////////////////////////////////// DECL //////////////////////////////////////////////////////

class UTGOR_MovementComponent;
class UTGOR_Movement;

/**
*
*/
UCLASS()
class REALITIES_API UTGOR_AbilityAction : public UTGOR_EquipableAction
{
	GENERATED_BODY()

public:

	UTGOR_AbilityAction();

	//////////////////////////////////////////// IMPLEMENTABLES ////////////////////////////////////////

	/** */

	////////////////////////////////////////////////////////////////////////////////////////////////////
protected:

	/** Computes current movement space of owning movement component, also returns actually applied force (opposed to only experienced) */
	UFUNCTION(BlueprintPure, Category = "TGOR Action", Meta = (Keywords = "C++"))
		FTGOR_MovementSpace ComputeMovementSpace(UTGOR_MovementComponent* Component) const;
	
	////////////////////////////////////////////////////////////////////////////////////////////////////


protected:

private:

};
