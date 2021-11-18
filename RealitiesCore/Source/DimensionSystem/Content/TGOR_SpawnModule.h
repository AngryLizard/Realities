// The Gateway of Realities: Planes of Existence.

#pragma once

#include "CoreMinimal.h"
#include "RealitiesUGC/Mod/TGOR_Insertions.h"

#include "CoreSystem/Content/TGOR_CoreContent.h"
#include "TGOR_SpawnModule.generated.h"

class UTGOR_SpawnModuleWidget;

UCLASS(Blueprintable)
class DIMENSIONSYSTEM_API UTGOR_SpawnModule : public UTGOR_CoreContent
{
	GENERATED_BODY()

public:
	UTGOR_SpawnModule();


	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Widget to be spawned for debugging */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "!TGOR Game")
		TSubclassOf<UTGOR_SpawnModuleWidget> DebugWidget;

};

