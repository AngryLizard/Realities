// The Gateway of Realities: Planes of Existence.

#pragma once

#include "Realities/Utility/TGOR_CustomEnumerations.h"
#include "Realities/Utility/Storage/TGOR_Buffer.h"
#include "Realities/Utility/Storage/TGOR_PackageGroup.h"
#include "Realities/Base/Instances/Dimension/TGOR_DimensionDataInstance.h"
#include "Realities/Base/Instances/Creature/TGOR_TrackerInstance.h"

#include "UObject/NoExportTypes.h"
#include "TGOR_UserInstance.generated.h"

class ATGOR_OnlineController;
class UTGOR_Creature;
class ATGOR_Avatar;

/**
* TGOR_UserProperties stores visible user information
*/
USTRUCT(BlueprintType)
struct REALITIES_API FTGOR_UserProperties
{
	void Write(FTGOR_GroupWritePackage& Package, UTGOR_Singleton* Context) const;
	void Read(FTGOR_GroupReadPackage& Package, UTGOR_Singleton* Context);

	GENERATED_BODY()
		FTGOR_UserProperties();

	UPROPERTY(BlueprintReadWrite)
		FString Name;

	UPROPERTY(BlueprintReadWrite)
		FLinearColor Colour;

	UPROPERTY(BlueprintReadWrite)
		int32 Serial;
};

/**
* TGOR_UserInfo stores visible user information for online users
*/
USTRUCT(BlueprintType)
struct REALITIES_API FTGOR_UserInfo
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

/**
* TGOR_UserBodyDisplay Provides a replicateable way of refering to userbodies
*/
USTRUCT(BlueprintType)
struct REALITIES_API FTGOR_UserBodyDisplay
{
	GENERATED_USTRUCT_BODY()
public:
	FTGOR_UserBodyDisplay();
	
	////////////////////////////////////////////////////////////////////////////////////////////////////

	/** Creature content */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UTGOR_Creature* Creature;

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
struct REALITIES_API FTGOR_UserBody
{
	GENERATED_USTRUCT_BODY()
public:
	FTGOR_UserBody();

	void Write(FTGOR_GroupWritePackage& Package, UTGOR_Singleton* Context) const;
	void Read(FTGOR_GroupReadPackage& Package, UTGOR_Singleton* Context);

	////////////////////////////////////////////////////////////////////////////////////////////////////

	/** This body's identifiable properties */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UTGOR_Creature* Creature;

	/** This body's identity (NOT REPLICATED) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FTGOR_DimensionIdentifier Identifier;

	/** This body's pawn if loaded (NOT STORED) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TSoftObjectPtr<ATGOR_Avatar> Avatar;
};

/**
* TGOR_UserInstance stores user information like crypto keys
*/
USTRUCT(BlueprintType)
struct REALITIES_API FTGOR_UserInstance
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
