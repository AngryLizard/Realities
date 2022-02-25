// The Gateway of Realities: Planes of Existence.


#include "TGOR_BlueprintFunctionLibrary.h"

#include "Engine/World.h"
#include "Engine/Engine.h"
#include "Engine.h"
#include "EngineUtils.h"

#include "Realities/Base/TGOR_WorldSettings.h"
#include "Realities/Base/System/Data/TGOR_DimensionData.h"
#include "Realities/Actors/Dimension/Volumes/TGOR_LevelVolume.h"
#include "Realities/Mod/Dimensions/TGOR_Dimension.h"
#include "Realities/Components/Dimension/Portals/TGOR_DimensionPortalComponent.h"
#include "Realities/Components/Dimension/TGOR_IdentityComponent.h"
#include "Realities/Base/Content/TGOR_Content.h"

#include "Developer/AssetTools/Public/AssetToolsModule.h"
#include "Editor/KismetCompiler/Public/KismetCompilerModule.h"
#include "Editor/UnrealEd/Public/PackageTools.h"
#include "Editor/UnrealEd/Public/ObjectTools.h"
#include "Editor/UnrealEd/Public/Toolkits/AssetEditorManager.h"
#include "Editor/UnrealEd/Public/FileHelpers.h"

#include "Kismet2/BlueprintEditorUtils.h"
#include "Kismet2/KismetEditorUtilities.h"

#include "Runtime/AssetRegistry/Public/AssetRegistryModule.h"

#include "Editor/UnrealEd/Public/Subsystems/AssetEditorSubsystem.h"

#include "Engine/LevelBounds.h"

UClass* UTGOR_BlueprintFunctionLibrary::CreateAssetFromClass(TSubclassOf<UTGOR_Content> Class, bool Open, bool Autosave)
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

