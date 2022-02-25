// The Gateway of Realities: Planes of Existence.

#pragma once

#include "CoreMinimal.h"
#include "Realities/Utility/Error/TGOR_Error.h"
#include "Realities/Utility/Storage/TGOR_PackageGroup.h"
#include "Realities/Utility/Storage/TGOR_PackageNetwork.h"
#include "Realities/Utility/Storage/TGOR_Serialisation.h"

#include "Engine/UserDefinedStruct.h"
#include "TGOR_Index.generated.h"


/**
* Index value between 0 and 0xFFFF
*/
USTRUCT(BlueprintType)
struct REALITIES_API FTGOR_Serial
{
	GENERATED_USTRUCT_BODY()
		SERIALISE_INIT_HEADER;

	FTGOR_Serial();
	FTGOR_Serial(int32 Index);
	bool operator==(int32 Other) const;
	bool operator!=(int32 Other) const;
	bool operator==(FTGOR_Serial Other) const;
	bool operator!=(FTGOR_Serial Other) const;

	void Write(FTGOR_GroupWritePackage& Package, UTGOR_Singleton* Context) const;
	void Read(FTGOR_GroupReadPackage& Package, UTGOR_Singleton* Context);
	void Send(FTGOR_NetworkWritePackage& Package, UTGOR_Singleton* Context) const;
	void Recv(FTGOR_NetworkReadPackage& Package, UTGOR_Singleton* Context);

	////////////////////////////////////////////////////////////////////////////////////////////////////

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int32 Index;


	uint32 GetTypeHash() const;
};

FORCEINLINE uint32 GetTypeHash(const FTGOR_Serial& Other)
{
	return Other.GetTypeHash();
}

/**
* Serial that allows INDEX_NONE as input
*/
USTRUCT(BlueprintType)
struct REALITIES_API FTGOR_Index : public FTGOR_Serial
{
	GENERATED_USTRUCT_BODY()
	SERIALISE_INIT_HEADER;

	FTGOR_Index();
	FTGOR_Index(int32 Index);
	FTGOR_Index(FTGOR_Serial Serial);
	bool operator==(FTGOR_Index Other) const;
	bool operator!=(FTGOR_Index Other) const;
	operator bool() const;

	void Send(FTGOR_NetworkWritePackage& Package, UTGOR_Singleton* Context) const;
	void Recv(FTGOR_NetworkReadPackage& Package, UTGOR_Singleton* Context);
};

FORCEINLINE uint32 GetTypeHash(const FTGOR_Index& Other)
{
	return Other.GetTypeHash();
}