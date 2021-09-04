// The Gateway of Realities: Planes of Existence.


#include "TGOR_UserInstance.h"
#include "CoreSystem/Storage/TGOR_Package.h"
#include "CoreSystem/TGOR_Singleton.h"
#include "CoreSystem/Gameplay/TGOR_GameInstance.h"
#include "CreatureSystem/Content/TGOR_Creature.h"
#include "DimensionSystem/Content/TGOR_Dimension.h"
#include "RealitiesUGC/Mod/TGOR_ContentManager.h"


FTGOR_UserProperties::FTGOR_UserProperties()
:	Name(""), 
	Color(1.0f, 1.0f, 1.0f, 1.0f),
	Serial(-1)
{
}

void FTGOR_UserProperties::Write(FTGOR_GroupWritePackage& Package, UTGOR_Singleton* Context) const
{
	Package.WriteEntry("Name", Name);
	Package.WriteEntry("Color", Color);
	Package.WriteEntry("Serial", Serial);
}

void FTGOR_UserProperties::Read(FTGOR_GroupReadPackage& Package, UTGOR_Singleton* Context)
{
	Package.ReadEntry("Name", Name);
	Package.ReadEntry("Color", Color);
	Package.ReadEntry("Serial", Serial);
}

FTGOR_UserInfo::FTGOR_UserInfo()
	: Ping(0.0f),
	Location(FVector::ZeroVector)
{
	Properties = FTGOR_UserProperties();
}


FTGOR_UserBodyDisplay::FTGOR_UserBodyDisplay()
:	Spawner(nullptr),
	Dimension(nullptr),
	Identifier(-1)
{
}

bool FTGOR_UserBodyDisplay::NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess)
{
	uint8 Mask;
	SERIALISE_INIT_SOURCE;
	SERIALISE_MASK_SOURCE(Spawner, Spawner);
	SERIALISE_MASK_SOURCE(Dimension, Dimension);

	Ar << Identifier;

	return true;
}

FTGOR_UserBody::FTGOR_UserBody()
	: Spawner(nullptr)
{
}

void FTGOR_UserBody::Write(FTGOR_GroupWritePackage& Package, UTGOR_Singleton* Context) const
{
	Package.WriteEntry("Spawner", Spawner);
}

void FTGOR_UserBody::Read(FTGOR_GroupReadPackage& Package, UTGOR_Singleton* Context)
{
	Package.ReadEntry("Spawner", Spawner);
}


FTGOR_UserInstance::FTGOR_UserInstance()
:	CurrentBody(-1),
	Privileges(ETGOR_Privileges::User),
	Controller(nullptr)
{
}

void FTGOR_UserInstance::Write(FTGOR_GroupWritePackage& Package, UTGOR_Singleton* Context) const
{
	Package.WriteEntry("Properties", Properties);
	Package.WriteEntry("Bodies", Bodies);
	Package.WriteEntry("CurrentBody", CurrentBody);
	Package.WriteEntry("PrivateKey", PrivateKey);
	Package.WriteEntry("PublicKey", PublicKey);
	Package.WriteEntry("Privileges", Privileges);
	Package.WriteEntry("Trackers", Trackers);
}

void FTGOR_UserInstance::Read(FTGOR_GroupReadPackage& Package, UTGOR_Singleton* Context)
{
	Package.ReadEntry("Properties", Properties);
	Package.ReadEntry("Bodies", Bodies);
	Package.ReadEntry("CurrentBody", CurrentBody);
	Package.ReadEntry("PrivateKey", PrivateKey);
	Package.ReadEntry("PublicKey", PublicKey);
	Package.ReadEntry("Privileges", Privileges);
	Package.ReadEntry("Trackers", Trackers);
}
