// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "RealitiesUGC/Mod/TGOR_ModInstance.h"

#include "GameFramework/WorldSettings.h"
#include "TGOR_ModWorldSettings.generated.h"


/**
 *
 */
UCLASS(Blueprintable)
class REALITIESUGC_API ATGOR_ModWorldSettings : public AWorldSettings
{
	GENERATED_BODY()

public:
	ATGOR_ModWorldSettings();

	////////////////////////////////////////////////////////////////////////////////////////////////////

	/* Core mod to load for this world */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "!TGOR System", Meta = (Keywords = "C++"))
		TSubclassOf<UTGOR_CoreMod> CoreModType;

};
