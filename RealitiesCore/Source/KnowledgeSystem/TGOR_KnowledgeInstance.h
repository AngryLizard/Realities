// The Gateway of Realities: Planes of Existence.

#pragma once

#include "CoreSystem/Storage/TGOR_PackageGroup.h"

#include "Engine/UserDefinedStruct.h"
#include "TGOR_KnowledgeInstance.generated.h"

class UTGOR_Knowledge;

/**
* TGOR_KnowledgeCollection is a set of knowledges that can be replicated and stored
*/
USTRUCT(BlueprintType)
struct KNOWLEDGESYSTEM_API FTGOR_KnowledgeCollection
{
	GENERATED_USTRUCT_BODY()

		FTGOR_KnowledgeCollection();

	CTGOR_GroupPackageCache LegacyCache;
	void Write(FTGOR_GroupWritePackage& Package, UTGOR_Singleton* Context) const;
	void Read(FTGOR_GroupReadPackage& Package, UTGOR_Singleton* Context);

	////////////////////////////////////////////////////////////////////////////////////////////////////

	TSet<TObjectPtr<UTGOR_Knowledge>> Data;

	////////////////////////////////////////////////////////////////////////////////////////////////////

	bool NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess);
};

template<>
struct TStructOpsTypeTraits<FTGOR_KnowledgeCollection> : public TStructOpsTypeTraitsBase2<FTGOR_KnowledgeCollection>
{
	enum
	{
		WithNetSerializer = true
	};
};


/**
* TGOR_KnowledgeCollection is a set of knowledges that can be replicated and stored
*/
USTRUCT(BlueprintType)
struct KNOWLEDGESYSTEM_API FTGOR_KnowledgePin : public FFastArraySerializerItem
{
	GENERATED_USTRUCT_BODY()

		FTGOR_KnowledgePin();

	CTGOR_GroupPackageCache LegacyCache;
	void Write(FTGOR_GroupWritePackage& Package, UTGOR_Singleton* Context) const;
	void Read(FTGOR_GroupReadPackage& Package, UTGOR_Singleton* Context);

	////////////////////////////////////////////////////////////////////////////////////////////////////

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "!TGOR Knowledge")
		TArray<float> Progress;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "!TGOR Knowledge")
		UTGOR_Knowledge* Knowledge;

	////////////////////////////////////////////////////////////////////////////////////////////////////

	bool NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess);
};

template<>
struct TStructOpsTypeTraits<FTGOR_KnowledgePin> : public TStructOpsTypeTraitsBase2<FTGOR_KnowledgePin>
{
	enum
	{
		WithNetSerializer = true
	};
};




USTRUCT(BlueprintType)
struct KNOWLEDGESYSTEM_API FTGOR_KnowledgePinBoard : public FFastArraySerializer
{
	GENERATED_USTRUCT_BODY()
		FTGOR_KnowledgePinBoard();

	UPROPERTY(BlueprintReadOnly)
		TArray<FTGOR_KnowledgePin> Pins;

	////////////////////////////////////////////////////////////////////////////////////////////////////

	bool NetDeltaSerialize(FNetDeltaSerializeInfo& DeltaParms)
	{
		return FFastArraySerializer::FastArrayDeltaSerialize<FTGOR_KnowledgePin, FTGOR_KnowledgePinBoard>(Pins, DeltaParms, *this);
	}
};

template<>
struct TStructOpsTypeTraits<FTGOR_KnowledgePinBoard> : public TStructOpsTypeTraitsBase2<FTGOR_KnowledgePinBoard>
{
	enum
	{
		WithNetDeltaSerializer = true
	};
};


/**
* TGOR_KnowledgePinRequest is used to request a pin to be added
*/
USTRUCT(BlueprintType)
struct KNOWLEDGESYSTEM_API FTGOR_KnowledgePinRequest
{
	GENERATED_USTRUCT_BODY()
		FTGOR_KnowledgePinRequest();
	
	////////////////////////////////////////////////////////////////////////////////////////////////////

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "!TGOR Knowledge")
		UTGOR_Knowledge* Knowledge;

	////////////////////////////////////////////////////////////////////////////////////////////////////

	bool NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess);
};

template<>
struct TStructOpsTypeTraits<FTGOR_KnowledgePinRequest> : public TStructOpsTypeTraitsBase2<FTGOR_KnowledgePinRequest>
{
	enum
	{
		WithNetSerializer = true
	};
};