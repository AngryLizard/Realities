// The Gateway of Realities: Planes of Existence.

#pragma once

#include "CoreMinimal.h"
#include "RealitiesUGC/Gameplay/TGOR_ModWorldSettings.h"
#include "TGOR_WorldSettings.generated.h"

class UTGOR_Mod;

/**
 * 
 */
UCLASS(Blueprintable)
class CORESYSTEM_API ATGOR_WorldSettings : public ATGOR_ModWorldSettings
{
	GENERATED_BODY()
	
public:
	ATGOR_WorldSettings();
	virtual void NotifyBeginPlay() override;

	////////////////////////////////////////////////////////////////////////////////////////////////////

	/* Overrides profile if not empty. This should be enabled if you want to test multiplayer in editor. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "!TGOR System", Meta = (Keywords = "C++"))
		FString ProfileOverride;

	/* Directory to save to */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "!TGOR System", Meta = (Keywords = "C++"))
		FString SaveDirectory;

};