UClass* UTGOR_BlueprintFunctionLibrary::CreateAssetFromClassNamed(TSubclassOf<UTGOR_Content> Class, const FString& RawName, bool Open, bool Autosave)
{
	FAssetToolsModule& AssetToolsModule = FModuleManager::GetModuleChecked<FAssetToolsModule>("AssetTools");
	const FString PackagePath = "/Game/Mods/Core/Generated/" + RawName;
	UPackage* OuterForAsset = CreatePackage(nullptr, *(PackagePath));

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


bool UTGOR_BlueprintFunctionLibrary::HasWorldDimension(UObject* WorldContextObject)
{
	UWorld* World = WorldContextObject->GetWorld();
	if (IsValid(World) && IsValid(World->PersistentLevel))
	{
		ATGOR_WorldSettings* WorldSettings = Cast<ATGOR_WorldSettings>(World->PersistentLevel->GetWorldSettings());
		if (IsValid(WorldSettings))
		{
			return *WorldSettings->Dimension != nullptr;
		}
	}
	return false;
}

void UTGOR_BlueprintFunctionLibrary::CreateWorldDimension(UObject* WorldContextObject, TSubclassOf<UTGOR_Dimension> DimensionClass)
{
	UWorld* World = WorldContextObject->GetWorld();
	if (IsValid(World) && IsValid(World->PersistentLevel))
	{
		ATGOR_WorldSettings* WorldSettings = Cast<ATGOR_WorldSettings>(World->PersistentLevel->GetWorldSettings());
		if (IsValid(WorldSettings))
		{
			WorldSettings->Dimension = CreateAssetFromClassNamed(DimensionClass, FString("DIM_") + World->GetName(), true, false);
			WorldSettings->MarkPackageDirty();
		}
	}
}

void UTGOR_BlueprintFunctionLibrary::AssignCurrentWorldToDimension(UObject* WorldContextObject)
{
	UWorld* World = WorldContextObject->GetWorld();
	if (IsValid(World) && IsValid(World->PersistentLevel))
	{
		ATGOR_WorldSettings* WorldSettings = Cast<ATGOR_WorldSettings>(World->PersistentLevel->GetWorldSettings());
		if (IsValid(WorldSettings) && *WorldSettings->Dimension)
		{
			// Get dimension
			UTGOR_Dimension* Dimension = WorldSettings->Dimension->GetDefaultObject<UTGOR_Dimension>();
			if (IsValid(Dimension))
			{
				Dimension->World = World;
				Dimension->SetIsAbstract(false);
				Dimension->MarkPackageDirty();
			}
		}
	}
}

void UTGOR_BlueprintFunctionLibrary::AssignCurrentBoundsToDimension(UObject* WorldContextObject, AActor* Volume)
{
	UWorld* World = WorldContextObject->GetWorld();
	if (IsValid(World) && IsValid(World->PersistentLevel))
	{
		ATGOR_WorldSettings* WorldSettings = Cast<ATGOR_WorldSettings>(World->PersistentLevel->GetWorldSettings());
		if (IsValid(WorldSettings) && *WorldSettings->Dimension)
		{
			// Get dimension
			UTGOR_Dimension* Dimension = WorldSettings->Dimension->GetDefaultObject<UTGOR_Dimension>();
			if (IsValid(Dimension))
			{
				FVector Origin, Extend;
				if (IsValid(Volume))
				{
					Volume->GetActorBounds(false, Origin, Extend);
				}
				else if (IsValid(World->PersistentLevel) && World->PersistentLevel->LevelBoundsActor.IsValid())
				{
					World->PersistentLevel->LevelBoundsActor->GetActorBounds(false, Origin, Extend);
				}
				else
				{
					// Find level volume
					for (FActorIterator Its(World); Its; ++Its)
					{
						if (Its->IsA(ATGOR_LevelVolume::StaticClass()))
						{
							Cast<ATGOR_LevelVolume>(*Its)->GetActorBounds(false, Origin, Extend);
						}
					}
				}
				Dimension->Bounds = (Extend + Origin.GetAbs()) * 2;
				Dimension->MarkPackageDirty();
			}
		}
	}
}

void UTGOR_BlueprintFunctionLibrary::UpdatePortalList(UObject* WorldContextObject)
{
	UWorld* World = WorldContextObject->GetWorld();
	if (IsValid(World) && IsValid(World->PersistentLevel))
	{
		ATGOR_WorldSettings* WorldSettings = Cast<ATGOR_WorldSettings>(World->PersistentLevel->GetWorldSettings());
		if (IsValid(WorldSettings) && *WorldSettings->Dimension)
		{
			// Get dimension
			UTGOR_Dimension* Dimension = WorldSettings->Dimension->GetDefaultObject<UTGOR_Dimension>();
			if (IsValid(Dimension))
			{
				// Get all portal names
				Dimension->PublicPortals.Empty();
				for (FActorIterator Its(World); Its; ++Its)
				{
					TArray<UTGOR_DimensionPortalComponent*> Components;
					Its->GetComponents(Components);
					for (UTGOR_DimensionPortalComponent* Component : Components)
					{
						if (Component->IsPublic)
						{
							Dimension->PublicPortals.Emplace(Component->GetPortalName());
						}
					}
				}
				Dimension->MarkPackageDirty();
			}
		}
	}
}

TArray<FName> UTGOR_BlueprintFunctionLibrary::GetPortalListFromWorld(UObject* WorldContextObject)
{
	UWorld* World = WorldContextObject->GetWorld();
	if (IsValid(World) && IsValid(World->PersistentLevel))
	{
		ATGOR_WorldSettings* WorldSettings = Cast<ATGOR_WorldSettings>(World->PersistentLevel->GetWorldSettings());
		if (IsValid(WorldSettings) && *WorldSettings->Dimension)
		{
			// Get dimension
			UTGOR_Dimension* Dimension = WorldSettings->Dimension->GetDefaultObject<UTGOR_Dimension>();
			if (IsValid(Dimension))
			{
				return Dimension->PublicPortals;
			}
		}
	}
	return TArray<FName>();
}

UTGOR_DimensionPortalComponent* UTGOR_BlueprintFunctionLibrary::GetPortalFromWorld(const FName& PortalName, UObject* WorldContextObject)
{
	UWorld* World = WorldContextObject->GetWorld();
	if (IsValid(World))
	{
		for (FActorIterator Its(World); Its; ++Its)
		{
			TArray<UTGOR_DimensionPortalComponent*> Components;
			Its->GetComponents(Components);
			for (UTGOR_DimensionPortalComponent* Component : Components)
			{
				if (Component->GetPortalName() == PortalName)
				{
					return Component;
				}
			}
		}
	}
	return nullptr;
}

FString UTGOR_BlueprintFunctionLibrary::GetCurrentWorldName(UObject* WorldContextObject)
{
	UWorld* World = WorldContextObject->GetWorld();
	if (IsValid(World))
	{
		return World->GetName();
	}
	return "None";
}


bool UTGOR_BlueprintFunctionLibrary::IsBlueprintContent(TSubclassOf<UTGOR_Content> Class)
{
	return Class->ClassGeneratedBy != nullptr;
}

void UTGOR_BlueprintFunctionLibrary::GetContentAttributes(TSubclassOf<UTGOR_Content> Class, FString& DefaultName, FText& DisplayName, bool& IsAbstract)
{
	UTGOR_Content* Content = Class->GetDefaultObject<UTGOR_Content>();
	if (IsValid(Content))
	{
		DefaultName = Content->GetDefaultName();
		DisplayName = Content->GetDisplay().DisplayName;
		IsAbstract = Content->IsAbstract();
	}
}

void UTGOR_BlueprintFunctionLibrary::SetContentDefaultName(TSubclassOf<UTGOR_Content> Class, const FString& DefaultName)
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

void UTGOR_BlueprintFunctionLibrary::SetContentDisplayName(TSubclassOf<UTGOR_Content> Class, const FText& DisplayName)
{
	if (IsBlueprintContent(Class))
	{
		UTGOR_Content* Content = Class->GetDefaultObject<UTGOR_Content>();
		if (IsValid(Content))
		{
			Content->SetRawDisplayName(DisplayName);
		}
	}
}

void UTGOR_BlueprintFunctionLibrary::SetContentAbstract(TSubclassOf<UTGOR_Content> Class, bool IsAbstract)
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

void UTGOR_BlueprintFunctionLibrary::AddContentInsertion(TSubclassOf<UTGOR_Content> Class, TSubclassOf<UTGOR_Content> Insertion)
{
	if (IsBlueprintContent(Class))
	{
		UTGOR_Content* Content = Class->GetDefaultObject<UTGOR_Content>();
		if (IsValid(Content))
		{
			Content->AddInsertion(Insertion);
		}
	}
}

void UTGOR_BlueprintFunctionLibrary::RemoveContentInsertion(TSubclassOf<UTGOR_Content> Class, TSubclassOf<UTGOR_Content> Insertion)
{
	if (IsBlueprintContent(Class))
	{
		UTGOR_Content* Content = Class->GetDefaultObject<UTGOR_Content>();
		if (IsValid(Content))
		{
			Content->RemoveInsertion(Insertion);
		}
	}
}

int32 UTGOR_BlueprintFunctionLibrary::CountIdentityComponents(UObject* WorldContextObject)
{
	int32 Count = 0;
	UWorld* World = WorldContextObject->GetWorld();
	if (IsValid(World))
	{
		for (FActorIterator Its(World); Its; ++Its)
		{
			TArray<UTGOR_IdentityComponent*> Components;
			Its->GetComponents(Components);
			Count += Components.Num();
		}
	}
	return Count;
}

int32 UTGOR_BlueprintFunctionLibrary::EnsureUniqeIdentifiers(UObject* WorldContextObject)
{
	int32 ChangeCount = 0;

	UWorld* World = WorldContextObject->GetWorld();
	if (IsValid(World))
	{
		for (FActorIterator Its(World); Its; ++Its)
		{
			TArray<UTGOR_IdentityComponent*> Components;
			Its->GetComponents(Components);
			for (UTGOR_IdentityComponent* Component : Components)
			{
				while (Component->GetActorIdentifier() < 0 || !HasUniqeIdentifier(Component, WorldContextObject))
				{
					Component->Identity.Identifier = UTGOR_DimensionData::GetRandomActorIdentifier();
					ChangeCount++;
				}
			}
		}
	}
	return ChangeCount;
}

bool UTGOR_BlueprintFunctionLibrary::HasUniqeIdentifier(UTGOR_IdentityComponent* Identity, UObject* WorldContextObject)
{
	const int32 Identifier = Identity->GetActorIdentifier();

	UWorld* World = WorldContextObject->GetWorld();
	if (IsValid(World))
	{
		for (FActorIterator Its(World); Its; ++Its)
		{
			TArray<UTGOR_IdentityComponent*> Components;
			Its->GetComponents(Components);
			for (UTGOR_IdentityComponent* Component : Components)
			{
				if (Identity != Component)
				{
					const int32 OtherIdentifier = Component->GetActorIdentifier();
					if (OtherIdentifier == Identifier)
					{
						return false;
					}
				}
			}
		}
	}
	return true;
}


TArray<AActor*> UTGOR_BlueprintFunctionLibrary::GetMissingIdentifierComponent(UObject* WorldContextObject)
{
	TArray<AActor*> Actors;

	UWorld* World = WorldContextObject->GetWorld();
	if (IsValid(World))
	{
		for (FActorIterator Its(World); Its; ++Its)
		{
			bool HasInterface = false;
			ITGOR_DimensionInterface* ActorDimensionInterface = Cast<ITGOR_DimensionInterface>(*Its);
			if (ActorDimensionInterface)
			{
				HasInterface = true;
			}

			ITGOR_SaveInterface* ActorSaveInterface = Cast<ITGOR_SaveInterface>(*Its);
			if (ActorSaveInterface)
			{
				HasInterface = true;
			}

			const TSet<UActorComponent*>& Components = Its->GetComponents();
			for (UActorComponent* Component : Components)
			{
				// Don't create loops with myself or other identitycomponents
				if (!Component->IsA(UTGOR_IdentityComponent::StaticClass()))
				{
					ITGOR_DimensionInterface* ComponentDimensionInterface = Cast<ITGOR_DimensionInterface>(Component);
					if (ComponentDimensionInterface)
					{
						HasInterface = true;
					}

					ITGOR_SaveInterface* ComponentSaveInterface = Cast<ITGOR_SaveInterface>(Component);
					if (ComponentSaveInterface)
					{
						HasInterface = true;
					}
				}
			}
			
			TArray<UTGOR_IdentityComponent*> Identities;
			Its->GetComponents(Identities);
			if (Identities.Num() == 0 && HasInterface)
			{
				Actors.Emplace(*Its);
			}
		}
	}

	return Actors;
}