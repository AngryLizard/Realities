// The Gateway of Realities: Planes of Existence.


#include "TGOR_BodypartInstance.h"

#include "CoreSystem/TGOR_Singleton.h"
#include "RealitiesUGC/Mod/TGOR_ContentManager.h"
#include "CustomisationSystem/Content/Customisations/TGOR_Customisation.h"
#include "CustomisationSystem/Content/TGOR_Bodypart.h"
#include "CoreSystem/Gameplay/TGOR_GameInstance.h"
#include "CoreSystem/Storage/TGOR_Package.h"

SERIALISE_INIT_SOURCE_IMPLEMENT(FTGOR_CustomisationInstance);
SERIALISE_INIT_SOURCE_IMPLEMENT(FTGOR_BodypartInstance);
SERIALISE_INIT_SOURCE_IMPLEMENT(FTGOR_AppearanceInstance);

FTGOR_CustomisationInstance::FTGOR_CustomisationInstance()
:	Content(nullptr)
{
}

void FTGOR_CustomisationInstance::Write(FTGOR_GroupWritePackage& Package, UTGOR_Singleton* Context) const
{
	Package.WriteEntry("Content", Content);

	if (IsValid(Content))
	{
		Content->Write(Package, Payload);
	}
}

void FTGOR_CustomisationInstance::Read(FTGOR_GroupReadPackage& Package, UTGOR_Singleton* Context)
{
	Package.ReadEntry("Content", Content);

	if (IsValid(Content))
	{
		Content->Read(Package, Payload);
	}
}

void FTGOR_CustomisationInstance::Send(FTGOR_NetworkWritePackage& Package, UTGOR_Context* Context) const
{
	Package.WriteEntry(Content);
	Package.WriteEntry(Payload.Bytes);
}

void FTGOR_CustomisationInstance::Recv(FTGOR_NetworkReadPackage& Package, UTGOR_Context* Context)
{
	Package.ReadEntry(Content);
	Package.ReadEntry(Payload.Bytes);
}


////////////////////////////////////////////////////////////////////////////////////////////////////

FTGOR_BodypartInstance::FTGOR_BodypartInstance()
:	Content(nullptr),
	Depth(0)
{
}

void FTGOR_BodypartInstance::Write(FTGOR_GroupWritePackage& Package, UTGOR_Singleton* Context) const
{
	Package.WriteEntry("Content", Content);
	Package.WriteEntry("Depth", Depth);
	Package.WriteEntry("Customisation", CustomisationInstances);
}

void FTGOR_BodypartInstance::Read(FTGOR_GroupReadPackage& Package, UTGOR_Singleton* Context)
{
	Package.ReadEntry("Content", Content);
	Package.ReadEntry("Depth", Depth);
	Package.ReadEntry("Customisation", CustomisationInstances);
}

void FTGOR_BodypartInstance::Send(FTGOR_NetworkWritePackage& Package, UTGOR_Context* Context) const
{
	Package.WriteEntry(Content);
	Package.WriteEntry(Depth);
	Package.WriteEntry(CustomisationInstances);
}

void FTGOR_BodypartInstance::Recv(FTGOR_NetworkReadPackage& Package, UTGOR_Context* Context)
{
	Package.ReadEntry(Content);
	Package.ReadEntry(Depth);
	Package.ReadEntry(CustomisationInstances);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

FTGOR_AppearanceInstance::FTGOR_AppearanceInstance()
{
}

void FTGOR_AppearanceInstance::Write(FTGOR_GroupWritePackage& Package, UTGOR_Singleton* Context) const
{
	Package.WriteEntry("Bodyparts", Bodyparts);
}

void FTGOR_AppearanceInstance::Read(FTGOR_GroupReadPackage& Package, UTGOR_Singleton* Context)
{
	Package.ReadEntry("Bodyparts", Bodyparts);
}

void FTGOR_AppearanceInstance::Send(FTGOR_NetworkWritePackage& Package, UTGOR_Context* Context) const
{
	Package.WriteEntry(Bodyparts);
}

void FTGOR_AppearanceInstance::Recv(FTGOR_NetworkReadPackage& Package, UTGOR_Context* Context)
{
	Package.ReadEntry(Bodyparts);
}
