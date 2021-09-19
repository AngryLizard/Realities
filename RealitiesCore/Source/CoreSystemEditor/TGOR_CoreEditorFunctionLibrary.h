// The Gateway of Realities: Planes of Existence.

#pragma once

#include "CoreMinimal.h"

#include "Kismet/BlueprintFunctionLibrary.h"
#include "TGOR_CoreEditorFunctionLibrary.generated.h"

class UTGOR_Content;

DECLARE_DYNAMIC_DELEGATE_OneParam(FActorSelectedDelegate, const TArray<AActor*>&, Actors);

/**
 * 
 */
UCLASS()
class CORESYSTEMEDITOR_API UTGOR_CoreEditorFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:

	/** Provides callback on actor selection changes in editor */
	UFUNCTION(BlueprintCallable, Category = "!TGOR EditorUtilities", Meta = (Keywords = "C++"))
		static void BindOnActorSelectionChangedEvent(FActorSelectedDelegate OnActorSelected);

	/** Generates a content blueprint of given class in Mods/Core/Generated */
	UFUNCTION(BlueprintCallable, Category = "!TGOR EditorUtilities", Meta = (Keywords = "C++"))
		static UClass* CreateAssetFromClass(TSubclassOf<UTGOR_Content> Class, bool Open, bool Autosave);

	/** Generates a content blueprint of given class in Mods/Core/Generated with given name */
	UFUNCTION(BlueprintCallable, Category = "!TGOR EditorUtilities", Meta = (Keywords = "C++"))
		static UClass* CreateAssetFromClassNamed(TSubclassOf<UTGOR_Content> Class, const FString& RawName, bool Open, bool Autosave);

	////////////////////////////////////////////////////////////////////////////////////////////////////

	/** Checks whether a class has a blueprint representation (otherwise it's a C++ class) */
	UFUNCTION(BlueprintPure, Category = "!TGOR EditorUtilities", Meta = (Keywords = "C++"))
		static bool IsBlueprintContent(TSubclassOf<UTGOR_Content> Class);

	/** Gets content defaultname and display names */
	UFUNCTION(BlueprintPure, Category = "!TGOR EditorUtilities", Meta = (Keywords = "C++"))
		static void GetContentAttributes(TSubclassOf<UTGOR_Content> Class, FString& DefaultName, FText& DisplayName, bool& IsAbstract);

	/** Sets content defaultname */
	UFUNCTION(BlueprintCallable, Category = "!TGOR EditorUtilities", Meta = (Keywords = "C++"))
		static void SetContentDefaultName(TSubclassOf<UTGOR_Content> Class, const FString& DefaultName);

	/** Sets content display name */
	UFUNCTION(BlueprintCallable, Category = "!TGOR EditorUtilities", Meta = (Keywords = "C++"))
		static void SetContentDisplayName(TSubclassOf<UTGOR_Content> Class, const FText& DisplayName);

	/** Sets whether content is abstract */
	UFUNCTION(BlueprintCallable, Category = "!TGOR EditorUtilities", Meta = (Keywords = "C++"))
		static void SetContentAbstract(TSubclassOf<UTGOR_Content> Class, bool IsAbstract);

};
