// Copyright Epic Games, Inc. All Rights Reserved.

#include "TGOR_ModRegistry.h"
#include "AssetRegistryModule.h"
#include "ARFilter.h"
#include "ProjectDescriptor.h"
#include "Interfaces/IPluginManager.h"
#include "Interfaces/IProjectManager.h"
#include "Misc/PackageName.h"
#include "RealitiesUGC/RealitiesUGC.h"

#include "RealitiesUGC/Mod/TGOR_Mod.h"
#include "RealitiesUGC/Mod/TGOR_Content.h"


FTGOR_UGCPackage::FTGOR_UGCPackage()
{
	PackagePath = "";
	EngineVersion = "";
	Author = "";
	Description = "";
	IsCorePackage = false;
}

bool UTGOR_ModRegistry::FindUGCPackages()
{
	const FProjectDescriptor* Project = IProjectManager::Get().GetCurrentProject();

	FTGOR_UGCPackage GamePackage;
	GamePackage.PackagePath = FName("/Game");
	GamePackage.EngineVersion = *Project->EngineAssociation; //FEngineVersion::Current();
	GamePackage.Author = TEXT("TGOR");
	GamePackage.Description = *Project->Description;
	GamePackage.IsCorePackage = false;
	UGCPackages.Add(GamePackage);

	TArray<TSharedRef<IPlugin>> EnabledPlugins = IPluginManager::Get().GetEnabledPlugins();
	for (const TSharedRef<IPlugin>& Plugin : EnabledPlugins)
	{
		const FPluginDescriptor& Descriptor = Plugin->GetDescriptor();
		const bool IsCorePackage = Descriptor.Category == "TGOR_Core";
		if (Plugin->GetLoadedFrom() == EPluginLoadedFrom::Project && (Descriptor.Category == "TGOR_Mod" || IsCorePackage))
		{
			FTGOR_UGCPackage Package;
			Package.PackagePath = FName(*Plugin->GetMountedAssetPath().LeftChop(1));
			Package.EngineVersion = *Descriptor.EngineVersion;
			Package.Author = *Descriptor.CreatedBy;
			Package.Description = *Descriptor.Description;
			Package.IsCorePackage = IsCorePackage;
			UGCPackages.Add(Package);
		}
	}
	
	return UGCPackages.Num() > 0;
}

bool UTGOR_ModRegistry::GetModsInPackages(TMap<UClass*, FName> &Classes)
{
	// Load up the AssetRegistry, Filter for Blueprints

	IAssetRegistry& AssetRegistry = GetAsstRegistry();
	FARFilter ARFilter;
	TArray<FAssetData> AssetList;
	ARFilter.bRecursivePaths = true;
	ARFilter.bIncludeOnlyOnDiskAssets = true;
	ARFilter.bRecursiveClasses = true;
	ARFilter.ClassNames.Add(UBlueprint::StaticClass()->GetFName());

	// Go through all packages
	for (const FTGOR_UGCPackage& Package : UGCPackages)
	{
		ARFilter.PackagePaths.Add(Package.PackagePath);
	}

	// Get all mod classes
	AssetRegistry.GetAssets(ARFilter, AssetList);
	for (const FAssetData& Asset : AssetList)
	{
        FAssetDataTagMapSharedView::FFindTagResult GeneratedClassResult = Asset.TagsAndValues.FindTag("GeneratedClass");
		if (GeneratedClassResult.IsSet())
		{
			FString ClassPath = FPackageName::ExportTextPathToObjectPath(*GeneratedClassResult.GetValue());
			if (!ClassPath.IsEmpty())
			{
				UClass* AssetClass = LoadObject<UClass>(NULL, *ClassPath);
				if (!IsValid(AssetClass))
				{
					//ERRET(FString("Modload: Couldn't load class path ") + ClassPath, Warning, false);
				}
				else if (AssetClass->IsChildOf(UTGOR_Mod::StaticClass()))
				{
					Classes.Add(AssetClass, Asset.PackagePath);
				}
			}
		}
	}

	return Classes.Num() > 0;
}

bool UTGOR_ModRegistry::GetContentInMod(UTGOR_Mod* Mod, TArray<UClass*>& Classes)
{
	// Load up the AssetRegistry, Filter for Blueprints

	IAssetRegistry& AssetRegistry = GetAsstRegistry();
	FARFilter ARFilter;
	TArray<FAssetData> AssetList;
	ARFilter.bRecursivePaths = true;
	ARFilter.bIncludeOnlyOnDiskAssets = true;
	ARFilter.bRecursiveClasses = true;
	ARFilter.ClassNames.Add(UBlueprint::StaticClass()->GetFName());
	ARFilter.PackagePaths.Add(Mod->PackagePath);

	if (Mod->IsA<UTGOR_CoreMod>())
	{
		for (const FTGOR_UGCPackage& Package : UGCPackages)
		{
			if (Package.IsCorePackage)
			{
				ARFilter.PackagePaths.Add(Package.PackagePath);
			}
		}
	}

	// Get all mod classes
	AssetRegistry.GetAssets(ARFilter, AssetList);
	for (const FAssetData& Asset : AssetList)
	{
		FAssetDataTagMapSharedView::FFindTagResult GeneratedClassResult = Asset.TagsAndValues.FindTag("GeneratedClass");
		if (GeneratedClassResult.IsSet())
		{
			FString ClassPath = FPackageName::ExportTextPathToObjectPath(*GeneratedClassResult.GetValue());
			if (!ClassPath.IsEmpty())
			{
				UClass* AssetClass = LoadObject<UClass>(Mod, *ClassPath);
				if (!IsValid(AssetClass))
				{
					//ERRET(FString("ContentLoad: Couldn't load class path ") + ClassPath, Warning, false);
				}
				else if (AssetClass->IsChildOf(UTGOR_Content::StaticClass()))
				{
					Classes.Add(AssetClass);
				}
			}
		}
	}

	return Classes.Num() > 0;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

UTGOR_Content* UTGOR_ModRegistry::ConstructContent(TSubclassOf<UTGOR_Content> Class)
{
	// Need outer that is network enabled
	UObject* Outer = GetOuter();

	UTGOR_Content*& Content = ContentPool.FindOrAdd(Class);
	if (IsValid(Content))
	{
		Content->Reset();
	}
	else if (*Class)
	{
		UTGOR_Content* DefaultObject = Class->GetDefaultObject<UTGOR_Content>();
		FName ContentName = *(FString("TGOR_") + DefaultObject->GetDefaultName());

		UTGOR_Content* ExistingObject = Cast<UTGOR_Content>(StaticFindObjectFast(nullptr, Outer, ContentName));
		if (IsValid(ExistingObject))
		{
			if (ExistingObject->IsA(Class))
			{
				Content = ExistingObject;
			}
			else
			{
				Content = nullptr;
				ERRET(FString::Printf(TEXT("Duplicate default names detected! %s vs %s"), *ExistingObject->GetName(), *Class->GetName()), Error, Content);
			}
		}
		else
		{
			Content = NewObject<UTGOR_Content>(Outer, Class, ContentName);
		}
	}
	else
	{
		Content = nullptr;
	}
	return Content;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

IAssetRegistry& UTGOR_ModRegistry::GetAsstRegistry()
{
	if (!CachedAssetRegistryModule)
	{
		CachedAssetRegistryModule = &FModuleManager::LoadModuleChecked<FAssetRegistryModule>(TEXT("AssetRegistry"));
	}

	check(CachedAssetRegistryModule);
	return CachedAssetRegistryModule->Get();
}

