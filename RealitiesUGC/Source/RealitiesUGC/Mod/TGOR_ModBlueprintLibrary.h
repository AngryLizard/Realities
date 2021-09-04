// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"
#include "TGOR_ModRegistry.h"
#include "TGOR_ModBlueprintLibrary.generated.h"


UCLASS() 
class REALITIESUGC_API UTGOR_ModBlueprintLibrary : public UBlueprintFunctionLibrary
{

	GENERATED_BODY()
public:

	// Gets the UGC Registry found in the GameInstance
	UFUNCTION(BlueprintPure, Category = "RealitiesUGC", meta = (WorldContext = "WorldContextObject"))
		static UTGOR_ModRegistry* GetModRegistry(UObject* WorldContextObject);
};
