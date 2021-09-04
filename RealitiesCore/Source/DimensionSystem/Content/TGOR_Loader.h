// The Gateway of Realities: Planes of Existence.

#pragma once

#include "CoreMinimal.h"

#include "CoreSystem/Content/TGOR_CoreContent.h"
#include "TGOR_Loader.generated.h"

////////////////////////////////////////////// DECL //////////////////////////////////////////////////////

class UTGOR_Dimension;


/**
*
*/
UCLASS()
class DIMENSIONSYSTEM_API UTGOR_Loader : public UTGOR_CoreContent
{
	GENERATED_BODY()

public:
	UTGOR_Loader();

	////////////////////////////////////////////////////////////////////////////////////////////////////

	/** Call function directly after loader finished (regions are not yet initialised) */
	UFUNCTION(BlueprintImplementableEvent, Category = "!TGOR Loader", Meta = (Keywords = "C++"))
		bool PostInit();

	/** Call function all actors and loader and regions have been initialised */
	UFUNCTION(BlueprintImplementableEvent, Category = "!TGOR Loader", Meta = (Keywords = "C++"))
		bool PostAssemble();

	////////////////////////////////////////////////////////////////////////////////////////////////////

	/** Check if loader can be called */
	UFUNCTION(BlueprintPure, Category = "!TGOR Loader", Meta = (Keywords = "C++"))
		virtual bool CanLoad(UWorld* World, const FTransform& Origin, UTGOR_Dimension* Dimension) const;

	/** Load this loader */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Loader", Meta = (Keywords = "C++"))
		virtual bool Load(UWorld* World, const FTransform& Origin, UTGOR_Dimension* Dimension);

	/** Check if loader is currently loaded */
	UFUNCTION(BlueprintPure, Category = "!TGOR Loader", Meta = (Keywords = "C++"))
		virtual bool IsLoaded(UWorld* World, const FTransform& Origin, UTGOR_Dimension* Dimension) const;

	/** Unload this loader */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Loader", Meta = (Keywords = "C++"))
		virtual bool Unload(UWorld* World, const FTransform& Origin, UTGOR_Dimension* Dimension);

	////////////////////////////////////////////////////////////////////////////////////////////////////

	// Whether this loader is optional
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "!TGOR Loader")
		bool Optional;


	/*
	// Spawns of this loader
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "!TGOR Loader")
		TArray<FTGOR_LoaderSpawn> Spawns;


	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	// Load all spawns using Content's DefaultMod (Doesn't work in editor), 
	// level/actor when Post false, removs when true
	void Load(UWorld* World, bool Post);

	// Loads spawn at index using specified mod for prefix
	void Load(const FString& ModName, UWorld* World, int32 Index);

	// Returns true if this loader is already loaded
	bool IsLoaded(const FString& ModName, UWorld* World, int32 Index);

	// Unloads spawn at index using specified mod for prefix
	void Unload(const FString& ModName, UWorld* World, int32 Index);

	// Changes location of spawn at index using specified mod for prefix
	void Move(const FString& ModName, UWorld* World, int32 Index, FTransform Transform);

	// Gets full transform of spawn at index using specified mod for prefix
	FTransform GetTransform(const FString& ModName, UWorld* World, int32 Index);

	ULevelStreaming* CreateLevelStreaming(UWorld* World, int32 Index);
	*/
};