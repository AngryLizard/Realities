// The Gateway of Realities: Planes of Existence.


#include "TGOR_UserInstance.h"
#include "Realities/Utility/Storage/TGOR_Package.h"
#include "Realities/Base/TGOR_Singleton.h"
#include "Realities/Base/TGOR_GameInstance.h"
#include "Realities/Mod/Spawner/Creatures/TGOR_Creature.h"
#include "Realities/Mod/Dimensions/TGOR_Dimension.h"
#include "Realities/Base/Content/TGOR_ContentManager.h"


FTGOR_UserProperties::FTGOR_UserProperties()
:	Name(""), 
	Colour(1.0f, 1.0f, 1.0f, 1.0f),
	Serial(-1)
{
}

void FTGOR_UserProperties::Write(FTGOR_GroupWritePackage& Package, UTGOR_Singleton* Context) const
{
	Package.WriteEntry("Name", Name);
	Package.WriteEntry("Colour", Colour);
	Package.WriteEntry("Serial", Serial);
}

void FTGOR_UserProperties::Read(FTGOR_GroupReadPackage& Package, UTGOR_Singleton* Context)
{
	Package.ReadEntry("Name", Name);
	Package.ReadEntry("Colour", Colour);
	Package.ReadEntry("Serial", Serial);
}

FTGOR_UserInfo::FTGOR_UserInfo()
	: Ping(0.0f),
	Location(FVector::ZeroVector)
{
	Properties = FTGOR_UserProperties();
}


FTGOR_UserBodyDisplay::FTGOR_UserBodyDisplay()
:	Creature(nullptr),
	Dimension(nullptr),
	Identifier(-1)
{
}

bool FTGOR_UserBodyDisplay::NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess)
{
	uint8 Mask;
	SERIALISE_INIT_SOURCE;
	SERIALISE_MASK_SOURCE(Creature, Creature);
	SERIALISE_MASK_SOURCE(Dimension, Dimension);

	Ar << Identifier;

	return true;
}

FTGOR_UserBody::FTGOR_UserBody()
	: Creature(nullptr),
	Avatar(nullptr)
{
}

void FTGOR_UserBody::Write(FTGOR_GroupWritePackage& Package, UTGOR_Singleton* Context) const
{
	Package.WriteEntry("Creature", Creature);
	Package.WriteEntry("Identifier", Identifier);
}

void FTGOR_UserBody::Read(FTGOR_GroupReadPackage& Package, UTGOR_Singleton* Context)
{
	Package.ReadEntry("Creature", Creature);
	Package.ReadEntry("Identifier", Identifier);
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
