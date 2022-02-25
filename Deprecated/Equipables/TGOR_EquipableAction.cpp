// The Gateway of Realities: Planes of Existence.


#include "TGOR_EquipableAction.h"
#include "Realities/Base/Tasks/TGOR_ActionTask.h"
#include "Realities/Components/Combat/TGOR_AimComponent.h"
#include "Realities/Components/System/TGOR_AttributeComponent.h"
#include "Realities/Components/System/TGOR_ActionComponent.h"
#include "Realities/Components/Movement/TGOR_MovementComponent.h"
#include "Realities/Mod/Unlocks/Movements/TGOR_Movement.h"
#include "Realities/Mod/Animations/TGOR_Animation.h"
#include "Realities/Mod/Targets/TGOR_Target.h"


FTGOR_EquipmentAim::FTGOR_EquipmentAim()
:	Aim(nullptr),
	Movement(nullptr),
	Location(FVector::ZeroVector)
{
}


UTGOR_EquipableAction::UTGOR_EquipableAction()
	: Super()
{
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void UTGOR_EquipableAction::Invariant_Implementation(UTGOR_ActionTask* Storage, UTGOR_ActionComponent* Component, ETGOR_ValidEnumeration& Branches) const
{
	Branches = ETGOR_ValidEnumeration::Valid;

	FTGOR_EquipmentAim Data;
	//if (ConstructData(Component, Data))
	{
		Precondition(Component, Data, Branches);
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////
