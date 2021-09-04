// The Gateway of Realities: Planes of Existence.

#pragma once

#include "CoreMinimal.h"

#include "Engine/UserDefinedStruct.h"
#include "TGOR_Index.generated.h"


/**
* Index value between 0 and 0xFFFF
*/
USTRUCT(BlueprintType)
struct REALITIESUTILITY_API FTGOR_Serial
{
	GENERATED_USTRUCT_BODY()

	FTGOR_Serial();
	FTGOR_Serial(int32 Index);
	bool operator==(int32 Other) const;
	bool operator!=(int32 Other) const;
	bool operator==(FTGOR_Serial Other) const;
	bool operator!=(FTGOR_Serial Other) const;

	////////////////////////////////////////////////////////////////////////////////////////////////////

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int32 Index;

	uint32 GetTypeHash() const;
	bool NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess);
};

template<> struct TStructOpsTypeTraits<FTGOR_Serial> : public TStructOpsTypeTraitsBase2<FTGOR_Serial>
{
	enum { WithNetSerializer = true };
};

FORCEINLINE uint32 GetTypeHash(const FTGOR_Serial& Other)
{
	return Other.GetTypeHash();
}

/**
* Serial that allows INDEX_NONE as input
*/
USTRUCT(BlueprintType)
struct REALITIESUTILITY_API FTGOR_Index : public FTGOR_Serial
{
	GENERATED_USTRUCT_BODY()

	FTGOR_Index();
	FTGOR_Index(int32 Index);
	FTGOR_Index(FTGOR_Serial Serial);
	bool operator==(FTGOR_Index Other) const;
	bool operator!=(FTGOR_Index Other) const;
	operator bool() const;

	bool NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess);
};

template<> struct TStructOpsTypeTraits<FTGOR_Index> : public TStructOpsTypeTraitsBase2<FTGOR_Index>
{
	enum { WithNetSerializer = true };
};

FORCEINLINE uint32 GetTypeHash(const FTGOR_Index& Other)
{
	return Other.GetTypeHash();
}