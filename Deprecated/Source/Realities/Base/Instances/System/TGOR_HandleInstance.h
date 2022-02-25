// The Gateway of Realities: Planes of Existence.

#pragma once

#include "CoreMinimal.h"
#include "Realities/Utility/TGOR_CustomEnumerations.h"
#include "Realities\Interfaces\TGOR_RegisterInterface.h"
#include "Templates/SubclassOf.h"

#include "Engine/UserDefinedStruct.h"
#include "TGOR_HandleInstance.generated.h"

class UTGOR_Mod;
class UTGOR_Content;


/**
* TGOR_ModSetup stores mods to load/send on/from the server
*/
USTRUCT(BlueprintType)
struct FTGOR_HandleInstance
{
	GENERATED_USTRUCT_BODY()
public:
	FTGOR_HandleInstance();

	/** Register to poll for activity */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TGOR System")
		TScriptInterface<ITGOR_RegisterInterface> Register;

	/** Key to poll register for activity */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TGOR System")
		UTGOR_Content* Key;
};
