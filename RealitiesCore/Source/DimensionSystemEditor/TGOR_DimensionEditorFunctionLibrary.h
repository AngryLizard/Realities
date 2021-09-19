// The Gateway of Realities: Planes of Existence.

#pragma once

#include "CoreMinimal.h"

#include "Kismet/BlueprintFunctionLibrary.h"
#include "TGOR_DimensionEditorFunctionLibrary.generated.h"

/**
 * 
 */
UCLASS()
class DIMENSIONSYSTEMEDITOR_API UTGOR_DimensionEditorFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:

	/** Whether the currently loaded world has an associated dimension */
	UFUNCTION(BlueprintPure, Category = "!TGOR EditorUtilities", Meta = (WorldContext = "WorldContextObject", Keywords = "C++"))
		static bool HasWorldDimension(UObject* WorldContextObject);

	/** Creates dimension for currently loaded word */
	UFUNCTION(BlueprintCallable, Category = "!TGOR EditorUtilities", Meta = (WorldContext = "WorldContextObject", Keywords = "C++"))
		static void CreateWorldDimension(UObject* WorldContextObject, TSubclassOf<UTGOR_Dimension> DimensionClass);

	/** Assign world asset to current dimension */
	UFUNCTION(BlueprintCallable, Category = "!TGOR EditorUtilities", Meta = (WorldContext = "WorldContextObject", Keywords = "C++"))
		static void AssignCurrentWorldToDimension(UObject* WorldContextObject);

	/** Assign world bounds to current dimension (Optionally takes a volume actor to consider) */
	UFUNCTION(BlueprintCallable, Category = "!TGOR EditorUtilities", Meta = (WorldContext = "WorldContextObject", Keywords = "C++"))
		static void AssignCurrentBoundsToDimension(UObject* WorldContextObject, AActor* Volume = nullptr);

	/** Updates portal list for currently loaded world */
	UFUNCTION(BlueprintCallable, Category = "!TGOR EditorUtilities", Meta = (WorldContext = "WorldContextObject", Keywords = "C++"))
		static void UpdateConnectionList(UObject* WorldContextObject);

	/** Gets list of portal names from currently loaded world */
	UFUNCTION(BlueprintCallable, Category = "!TGOR EditorUtilities", Meta = (WorldContext = "WorldContextObject", Keywords = "C++"))
		static TArray<FName> GetConnectionListFromWorld(UObject* WorldContextObject);

	/** Gets portal location from name */
	UFUNCTION(BlueprintCallable, Category = "!TGOR EditorUtilities", Meta = (WorldContext = "WorldContextObject", Keywords = "C++"))
		static UTGOR_ConnectionComponent* GetConnectionFromWorld(const FName& ConnectionName, UObject* WorldContextObject);

	/** Gets current world name */
	UFUNCTION(BlueprintPure, Category = "!TGOR EditorUtilities", Meta = (WorldContext = "WorldContextObject", Keywords = "C++"))
		static FString GetCurrentWorldName(UObject* WorldContextObject);

	////////////////////////////////////////////////////////////////////////////////////////////////////

	/** Find number of identity components */
	UFUNCTION(BlueprintCallable, Category = "!TGOR EditorUtilities", Meta = (WorldContext = "WorldContextObject", Keywords = "C++"))
		static int32 CountIdentityComponents(UObject* WorldContextObject);

	/** Makes sure all identity components have a unique identifier. Returns how many identifiers were changed. */
	UFUNCTION(BlueprintCallable, Category = "!TGOR EditorUtilities", Meta = (WorldContext = "WorldContextObject", Keywords = "C++"))
		static int32 EnsureUniqeIdentifiers(UObject* WorldContextObject);

	/** Check whether a identity component has a unique identifier */
	UFUNCTION(BlueprintPure, Category = "!TGOR EditorUtilities", Meta = (WorldContext = "WorldContextObject", Keywords = "C++"))
		static bool HasUniqeIdentifier(UTGOR_IdentityComponent* Identity, UObject* WorldContextObject);

	/** Returns all actor that have a dimension or save interface but no identifier component. */
	UFUNCTION(BlueprintCallable, Category = "!TGOR EditorUtilities", Meta = (WorldContext = "WorldContextObject", Keywords = "C++"))
		static TArray<AActor*> GetMissingIdentifierComponent(UObject* WorldContextObject);

};
