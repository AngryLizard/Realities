// The Gateway of Realities: Planes of Existence.

#pragma once

#include "CoreMinimal.h"
#include "Realities/Utility/Storage/TGOR_Serialisation.h"
#include "Realities/Utility/Datastructures/TGOR_Time.h"
#include "Realities/Utility/Storage/TGOR_PackageGroup.h"

#include "Engine/UserDefinedStruct.h"
#include "TGOR_DimensionDataInstance.generated.h"

class UTGOR_Spawner;
class UTGOR_Dimension;
class UTGOR_DimensionData;
class UTGOR_IdentityComponent;

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
struct REALITIES_API FTGOR_DimensionIdentifier
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
struct REALITIES_API FTGOR_ComponentIdentifier : public FTGOR_DimensionIdentifier
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
struct REALITIES_API FTGOR_DimensionDataInstance
{
	GENERATED_USTRUCT_BODY()
	FTGOR_DimensionDataInstance();

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
struct REALITIES_API FTGOR_DimensionUpdateEntry
{
	GENERATED_USTRUCT_BODY()
public:
	FTGOR_DimensionUpdateEntry();

	/* Last timestamp update */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FTGOR_Time Timestamp;

	/* Dimension content */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UTGOR_Dimension* Content;

	bool NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess);
};

template<>
struct TStructOpsTypeTraits<FTGOR_DimensionUpdateEntry> : public TStructOpsTypeTraitsBase2<FTGOR_DimensionUpdateEntry>
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
struct REALITIES_API FTGOR_DimensionUpdateList
{
	GENERATED_USTRUCT_BODY()
public:
	FTGOR_DimensionUpdateList();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool IsServerApproved;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FName Owner;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TMap<FName, FTGOR_DimensionUpdateEntry> Dimensions;

	bool NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess);
};

template<>
struct TStructOpsTypeTraits<FTGOR_DimensionUpdateList> : public TStructOpsTypeTraitsBase2<FTGOR_DimensionUpdateList>
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
struct REALITIES_API FTGOR_DimensionUpdateState
{
	GENERATED_USTRUCT_BODY()
public:
	FTGOR_DimensionUpdateState();
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TMap<FName, FVector> Dimensions;

	bool NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess);
};

template<>
struct TStructOpsTypeTraits<FTGOR_DimensionUpdateState> : public TStructOpsTypeTraitsBase2<FTGOR_DimensionUpdateState>
{
	enum
	{
		WithNetSerializer = true
	};
};

/**
* FTGOR_DimensionSelection allows selecting a portal and dimension content
*/
USTRUCT(BlueprintType)
struct REALITIES_API FTGOR_PortalSelection
{
	GENERATED_USTRUCT_BODY()
		FTGOR_PortalSelection();

	////////////////////////////////////////////////////////////////////////////////////////////////////

	/* Dimension */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TSubclassOf<UTGOR_Dimension> Dimension;

	/* Dimension name suffix for a unique name */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FString Suffix;

	/* Portal name */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FName Portal;
};