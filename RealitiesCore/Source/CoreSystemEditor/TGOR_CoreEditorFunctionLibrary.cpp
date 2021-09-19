// The Gateway of Realities: Planes of Existence.


#include "TGOR_CoreEditorFunctionLibrary.h"

#include "Engine/World.h"
#include "Engine.h"
#include "EngineUtils.h"
#include "FileHelpers.h"

#include "CoreSystem/Content/TGOR_CoreContent.h"

#include "Developer/AssetTools/Public/AssetToolsModule.h"
#include "Editor/KismetCompiler/Public/KismetCompilerModule.h"
#include "Editor/UnrealEd/Public/PackageTools.h"
#include "Editor/UnrealEd/Public/ObjectTools.h"

#include "Kismet2/KismetEditorUtilities.h"

#include "Runtime/AssetRegistry/Public/AssetRegistryModule.h"

#include "Editor/UnrealEd/Public/Subsystems/AssetEditorSubsystem.h"

#include "LevelEditor.h"


#define LOCTEXT_NAMESPACE "TGOR_CoreSystemEditorFunctionLibrary"

void UTGOR_CoreEditorFunctionLibrary::BindOnActorSelectionChangedEvent(FActorSelectedDelegate OnActorSelected)
{
	FLevelEditorModule& EditorModule = FModuleManager::GetModuleChecked<FLevelEditorModule>("LevelEditor");
	EditorModule.OnActorSelectionChanged().AddLambda(
		[OnActorSelected](const TArray<UObject*>& Objects, bool)
		{
			TArray<AActor*> Actors;
			for (UObject* Object : Objects)
			{
				if (AActor* Actor = Cast<AActor>(Object))
				{
					Actors.Emplace(Actor);
				}
			}
			FEditorScriptExecutionGuard ScriptGuard;
			OnActorSelected.ExecuteIfBound(Actors);
		});
}


UClass* UTGOR_CoreEditorFunctionLibrary::CreateAssetFromClass(TSubclassOf<UTGOR_Content> Class, bool Open, bool Autosave)
{
	// Generate name
	FString RawName = Class->GetName();
	RawName = UPackageTools::SanitizePackageName(RawName);
	RawName = ObjectTools::SanitizeObjectName(RawName);

	if (RawName.EndsWith("_C"))
	{
		RawName = RawName.LeftChop(2);
	}

	return CreateAssetFromClassNamed(Class, RawName, Open, Autosave);
}

UClass* UTGOR_CoreEditorFunctionLibrary::CreateAssetFromClassNamed(TSubclassOf<UTGOR_Content> Class, const FString& RawName, bool Open, bool Autosave)
{
	FAssetToolsModule& AssetToolsModule = FModuleManager::GetModuleChecked<FAssetToolsModule>("AssetTools");
	const FString PackagePath = "/Game/Mods/Core/Generated/" + RawName;
	UPackage* OuterForAsset = CreatePackage(*(PackagePath));

	// Get unique name
	int32 Counter = 0;
	FString Name = RawName;
	while (FindObject<UBlueprint>(OuterForAsset, *Name) != NULL)
	{
		Name = RawName + FString::FromInt(Counter++);
	}

	// Get blueprint types
	UClass* BlueprintClass = nullptr;
	UClass* BlueprintGeneratedClass = nullptr;
	IKismetCompilerInterface& KismetCompilerModule = FModuleManager::LoadModuleChecked<IKismetCompilerInterface>("KismetCompiler");
	KismetCompilerModule.GetBlueprintTypesForClass(Class, BlueprintClass, BlueprintGeneratedClass);

	// Create Blueprint
	UBlueprint* NewBlueprint = FKismetEditorUtilities::CreateBlueprint(Class, OuterForAsset, FName(*Name), BPTYPE_Normal, BlueprintClass, BlueprintGeneratedClass, FName("GeneratingBlueprintContent"));
	FAssetRegistryModule::AssetCreated(NewBlueprint);
	
	// Open editor if desired
	if (Open)
	{
		UAssetEditorSubsystem* AssetEditorSubsystem = GEditor->GetEditorSubsystem<UAssetEditorSubsystem>();
		AssetEditorSubsystem->OpenEditorForAsset(NewBlueprint);
	}

	// Save new content
	OuterForAsset->SetDirtyFlag(true);

	if (Autosave)
	{
		TArray<UPackage*> PackagesToSave;
		PackagesToSave.Add(OuterForAsset);
		FEditorFileUtils::PromptForCheckoutAndSave(PackagesToSave, false, false);
	}

	return NewBlueprint->GeneratedClass;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

bool UTGOR_CoreEditorFunctionLibrary::IsBlueprintContent(TSubclassOf<UTGOR_Content> Class)
{
	return Class->ClassGeneratedBy != nullptr;
}

void UTGOR_CoreEditorFunctionLibrary::GetContentAttributes(TSubclassOf<UTGOR_Content> Class, FString& DefaultName, FText& DisplayName, bool& IsAbstract)
{
	UTGOR_Content* Content = Class->GetDefaultObject<UTGOR_Content>();
	if (IsValid(Content))
	{
		DefaultName = Content->GetDefaultName();
		DisplayName = FText::FromString(DefaultName);//Content->GetDisplay().DisplayName;
		IsAbstract = Content->IsAbstract();
	}
}

void UTGOR_CoreEditorFunctionLibrary::SetContentDefaultName(TSubclassOf<UTGOR_Content> Class, const FString& DefaultName)
{
	if (IsBlueprintContent(Class))
	{
		UTGOR_Content* Content = Class->GetDefaultObject<UTGOR_Content>();
		if (IsValid(Content))
		{
			Content->SetDefaultName(DefaultName);
		}
	}
}

void UTGOR_CoreEditorFunctionLibrary::SetContentDisplayName(TSubclassOf<UTGOR_Content> Class, const FText& DisplayName)
{
	if (IsBlueprintContent(Class))
	{
		UTGOR_Content* Content = Class->GetDefaultObject<UTGOR_Content>();
		if (IsValid(Content))
		{
			UTGOR_CoreContent* CoreContent = Class->GetDefaultObject<UTGOR_CoreContent>();
			if (IsValid(CoreContent))
			{
				CoreContent->SetRawDisplayName(DisplayName);
			}
			else
			{

			}
		}
	}
}

void UTGOR_CoreEditorFunctionLibrary::SetContentAbstract(TSubclassOf<UTGOR_Content> Class, bool IsAbstract)
{
	if (IsBlueprintContent(Class))
	{
		UTGOR_Content* Content = Class->GetDefaultObject<UTGOR_Content>();
		if (IsValid(Content))
		{
			Content->SetIsAbstract(IsAbstract);
		}
	}
}

#undef LOCTEXT_NAMESPACE