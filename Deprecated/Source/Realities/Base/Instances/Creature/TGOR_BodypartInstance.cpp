// The Gateway of Realities: Planes of Existence.


#include "TGOR_BodypartInstance.h"

#include "Realities/Base/TGOR_Singleton.h"
#include "Realities/Base/Content/TGOR_ContentManager.h"
#include "Realities/Mod/Customisations/TGOR_Customisation.h"
#include "Realities/Mod/Bodyparts/TGOR_Bodypart.h"
#include "Realities/Mod/Skins/TGOR_Skin.h"
#include "Realities/Base/Content/TGOR_ContentManager.h"
#include "Realities/Base/TGOR_GameInstance.h"
#include "Realities/Utility/Storage/TGOR_Package.h"


FTGOR_CustomisationInstance::FTGOR_CustomisationInstance()
:	Content(nullptr),
	Index(-1)
{
}

void FTGOR_CustomisationInstance::Write(FTGOR_GroupWritePackage& Package, UTGOR_Singleton* Context) const
{
	Package.WriteEntry("Content", Content);
	Package.WriteEntry("Index", Index);

	if (IsValid(Content))
	{
		Content->Write(Package, Payload);
	}
}

void FTGOR_CustomisationInstance::Read(FTGOR_GroupReadPackage& Package, UTGOR_Singleton* Context)
{
	Package.ReadEntry("Content", Content);
	Package.ReadEntry("Index", Index);

	if (IsValid(Content))
	{
		Content->Read(Package, Payload);
	}
}

bool FTGOR_CustomisationInstance::NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess)
{
	uint8 Mask;
	SERIALISE_INIT_SOURCE
	SERIALISE_MASK_SOURCE(Content, Customisation)
	Ar << Index;
	Ar << Payload;

	return(bOutSuccess);
}



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

bool FTGOR_BodypartInstance::NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess)
{
	uint8 Mask;
	SERIALISE_INIT_SOURCE
	SERIALISE_MASK_SOURCE(Content, Bodypart)
	SERIALISE_MASK_CUSTOM_LIST(CustomisationInstances)

	Ar << Depth;

	return(bOutSuccess);
}