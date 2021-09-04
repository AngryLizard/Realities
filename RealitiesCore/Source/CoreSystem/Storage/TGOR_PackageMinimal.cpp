// The Gateway of Realities: Planes of Existence.


#include "TGOR_PackageMinimal.h"
#include "GenericPlatform/GenericPlatformFile.h"
#include "Net/UnrealNetwork.h"

bool FTGOR_Version::operator==(const FTGOR_Version& Other) const
{
	return this->Identifiers == Other.Identifiers;
}

bool FTGOR_Version::operator!=(const FTGOR_Version& Other) const
{
	return this->Identifiers != Other.Identifiers;
}

void FTGOR_Version::Read(IFileHandle* FileHandle)
{
	uint32 HistorySize = 0;
	FileHandle->Read((uint8*)& HistorySize, sizeof(uint32));
	Identifiers.SetNum(HistorySize);
	for (FString& Identifier : Identifiers)
	{
		uint32 IdentifierLength = 0;
		FileHandle->Read((uint8*)& IdentifierLength, sizeof(uint32));

		Identifier = FString::ChrN(IdentifierLength, ' ');
		FileHandle->Read((uint8*)Identifier.GetCharArray().GetData(), IdentifierLength * sizeof(TCHAR));
	}
}

void FTGOR_Version::Write(IFileHandle* FileHandle) const
{
	const uint32 HistorySize = Identifiers.Num();
	FileHandle->Write((const uint8*)& HistorySize, sizeof(uint32));
	for (const FString Identifier : Identifiers)
	{
		const uint32 IdentifierLength = Identifier.Len();
		FileHandle->Write((const uint8*)& IdentifierLength, sizeof(uint32));

		FileHandle->Write((const uint8*)* Identifier, IdentifierLength * sizeof(TCHAR));
	}
}

