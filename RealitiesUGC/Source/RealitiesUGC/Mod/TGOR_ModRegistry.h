// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "Engine.h"
#include "TGOR_ModRegistry.generated.h"

class UTGOR_Content;

USTRUCT(BlueprintType)
struct FTGOR_UGCPackage
{
	GENERATED_BODY()
		FTGOR_UGCPackage();

	UPROPERTY(BlueprintReadOnly, Category = "RealitiesUGC")
		FName PackagePath;

	UPROPERTY(BlueprintReadOnly, Category = "RealitiesUGC")
		FString EngineVersion;

	UPROPERTY(BlueprintReadOnly, Category = "RealitiesUGC")
		FString Author;

	UPROPERTY(BlueprintReadOnly, Category = "RealitiesUGC")
		FString Description;

	UPROPERTY(BlueprintReadOnly, Category = "RealitiesUGC")
		bool IsCorePackage;
};

UCLASS(BlueprintType)
class REALITIESUGC_API UTGOR_ModRegistry : public UObject
{
	GENERATED_BODY()

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	// This is our list of UGC packages. Populated by FindUGCPackages()
	UPROPERTY(BlueprintReadOnly, Category = "RealitiesUGC")
		TArray<FTGOR_UGCPackage> UGCPackages;

	// A pairing of Origins and Overrides. This is what the gameplay logic references when loading an effective class
    UPROPERTY(BlueprintReadOnly, Category = "RealitiesUGC|Actor Replacement")
		TMap<TSubclassOf<AActor> /*Origin*/, TSubclassOf<AActor> /*Override*/> RegisteredOverrides;
    

	// This populates UGCPackages based on what is found in UGC plugin files. If you're mounting new /Plugin paks at runtime. Expose this to Blueprints.
	UFUNCTION(Blueprintcallable)
		bool FindUGCPackages();
    
	// Returns All Mod classes in all UGC package.
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "RealitiesUGC")
		bool GetModsInPackages(TMap<UClass*, FName>& Classes);

	// Returns All Mod classes in all UGC package.
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "RealitiesUGC")
		bool GetContentInMod(UTGOR_Mod* Mod, TArray<UClass*>& Classes);

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Creates content class from class (used so references persist over different versions) */
	UFUNCTION(BlueprintCallable, Category = "!TGOR System|Internal", Meta = (Keywords = "C++"))
		UTGOR_Content* ConstructContent(TSubclassOf<UTGOR_Content> Class);

	////////////////////////////////////////////////////////////////////////////////////////////////////
protected:

	/** Pool of content objects to make construction easier */
	UPROPERTY(BlueprintReadOnly, Category = "!TGOR System")
		TMap<TSubclassOf<UTGOR_Content>, UTGOR_Content*> ContentPool;

	////////////////////////////////////////////////////////////////////////////////////////////////////
private:

	FAssetRegistryModule* CachedAssetRegistryModule;
	IAssetRegistry& GetAsstRegistry();

};
