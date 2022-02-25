// The Gateway of Realities: Planes of Existence.


#include "TGOR_KnowledgeInstance.h"

#include "Realities/Base/TGOR_Singleton.h"
#include "Realities/Base/TGOR_GameInstance.h"
#include "Realities/Base/Content/TGOR_Content.h"
#include "Realities/Base/Content/TGOR_ContentManager.h"
#include "Realities/Utility/Storage/TGOR_Package.h"

#include "Realities/Mod/Trackers/TGOR_Tracker.h"
#include "Realities/Mod/Knowledges/TGOR_Knowledge.h"

FTGOR_KnowledgeCollection::FTGOR_KnowledgeCollection()
{
}

void FTGOR_KnowledgeCollection::Write(FTGOR_GroupWritePackage& Package, UTGOR_Singleton* Context) const
{
	Package.WriteEntry("Data", Data.Array());
}

void FTGOR_KnowledgeCollection::Read(FTGOR_GroupReadPackage& Package, UTGOR_Singleton* Context)
{
	TArray<UTGOR_Knowledge*> Knowledges;
	Package.ReadEntry("Data", Knowledges);
	Data = TSet<UTGOR_Knowledge*>(Knowledges);
}


bool FTGOR_KnowledgeCollection::NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess)
{
	uint8 Mask;
	SERIALISE_INIT_SOURCE;

	// Send data size
	int32 Num = Data.Num();
	Ar << Num;

	if (Ar.IsLoading())
	{
		// Repopulate set
		Data.Reset();
		for (int32 Index = 0; Index < Num; Index++)
		{
			UTGOR_Knowledge* Knowledge = nullptr;
			SERIALISE_MASK_SOURCE(Knowledge, Knowledge);
			Data.Add(Knowledge);
		}
	}
	else
	{
		// Store each entry
		for (UTGOR_Knowledge* Knowledge : Data)
		{
			SERIALISE_MASK_SOURCE(Knowledge, Knowledge);
		}
	}

	return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

FTGOR_KnowledgePin::FTGOR_KnowledgePin()
	: Knowledge(nullptr)
{
}

void FTGOR_KnowledgePin::Write(FTGOR_GroupWritePackage& Package, UTGOR_Singleton* Context) const
{
	Package.WriteEntry("Progress", Progress);
	Package.WriteEntry("Knowledge", Knowledge);
}

void FTGOR_KnowledgePin::Read(FTGOR_GroupReadPackage& Package, UTGOR_Singleton* Context)
{
	Package.ReadEntry("Progress", Progress);
	Package.ReadEntry("Knowledge", Knowledge);
}



bool FTGOR_KnowledgePin::NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess)
{
	uint8 Mask;
	SERIALISE_INIT_SOURCE;
	SERIALISE_MASK_SOURCE(Knowledge, Knowledge);

	Ar << Progress;
	return bOutSuccess = true;
}

FTGOR_KnowledgePinBoard::FTGOR_KnowledgePinBoard()
	: Super()
{
}

////////////////////////////////////////////////////////////////////////////////////////////////////


FTGOR_KnowledgePinRequest::FTGOR_KnowledgePinRequest()
	: Knowledge(nullptr)
{
}

bool FTGOR_KnowledgePinRequest::NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess)
{
	uint8 Mask;
	SERIALISE_INIT_SOURCE;
	SERIALISE_MASK_SOURCE(Knowledge, Knowledge);

	return bOutSuccess = true;
}