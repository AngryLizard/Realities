// The Gateway of Realities: Planes of Existence.

#pragma once

#include "CoreMinimal.h"
#include "RealitiesUtility/Structures/TGOR_Time.h"

#include "CoreSystem/Storage/TGOR_Serialisation.h"
#include "CoreSystem/Storage/TGOR_PackageGroup.h"
#include "CoreSystem/Storage/TGOR_PackageNetwork.h"

#include "Engine/UserDefinedStruct.h"
#include "TGOR_DimensionInstance.generated.h"

class UTGOR_Spawner;
class UTGOR_Dimension;
class UTGOR_DimensionData;
class UTGOR_IdentityComponent;
class UTGOR_ConnectionComponent;

UENUM(BlueprintType)
enum class ETGOR_DimensionLoadingStage : uint8
{
	Cleanup,
	UnloadingClients,
	Unloaded,
	Spawned,
	Loaded,
	CosmeticHidden,
	CosmeticShown
};

/**
* TGOR_DimensionIdentity stores identifiable information about an identity
*/
USTRUCT(BlueprintType)
struct DIMENSIONSYSTEM_API FTGOR_DimensionIdentifier
{
	GENERATED_USTRUCT_BODY()
public:
	FTGOR_DimensionIdentifier();

	void Write(FTGOR_GroupWritePackage& Package, UTGOR_Singleton* Context) const;
	void Read(FTGOR_GroupReadPackage& Package, UTGOR_Singleton* Context);

	////////////////////////////////////////////////////////////////////////////////////////////////////

	/** This body's identity */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FName DimensionIdentifier;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int32 ActorIdentifier;
};

/**
* TGOR_ComponentIdentifier stores identifiable information about a component belonging to an identity
*/
USTRUCT(BlueprintType)
struct DIMENSIONSYSTEM_API FTGOR_ComponentIdentifier : public FTGOR_DimensionIdentifier
{
	GENERATED_USTRUCT_BODY()
public:
	FTGOR_ComponentIdentifier();

	void Write(FTGOR_GroupWritePackage& Package, UTGOR_Singleton* Context) const;
	void Read(FTGOR_GroupReadPackage& Package, UTGOR_Singleton* Context);

	////////////////////////////////////////////////////////////////////////////////////////////////////

	/** Component on given actor if available */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FString ComponentIdentifier;
};

/**
* TGOR_DimensionInstance stores dimension and player information
*/
USTRUCT(BlueprintType)
struct DIMENSIONSYSTEM_API FTGOR_DimensionInstance
{
	GENERATED_USTRUCT_BODY()
	FTGOR_DimensionInstance();

	void Write(FTGOR_GroupWritePackage& Package, UTGOR_Singleton* Context) const;
	void Read(FTGOR_GroupReadPackage& Package, UTGOR_Singleton* Context);

	////////////////////////////////////////////////////////////////////////////////////////////////////

	/* Whether dimension is open to be entered (Usually false during unloading phase) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool Open;

	/* File path */
	UPROPERTY(EditDefaultsOnly)
		FString Path;

	/* Type of this dimension (Dimension index) */
	UPROPERTY(EditDefaultsOnly)
		UTGOR_Dimension* Content;

	/* Loaded base */
	UPROPERTY(EditDefaultsOnly)
		UTGOR_DimensionData* Dimension;

	/* Dimension location */
	UPROPERTY(EditDefaultsOnly)
		FVector Location;

	/* Dimension owner */
	UPROPERTY(EditDefaultsOnly)
		FTGOR_DimensionIdentifier Owner;

	/* Dimension loading stage */
	UPROPERTY(EditDefaultsOnly)
		ETGOR_DimensionLoadingStage Stage;
};

/**
*
*/
USTRUCT(BlueprintType)
struct DIMENSIONSYSTEM_API FTGOR_DimensionRequestEntry
{
	GENERATED_USTRUCT_BODY()
public:
	FTGOR_DimensionRequestEntry();

	/* Last timestamp update */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FTGOR_Time Timestamp;

	/* Dimension content */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UTGOR_Dimension* Content;

	bool NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess);
};

template<>
struct TStructOpsTypeTraits<FTGOR_DimensionRequestEntry> : public TStructOpsTypeTraitsBase2<FTGOR_DimensionRequestEntry>
{
	enum
	{
		WithNetSerializer = true
	};
};

/**
*
*/
USTRUCT(BlueprintType)
struct DIMENSIONSYSTEM_API FTGOR_DimensionRequestList
{
	GENERATED_USTRUCT_BODY()
public:
	FTGOR_DimensionRequestList();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FName Owner;

	TMap<FName, FTGOR_DimensionRequestEntry> Dimensions;

	bool NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess);
};

template<>
struct TStructOpsTypeTraits<FTGOR_DimensionRequestList> : public TStructOpsTypeTraitsBase2<FTGOR_DimensionRequestList>
{
	enum
	{
		WithNetSerializer = true
	};
};



/**
*
*/
USTRUCT(BlueprintType)
struct DIMENSIONSYSTEM_API FTGOR_DimensionRequestState
{
	GENERATED_USTRUCT_BODY()
public:
	FTGOR_DimensionRequestState();
	
	TMap<FName, FVector> Dimensions;

	bool NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess);
};

template<>
struct TStructOpsTypeTraits<FTGOR_DimensionRequestState> : public TStructOpsTypeTraitsBase2<FTGOR_DimensionRequestState>
{
	enum
	{
		WithNetSerializer = true
	};
};

/**
*
*/
USTRUCT(BlueprintType)
struct DIMENSIONSYSTEM_API FTGOR_ConnectionCollection
{
	GENERATED_USTRUCT_BODY()
		SERIALISE_INIT_HEADER;
public:
	FTGOR_ConnectionCollection();

	void Send(FTGOR_NetworkWritePackage& Package, UTGOR_Context* Context) const;
	void Recv(FTGOR_NetworkReadPackage& Package, UTGOR_Context* Context);

	TMap<FName, TWeakObjectPtr<UTGOR_ConnectionComponent>> Components;

};

template<>
struct TStructOpsTypeTraits<FTGOR_ConnectionCollection> : public TStructOpsTypeTraitsBase2<FTGOR_ConnectionCollection>
{
	enum
	{
		WithNetSerializer = true
	};
};


/**
* TGOR_ConnectionSelection allows selecting a connection and dimension content
*/
USTRUCT(BlueprintType)
struct DIMENSIONSYSTEM_API FTGOR_ConnectionSelection
{
	GENERATED_USTRUCT_BODY()
		FTGOR_ConnectionSelection();

	////////////////////////////////////////////////////////////////////////////////////////////////////

	/* Dimension */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TSubclassOf<UTGOR_Dimension> Dimension;

	/* Dimension name suffix for a unique name */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FString Suffix;

	/* Portal name */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FName Connection;
};