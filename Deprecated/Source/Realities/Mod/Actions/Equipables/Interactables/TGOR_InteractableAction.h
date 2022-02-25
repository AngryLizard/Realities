// TGOR (C) // CHECKED //
#pragma once

#include "Realities/Utility/TGOR_CustomEnumerations.h"
#include "Realities/Base/Instances/Combat/TGOR_AimInstance.h"

#include "Realities/Mod/Actions/Equipables/TGOR_EquipableAction.h"
#include "TGOR_InteractableAction.generated.h"

////////////////////////////////////////////// DECL //////////////////////////////////////////////////////

class UTGOR_AimComponent;

/**
*
*/
UCLASS()
class REALITIES_API UTGOR_InteractableAction : public UTGOR_EquipableAction
{
	GENERATED_BODY()

public:

	////////////////////////////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////// ENGINE ////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////

	UTGOR_InteractableAction();

	////////////////////////////////////////////////////////////////////////////////////////////////////

	/** Influence aimed at InteractionComponent */
	UFUNCTION(BlueprintCallable, Category = "TGOR Action", Meta = (Keywords = "C++"))
		void InfluenceTarget(UTGOR_ActionComponent* Component, const FTGOR_AimInstance& Aim, const FTGOR_ForceInstance& Forces);


protected:

private:

};
