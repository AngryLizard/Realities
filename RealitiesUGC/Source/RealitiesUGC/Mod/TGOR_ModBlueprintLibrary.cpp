// Copyright Epic Games, Inc. All Rights Reserved.

#include "TGOR_ModBlueprintLibrary.h"
#include "Kismet/GameplayStatics.h"

#include "../Gameplay/TGOR_ModGameInstance.h"
#include "TGOR_ModRegistry.h"

UTGOR_ModRegistry* UTGOR_ModBlueprintLibrary::GetModRegistry(UObject* WorldContextObject)
{
	UTGOR_ModGameInstance* GameInstance = Cast<UTGOR_ModGameInstance>(UGameplayStatics::GetGameInstance(WorldContextObject));
	return (GameInstance) ? GameInstance->ModRegistry : nullptr;
}
