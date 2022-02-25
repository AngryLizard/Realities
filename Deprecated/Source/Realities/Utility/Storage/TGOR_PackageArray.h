// The Gateway of Realities: Planes of Existence.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include <functional>
#include <type_traits>
#include "Templates/SubclassOf.h"

#include "TGOR_PackageMinimal.h"
#include "Engine/UserDefinedStruct.h"

#include "TGOR_PackageArray.generated.h"

////////////////////////////////////////////////////////////////////////////////////////////////////
// Array package stores index - value pairs, allows for sparse implementation


struct CTGOR_ArrayPackageRegister : public CTGOR_PackageRegister
{
	int32 Index;
	uint32 Size;
};

template<>
struct CTGOR_PackageCache<CTGOR_ArrayPackageRegister>
{
private:
	uint32 Version;
	int32 Index;
	TArray<uint8> Data;

public:
	void Write(const CTGOR_Database* Database, uint32 Start, const CTGOR_ArrayPackageRegister& Register)
	{
		Version = Register.Version;
		Index = Register.Index;
		Data.SetNum(Register.Size);
		Database->Read(Start, Data.GetData(), Register.Size);
	}

	void Read(CTGOR_Database* Database, uint32 Start, CTGOR_ArrayPackageRegister& Register) const
	{
		Register.Version = Version;
		Register.Index = Index;
		Register.Size = Data.Num();
		Database->Write(Start, Data.GetData(), Register.Size);
	}
};

using CTGOR_ArrayPackageCache = TArray<CTGOR_PackageCache<CTGOR_ArrayPackageRegister>>;

USTRUCT(BlueprintType)
struct FTGOR_ArrayWritePackage
{
	GENERATED_USTRUCT_BODY()

	CTGOR_VersionWritePackage<CTGOR_ArrayPackageRegister> Package;
	FTGOR_ArrayWritePackage() : Package(nullptr, nullptr, 0) {}
	FTGOR_ArrayWritePackage(CTGOR_Database* Database, UTGOR_Context* Context, uint32 Position) : Package(Database, Context, Position) {}

	template<typename S> void WriteEntry(int32 Index, const S& In)
	{
		CTGOR_ArrayPackageRegister NewRegister;
		NewRegister.Cache = true;
		NewRegister.Version = Package.Database->GetVersionNumber();
		NewRegister.Index = Index;

		uint32 Start = Package.GetCurrentEnd();
		CTGOR_Pack<S>::DataWrite(Package.Context, Package.Database, Start, NewRegister.Size, In);
		Package.Registers.Emplace(NewRegister);
	}
};

USTRUCT(BlueprintType)
struct FTGOR_ArrayReadPackage
{
	GENERATED_USTRUCT_BODY()

	CTGOR_VersionReadPackage<CTGOR_ArrayPackageRegister> Package;
	FTGOR_ArrayReadPackage() : Package(nullptr, nullptr, 0) {}
	FTGOR_ArrayReadPackage(const CTGOR_Database* Database, UTGOR_Context* Context, uint32 Position) : Package(Database, Context, Position) {}

	template<typename S> bool ReadEntry(int32 Index, S& Out)
	{
		uint32 Start = Package.Position;
		for (CTGOR_ArrayPackageRegister& Register : Package.Registers)
		{
			if (Register.Index == Index)
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
