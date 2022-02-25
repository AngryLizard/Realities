// The Gateway of Realities: Planes of Existence.

#pragma once

#include "CoreMinimal.h"
#include "Realities/Utility/TGOR_CustomEnumerations.h"
#include "Templates/SubclassOf.h"

#include "Engine/UserDefinedStruct.h"
#include "TGOR_ModInstance.generated.h"

class UTGOR_Mod;
class UTGOR_Content;

/**
* TGOR_ContentLink 
*/
USTRUCT(BlueprintType)
struct FTGOR_ContentLink
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		ETGOR_ModAction Action;

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
		ETGOR_ModAction Action;

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
		FString Name;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FString Version;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FString Class;
};

/**
* TGOR_ModSetup stores mods to load/send on/from the server
*/
USTRUCT(BlueprintType)
struct FTGOR_ModInstance
{
	GENERATED_USTRUCT_BODY()
public:
	FTGOR_ModInstance();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TSubclassOf<UTGOR_Mod> BaseMod;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		FString BaseVersion;

	UPROPERTY(BlueprintReadOnly)
		TArray<FTGOR_ModEntry> ActiveMods;

	bool IsEqual(FTGOR_ModInstance& ModSetup);
};