void FTGOR_Version::AddIdentifier(const FString& Identifier)
{
	Identifiers.Emplace(Identifier);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

CTGOR_Archive::CTGOR_Archive(FArchive& Ar, UPackageMap* Map)
	: Data(&Ar), Mapping(Map)
{
}

void CTGOR_Archive::Write(const uint8* Src, uint32 Size)
{
	if (Data->IsSaving())
	{
		// TODO: This is kinda stupid, but what can ye do.
		Data->Serialize(const_cast<uint8*>(Src), Size);
	}
}

void CTGOR_Archive::Read(uint8* Dest, uint32 Size) const
{
	if (Data->IsLoading())
	{
		Data->Serialize(Dest, Size);
	}
}

void CTGOR_Archive::WriteBits(const uint8* Src, uint32 Count)
{
	if (Data->IsSaving())
	{
		// TODO: This is kinda stupid, but what can ye do.
		Data->SerializeBits(const_cast<uint8*>(Src), Count);
	}
}

void CTGOR_Archive::ReadBits(uint8* Dest, uint32 Count) const
{
	if (Data->IsLoading())
	{
		Data->SerializeBits(Dest, Count);
	}
}

void CTGOR_Archive::WriteVector(const FVector& Src)
{
	if (Data->IsSaving())
	{
		WritePackedVector<100, 30>(Src, *Data);
	}
}

void CTGOR_Archive::ReadVector(FVector& Dest) const
{
	if (Data->IsLoading())
	{
		ReadPackedVector<100, 30>(Dest, *Data);
	}
}

void CTGOR_Archive::WriteRotator(const FRotator& Src)
{
	if (Data->IsSaving())
	{
		FRotator Rotator = Src;
		Rotator.SerializeCompressedShort(*Data);
	}
}

void CTGOR_Archive::ReadRotator(FRotator& Dest) const
{
	if (Data->IsLoading())
	{
		Dest.SerializeCompressedShort(*Data);
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////

CTGOR_Database::CTGOR_Database()
:	DataSize(0)
{
}

uint32 CTGOR_Database::SerializeIdentifier(const FString& Identifier)
{
	uint32* Ptr = Mapping.Find(Identifier);
	if (Ptr)
	{
		return *Ptr;
	}
	const uint32 Serial = Identifiers.Emplace(Identifier);
	Mapping.Add(Identifier, Serial);
	return Serial;
}

uint32 CTGOR_Database::GetSerial(const FString& Identifier) const
{
	const uint32* Ptr = Mapping.Find(Identifier);
	if (Ptr)
	{
		return *Ptr;
	}
	return -1;
}

FString CTGOR_Database::GetIdentifier(uint32 Serial) const
{
	if (Identifiers.IsValidIndex(Serial))
	{
		return Identifiers[Serial];
	}
	return "Invalid";
}

uint32 CTGOR_Database::GetIdentifierCount() const
{
	return Identifiers.Num();
}

void CTGOR_Database::ResetIdentifiers(uint32 Reserve)
{
	Identifiers.Reset(Reserve);
	Mapping.Reset();
	Mapping.Reserve(Reserve);
}

uint32 CTGOR_Database::GetVersionNumber() const
{
	return History.Num();
}

void CTGOR_Database::Write(uint32 Position, const uint8* Src, uint32 Size)
{
	const int32 Page = Position / PAGESIZE;
	while (Page >= Data.Num())
	{
		Data.Emplace(CTGOR_Page());
	}

	// Get position within page
	const uint32 Offset = Position - Page * PAGESIZE;

	// Make sure not to overflow
	const uint32 Rest = FMath::Min((PAGESIZE - Offset), Size);
	memcpy(Data[Page].Data + Offset, Src, Rest);

	// Continue if not all data has been written yet
	if (Size > Rest)
	{
		Write(Position + Offset, Src + Rest, Size - Rest);
	}
}

void CTGOR_Database::Read(uint32 Position, uint8* Dest, uint32 Size) const
{
	const int32 Page = Position / PAGESIZE;
	if (Page >= Data.Num())
	{
		return;
	}

	// Get position within page
	const uint32 Offset = Position - Page * PAGESIZE;

	// Make sure not to overflow
	const uint32 Rest = FMath::Min((PAGESIZE - Offset), Size);
	memcpy(Dest, Data[Page].Data + Offset, Rest);

	// Continue if not all data has been written yet
	if (Size > Rest)
	{
		Read(Position + Offset, Dest + Rest, Size - Rest);
	}
}


void CTGOR_Database::ExpandHistory(const FTGOR_Version& Version)
{
	// Expand history if current version doesn't match last entry
	if (History.Num() == 0 || History.Last() != Version)
	{
		History.Emplace(Version);
	}
}

void CTGOR_Database::Read(IFileHandle* FileHandle)
{
	// Read version history from file
	uint32 HistorySize = 0;
	FileHandle->Read((uint8*)& HistorySize, sizeof(uint32));
	History.SetNum(HistorySize);
	for (FTGOR_Version& Version : History)
	{
		Version.Read(FileHandle);
	}

	// Read dictionary from file
	uint32 DictionarySize = 0;
	FileHandle->Read((uint8*)& DictionarySize, sizeof(uint32));
	ResetIdentifiers(DictionarySize);
	for (uint32 Serial = 0; Serial < DictionarySize; Serial++)
	{
		uint32 IdentifierLength = 0;
		FileHandle->Read((uint8*)& IdentifierLength, sizeof(uint32));

		FString Identifier = FString::ChrN(IdentifierLength, ' ');
		FileHandle->Read((uint8*)Identifier.GetCharArray().GetData(), IdentifierLength * sizeof(TCHAR));
		SerializeIdentifier(Identifier);
	}

	// Read buffer from file
	FileHandle->Read((uint8*)& DataSize, sizeof(uint32));
	Data.SetNum(FMath::DivideAndRoundUp<uint32>(DataSize, PAGESIZE));
	FileHandle->Read((uint8*)Data.GetData(), DataSize);
}

void CTGOR_Database::Write(IFileHandle* FileHandle) const
{
	// Write version history from file
	const uint32 HistorySize = History.Num();
	FileHandle->Write((const uint8*)& HistorySize, sizeof(uint32));
	for (const FTGOR_Version& Version : History)
	{
		Version.Write(FileHandle);
	}

	// Write dictionary to file
	const uint32 DictionarySize = GetIdentifierCount();
	FileHandle->Write((const uint8*)& DictionarySize, sizeof(uint32));
	for (uint32 Serial = 0; Serial < DictionarySize; Serial++)
	{
		const FString Identifier = GetIdentifier(Serial);
		const uint32 IdentifierLength = Identifier.Len();
		FileHandle->Write((const uint8*)& IdentifierLength, sizeof(uint32));
		FileHandle->Write((const uint8*)* Identifier, IdentifierLength * sizeof(TCHAR));
	}

	// Write buffer to file
	FileHandle->Write((const uint8*)& DataSize, sizeof(uint32));
	FileHandle->Write((const uint8*)Data.GetData(), DataSize);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
