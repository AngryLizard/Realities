// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"

#include "../Mod/TGOR_ModInstance.h"
#include "../Mod/TGOR_ContentManager.h"
#include "../Mod/TGOR_ModRegistry.h"
#include "TGOR_ModGameInstance.generated.h"

class UTGOR_Content;

/**
 * 
 */
UCLASS(BlueprintType)
class REALITIESUGC_API UTGOR_ModGameInstance : public UGameInstance
{
	GENERATED_BODY()

		friend class UTGOR_ModBlueprintLibrary;

public:
	virtual void OnWorldChanged(UWorld* OldWorld, UWorld* NewWorld) override;

	////////////////////////////////////////////////////////////////////////////////////////////////////
protected:

	/** Called before new mods are loaded */
	UFUNCTION(BlueprintCallable, Category = "RealitiesUGC", Meta = (Keywords = "C++"))
		virtual void PreModLoad();

	/** Called after new mods were loaded */
	UFUNCTION(BlueprintCallable, Category = "RealitiesUGC", Meta = (Keywords = "C++"))
		virtual void PostModLoad(ETGOR_ModloadEnumeration Result);


	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Type of the local core mod to be loaded if there are no compatible world settings */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "RealitiesUGC")
		TSubclassOf<UTGOR_Mod> DefaultCoreModType;

	////////////////////////////////////////////////////////////////////////////////////////////////////
protected:

	/** Mods to be loaded when a world is loaded */
	UPROPERTY(BlueprintReadOnly, Category = "RealitiesUGC")
		FTGOR_ModInstance ModSetup;

	/** The Registry that holds information about UGC and assigned class overrides */
	UPROPERTY(Transient, BlueprintReadOnly, Category = "RealitiesUGC")
		UTGOR_ModRegistry* ModRegistry;

	/** The content manager holding all content classes */
	UPROPERTY(Transient, BlueprintReadOnly, Category = "RealitiesUGC")
		UTGOR_ContentManager* ContentManager;

};
