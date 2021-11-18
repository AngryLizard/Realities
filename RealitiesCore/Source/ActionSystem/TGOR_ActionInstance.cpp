// The Gateway of Realities: Planes of Existence.


#include "TGOR_ActionInstance.h"

#include "RealitiesUGC/Mod/TGOR_ContentManager.h"

#include "CoreSystem/Gameplay/TGOR_GameInstance.h"
#include "CoreSystem/TGOR_Singleton.h"
#include "CoreSystem/Storage/TGOR_Package.h"

#include "ActionSystem/Content/TGOR_Input.h"

DEFINE_LOG_CATEGORY(TGOR_ActionLogs);

SERIALISE_INIT_SOURCE_IMPLEMENT(FTGOR_ActionInputs);
SERIALISE_INIT_SOURCE_IMPLEMENT(FTGOR_ActionState);
SERIALISE_INIT_SOURCE_IMPLEMENT(FTGOR_LoadoutInstance);

FTGOR_ActionInputs::FTGOR_ActionInputs()
{
}

void FTGOR_ActionInputs::Send(FTGOR_NetworkWritePackage& Package, UTGOR_Singleton* Context) const
{
	Package.WriteEntry(Inputs);
}

void FTGOR_ActionInputs::Recv(FTGOR_NetworkReadPackage& Package, UTGOR_Singleton* Context)
{
	Package.ReadEntry(Inputs);
}


FTGOR_ActionState::FTGOR_ActionState()
:	ActiveSlot(INDEX_NONE),
	Time(),
	State(ETGOR_ActionStateEnumeration::Dead)
{
}

void FTGOR_ActionState::Send(FTGOR_NetworkWritePackage& Package, UTGOR_Singleton* Context) const
{
	Package.WriteEntry(Aim);
	Package.WriteEntry(ActiveSlot);
	Package.WriteEntry(Time);
	Package.WriteEntry(State);
}

void FTGOR_ActionState::Recv(FTGOR_NetworkReadPackage& Package, UTGOR_Singleton* Context)
{
	Package.ReadEntry(Aim);
	Package.ReadEntry(ActiveSlot);
	Package.ReadEntry(Time);
	Package.ReadEntry(State);
}


FTGOR_LoadoutInstance::FTGOR_LoadoutInstance()
{
}

void FTGOR_LoadoutInstance::Write(FTGOR_GroupWritePackage& Package, UTGOR_Singleton* Context) const
{
	Package.WriteEntry("Actions", Actions);
}

void FTGOR_LoadoutInstance::Read(FTGOR_GroupReadPackage& Package, UTGOR_Singleton* Context)
{
	Package.ReadEntry("Actions", Actions);
}

void FTGOR_LoadoutInstance::Send(FTGOR_NetworkWritePackage& Package, UTGOR_Context* Context) const
{
	Package.WriteEntry(Actions);
}

void FTGOR_LoadoutInstance::Recv(FTGOR_NetworkReadPackage& Package, UTGOR_Context* Context)
{
	Package.ReadEntry(Actions);
}