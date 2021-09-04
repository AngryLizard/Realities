// The Gateway of Realities: Planes of Existence.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"

#include "TGOR_PackageMinimal.h"

#include "TGOR_PackageMap.generated.h"

////////////////////////////////////////////////////////////////////////////////////////////////////
// Map package stores key - value pairs

struct CORESYSTEM_API CTGOR_MapPackageRegister : public CTGOR_PackageRegister
{
	int32 Index;
	uint32 KeySize;
	uint32 ValueSize;
};


template<>
struct CTGOR_PackageCache<CTGOR_MapPackageRegister>
{
private:
	uint32 Version;
	int32 Index;
	TArray<uint8> KeyData;
	TArray<uint8> ValueData;

public:
	void Write(const CTGOR_Database* Database, uint32 Start, const CTGOR_MapPackageRegister& Register)
	{
		Version = Register.Version;
		Index = Register.Index;
		KeyData.SetNum(Register.KeySize);
		Database->Read(Start, KeyData.GetData(), Register.KeySize);
		ValueData.SetNum(Register.ValueSize);
		Database->Read(Start, ValueData.GetData(), Register.ValueSize);
	}

	void Read(CTGOR_Database* Database, uint32 Start, CTGOR_MapPackageRegister& Register)
	{
		Register.Version = Version;
		Register.Index = Index;
		Register.KeySize = KeyData.Num();
		Database->Write(Start, KeyData.GetData(), Register.KeySize);
		Register.ValueSize = ValueData.Num();
		Database->Write(Start, ValueData.GetData(), Register.ValueSize);
	}
};

using CTGOR_MapPackageCache = TArray<CTGOR_PackageCache<CTGOR_MapPackageRegister>>;

USTRUCT(BlueprintType)
struct CORESYSTEM_API FTGOR_MapWritePackage
{
	GENERATED_USTRUCT_BODY()

	CTGOR_VersionWritePackage<CTGOR_MapPackageRegister> Package;
	FTGOR_MapWritePackage() : Package(nullptr, nullptr, 0) {}
	FTGOR_MapWritePackage(CTGOR_Database* Database, UTGOR_Context* Context, uint32 Position) : Package(Database, Context, Position) {}

	template<typename R, typename S> void WriteEntry(int32 Index, const R& KeyIn, const S& ValueIn)
	{
		CTGOR_MapPackageRegister NewRegister;
		NewRegister.Cache = true;
		NewRegister.Version = Package.Database->GetVersionNumber();
		NewRegister.Index = Index;

		uint32 Start = Package.GetCurrentEnd();
		CTGOR_Pack<R>::DataWrite(Package.Context, Package.Database, Start, NewRegister.KeySize, KeyIn);
		CTGOR_Pack<S>::DataWrite(Package.Context, Package.Database, Start + NewRegister.KeySize, NewRegister.ValueSize, ValueIn);
		Package.Registers.Emplace(NewRegister);
	}
};

USTRUCT(BlueprintType)
struct CORESYSTEM_API FTGOR_MapReadPackage
{
	GENERATED_USTRUCT_BODY()

	CTGOR_VersionReadPackage<CTGOR_MapPackageRegister> Package;
	FTGOR_MapReadPackage() : Package(nullptr, nullptr, 0) {}
	FTGOR_MapReadPackage(const CTGOR_Database* Database, UTGOR_Context* Context, uint32 Position) : Package(Database, Context, Position) {}

	template<typename R> bool ReadKey(int32 Index, R& KeyOut)
	{
		uint32 Start = Package.Position;
		for (CTGOR_MapPackageRegister& Register : Package.Registers)
		{
			if (Register.Index == Index)
			{
				if (CTGOR_Pack<R>::DataRead(Package.Context, Package.Database, Start, Register.KeySize, KeyOut))
				{
					return true;
				}
			}
			Start += Package.GetRegisterSize(Register);
		}
		return false;
	}

	template<typename S> bool ReadValue(int32 Index, S& ValueOut)
	{
		uint32 Start = Package.Position;
		for (CTGOR_MapPackageRegister& Register : Package.Registers)
		{
			if (Register.Index == Index)
			{
				if (CTGOR_Pack<S>::DataRead(Package.Context, Package.Database, Start + Register.KeySize, Register.ValueSize, ValueOut))
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
