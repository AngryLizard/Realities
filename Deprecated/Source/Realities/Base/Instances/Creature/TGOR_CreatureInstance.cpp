// The Gateway of Realities: Planes of Existence.


#include "TGOR_CreatureInstance.h"

#include "Realities/Base/TGOR_Singleton.h"
#include "Realities/Base/Content/TGOR_ContentManager.h"
#include "Realities/Utility/Storage/TGOR_Package.h"

#include "Realities/Mod/Actions/TGOR_Action.h"
#include "Realities/Mod/Colours/TGOR_Colour.h"
#include "Realities/Mod/Effects/TGOR_Effect.h"
#include "Realities/Mod/Spawner/Items/TGOR_Item.h"
#include "Realities/Mod/Organs/TGOR_Organ.h"
#include "Realities/Mod/Skins/TGOR_Skin.h"
#include "Realities/Mod/Spawner/Creatures/TGOR_Creature.h"
#include "Realities/Base/TGOR_GameState.h"

FTGOR_CreatureAppearanceInstance::FTGOR_CreatureAppearanceInstance()
{
}

void FTGOR_CreatureAppearanceInstance::Write(FTGOR_GroupWritePackage& Package, UTGOR_Singleton* Context) const
{
	Package.WriteEntry("Bodyparts", Bodyparts);
}

void FTGOR_CreatureAppearanceInstance::Read(FTGOR_GroupReadPackage& Package, UTGOR_Singleton* Context)
{
	Package.ReadEntry("Bodyparts", Bodyparts);
}


bool FTGOR_CreatureAppearanceInstance::NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess)
{
	uint8 Mask;
	SERIALISE_INIT_SOURCE
	SERIALISE_MASK_CUSTOM_LIST(Bodyparts)
	
	return(bOutSuccess);
}


FTGOR_CreatureSetupInstance::FTGOR_CreatureSetupInstance()
: Creature(nullptr)
{
}



void FTGOR_CreatureSetupInstance::Write(FTGOR_GroupWritePackage& Package, UTGOR_Singleton* Context) const
{
	Package.WriteEntry("Appearance", Appearance);
	Package.WriteEntry("Creature", Creature);
}

void FTGOR_CreatureSetupInstance::Read(FTGOR_GroupReadPackage& Package, UTGOR_Singleton* Context)
{
	Package.ReadEntry("Appearance", Appearance);
	Package.ReadEntry("Creature", Creature);
}


bool FTGOR_CreatureSetupInstance::NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess)
{
	uint8 Mask;
	SERIALISE_INIT_SOURCE;
	SERIALISE_MASK_SOURCE(Creature, Creature);

	Appearance.NetSerialize(Ar, Map, bOutSuccess);

	return(bOutSuccess);
}