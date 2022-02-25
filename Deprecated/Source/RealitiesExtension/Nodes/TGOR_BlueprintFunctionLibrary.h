// The Gateway of Realities: Planes of Existence.

#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"
#include "TGOR_BlueprintFunctionLibrary.generated.h"

class UTGOR_Content;

/**
 * 
 */
UCLASS()
class REALITIESEXTENSION_API UTGOR_BlueprintFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:

	/** Generates a content blueprint of given class in Mods/Core/Generated */
	UFUNCTION(BlueprintCallable, Category = "TGOR EditorUtilities", Meta = (Keywords = "C++"))
		static UClass* CreateAssetFromClass(TSubclassOf<UTGOR_Content> Class, bool Open, bool Autosave);

	/** Generates a content blueprint of given class in Mods/Core/Generated with given name */
	UFUNCTION(BlueprintCallable, Category = "TGOR EditorUtilities", Meta = (Keywords = "C++"))
		static UClass* CreateAssetFromClassNamed(TSubclassOf<UTGOR_Content> Class, const FString& RawName, bool Open, bool Autosave);


	/** Whether the currently loaded world has an associated dimension */
	UFUNCTION(BlueprintPure, Category = "TGOR EditorUtilities", Meta = (WorldContext = "WorldContextObject", Keywords = "C++"))
		static bool HasWorldDimension(UObject* WorldContextObject);

	/** Creates dimension for currently loaded word */
	UFUNCTION(BlueprintCallable, Category = "TGOR EditorUtilities", Meta = (WorldContext = "WorldContextObject", Keywords = "C++"))
		static void CreateWorldDimension(UObject* WorldContextObject, TSubclassOf<UTGOR_Dimension> DimensionClass);

	/** Assign world asset to current dimension */
	UFUNCTION(BlueprintCallable, Category = "TGOR EditorUtilities", Meta = (WorldContext = "WorldContextObject", Keywords = "C++"))
		static void AssignCurrentWorldToDimension(UObject* WorldContextObject);

	/** Assign world bounds to current dimension (Optionally takes a volume actor to consider) */
	UFUNCTION(BlueprintCallable, Category = "TGOR EditorUtilities", Meta = (WorldContext = "WorldContextObject", Keywords = "C++"))
		static void AssignCurrentBoundsToDimension(UObject* WorldContextObject, AActor* Volume = nullptr);

	/** Updates portal list for currently loaded world */
	UFUNCTION(BlueprintCallable, Category = "TGOR EditorUtilities", Meta = (WorldContext = "WorldContextObject", Keywords = "C++"))
		static void UpdatePortalList(UObject* WorldContextObject);

	/** Gets list of portal names from currently loaded world */
	UFUNCTION(BlueprintCallable, Category = "TGOR EditorUtilities", Meta = (WorldContext = "WorldContextObject", Keywords = "C++"))
		static TArray<FName> GetPortalListFromWorld(UObject* WorldContextObject);

	/** Gets portal location from name */
	UFUNCTION(BlueprintCallable, Category = "TGOR EditorUtilities", Meta = (WorldContext = "WorldContextObject", Keywords = "C++"))
		static UTGOR_DimensionPortalComponent* GetPortalFromWorld(const FName& PortalName, UObject* WorldContextObject);

	/** Gets current world name */
	UFUNCTION(BlueprintPure, Category = "TGOR EditorUtilities", Meta = (WorldContext = "WorldContextObject", Keywords = "C++"))
		static FString GetCurrentWorldName(UObject* WorldContextObject);


	/** Checks whether a class has a blueprint representation (otherwise it's a C++ class) */
	UFUNCTION(BlueprintPure, Category = "TGOR EditorUtilities", Meta = (Keywords = "C++"))
		static bool IsBlueprintContent(TSubclassOf<UTGOR_Content> Class);

	/** Gets content defaultname and display names */
	UFUNCTION(BlueprintPure, Category = "TGOR EditorUtilities", Meta = (Keywords = "C++"))
		static void GetContentAttributes(TSubclassOf<UTGOR_Content> Class, FString& DefaultName, FText& DisplayName, bool& IsAbstract);

	/** Sets content defaultname */
	UFUNCTION(BlueprintCallable, Category = "TGOR EditorUtilities", Meta = (Keywords = "C++"))
		static void SetContentDefaultName(TSubclassOf<UTGOR_Content> Class, const FString& DefaultName);

	/** Sets content display name */
	UFUNCTION(BlueprintCallable, Category = "TGOR EditorUtilities", Meta = (Keywords = "C++"))
		static void SetContentDisplayName(TSubclassOf<UTGOR_Content> Class, const FText& DisplayName);

	/** Sets whether content is abstract */
	UFUNCTION(BlueprintCallable, Category = "TGOR EditorUtilities", Meta = (Keywords = "C++"))
		static void SetContentAbstract(TSubclassOf<UTGOR_Content> Class, bool IsAbstract);

	/** Add insertion class to a given content class */
	UFUNCTION(BlueprintCallable, Category = "TGOR EditorUtilities", Meta = (Keywords = "C++"))
		static void AddContentInsertion(TSubclassOf<UTGOR_Content> Class, TSubclassOf<UTGOR_Content> Insertion);

	/** Remove insertion class from a given content class */
	UFUNCTION(BlueprintCallable, Category = "TGOR EditorUtilities", Meta = (Keywords = "C++"))
		static void RemoveContentInsertion(TSubclassOf<UTGOR_Content> Class, TSubclassOf<UTGOR_Content> Insertion);


	/** Find number of identity components */
	UFUNCTION(BlueprintCallable, Category = "TGOR EditorUtilities", Meta = (WorldContext = "WorldContextObject", Keywords = "C++"))
		static int32 CountIdentityComponents(UObject* WorldContextObject);

	/** Makes sure all identity components have a unique identifier. Returns how many identifiers were changed. */
	UFUNCTION(BlueprintCallable, Category = "TGOR EditorUtilities", Meta = (WorldContext = "WorldContextObject", Keywords = "C++"))
		static int32 EnsureUniqeIdentifiers(UObject* WorldContextObject);

	/** Check whether a identity component has a unique identifier */
	UFUNCTION(BlueprintPure, Category = "TGOR EditorUtilities", Meta = (WorldContext = "WorldContextObject", Keywords = "C++"))
		static bool HasUniqeIdentifier(UTGOR_IdentityComponent* Identity, UObject* WorldContextObject);

	/** Returns all actor that have a dimension or save interface but no identifier component. */
	UFUNCTION(BlueprintCallable, Category = "TGOR EditorUtilities", Meta = (WorldContext = "WorldContextObject", Keywords = "C++"))
		static TArray<AActor*> GetMissingIdentifierComponent(UObject* WorldContextObject);

};
