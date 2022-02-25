// The Gateway of Realities: Planes of Existence.


#include "TGOR_DimensionDataInstance.h"

#include "Realities/Base/TGOR_Singleton.h"
#include "Realities/Mod/Dimensions/TGOR_Dimension.h"
#include "Realities/Mod/Spawner/Creatures/TGOR_Creature.h"
#include "Realities/Base/Content/TGOR_ContentManager.h"
#include "Realities/Base/TGOR_GameState.h"
#include "Realities/Base/TGOR_GameInstance.h"

#include "Realities/Utility/Storage/TGOR_Package.h"

FTGOR_DimensionIdentifier::FTGOR_DimensionIdentifier()
:	DimensionIdentifier(""),
	ActorIdentifier(-1)
{
}

void FTGOR_DimensionIdentifier::Write(FTGOR_GroupWritePackage& Package, UTGOR_Singleton* Context) const
{
	Package.WriteEntry("DimensionIdentifier", DimensionIdentifier);
	Package.WriteEntry("ActorIdentifier", ActorIdentifier);
}

void FTGOR_DimensionIdentifier::Read(FTGOR_GroupReadPackage& Package, UTGOR_Singleton* Context)
{
	Package.ReadEntry("DimensionIdentifier", DimensionIdentifier);
	Package.ReadEntry("ActorIdentifier", ActorIdentifier);
}


FTGOR_ComponentIdentifier::FTGOR_ComponentIdentifier()
:	FTGOR_DimensionIdentifier(),
	ComponentIdentifier("")
{
}


void FTGOR_ComponentIdentifier::Write(FTGOR_GroupWritePackage& Package, UTGOR_Singleton* Context) const
{
	FTGOR_DimensionIdentifier::Write(Package, Context);
	Package.WriteEntry("ComponentIdentifier", ComponentIdentifier);
}

void FTGOR_ComponentIdentifier::Read(FTGOR_GroupReadPackage& Package, UTGOR_Singleton* Context)
{
	FTGOR_DimensionIdentifier::Read(Package, Context);
	Package.ReadEntry("ComponentIdentifier", ComponentIdentifier);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

/** Component on given actor if available */
UPROPERTY(EditAnywhere, BlueprintReadWrite)
FName ComponentIdentifier;


FTGOR_DimensionDataInstance::FTGOR_DimensionDataInstance()
:	Open(false),
	Path(""),
	Content(nullptr),
	Dimension(nullptr),
	Location(FVector::ZeroVector),
	Owner(),
	Stage(ETGOR_DimensionLoadingStage::Unloaded)
{
}

void FTGOR_DimensionDataInstance::Write(FTGOR_GroupWritePackage& Package, UTGOR_Singleton* Context) const
{
	Package.WriteEntry("Path", Path);
	Package.WriteEntry("Content", Content);
	Package.WriteEntry("Owner", Owner);
}

void FTGOR_DimensionDataInstance::Read(FTGOR_GroupReadPackage& Package, UTGOR_Singleton* Context)
{
	Package.ReadEntry("Path", Path);
	Package.ReadEntry("Content", Content);
	Package.ReadEntry("Owner", Owner);
}

FTGOR_DimensionUpdateEntry::FTGOR_DimensionUpdateEntry()
	: Timestamp(), Content(nullptr)
{
}

bool FTGOR_DimensionUpdateEntry::NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess)
{
	bOutSuccess = true;

	uint8 Mask;
	SERIALISE_INIT_SOURCE
	SERIALISE_MASK_SOURCE(Content, Dimension);

	return(bOutSuccess);
}



FTGOR_DimensionUpdateList::FTGOR_DimensionUpdateList()
:	IsServerApproved(false),
	Owner("")
{
}


bool FTGOR_DimensionUpdateList::NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess)
{
	bOutSuccess = true;

	Ar << IsServerApproved;
	Ar << Owner;

	int32 Num = Dimensions.Num();
	Ar << Num;

	if (Ar.IsLoading())
	{
		Dimensions.Empty();
		for (int i = 0; i < Num; i++)
		{
			FName Key;
			Ar << Key;
			FTGOR_DimensionUpdateEntry& Entry = Dimensions.FindOrAdd(Key);
			Entry.NetSerialize(Ar, Map, bOutSuccess);
			if (!bOutSuccess) break;
		}
	}
	else
	{
		for (auto& pair : Dimensions)
		{
			Ar << pair.Key;
			pair.Value.NetSerialize(Ar, Map, bOutSuccess);
			if (!bOutSuccess) break;
		}
	}

	return(bOutSuccess);
}


FTGOR_DimensionUpdateState::FTGOR_DimensionUpdateState()
{

}

bool FTGOR_DimensionUpdateState::NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess)
{
	bOutSuccess = true;

	int32 Num = Dimensions.Num();
	Ar << Num;
	if (Ar.IsLoading())
	{
		Dimensions.Empty();
		for (int i = 0; i < Num; i++)
		{
			FName Key;
			Ar << Key;
			FVector& Entry = Dimensions.FindOrAdd(Key);
			Ar << Entry;
			if (!bOutSuccess) break;
		}
	}
	else
	{
		for (auto& pair : Dimensions)
		{
			Ar << pair.Key;
			Ar << pair.Value;
			if (!bOutSuccess) break;
		}
	}

	return(bOutSuccess);
}



FTGOR_PortalSelection::FTGOR_PortalSelection()
	: Suffix(""), Portal("")
{

}
