// The Gateway of Realities: Planes of Existence.


#include "TGOR_SimulationInstance.h"

#include "CoreSystem/TGOR_Singleton.h"
#include "RealitiesUGC/Mod/TGOR_ContentManager.h"
#include "CustomisationSystem/Content/Customisations/TGOR_Customisation.h"
#include "CustomisationSystem/Content/TGOR_Bodypart.h"
#include "CoreSystem/Gameplay/TGOR_GameInstance.h"
#include "CoreSystem/Storage/TGOR_Package.h"

SERIALISE_INIT_SOURCE_IMPLEMENT(FTGOR_SimulationInstance);

////////////////////////////////////////////////////////////////////////////////////////////////////

FTGOR_SimulationInstance::FTGOR_SimulationInstance()
{
}

void FTGOR_SimulationInstance::Write(FTGOR_GroupWritePackage& Package, UTGOR_Singleton* Context) const
{
}

void FTGOR_SimulationInstance::Read(FTGOR_GroupReadPackage& Package, UTGOR_Singleton* Context)
{
}

void FTGOR_SimulationInstance::Send(FTGOR_NetworkWritePackage& Package, UTGOR_Context* Context) const
{
}

void FTGOR_SimulationInstance::Recv(FTGOR_NetworkReadPackage& Package, UTGOR_Context* Context)
{
}