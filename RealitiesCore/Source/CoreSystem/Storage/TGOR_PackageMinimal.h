// The Gateway of Realities: Planes of Existence.

#pragma once

#include "CoreMinimal.h"

#include "Runtime/Core/Public/Serialization/Archive.h"
#include "TGOR_PackageMinimal.generated.h"

#define PAGESIZE 4096

class UTGOR_Content;
class UTGOR_Singleton;

////////////////////////////////////////////////////////////////////////////////////////////////////

using UTGOR_Context = UTGOR_Singleton;

////////////////////////////////////////////////////////////////////////////////////////////////////

struct CORESYSTEM_API CTGOR_Page
{
	uint8 Data[PAGESIZE];
};

////////////////////////////////////////////////////////////////////////////////////////////////////

USTRUCT(BlueprintType)
struct CORESYSTEM_API FTGOR_Version
{
	GENERATED_USTRUCT_BODY()

	bool operator==(const FTGOR_Version& Other) const;
	bool operator!=(const FTGOR_Version& Other) const;
	void Read(IFileHandle* FileHandle);
	void Write(IFileHandle* FileHandle) const;

	void AddIdentifier(const FString& Identifier);

private:
	TArray<FString> Identifiers;
};

////////////////////////////////////////////////////////////////////////////////////////////////////

class CORESYSTEM_API CTGOR_Archive
{
	FArchive* Data;
	UPackageMap* Mapping;

public:
	CTGOR_Archive(FArchive& Ar, UPackageMap* Map);

	// Network I/O
	void Write(const uint8* Src, uint32 Size);
	void Read(uint8* Dest, uint32 Size) const;
	void WriteBits(const uint8* Src, uint32 Count);
	void ReadBits(uint8* Dest, uint32 Count) const;
	void WriteVector(const FVector& Src);
	void ReadVector(FVector& Dest) const;
	void WriteRotator(const FRotator& Src);
	void ReadRotator(FRotator& Dest) const;

	template<typename T>
	bool Map(UObject*& Object) const
	{
		if (Mapping)
		{
			return Mapping->SerializeObject(*Data, T::StaticClass(), Object);
		}
		return false;
	}

	template<typename T>
	bool Serialise(UTGOR_Context* Context, T& t)
	{
		if (Data->IsLoading())
		{
			return CTGOR_Arch<T>::DataRead(Context, this, t);
		}

		CTGOR_Arch<T>::DataWrite(Context, this, t);
		return true;
	}
};


////////////////////////////////////////////////////////////////////////////////////////////////////

class CORESYSTEM_API CTGOR_Database
{
private:
	TArray<FTGOR_Version> History;

	TMap<FString, uint32> Mapping;
	TArray<FString> Identifiers;

private:
	TArray<CTGOR_Page> Data;
	uint32 DataSize;

public:
	CTGOR_Database();

	// Database I/O
	void Write(uint32 Position, const uint8* Src, uint32 Size);
	void Read(uint32 Position, uint8* Dest, uint32 Size) const;

	// Database dictionary
	uint32 SerializeIdentifier(const FString& Identifier);
	uint32 GetSerial(const FString& Identifier) const;
	FString GetIdentifier(uint32 Serial) const;
	uint32 GetIdentifierCount() const;
	void ResetIdentifiers(uint32 Reserve);

	// Version control
	void ExpandHistory(const FTGOR_Version& Version);
	uint32 GetVersionNumber() const;

	void Read(IFileHandle* FileHandle);
	void Write(IFileHandle* FileHandle) const;

	template<typename T>
	void InitialWrite(UTGOR_Context* Context, const T& In)
	{
		ResetIdentifiers(0);
		CTGOR_Pack<T>::DataWrite(Context, this, 0, DataSize, In);
	}

	template<typename T>
	bool InitialRead(UTGOR_Context* Context, T& Out)
	{
		return CTGOR_Pack<T>::DataRead(Context, this, 0, DataSize, Out);
	}
};

////////////////////////////////////////////////////////////////////////////////////////////////////

class CTGOR_Package;

template<typename T, typename = void>
class CTGOR_Pack
{
public:
	static CORESYSTEM_API FString DataType();
	static void DataWrite(UTGOR_Context* Context, CTGOR_Database* Database, uint32 Position, uint32& Size, const T& In);
	static bool DataRead(UTGOR_Context* Context, const CTGOR_Database* Database, uint32 Position, uint32 Size, T& Out);
};

template<typename T, typename = void>
class CTGOR_Arch
{
public:
	static void DataWrite(UTGOR_Context* Context, CTGOR_Archive* Database, const T& In);
	static bool DataRead(UTGOR_Context* Context, const CTGOR_Archive* Database, T& Out);
};

