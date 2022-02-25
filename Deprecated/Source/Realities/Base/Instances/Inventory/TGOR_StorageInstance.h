#pragma once

#include "CoreMinimal.h"
#include "Realities/Utility/Storage/TGOR_PackageGroup.h"
#include "Realities/Utility/Storage/TGOR_PackageNetwork.h"

#include "Realities/Utility/Storage/TGOR_Serialisation.h"
#include "TGOR_StorageInstance.generated.h"

class UTGOR_Module;

UENUM(BlueprintType)
enum class ETGOR_NetvarEnumeration : uint8
{
	/** Not replicated */
	Local,
	/** Only server replicates */
	Server,
	/** Everyone can replicate */
	Client
};

USTRUCT(BlueprintType)
struct FTGOR_StorageDeclaration
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TMap<FName, TSubclassOf<UTGOR_Module>> Modules;
};
