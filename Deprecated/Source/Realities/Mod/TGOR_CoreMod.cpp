// The Gateway of Realities: Planes of Existence. By Salireths.


#include "TGOR_CoreMod.h"

#include "Realities/Mod/Actions/TGOR_Action.h"
#include "Realities/Mod/Actions/Equipables/Abilities/TGOR_AbilityAction.h"
#include "Realities/Mod/Actions/Equipables/Interactables/TGOR_InteractableAction.h"
#include "Realities/Mod/Actions/Equipables/Inventories/TGOR_InventoryAction.h"
#include "Realities/Mod/Actions/Equipables/Useables/TGOR_UseableAction.h"
#include "Realities/Mod/Actions/Equipables/Weapons/TGOR_WeaponAction.h"

#include "Realities/Mod/Animations/TGOR_Animation.h"
#include "Realities/Mod/Bodyparts/TGOR_Bodypart.h"

#include "Realities/Mod/Cameras/TGOR_Camera.h"
#include "Realities/Mod/Cameras/TGOR_DollyCamera.h"
#include "Realities/Mod/Cameras/TGOR_FreeCamera.h"
#include "Realities/Mod/Cameras/TGOR_FixCamera.h"
#include "Realities/Mod/Cameras/TGOR_HeightCamera.h"
#include "Realities/Mod/Cameras/TGOR_LookatCamera.h"
#include "Realities/Mod/Cameras/TGOR_PivotCamera.h"
#include "Realities/Mod/Cameras/TGOR_ShoulderCamera.h"
#include "Realities/Mod/Cameras/TGOR_ZoomCamera.h"
#include "Realities/Mod/Cameras/TGOR_ParamCamera.h"

#include "Realities/Mod/Channels/TGOR_Channel.h"
#include "Realities/Mod/Colours/TGOR_Colour.h"
#include "Realities/Mod/Commands/TGOR_Command.h"
#include "Realities/Mod/Dimensions/TGOR_Dimension.h"
#include "Realities/Mod/Forces/TGOR_Force.h"
#include "Realities/Mod/Inputs/TGOR_Input.h"
#include "Realities/Mod/Loaders/TGOR_Loader.h"
#include "Realities/Mod/Matters/TGOR_Matter.h"
#include "Realities/Mod/Effects/TGOR_Effect.h"
#include "Realities/Mod/Events/TGOR_Event.h"
#include "Realities/Mod/Customisations/TGOR_Customisation.h"
#include "Realities/Mod/Spawner/Items/TGOR_Item.h"
#include "Realities/Mod/Spawner/Items/TGOR_InventoryItem.h"
#include "Realities/Mod/Spawner/Items/TGOR_MatterItem.h"
#include "Realities/Mod/Movements/TGOR_Movement.h"
#include "Realities/Mod/Organs/TGOR_Organ.h"
#include "Realities/Mod/Overlays/TGOR_Overlay.h"
#include "Realities/Mod/Pipelines/TGOR_Pipeline.h"
#include "Realities/Mod/Processes/TGOR_Process.h"
#include "Realities/Mod/Processes/TGOR_Science.h"
#include "Realities/Mod/Processes/TGOR_Craft.h"
#include "Realities/Mod/Skins/TGOR_Skin.h"
#include "Realities/Mod/Targets/Sockets/TGOR_Socket.h"
#include "Realities/Mod/Targets/Sockets/TGOR_PipelineSocket.h"
#include "Realities/Mod/Targets/Sockets/TGOR_NamedSocket.h"
#include "Realities/Mod/Spawner/Creatures/TGOR_Creature.h"
#include "Realities/Mod/Spawner/TGOR_Spawner.h"
#include "Realities/Mod/Loaders/TGOR_Loader.h"
#include "Realities/Mod/Knowledges/TGOR_RootKnowledge.h"
#include "Realities/Mod/Trackers/TGOR_Tracker.h"
#include "Realities/Mod/Targets/TGOR_Target.h"
#include "Realities/Mod/Targets/TGOR_ComponentTarget.h"