////////////////////////////////////////////////////////////////////////////////////////////////////

template<typename T>
struct CTGOR_PackageCache
{
	void Write(const CTGOR_Database* Database, uint32 Start, const T& Register);
	void Read(CTGOR_Database* Database, uint32 Start, T& Register) const;
};

struct CORESYSTEM_API CTGOR_PackageRegister
{
	uint32 Version;
	bool Cache;
};

struct CORESYSTEM_API CTGOR_PackageBase
{
	UTGOR_Context* Context;

	CTGOR_PackageBase(UTGOR_Context* Context) : Context(Context) {}

	UTGOR_Context* GetContext() { return Context; } // Get current context
};

template<typename T>
struct CTGOR_RegisterList
{
	TArray<T> Registers;
	uint32 Position;

	CTGOR_RegisterList(uint32 Position) : Position(Position) {}

	int32 GetRegisterCount() const { return Registers.Num(); }; // Get number of registers
	CORESYSTEM_API uint32 GetRegisterSize(const T& Register) const; // Get size of register content in bytes

	uint32 GetCurrentEnd() const
	{
		uint32 Start = Position;
		for (const T& Register : Registers)
		{
			Start += GetRegisterSize(Register);
		}
		return Start;
	}
};

struct CORESYSTEM_API CTGOR_NetworkWritePackage : public CTGOR_PackageBase
{
	CTGOR_Archive* Database;
	CTGOR_NetworkWritePackage(CTGOR_Archive* Database, UTGOR_Context* Context) : CTGOR_PackageBase(Context), Database(Database) {}
};

template<typename T>
struct CTGOR_VersionWritePackage : public CTGOR_PackageBase, public CTGOR_RegisterList<T>
{
	CTGOR_Database* Database;
	CTGOR_VersionWritePackage(CTGOR_Database* Database, UTGOR_Context* Context, uint32 Position) : CTGOR_PackageBase(Context), CTGOR_RegisterList<T>(Position), Database(Database) {}

	void WriteRegister(uint32& Size)
	{
		const uint32 End = GetCurrentEnd();
		
		// Store whole register array
		const uint32 RegisterCount = Registers.Num();
		const uint32 RegisterSize = RegisterCount * sizeof(T);
		Database->Write(End, (const uint8*)Registers.GetData(), RegisterSize);
		Database->Write(End + RegisterSize, (const uint8*)&RegisterCount, sizeof(uint32));
		Size = (End - Position) + RegisterSize + sizeof(uint32);
	}

	void ReadCached(const TArray<CTGOR_PackageCache<T>>& Cache)
	{
		uint32 Start = GetCurrentEnd();
		for (const CTGOR_PackageCache<T>& Entry : Cache)
		{
			T NewRegister;
			Entry.Read(Database, Start, NewRegister);
			Registers.Emplace(NewRegister);
			Start += GetRegisterSize(NewRegister);
		}
	}
};

struct CORESYSTEM_API CTGOR_NetworkReadPackage : public CTGOR_PackageBase
{
	const CTGOR_Archive* Database;
	CTGOR_NetworkReadPackage(const CTGOR_Archive* Database, UTGOR_Context* Context) : CTGOR_PackageBase(Context), Database(Database) {}
};


template<typename T>
struct CTGOR_VersionReadPackage : public CTGOR_PackageBase, public CTGOR_RegisterList<T>
{
	const CTGOR_Database* Database;
	CTGOR_VersionReadPackage(const CTGOR_Database* Database, UTGOR_Context* Context, uint32 Position) : CTGOR_PackageBase(Context), CTGOR_RegisterList<T>(Position), Database(Database) {}

	void ReadRegister(uint32 Size)
	{
		// Get total register count and initialise registers
		uint32 RegisterCount = 0;
		Database->Read(Position + Size - sizeof(uint32), (uint8*)& RegisterCount, sizeof(uint32));
		check(Size < 0xFFFF && "Anomaly detected while reading: Size too big.");
		Registers.SetNum(RegisterCount);

		// Load whole register array
		const uint32 RegisterSize = RegisterCount * sizeof(T);
		const uint32 RegisterPosition = Position + Size - sizeof(uint32) - RegisterSize;
		Database->Read(RegisterPosition, (uint8*)Registers.GetData(), RegisterSize);
	}

	void WriteCached(TArray<CTGOR_PackageCache<T>>& Cache) const
	{
		Cache.Reset();
		uint32 Start = Position;
		for (const T& Register : Registers)
		{
			if (Register.Cache)
			{
				Cache.Emplace(CTGOR_PackageCache<T>());
				Cache.Last().Write(Database, Start, Register);
			}
			Start += GetRegisterSize(Register);
		}
	}
};


////////////////////////////////////////////////////////////////////////////////////////////////////
