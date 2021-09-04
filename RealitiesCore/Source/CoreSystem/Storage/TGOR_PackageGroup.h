// The Gateway of Realities: Planes of Existence.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"

#include "TGOR_PackageMinimal.h"

#include "TGOR_PackageGroup.generated.h"

////////////////////////////////////////////////////////////////////////////////////////////////////
// Group package stores name - value pairs

struct CORESYSTEM_API CTGOR_GroupPackageRegister : public CTGOR_PackageRegister
{
	uint32 Name;
	uint32 Type;
	uint32 Size;
};

template<>
struct CTGOR_PackageCache<CTGOR_GroupPackageRegister>
{
private:
	uint32 Version;
	FString Name;
	FString Type;
	TArray<uint8> Data;

public:
	void Write(const CTGOR_Database* Database, uint32 Start, const CTGOR_GroupPackageRegister& Register)
	{
		Version = Register.Version;
		Name = Database->GetIdentifier(Register.Name);
		Type = Database->GetIdentifier(Register.Type);
		Data.SetNum(Register.Size);
		Database->Read(Start, Data.GetData(), Register.Size);
	}

	void Read(CTGOR_Database* Database, uint32 Start, CTGOR_GroupPackageRegister& Register) const
	{
		Register.Version = Version;
		Register.Name = Database->SerializeIdentifier(Name);
		Register.Type = Database->SerializeIdentifier(Type);
		Register.Size = Data.Num();
		Database->Write(Start, Data.GetData(), Register.Size);
	}
};

using CTGOR_GroupPackageCache = TArray<CTGOR_PackageCache<CTGOR_GroupPackageRegister>>;


USTRUCT(BlueprintType)
struct CORESYSTEM_API FTGOR_GroupWritePackage
{
	GENERATED_USTRUCT_BODY()

	CTGOR_VersionWritePackage<CTGOR_GroupPackageRegister> Package;
	FTGOR_GroupWritePackage() : Package(nullptr, nullptr, 0) {}
	FTGOR_GroupWritePackage(CTGOR_Database* Database, UTGOR_Context* Context, uint32 Position) : Package(Database, Context, Position) {}

	template<typename S> void WriteEntry(const FString& Identifier, const S& In)
	{
		CTGOR_GroupPackageRegister NewRegister;
		NewRegister.Cache = true;
		NewRegister.Version = Package.Database->GetVersionNumber();
		NewRegister.Name = Package.Database->SerializeIdentifier(Identifier);
		NewRegister.Type = Package.Database->SerializeIdentifier(CTGOR_Pack<S>::DataType());

		uint32 Start = Package.GetCurrentEnd();
		CTGOR_Pack<S>::DataWrite(Package.Context, Package.Database, Start, NewRegister.Size, In);
		Package.Registers.Emplace(NewRegister);
	}
};


USTRUCT(BlueprintType)
struct CORESYSTEM_API FTGOR_GroupReadPackage
{
	GENERATED_USTRUCT_BODY()

	CTGOR_VersionReadPackage<CTGOR_GroupPackageRegister> Package;
	FTGOR_GroupReadPackage() : Package(nullptr, nullptr, 0) {}
	FTGOR_GroupReadPackage(const CTGOR_Database* Database, UTGOR_Context* Context, uint32 Position) : Package(Database, Context, Position) {}

	template<typename S> bool ReadEntry(const FString& Identifier, S& Out)
	{
		const int32 Name = Package.Database->GetSerial(Identifier);
		const int32 Type = Package.Database->GetSerial(CTGOR_Pack<S>::DataType());

		uint32 Start = Package.Position;
		for (CTGOR_GroupPackageRegister& Register : Package.Registers)
		{
			if (Register.Name == Name && Register.Type == Type)
			{
				if (CTGOR_Pack<S>::DataRead(Package.Context, Package.Database, Start, Register.Size, Out))
				{
					Register.Cache = false;
					return true;
				}
			}
			Start += Package.GetRegisterSize(Register);
		}
		return false;
	}
};

////////////////////////////////////////////////////////////////////////////////////////////////////

class CORESYSTEM_API CTGOR_InOutGroup
{
public:
	CTGOR_InOutGroup() {};
	virtual ~CTGOR_InOutGroup() {};

	static const char* TYPENAME; // Optional
	CTGOR_GroupPackageCache LegacyCache;
	virtual void Write(FTGOR_GroupWritePackage&, UTGOR_Context*) const = 0;
	virtual void Read(FTGOR_GroupReadPackage&, UTGOR_Context*) = 0;
};
