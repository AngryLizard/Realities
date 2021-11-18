// The Gateway of Realities: Planes of Existence.


#include "TGOR_CreatureInstance.h"

#include "RealitiesUGC/Mod/TGOR_ContentManager.h"

#include "CoreSystem/TGOR_Singleton.h"
#include "CoreSystem/Storage/TGOR_Package.h"

#include "CustomisationSystem/Content/TGOR_Skin.h"
#include "CreatureSystem/Content/TGOR_Creature.h"
#include "ActionSystem/Content/TGOR_Action.h"

#include "CoreSystem/Gameplay/TGOR_GameInstance.h"

SERIALISE_INIT_SOURCE_IMPLEMENT(FTGOR_CreatureSetupInstance);


////////////////////////////////////////////////////////////////////////////////////////////////////

FTGOR_CreatureSetupInstance::FTGOR_CreatureSetupInstance()
{
}

void FTGOR_CreatureSetupInstance::Write(FTGOR_GroupWritePackage& Package, UTGOR_Singleton* Context) const
{
	Package.WriteEntry("Appearance", Appearance);
}

void FTGOR_CreatureSetupInstance::Read(FTGOR_GroupReadPackage& Package, UTGOR_Singleton* Context)
{
	Package.ReadEntry("Appearance", Appearance);
}

void FTGOR_CreatureSetupInstance::Send(FTGOR_NetworkWritePackage& Package, UTGOR_Context* Context) const
{
	Package.WriteEntry(Appearance);
}

void FTGOR_CreatureSetupInstance::Recv(FTGOR_NetworkReadPackage& Package, UTGOR_Context* Context)
{
	Package.ReadEntry(Appearance);
}
