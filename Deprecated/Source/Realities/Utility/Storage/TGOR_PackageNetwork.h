// The Gateway of Realities: Planes of Existence.

#pragma once

#include "CoreMinimal.h"

#include "TGOR_PackageGroup.h"
#include "Engine/UserDefinedStruct.h"
#include "TGOR_PackageNetwork.generated.h"

////////////////////////////////////////////////////////////////////////////////////////////////////
// Network package stores compressed data

USTRUCT(BlueprintType)
struct FTGOR_NetworkWritePackage
{
	GENERATED_USTRUCT_BODY()

	CTGOR_NetworkWritePackage Package;
	FTGOR_NetworkWritePackage() : Package(nullptr, nullptr) {}
	FTGOR_NetworkWritePackage(CTGOR_Archive* Database, UTGOR_Context* Context) : Package(Database, Context) {}

	template<typename S> void WriteEntry(const S& In)
	{
		CTGOR_Arch<S>::DataWrite(Package.Context, Package.Database, In);
	}
};

USTRUCT(BlueprintType)
struct FTGOR_NetworkReadPackage
{
	GENERATED_USTRUCT_BODY()

	CTGOR_NetworkReadPackage Package;
	FTGOR_NetworkReadPackage() : Package(nullptr, nullptr) {}
	FTGOR_NetworkReadPackage(const CTGOR_Archive* Database, UTGOR_Context* Context) : Package(Database, Context) {}

	template<typename S> bool ReadEntry(S& Out)
	{
		return  CTGOR_Arch<S>::DataRead(Package.Context, Package.Database, Out);
	}
};

////////////////////////////////////////////////////////////////////////////////////////////////////

class CTGOR_InOutNetwork
{
public:
	CTGOR_InOutNetwork() {};
	virtual ~CTGOR_InOutNetwork() {};

	virtual void Send(FTGOR_NetworkWritePackage&, UTGOR_Context*) const = 0;
	virtual void Recv(FTGOR_NetworkReadPackage&, UTGOR_Context*) = 0;
};
