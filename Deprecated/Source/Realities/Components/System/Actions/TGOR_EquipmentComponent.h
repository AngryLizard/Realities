// TGOR (C) // CHECKED //
#pragma once

#include "CoreMinimal.h"

#include "Realities/Components/System/Actions/TGOR_ActionComponent.h"
#include "TGOR_EquipmentComponent.generated.h"

/**
* UTGOR_MovementModeComponent allows equipment of movement modes
*/
UCLASS(ClassGroup = (Custom), Blueprintable, meta = (BlueprintSpawnableComponent))
class REALITIES_API UTGOR_EquipmentComponent : public UTGOR_ActionComponent
{
	GENERATED_BODY()

public:

	////////////////////////////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////// ENGINE ////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////

	UTGOR_EquipmentComponent();
	
	////////////////////////////////////////////////////////////////////////////////////////////////////

	/////////////////////////////////////////////// INTERNAL ///////////////////////////////////////////

protected:

private:

};
