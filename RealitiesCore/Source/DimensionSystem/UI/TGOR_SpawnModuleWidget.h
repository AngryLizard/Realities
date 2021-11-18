// The Gateway of Realities: Planes of Existence.

#pragma once

#include "CoreMinimal.h"

#include "UISystem/UI/TGOR_Widget.h"
#include "TGOR_SpawnModuleWidget.generated.h"

class UTGOR_SpawnModule;

/**
 * 
 */
UCLASS()
class DIMENSIONSYSTEM_API UTGOR_SpawnModuleWidget : public UTGOR_Widget
{
	GENERATED_BODY()
	
public:
	UTGOR_SpawnModuleWidget(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Owner Object of this module (usually a component) */
	UPROPERTY(BlueprintReadOnly, Category = "!TGOR Game", Meta = (ExposeOnSpawn, Keywords = "C++"))
		TWeakObjectPtr<UObject> OwnerObject;

	/** Owner index of this module */
	UPROPERTY(BlueprintReadOnly, Category = "!TGOR Game", Meta = (ExposeOnSpawn, Keywords = "C++"))
		int32 OwnerIndex;

	/** Widget to be spawned for debugging */
	UPROPERTY(BlueprintReadOnly, Category = "!TGOR Game", Meta = (ExposeOnSpawn, Keywords = "C++"))
		UTGOR_SpawnModule* SpawnModule = nullptr;
};
