// Copyright Epic Games, Inc. All Rights Reserved.

#include "TGOR_SpawnerFactory.h"
#include "AssetRegistry/AssetRegistryModule.h"

#include "DimensionSystem/Content/TGOR_Spawner.h"
#include "DimensionSystem/Components/TGOR_IdentityComponent.h"

//////////////////////////////////////////////////////////////////////////
// UTGOR_SpawnerFactory

UTGOR_SpawnerFactory::UTGOR_SpawnerFactory(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	DisplayName = NSLOCTEXT("Spawner", "SpawnerDisplayName", "Add Spawner");
	NewActorClass = AActor::StaticClass();
}

AActor* UTGOR_SpawnerFactory::GetDefaultActor(const FAssetData& AssetData)
{
	if (!AssetData.IsValid() || !AssetData.GetClass()->IsChildOf(UBlueprint::StaticClass()))
	{
		return nullptr;
	}

	const FString GeneratedClassPath = AssetData.GetTagValueRef<FString>(FBlueprintTags::GeneratedClassPath);
	if (GeneratedClassPath.IsEmpty())
	{
		return nullptr;
	}

	UClass* GeneratedClass = Cast<UClass>(StaticLoadObject(UClass::StaticClass(), NULL, *GeneratedClassPath, NULL, LOAD_NoWarn, NULL));
	if (!IsValid(GeneratedClass))
	{
		return nullptr;
	}

	FEditorScriptExecutionGuard ScriptGuard;
	UTGOR_Spawner* Spawner = GeneratedClass->GetDefaultObject<UTGOR_Spawner>();
	TSubclassOf<AActor> ActorClass = Spawner->GetSpawnClass();
	if (!*ActorClass)
	{
		return nullptr;
	}
	return ActorClass->GetDefaultObject<AActor>();
}

void UTGOR_SpawnerFactory::PostSpawnActor(UObject* Asset, AActor* NewActor)
{
	Super::PostSpawnActor(Asset, NewActor);

	UBlueprint* Blueprint = Cast<UBlueprint>(Asset);
	if (IsValid(Blueprint) && IsValid(NewActor))
	{
		UTGOR_IdentityComponent* Identity = NewActor->FindComponentByClass<UTGOR_IdentityComponent>();
		if (IsValid(Identity))
		{
			Identity->DefaultSpawner = Blueprint->GeneratedClass;
		}
	}
}

bool UTGOR_SpawnerFactory::CanCreateActorFrom(const FAssetData& AssetData, FText& OutErrorMsg)
{
	if (!AssetData.IsValid() || !AssetData.GetClass()->IsChildOf(UBlueprint::StaticClass()))
	{
		OutErrorMsg = NSLOCTEXT("Spawner", "NoBlueprint", "No Blueprint was specified, or the specified Blueprint needs to be compiled.");
		return false;
	}

	const FString ParentClassPath = AssetData.GetTagValueRef<FString>(FBlueprintTags::ParentClassPath);
	if (ParentClassPath.IsEmpty())
	{
		OutErrorMsg = NSLOCTEXT("Spawner", "NoBlueprint", "No Blueprint was specified, or the specified Blueprint needs to be compiled.");
		return false;
	}

	UClass* ParentClass = FindObject<UClass>(NULL, *ParentClassPath);

	bool bIsActorBased = false;
	if (ParentClass != NULL)
	{
		// The parent class is loaded. Make sure it is derived from AActor
		bIsActorBased = ParentClass->IsChildOf(UTGOR_Spawner::StaticClass());
	}
	else
	{
		// The parent class does not exist or is not loaded.
		// Ask the asset registry for the ancestors of this class to see if it is an unloaded blueprint generated class.
		FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry");
		IAssetRegistry& AssetRegistry = AssetRegistryModule.Get();

		const FStringView ObjectPath = FPackageName::ExportTextPathToObjectPath(FStringView(ParentClassPath));
		const FTopLevelAssetPath ParentAssetClassPath = FTopLevelAssetPath(FPackageName::ObjectPathToObjectName(ObjectPath));
		TArray<FTopLevelAssetPath> AncestorClassNames;
		AssetRegistry.GetAncestorClassNames(ParentAssetClassPath, AncestorClassNames);

		bIsActorBased = AncestorClassNames.Contains(UTGOR_Spawner::StaticClass()->GetClassPathName());
	}

	if (!bIsActorBased)
	{
		OutErrorMsg = NSLOCTEXT("Spawner", "NotActor", "The specified Blueprint must be Spawner based.");
		return false;
	}

	return true;
}
