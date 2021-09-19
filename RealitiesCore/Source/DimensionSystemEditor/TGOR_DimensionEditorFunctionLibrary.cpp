// The Gateway of Realities: Planes of Existence.

#include "TGOR_DimensionEditorFunctionLibrary.h"

#include "Engine/World.h"
#include "Engine.h"
#include "EngineUtils.h"

#include "DimensionSystem/Data/TGOR_DimensionData.h"
#include "DimensionSystem/Volumes/TGOR_LevelVolume.h"
#include "DimensionSystem/Content/TGOR_Dimension.h"
#include "DimensionSystem/Components/TGOR_ConnectionComponent.h"
#include "DimensionSystem/Components/TGOR_IdentityComponent.h"
#include "DimensionSystem/Gameplay/TGOR_DimensionWorldSettings.h"

#include "CoreSystemEditor/TGOR_CoreEditorFunctionLibrary.h"

#include "Engine/LevelBounds.h"


#define LOCTEXT_NAMESPACE "TGOR_DimensionEditorFunctionLibrary"

bool UTGOR_DimensionEditorFunctionLibrary::HasWorldDimension(UObject* WorldContextObject)
{
	UWorld* World = WorldContextObject->GetWorld();
	if (IsValid(World) && IsValid(World->PersistentLevel))
	{
		ATGOR_DimensionWorldSettings* WorldSettings = Cast<ATGOR_DimensionWorldSettings>(World->PersistentLevel->GetWorldSettings());
		if (IsValid(WorldSettings))
		{
			return *WorldSettings->Dimension != nullptr;
		}
	}
	return false;
}

void UTGOR_DimensionEditorFunctionLibrary::CreateWorldDimension(UObject* WorldContextObject, TSubclassOf<UTGOR_Dimension> DimensionClass)
{
	UWorld* World = WorldContextObject->GetWorld();
	if (IsValid(World) && IsValid(World->PersistentLevel))
	{
		ATGOR_DimensionWorldSettings* WorldSettings = Cast<ATGOR_DimensionWorldSettings>(World->PersistentLevel->GetWorldSettings());
		if (IsValid(WorldSettings))
		{
			WorldSettings->Dimension = UTGOR_CoreEditorFunctionLibrary::CreateAssetFromClassNamed(DimensionClass, FString("DIM_") + World->GetName(), true, false);
			WorldSettings->MarkPackageDirty();
		}
	}
}

void UTGOR_DimensionEditorFunctionLibrary::AssignCurrentWorldToDimension(UObject* WorldContextObject)
{
	UWorld* World = WorldContextObject->GetWorld();
	if (IsValid(World) && IsValid(World->PersistentLevel))
	{
		ATGOR_DimensionWorldSettings* WorldSettings = Cast<ATGOR_DimensionWorldSettings>(World->PersistentLevel->GetWorldSettings());
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

void UTGOR_DimensionEditorFunctionLibrary::AssignCurrentBoundsToDimension(UObject* WorldContextObject, AActor* Volume)
{
	UWorld* World = WorldContextObject->GetWorld();
	if (IsValid(World) && IsValid(World->PersistentLevel))
	{
		ATGOR_DimensionWorldSettings* WorldSettings = Cast<ATGOR_DimensionWorldSettings>(World->PersistentLevel->GetWorldSettings());
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

void UTGOR_DimensionEditorFunctionLibrary::UpdateConnectionList(UObject* WorldContextObject)
{
	UWorld* World = WorldContextObject->GetWorld();
	if (IsValid(World) && IsValid(World->PersistentLevel))
	{
		ATGOR_DimensionWorldSettings* WorldSettings = Cast<ATGOR_DimensionWorldSettings>(World->PersistentLevel->GetWorldSettings());
		if (IsValid(WorldSettings) && *WorldSettings->Dimension)
		{
			// Get dimension
			UTGOR_Dimension* Dimension = WorldSettings->Dimension->GetDefaultObject<UTGOR_Dimension>();
			if (IsValid(Dimension))
			{
				// Get all portal names
				Dimension->PublicConnections.Empty();
				for (FActorIterator Its(World); Its; ++Its)
				{
					TArray<UTGOR_ConnectionComponent*> Components;
					Its->GetComponents(Components);
					for (UTGOR_ConnectionComponent* Component : Components)
					{
						if (Component->IsPublic)
						{
							Dimension->PublicConnections.Emplace(Component->GetConnectionName());
						}
					}
				}
				Dimension->MarkPackageDirty();
			}
		}
	}
}

TArray<FName> UTGOR_DimensionEditorFunctionLibrary::GetConnectionListFromWorld(UObject* WorldContextObject)
{
	UWorld* World = WorldContextObject->GetWorld();
	if (IsValid(World) && IsValid(World->PersistentLevel))
	{
		ATGOR_DimensionWorldSettings* WorldSettings = Cast<ATGOR_DimensionWorldSettings>(World->PersistentLevel->GetWorldSettings());
		if (IsValid(WorldSettings) && *WorldSettings->Dimension)
		{
			// Get dimension
			UTGOR_Dimension* Dimension = WorldSettings->Dimension->GetDefaultObject<UTGOR_Dimension>();
			if (IsValid(Dimension))
			{
				return Dimension->PublicConnections;
			}
		}
	}
	return TArray<FName>();
}

UTGOR_ConnectionComponent* UTGOR_DimensionEditorFunctionLibrary::GetConnectionFromWorld(const FName& ConnectionName, UObject* WorldContextObject)
{
	UWorld* World = WorldContextObject->GetWorld();
	if (IsValid(World))
	{
		for (FActorIterator Its(World); Its; ++Its)
		{
			TArray<UTGOR_ConnectionComponent*> Components;
			Its->GetComponents(Components);
			for (UTGOR_ConnectionComponent* Component : Components)
			{
				if (Component->GetConnectionName() == ConnectionName)
				{
					return Component;
				}
			}
		}
	}
	return nullptr;
}

FString UTGOR_DimensionEditorFunctionLibrary::GetCurrentWorldName(UObject* WorldContextObject)
{
	UWorld* World = WorldContextObject->GetWorld();
	if (IsValid(World))
	{
		return World->GetName();
	}
	return "None";
}

////////////////////////////////////////////////////////////////////////////////////////////////////

int32 UTGOR_DimensionEditorFunctionLibrary::CountIdentityComponents(UObject* WorldContextObject)
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

int32 UTGOR_DimensionEditorFunctionLibrary::EnsureUniqeIdentifiers(UObject* WorldContextObject)
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
					Component->DimensionIdentity.Identifier = UTGOR_DimensionData::GetRandomActorIdentifier();
					ChangeCount++;
				}
			}
		}
	}
	return ChangeCount;
}

bool UTGOR_DimensionEditorFunctionLibrary::HasUniqeIdentifier(UTGOR_IdentityComponent* Identity, UObject* WorldContextObject)
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


TArray<AActor*> UTGOR_DimensionEditorFunctionLibrary::GetMissingIdentifierComponent(UObject* WorldContextObject)
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

#undef LOCTEXT_NAMESPACE