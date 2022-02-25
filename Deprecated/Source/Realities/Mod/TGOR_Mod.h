// The Gateway of Realities: Planes of Existence.

#pragma once

#include "UObject/Object.h"
#include "Realities/Utility/Error/TGOR_Error.h"
#include "Realities/Base/Content/TGOR_ContentManager.h"

#include "Realities/Utility/TGOR_CustomEnumerations.h"
#include "Runtime/GameplayTags/Classes/GameplayTagContainer.h"

#include "Realities/Base/TGOR_Object.h"
#include "TGOR_Mod.generated.h"

UCLASS(Blueprintable)
class REALITIES_API UTGOR_Mod : public UTGOR_Object
{
	GENERATED_BODY()

	friend class UTGOR_ContentManager;

public:
	UTGOR_Mod(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	////////////////////////////////////////////////////////////////////////////////////////////////////

	/** Mod name */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TGOR Content")
		FString Name;

	/** Mod folder name */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TGOR Content")
		FString Path;

	/** Mod version */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TGOR Content")
		FString Version;

	/** Gameplaytags this mod adds to the database */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TGOR Content")
		FGameplayTagContainer FilterTags;

	////////////////////////////////////////////////////////////////////////////////////////////////////
protected:

	/** Custom content links */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TGOR Content")
		TArray<FTGOR_ContentLink> ContentLinks;

	/** Custom content insertions */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TGOR Content")
		TArray<FTGOR_ContentInsertion> ContentInsertions;

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Return content classes to be added/removed/replaced conditionally based on the content classes loaded so far. */
	UFUNCTION(BlueprintImplementableEvent, Category = "TGOR Content", Meta = (Keywords = "C++"))
		TArray<FTGOR_ContentLink> PreModLoad(UTGOR_Content* ContentRoot);

	/** Apply changes to content classes loaded so far. This happens after new content has been added/removed/replaced but before insertion lists are created. */
	UFUNCTION(BlueprintImplementableEvent, Category = "TGOR Content", Meta = (Keywords = "C++"))
		void PostModLoad(UTGOR_Content* ContentRoot);

	/** Apply changes to content classes after all mods have been loaded and insertion lists of mods are created */
	UFUNCTION(BlueprintImplementableEvent, Category = "TGOR Content", Meta = (Keywords = "C++"))
		void PostAllModLoad(UTGOR_Content* ContentRoot);

};