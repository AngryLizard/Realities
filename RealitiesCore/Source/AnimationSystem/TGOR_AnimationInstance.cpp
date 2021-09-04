// The Gateway of Realities: Planes of Existence.

#include "TGOR_AnimationInstance.h"
#include "CoreSystem/TGOR_Singleton.h"
#include "CoreSystem/Storage/TGOR_Package.h"
#include "CoreSystem/Gameplay/TGOR_GameInstance.h"

#include "AnimationSystem/Content/TGOR_Archetype.h"
#include "AnimationSystem/Content/TGOR_RigParam.h"

SERIALISE_INIT_SOURCE_IMPLEMENT(FTGOR_AnimationInstance);


FTGOR_AnimationInstance::FTGOR_AnimationInstance()
	: Archetype(nullptr)
{
}

void FTGOR_AnimationInstance::Write(FTGOR_GroupWritePackage& Package, UTGOR_Singleton* Context) const
{
	Package.WriteEntry("Archetype", Archetype);
	Package.WriteEntry("Params", Params);
}

void FTGOR_AnimationInstance::Read(FTGOR_GroupReadPackage& Package, UTGOR_Singleton* Context)
{
	Package.ReadEntry("Archetype", Archetype);
	Package.ReadEntry("Params", Params);
}

void FTGOR_AnimationInstance::Send(FTGOR_NetworkWritePackage& Package, UTGOR_Context* Context) const
{
	Package.WriteEntry(Archetype);
	Package.WriteEntry(Params);
}

void FTGOR_AnimationInstance::Recv(FTGOR_NetworkReadPackage& Package, UTGOR_Context* Context)
{
	Package.ReadEntry(Archetype);
	Package.ReadEntry(Params);
}
