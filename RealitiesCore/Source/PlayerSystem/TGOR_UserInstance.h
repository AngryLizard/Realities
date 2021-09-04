// The Gateway of Realities: Planes of Existence.

#pragma once

#include "CoreSystem/Storage/TGOR_Buffer.h"
#include "CoreSystem/Storage/TGOR_PackageGroup.h"

#include "DimensionSystem/TGOR_DimensionInstance.h"
#include "KnowledgeSystem/TGOR_TrackerInstance.h"

#include "UObject/NoExportTypes.h"
#include "TGOR_UserInstance.generated.h"

class ATGOR_OnlineController;
class UTGOR_Spawner;

/**
* TGOR_UserProperties stores visible user information
*/
USTRUCT(BlueprintType)
struct PLAYERSYSTEM_API FTGOR_UserProperties
{
	void Write(FTGOR_GroupWritePackage& Package, UTGOR_Singleton* Context) const;
	void Read(FTGOR_GroupReadPackage& Package, UTGOR_Singleton* Context);

	GENERATED_BODY()
		FTGOR_UserProperties();

	UPROPERTY(BlueprintReadWrite)
		FString Name;

	UPROPERTY(BlueprintReadWrite)
		FLinearColor Color;

	UPROPERTY(BlueprintReadWrite)
		int32 Serial;
};

/**
* TGOR_UserInfo stores visible user information for online users
*/
USTRUCT(BlueprintType)
struct PLAYERSYSTEM_API FTGOR_UserInfo
{
	GENERATED_BODY()
		FTGOR_UserInfo();

	UPROPERTY(BlueprintReadWrite)
		FTGOR_UserProperties Properties;

	UPROPERTY(BlueprintReadWrite)
		float Ping;

	UPROPERTY(BlueprintReadWrite)
		FVector Location;
};

UENUM(BlueprintType)
enum class ETGOR_Privileges : uint8
{
	Invalid,
	User,
	Operator
};


/**
* TGOR_UserBodyDisplay Provides a replicateable way of refering to userbodies
*/
USTRUCT(BlueprintType)
struct PLAYERSYSTEM_API FTGOR_UserBodyDisplay
{
	GENERATED_USTRUCT_BODY()
public:
	FTGOR_UserBodyDisplay();
	
	////////////////////////////////////////////////////////////////////////////////////////////////////

	/** Creature content */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UTGOR_Spawner* Spawner;

	/** Dimension content */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UTGOR_Dimension* Dimension;

	/** Body identifier */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int32 Identifier;

	bool NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess);
};

template<>
struct TStructOpsTypeTraits<FTGOR_UserBodyDisplay> : public TStructOpsTypeTraitsBase2<FTGOR_UserBodyDisplay>
{
	enum
	{
		WithNetSerializer = true
	};
};

/**
* TGOR_UserBody stores identifiable information about an identity that belongs to a user
*/
USTRUCT(BlueprintType)
struct PLAYERSYSTEM_API FTGOR_UserBody
{
	GENERATED_USTRUCT_BODY()
public:
	FTGOR_UserBody();

	void Write(FTGOR_GroupWritePackage& Package, UTGOR_Singleton* Context) const;
	void Read(FTGOR_GroupReadPackage& Package, UTGOR_Singleton* Context);

	////////////////////////////////////////////////////////////////////////////////////////////////////

	/** This body's identifiable properties */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UTGOR_Spawner* Spawner;
};

/**
* TGOR_UserInstance stores user information like crypto keys
*/
USTRUCT(BlueprintType)
struct PLAYERSYSTEM_API FTGOR_UserInstance
{
	GENERATED_BODY()
		FTGOR_UserInstance();

	void Write(FTGOR_GroupWritePackage& Package, UTGOR_Singleton* Context) const;
	void Read(FTGOR_GroupReadPackage& Package, UTGOR_Singleton* Context);

	////////////////////////////////////////////////////////////////////////////////////////////////////

	UPROPERTY(BlueprintReadWrite)
		FTGOR_UserProperties Properties;

	// Dimensions owned by this player
	UPROPERTY(BlueprintReadWrite)
		TMap<UTGOR_Dimension*, FName> Dimensions;

	UPROPERTY(BlueprintReadWrite)
		TMap<int32, FTGOR_UserBody> Bodies;

	UPROPERTY(BlueprintReadWrite)
		int32 CurrentBody;
		
	UPROPERTY(BlueprintReadWrite)
		FTGOR_Buffer PrivateKey;

	UPROPERTY(BlueprintReadWrite)
		FTGOR_Buffer PublicKey;

	// Temporary password to check private key with
	UPROPERTY(BlueprintReadWrite)
		FTGOR_Buffer Password;

	// Privileges this user has
	UPROPERTY(BlueprintReadWrite)
		ETGOR_Privileges Privileges;

	/** Tracker history of this user */
	UPROPERTY(BlueprintReadWrite)
		FTGOR_TrackerData Trackers;

	/** This user's controller if online (NOT STORED) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TSoftObjectPtr<ATGOR_OnlineController> Controller;
};