UTGOR_CoreMod::UTGOR_CoreMod(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	Name = L"CoreMod";
	Version = L"v0.0";

	FTGOR_ContentLink Link;
	Link.Action = ETGOR_ModAction::Add;

	// Adding all base classes for better categorisation
	Link.Content = UTGOR_Action::StaticClass();
	ContentLinks.Emplace(Link);

	Link.Content = UTGOR_AbilityAction::StaticClass();
	ContentLinks.Emplace(Link);

	Link.Content = UTGOR_InteractableAction::StaticClass();
	ContentLinks.Emplace(Link);

	Link.Content = UTGOR_InventoryAction::StaticClass();
	ContentLinks.Emplace(Link);

	Link.Content = UTGOR_UseableAction::StaticClass();
	ContentLinks.Emplace(Link);

	Link.Content = UTGOR_WeaponAction::StaticClass();
	ContentLinks.Emplace(Link);

	////
	Link.Content = UTGOR_Animation::StaticClass();
	ContentLinks.Emplace(Link);

	////
	Link.Content = UTGOR_Bodypart::StaticClass();
	ContentLinks.Emplace(Link);

	////
	Link.Content = UTGOR_Camera::StaticClass();
	ContentLinks.Emplace(Link);

	Link.Content = UTGOR_DollyCamera::StaticClass();
	ContentLinks.Emplace(Link);

	Link.Content = UTGOR_FreeCamera::StaticClass();
	ContentLinks.Emplace(Link);

	Link.Content = UTGOR_FixCamera::StaticClass();
	ContentLinks.Emplace(Link);

	Link.Content = UTGOR_HeightCamera::StaticClass();
	ContentLinks.Emplace(Link);

	Link.Content = UTGOR_LookatCamera::StaticClass();
	ContentLinks.Emplace(Link);

	Link.Content = UTGOR_PivotCamera::StaticClass();
	ContentLinks.Emplace(Link);

	Link.Content = UTGOR_ShoulderCamera::StaticClass();
	ContentLinks.Emplace(Link);

	Link.Content = UTGOR_ZoomCamera::StaticClass();
	ContentLinks.Emplace(Link);

	Link.Content = UTGOR_ParamCamera::StaticClass();
	ContentLinks.Emplace(Link);

	////
	Link.Content = UTGOR_Channel::StaticClass();
	ContentLinks.Emplace(Link);

	////
	Link.Content = UTGOR_Colour::StaticClass();
	ContentLinks.Emplace(Link);

	////
	Link.Content = UTGOR_Command::StaticClass();
	ContentLinks.Emplace(Link);

	////
	Link.Content = UTGOR_Dimension::StaticClass();
	ContentLinks.Emplace(Link);

	////
	Link.Content = UTGOR_Effect::StaticClass();
	ContentLinks.Emplace(Link);

	////
	Link.Content = UTGOR_Event::StaticClass();
	ContentLinks.Emplace(Link);

	////
	Link.Content = UTGOR_Force::StaticClass();
	ContentLinks.Emplace(Link);

	////
	Link.Content = UTGOR_Input::StaticClass();
	ContentLinks.Emplace(Link);

	////
	Link.Content = UTGOR_Loader::StaticClass();
	ContentLinks.Emplace(Link);

	////
	Link.Content = UTGOR_Matter::StaticClass();
	ContentLinks.Emplace(Link);

	////
	Link.Content = UTGOR_Customisation::StaticClass();
	ContentLinks.Emplace(Link);

	////
	Link.Content = UTGOR_Item::StaticClass();
	ContentLinks.Emplace(Link);

	Link.Content = UTGOR_InventoryItem::StaticClass();
	ContentLinks.Emplace(Link);

	Link.Content = UTGOR_MatterItem::StaticClass();
	ContentLinks.Emplace(Link);

	////
	//Link.Content = UTGOR_Metamorph::StaticClass();
	//ContentLinks.Emplace(Link);

	////
	Link.Content = UTGOR_Movement::StaticClass();
	ContentLinks.Emplace(Link);

	////
	Link.Content = UTGOR_Organ::StaticClass();
	ContentLinks.Emplace(Link);

	////
	Link.Content = UTGOR_Overlay::StaticClass();
	ContentLinks.Emplace(Link);

	////
	Link.Content = UTGOR_Pipeline::StaticClass();
	ContentLinks.Emplace(Link);

	////
	Link.Content = UTGOR_Process::StaticClass();
	ContentLinks.Emplace(Link);

	////
	Link.Content = UTGOR_Science::StaticClass();
	ContentLinks.Emplace(Link);

	////
	Link.Content = UTGOR_Craft::StaticClass();
	ContentLinks.Emplace(Link);

	////
	Link.Content = UTGOR_Skin::StaticClass();
	ContentLinks.Emplace(Link);

	////
	Link.Content = UTGOR_Socket::StaticClass();
	ContentLinks.Emplace(Link);

	Link.Content = UTGOR_PipelineSocket::StaticClass();
	ContentLinks.Emplace(Link);

	Link.Content = UTGOR_NamedSocket::StaticClass();
	ContentLinks.Emplace(Link);

	////
	Link.Content = UTGOR_Creature::StaticClass();
	ContentLinks.Emplace(Link);

	////
	Link.Content = UTGOR_Spawner::StaticClass();
	ContentLinks.Emplace(Link);

	////
	Link.Content = UTGOR_Loader::StaticClass();
	ContentLinks.Emplace(Link);

	////
	Link.Content = UTGOR_Tracker::StaticClass();
	ContentLinks.Emplace(Link);

	////
	Link.Content = UTGOR_Knowledge::StaticClass();
	ContentLinks.Emplace(Link);

	Link.Content = UTGOR_RootKnowledge::StaticClass();
	ContentLinks.Emplace(Link);

	////
	Link.Content = UTGOR_Target::StaticClass();
	ContentLinks.Emplace(Link);

	Link.Content = UTGOR_ComponentTarget::StaticClass();
	ContentLinks.Emplace(Link);
}