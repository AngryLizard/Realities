// The Gateway of Realities: Planes of Existence.

#pragma once

#include "CoreMinimal.h"
#include "Templates/SubclassOf.h"

#include "TGOR_ModInstance.generated.h"


class UTGOR_Mod;
class UTGOR_Content;

UENUM(BlueprintType)
enum class ETGOR_ModloadEnumeration : uint8
{
	Success,
	NotFound,
	WrongMod,
	WrongVersion
};

UENUM(BlueprintType)
enum class ETGOR_ModActionEnumeration : uint8
{
	Add,
	Remove
};

UENUM(BlueprintType)
enum class ETGOR_SetupStateEnumeration : uint8
{
	Pending,
	Loading,
	Finished
};

/**
* TGOR_ContentLink 
*/
USTRUCT(BlueprintType)
struct FTGOR_ContentLink
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		ETGOR_ModActionEnumeration Action = ETGOR_ModActionEnumeration::Add;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TSubclassOf<UTGOR_Content> Content;
};

/**
* TGOR_ContentInsertion
*/
USTRUCT(BlueprintType)
struct FTGOR_ContentInsertion
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		ETGOR_ModActionEnumeration Action = ETGOR_ModActionEnumeration::Add;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TSubclassOf<UTGOR_Content> Content;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TSubclassOf<UTGOR_Content> Target;
};

/**
* TGOR_ModEntry stores one mod to load/send on/from the server
*/
USTRUCT(BlueprintType)
struct FTGOR_ModEntry
{
	GENERATED_USTRUCT_BODY()
public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FString Name = "";

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int32 Version = -1;
};

/**
* TGOR_ModSetup stores mods to load/send on/from the server
*/
USTRUCT(BlueprintType)
struct FTGOR_ModInstance
{
	GENERATED_USTRUCT_BODY()
public:

	UPROPERTY(BlueprintReadOnly)
		TArray<FTGOR_ModEntry> ActiveMods;

	bool IsEqual(FTGOR_ModInstance& ModSetup);
};


/**
 *
 */
USTRUCT(BlueprintType)
struct REALITIESUGC_API FTGOR_ModSetupState
{
	GENERATED_USTRUCT_BODY()
		FTGOR_ModSetupState();
public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "!TGOR System")
		FText Status;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "!TGOR System")
		ETGOR_SetupStateEnumeration State = ETGOR_SetupStateEnumeration::Pending;
};
