#pragma once

#include "CoreMinimal.h"

#include "CoreSystem/Storage/TGOR_Serialisation.h"

#include "TGOR_StorageInstance.generated.h"

class UTGOR_Module;

USTRUCT(BlueprintType)
struct FTGOR_StorageDeclaration
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TMap<FName, TSubclassOf<UTGOR_Module>> Modules;
};
