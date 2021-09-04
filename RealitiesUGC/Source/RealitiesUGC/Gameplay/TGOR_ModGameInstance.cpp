// Copyright Epic Games, Inc. All Rights Reserved.

#include "TGOR_ModGameInstance.h"

#include "TGOR_ModWorldSettings.h"
#include "../Mod/TGOR_Content.h"
#include "../Mod/TGOR_Mod.h"

void UTGOR_ModGameInstance::OnWorldChanged(UWorld* OldWorld, UWorld* NewWorld)
{
	// Instnatiate the registry and find mod packages
	if (IsValid(NewWorld))
	{
		ModRegistry = NewObject<UTGOR_ModRegistry>(NewWorld);
		ModRegistry->FindUGCPackages();

		ContentManager = NewObject<UTGOR_ContentManager>(NewWorld);
		ContentManager->AssignWorld(NewWorld);
		PreModLoad();

		// Extract which mod to load
		TSubclassOf<UTGOR_Mod> CoreModType = DefaultCoreModType;

		ATGOR_ModWorldSettings* Settings = Cast<ATGOR_ModWorldSettings>(NewWorld->GetWorldSettings());
		if (IsValid(Settings) && *Settings->CoreModType)
		{
			CoreModType = Settings->CoreModType;
		}

		// Load mods locally if no setup is specified
		if (ModSetup.ActiveMods.Num() == 0)
		{
			ModSetup = ContentManager->GetLocalModSetup(ModRegistry, CoreModType);
		}

		const ETGOR_ModloadEnumeration Result = ContentManager->LoadModSetup(ModRegistry, ModSetup);
		PostModLoad(Result);
	}
	Super::OnWorldChanged(OldWorld, NewWorld);
}

void UTGOR_ModGameInstance::PreModLoad()
{
}

void UTGOR_ModGameInstance::PostModLoad(ETGOR_ModloadEnumeration Result)
{
}
