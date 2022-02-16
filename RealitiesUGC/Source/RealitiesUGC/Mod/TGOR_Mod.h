// The Gateway of Realities: Planes of Existence.

#pragma once

#include "RealitiesUGC/Mod/TGOR_ContentManager.h"
#include "TGOR_ModInstance.h"

#include "Runtime/GameplayTags/Classes/GameplayTagContainer.h"
#include "RealitiesUtility/Utility/TGOR_WorldObject.h"
#include "TGOR_Mod.generated.h"


class UTGOR_ModSetup;

UCLASS(Blueprintable)
class REALITIESUGC_API UTGOR_Mod : public UTGOR_WorldObject
{
	GENERATED_BODY()

	friend class UTGOR_ContentManager;

public:
	UTGOR_Mod(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	////////////////////////////////////////////////////////////////////////////////////////////////////

	/** Mod name and version */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "!TGOR Content")
		FTGOR_ModEntry Entry;

	/** Mod dependencies */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "!TGOR Content")
		TArray<TSubclassOf<UTGOR_Mod>> Dependencies;

	/** Gameplaytags this mod adds to the database */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "!TGOR Content")
		FGameplayTagContainer FilterTags;

	/** Whether this mod should be ignored by local mod setup */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "!TGOR Content")
		bool Ignore;

	/** Mod plugin path */
	UPROPERTY()
		FName PackagePath;

	////////////////////////////////////////////////////////////////////////////////////////////////////

	/** Setups used by this mod (added in order) */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "!TGOR Content")
		TArray<TSubclassOf<UTGOR_ModSetup>> Setups;

	////////////////////////////////////////////////////////////////////////////////////////////////////
protected:

	/** Custom content links */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "!TGOR Content")
		TArray<FTGOR_ContentLink> ContentLinks;

	/** Custom content insertions */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "!TGOR Content")
		TArray<FTGOR_ContentInsertion> ContentInsertions;

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Get mod name */
	UFUNCTION(BlueprintPure, Category = "!TGOR Content", Meta = (Keywords = "C++"))
		FString GetModIdentifier() const;

	/** Check whether this mod is dependent on a given mod */
	UFUNCTION(BlueprintPure, Category = "!TGOR Content", Meta = (Keywords = "C++"))
		bool HasDependency(const UTGOR_Mod* Mod) const;

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Return content classes to be added/removed/replaced conditionally based on the content classes loaded so far. */
	UFUNCTION(BlueprintImplementableEvent, Category = "!TGOR Content", Meta = (Keywords = "C++"))
		TArray<FTGOR_ContentLink> PreModLoad(UTGOR_Content* ContentRoot);

	/** Apply changes to content classes loaded so far. This happens after new content has been added/removed/replaced but before insertion lists are created. */
	UFUNCTION(BlueprintImplementableEvent, Category = "!TGOR Content", Meta = (Keywords = "C++"))
		void PostModLoad(UTGOR_Content* ContentRoot);

	/** Apply changes to content classes after all mods have been loaded and insertion lists of mods are created */
	UFUNCTION(BlueprintImplementableEvent, Category = "!TGOR Content", Meta = (Keywords = "C++"))
		void PostAllModLoad(UTGOR_Content* ContentRoot);
};


UCLASS(Blueprintable)
class REALITIESUGC_API UTGOR_CoreMod : public UTGOR_Mod
{
	GENERATED_BODY()

};