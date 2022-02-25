// The Gateway of Realities: Planes of Existence.


#include "TGOR_EquipmentComponent.h"

#include "Realities/Components/Combat/TGOR_AimComponent.h"
#include "Realities/Components/System/Actions/TGOR_ActionComponent.h"

#include "Realities/Mod/Actions/Equipables/Weapons/TGOR_WeaponAction.h"
#include "Realities/Mod/Actions/Equipables/Useables/TGOR_UseableAction.h"
#include "Realities/Mod/Actions/Equipables/Inventories/TGOR_InventoryAction.h"
#include "Realities/Mod/Actions/Equipables/Interactables/TGOR_InteractableAction.h"


UTGOR_EquipmentComponent::UTGOR_EquipmentComponent()
	: Super()
{
	ActionContainers.Add(FTGOR_ActionContainer(UTGOR_InventoryAction::StaticClass()));
	ActionContainers.Add(FTGOR_ActionContainer(UTGOR_WeaponAction::StaticClass()));
	ActionContainers.Add(FTGOR_ActionContainer(UTGOR_UseableAction::StaticClass()));
	ActionContainers.Add(FTGOR_ActionContainer(UTGOR_InteractableAction::StaticClass()));

	AnimationSlotName = "Equipment";
}

