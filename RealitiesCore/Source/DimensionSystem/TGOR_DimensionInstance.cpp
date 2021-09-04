// The Gateway of Realities: Planes of Existence.


#include "TGOR_DimensionInstance.h"

#include "RealitiesUGC/Mod/TGOR_ContentManager.h"

#include "CoreSystem/TGOR_Singleton.h"
#include "CoreSystem/Gameplay/TGOR_GameInstance.h"
#include "CoreSystem/Storage/TGOR_Package.h"

#include "DimensionSystem/Components/TGOR_ConnectionComponent.h"
#include "DimensionSystem/Content/TGOR_Dimension.h"

SERIALISE_INIT_SOURCE_IMPLEMENT(FTGOR_ConnectionCollection);

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


FTGOR_DimensionInstance::FTGOR_DimensionInstance()
:	Open(false),
	Path(""),
	Content(nullptr),
	Dimension(nullptr),
	Location(FVector::ZeroVector),
	Owner(),
	Stage(ETGOR_DimensionLoadingStage::Unloaded)
{
}

void FTGOR_DimensionInstance::Write(FTGOR_GroupWritePackage& Package, UTGOR_Singleton* Context) const
{
	Package.WriteEntry("Path", Path);
	Package.WriteEntry("Content", Content);
	Package.WriteEntry("Owner", Owner);
}

void FTGOR_DimensionInstance::Read(FTGOR_GroupReadPackage& Package, UTGOR_Singleton* Context)
{
	Package.ReadEntry("Path", Path);
	Package.ReadEntry("Content", Content);
	Package.ReadEntry("Owner", Owner);
}

FTGOR_DimensionRequestEntry::FTGOR_DimensionRequestEntry()
	: Timestamp(), Content(nullptr)
{
}

bool FTGOR_DimensionRequestEntry::NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess)
{
	bOutSuccess = true;

	uint8 Mask;
	SERIALISE_INIT_SOURCE
	SERIALISE_MASK_SOURCE(Content, Dimension);

	return(bOutSuccess);
}



FTGOR_DimensionRequestList::FTGOR_DimensionRequestList()
:	Owner("")
{
}


bool FTGOR_DimensionRequestList::NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess)
{
	bOutSuccess = true;

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
			FTGOR_DimensionRequestEntry& Entry = Dimensions.FindOrAdd(Key);
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


FTGOR_DimensionRequestState::FTGOR_DimensionRequestState()
{

}

bool FTGOR_DimensionRequestState::NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess)
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


FTGOR_ConnectionCollection::FTGOR_ConnectionCollection()
{
}


void FTGOR_ConnectionCollection::Send(FTGOR_NetworkWritePackage& Package, UTGOR_Context* Context) const
{
	Package.WriteEntry(Components);
}

void FTGOR_ConnectionCollection::Recv(FTGOR_NetworkReadPackage& Package, UTGOR_Context* Context)
{
	Package.ReadEntry(Components);
}




FTGOR_ConnectionSelection::FTGOR_ConnectionSelection()
	: Suffix(""), Connection("")
{

}
