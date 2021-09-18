// The Gateway of Realities: Planes of Existence.


#include "TGOR_WorldData.h"

#include "RealitiesUGC/Mod/TGOR_ContentManager.h"
#include "CoreSystem/TGOR_Singleton.h"
#include "CoreSystem/Gameplay/TGOR_GameInstance.h"
#include "CoreSystem/Gameplay/TGOR_WorldSettings.h"
#include "DimensionSystem/Data/TGOR_DimensionData.h"
#include "CoreSystem/Components/TGOR_Component.h"
#include "DimensionSystem/Components/TGOR_WorldComponent.h"
#include "DimensionSystem/Components/TGOR_IdentityComponent.h"
#include "DimensionSystem/Components/TGOR_DimensionGateComponent.h"
#include "DimensionSystem/Volumes/TGOR_LevelVolume.h"
#include "DimensionSystem/Volumes/TGOR_PhysicsVolume.h"
#include "DimensionSystem/Content/TGOR_Dimension.h"
#include "DimensionSystem/Content/TGOR_Loader.h"
#include "DimensionSystem/Gameplay/TGOR_DimensionGameState.h"
#include "DimensionSystem/Gameplay/TGOR_DimensionController.h"

#include "Engine/Classes/Components/ModelComponent.h"
#include "Engine/LevelStreaming.h"
#include "Engine/World.h"

#include "CoreSystem/Storage/TGOR_Package.h"


UTGOR_WorldData::UTGOR_WorldData()
{
	ActiveIdentifier = "";

	RealityHeight = SPATIAL_BOUNDS;
	MiniDimensionHeight = SPATIAL_BOUNDS;

	Folder = "Worlds";
}


UWorld* UTGOR_WorldData::GetWorld() const
{
	return(PersistentWorld.Get());
}


void UTGOR_WorldData::Write_Implementation(FTGOR_GroupWritePackage& Package, UTGOR_Singleton* Context) const
{
	// We store identifiers and value separately to not override unused fields during read
	TArray<FName> Identifiers;
	Dimensions.GetKeys(Identifiers);
	Package.WriteEntry("Dimensions", Identifiers);

	// Write all dimensions in their separate fields
	for (const FName& Identifier : Identifiers)
	{
		Package.WriteEntry(Identifier.ToString(), Dimensions[Identifier]);
	}

	Package.WriteEntry("Trackers", TrackedActors);
}

bool UTGOR_WorldData::Read_Implementation(FTGOR_GroupReadPackage& Package, UTGOR_Singleton* Context)
{
	TArray<FName> Identifiers;
	if (Package.ReadEntry("Dimensions", Identifiers))
	{
		// Create or override data
		for (const FName& Identifier : Identifiers)
		{
			if (!Package.ReadEntry(Identifier.ToString(), Dimensions.FindOrAdd(Identifier)))
			{
				RPORT("Dimension not found!");
			}
		}

		Package.ReadEntry("Trackers", TrackedActors);
		return true;
	}
	return false;
}

FString UTGOR_WorldData::GetDimensionDirectory() const
{
	UWorld* World = GetWorld();
	ATGOR_DimensionGameState* GameState = Cast<ATGOR_DimensionGameState>(World->GetGameState());
	if (IsValid(GameState))
	{
		return UTGOR_GameInstance::GetSaveDirectory(World) / "Dimensions";
	}
	return "";
}

UTGOR_IdentityComponent* UTGOR_WorldData::GetIdentity(const FTGOR_DimensionIdentifier& Identifier, ETGOR_FetchEnumeration& Branches) const
{
	Branches = ETGOR_FetchEnumeration::Empty;
	if (HasIdentifier(Identifier.DimensionIdentifier))
	{
		const FTGOR_DimensionInstance& Instance = Dimensions[Identifier.DimensionIdentifier];
		if (IsValid(Instance.Dimension))
		{
			return Instance.Dimension->GetDimensionObject(Identifier.ActorIdentifier, Branches);
		}
	}
	return nullptr;
}

FTGOR_DimensionIdentifier UTGOR_WorldData::GetIdentifier(AActor* Actor, ETGOR_FetchEnumeration& Branches) const
{
	Branches = ETGOR_FetchEnumeration::Empty;
	FTGOR_DimensionIdentifier Identifier;
	if (IsValid(Actor))
	{
		UTGOR_IdentityComponent* Identity = Actor->FindComponentByClass<UTGOR_IdentityComponent>();
		if (IsValid(Identity))
		{
			Identifier.DimensionIdentifier = Identity->GetDimensionIdentifier(Branches);
			Identifier.ActorIdentifier = Identity->GetActorIdentifier();
		}
	}
	return Identifier;
}

UActorComponent* UTGOR_WorldData::GetIdentityComponent(TSubclassOf<UActorComponent> ComponentClass, const FTGOR_ComponentIdentifier& Identifier, ETGOR_FetchEnumeration& Branches) const
{
	UTGOR_IdentityComponent* Identity = GetIdentity(Identifier, Branches);
	if (IsValid(Identity))
	{
		AActor* Actor = Identity->GetOwner();
		TArray<UActorComponent*> Components;
		Actor->GetComponents(ComponentClass, Components);
		for (UActorComponent* Component : Components)
		{
			if (Component->GetName().Equals(Identifier.ComponentIdentifier))
			{
				Branches = ETGOR_FetchEnumeration::Found;
				return Component;
			}
		}
	}
	return nullptr;
}

FTGOR_ComponentIdentifier UTGOR_WorldData::GetComponentIdentifier(UActorComponent* Component, ETGOR_FetchEnumeration& Branches) const
{
	Branches = ETGOR_FetchEnumeration::Empty;
	FTGOR_ComponentIdentifier Identifier;
	if (IsValid(Component))
	{
		Identifier.FTGOR_DimensionIdentifier::operator=(GetIdentifier(Component->GetOwner(), Branches));
		Identifier.ComponentIdentifier = Component->GetName();
	}
	return Identifier;
}


void UTGOR_WorldData::SaveWorldToFile()
{
	const FString Timestamp = FDateTime::Now().ToString();
	for (auto& Dimension : Dimensions)
	{
		// Regard currently loaded dimensions as open.
		// IsOpen can be false also during closage which is where a dimension should be able to be saved also.
		if (IsValid(Dimension.Value.Dimension))
		{
			Dimension.Value.Path = Dimension.Key.ToString() + Timestamp;
			Dimension.Value.Dimension->SetFilename(Dimension.Value.Path);
			Dimension.Value.Dimension->SaveToFile();
		}
	}

	// Save world
	SetFilename(FString("World")); //  + Timestamp TODO: Add timestamp when we have a registry
	SaveToFile();
}


void UTGOR_WorldData::LoadWorldFromFile()
{
	// Load world
	SetFilename(FString("World"));
	LoadFromFile();

	for (auto& Dimension : Dimensions)
	{
		// Attempt loading for already open dimensions
		if (IsValid(Dimension.Value.Dimension) && !Dimension.Value.Path.IsEmpty())
		{
			Dimension.Value.Dimension->SetFilename(Dimension.Value.Path);
			Dimension.Value.Dimension->LoadFromFile();
		}
	}

	// Update realities
	UpdateControllers();
}

////////////////////////////////////////////////////////////////////////////////////////////////////

UTGOR_DimensionData* UTGOR_WorldData::GetDimensionFromLocation(const FVector& Location, ETGOR_FetchEnumeration& Branches)
{
	Branches = ETGOR_FetchEnumeration::Empty;

	for (const auto& Dimension : Dimensions)
	{
		ATGOR_PhysicsVolume* Volume = Dimension.Value.Dimension->LevelVolume;
		if (IsValid(Volume) && Volume->IsInside(Location))
		{
			Branches = ETGOR_FetchEnumeration::Found;
			return(Dimension.Value.Dimension);
		}
	}

	return(nullptr);
}

FVector UTGOR_WorldData::AllocateMiniDimension(const FVector& Bounds, ETGOR_FetchEnumeration& Branches)
{
	Branches = ETGOR_FetchEnumeration::Empty;

	FVector Result;
	if (MiniDimensions.Insert(Bounds, Result))
	{
		Branches = ETGOR_FetchEnumeration::Found;
	}
	return(Result);
}

FVector UTGOR_WorldData::AllocateRealDimension(const FVector& Bounds, ETGOR_FetchEnumeration& Branches)
{
	Branches = ETGOR_FetchEnumeration::Empty;

	FVector Result;
	if (RealDimensions.Insert(Bounds, Result))
	{
		Branches = ETGOR_FetchEnumeration::Found;
	}
	return(Result);
}

void UTGOR_WorldData::DeallocateMiniDimension(const FVector& Location, ETGOR_FetchEnumeration& Branches)
{
	Branches = ETGOR_FetchEnumeration::Empty;
	if (MiniDimensions.Remove(Location))
	{
		Branches = ETGOR_FetchEnumeration::Found;
	}
}

void UTGOR_WorldData::DeallocateRealDimension(const FVector& Location, ETGOR_FetchEnumeration& Branches)
{
	Branches = ETGOR_FetchEnumeration::Empty;
	if (RealDimensions.Remove(Location))
	{
		Branches = ETGOR_FetchEnumeration::Found;
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void UTGOR_WorldData::AssemblePersistent(UTGOR_Dimension* Dimension, bool IsServer)
{
	SINGLETON_CHK;

	// Register persistent level, only load if not already done
	PersistentIdentifier = FName(*Dimension->GetDefaultName());
	if (!HasFinishedLoading(PersistentIdentifier))
	{
		AddIdentifier(Dimension, PersistentIdentifier);

		// Initialise spatialisation
		const float MidMargin = Dimension->Bounds.Z;
		RealDimensions = CTGOR_SpatialRoot(SPATIAL_LOCATION(MidMargin), SPATIAL_SIZE(MidMargin + RealityHeight), 2);
		MiniDimensions = CTGOR_SpatialRoot(SPATIAL_LOCATION(-(MidMargin + MiniDimensionHeight)), SPATIAL_SIZE(MiniDimensionHeight), 8);

		TArray<int32> Option;
		UTGOR_DimensionData* Base = NewObject<UTGOR_DimensionData>(this);
		if (IsValid(Base))
		{
			// Assign dimension
			FTGOR_DimensionInstance& Instance = Dimensions[PersistentIdentifier];
			Instance.Dimension = Base;

			// Set dimension vars
			Base->Origin = Dimension->Offset;
			Base->Identifier = PersistentIdentifier;
			Base->LoadedLevel = PersistentWorld->PersistentLevel;
			Dimension->SetCosmeticHidden(Base, true);

			// Grab root volume
			for (AActor* Actor : Base->LoadedLevel->Actors)
			{
				if (IsValid(Actor) && Actor->IsA(ATGOR_LevelVolume::StaticClass()))
				{
					MainVolume = Cast<ATGOR_LevelVolume>(Actor);
					break;
				}
			}
			if (!IsValid(MainVolume))
			{
				ERROR("No Volume found!", Error)
			}

			// Only load on server
			if (IsServer)
			{
				Dimension->Load(Base, Instance.Path, Option);
			}
			Base->ActorsAssembled = true;


			ATGOR_DimensionGameState* GameState = Cast<ATGOR_DimensionGameState>(GetWorld()->GetGameState());
			if (IsValid(GameState) && GameState->HasAuthority())
			{
				GameState->DimensionRequestState.Dimensions.Add(PersistentIdentifier, Instance.Location);
			}

			Instance.Open = true;
			Instance.Location = FVector::ZeroVector;
			Instance.Stage = ETGOR_DimensionLoadingStage::CosmeticHidden;
		}
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////

FName UTGOR_WorldData::GetUniqueIdentifier(const FString& Identifier)
{
	int32 Count = 0;
	FName Name = FName(*Identifier);

	while (Dimensions.Contains(Name))
	{
		// Create indexed name
		Name = FName(*(Identifier + "_" + FString::FromInt(++Count)));
	}
	return(Name);
}

bool UTGOR_WorldData::AddIdentifier(TSubclassOf<UTGOR_Dimension> Class, const FName& Identifier)
{
	SINGLETON_RETCHK(false);
	UTGOR_ContentManager* ContentManager = Singleton->GetContentManager();
	UTGOR_Dimension* Content = ContentManager->GetTFromType<UTGOR_Dimension>(Class);
	if (!IsValid(Content))
	{
		return(false);
	}

	return(AddIdentifier(Content, Identifier));
}

bool UTGOR_WorldData::AddIdentifier(UTGOR_Dimension* Content, const FName& Identifier)
{
	if (Dimensions.Contains(Identifier))
	{
		return(false);
	}

	// Create new dimension slot
	FTGOR_DimensionInstance NewInstance;
	NewInstance.Content = Content;
	NewInstance.Dimension = nullptr;

	// Register dimension
	Dimensions.Add(Identifier, NewInstance);

	return(true);
}


FName UTGOR_WorldData::EnsureIdentifier(TSubclassOf<UTGOR_Dimension> Class, const FString& Suffix, ETGOR_PushEnumeration& Branches)
{
	Branches = ETGOR_PushEnumeration::Failed;

	// Find first match
	for (auto& Pair : Dimensions)
	{
		UTGOR_Dimension* Dimension = Pair.Value.Content;
		if (IsValid(Dimension) && Dimension->IsA(Class))
		{
			// Check for suffix
			if (Suffix.IsEmpty() || Pair.Key.GetPlainNameString().Contains(Suffix))
			{
				Branches = ETGOR_PushEnumeration::Found;
				return(Pair.Key);
			}
		}
	}

	// Get resourceable
	SINGLETON_RETCHK(FName());
	UTGOR_ContentManager* ContentManager = Singleton->GetContentManager();
	UTGOR_Dimension* Content = ContentManager->GetTFromType<UTGOR_Dimension>(Class);
	if (!IsValid(Content))
	{
		return(FName());
	}

	// Get and register new identifier
	FName Identifier = GetUniqueIdentifier(Content->GetDefaultName() + Suffix);
	if (AddIdentifier(Content, Identifier))
	{
		Branches = ETGOR_PushEnumeration::Created;
	}
	return(Identifier);
}

bool UTGOR_WorldData::SetIdentifier(UTGOR_Dimension* Content, const FName& Identifier)
{
	if (HasIdentifier(Identifier))
	{
		FTGOR_DimensionInstance& Instance = Dimensions[Identifier];
		if (Content == nullptr || Instance.Content != Content)
		{
			// Don't replace an already loaded dimension
			if (HasBeenSpawned(Identifier))
			{
				return(false);
			}
			Instance.Content = Content;
			Instance.Dimension = nullptr;
		}
		return(true);
	}
	else
	{
		return(AddIdentifier(Content, Identifier));
	}
}

bool UTGOR_WorldData::HasIdentifier(const FName& Identifier) const
{
	return(Dimensions.Contains(Identifier));
}

bool UTGOR_WorldData::IsReality(const FName& Identifier) const
{
	if (HasIdentifier(Identifier))
	{
		return Dimensions[Identifier].Content->Reality;
	}
	return false;
}

void UTGOR_WorldData::SetOwner(const FName& Identifier, const FTGOR_DimensionIdentifier& Owner)
{
	if (HasIdentifier(Identifier))
	{
		Dimensions[Identifier].Owner = Owner;
	}
}

FTGOR_DimensionIdentifier UTGOR_WorldData::GetOwner(const FName& Identifier) const
{
	if (HasIdentifier(Identifier))
	{
		return Dimensions[Identifier].Owner;
	}
	return FTGOR_DimensionIdentifier();
}

////////////////////////////////////////////////////////////////////////////////////////////////////

FName UTGOR_WorldData::GetPersistentIdentifier() const
{
	return PersistentIdentifier;
}

FName UTGOR_WorldData::GetRealityIdentifier() const
{
	TArray<FName> Identifiers;
	Dimensions.GetKeys(Identifiers);
	for (const FName Identifier : Identifiers)
	{
		if (HasFinishedLoading(Identifier) && IsReality(Identifier) && IsOpen(Identifier))
		{
			return Identifier;
		}
	}
	return GetPersistentIdentifier();
}

UTGOR_DimensionData* UTGOR_WorldData::GetDimension(const FName& Identifier, ETGOR_FetchEnumeration& Branches)
{
	Branches = ETGOR_FetchEnumeration::Empty;
	FTGOR_DimensionInstance* Dimension = Dimensions.Find(Identifier);
	if (Dimension && IsValid(Dimension->Dimension))
	{
		Branches = ETGOR_FetchEnumeration::Found;
		return(Dimension->Dimension);
	}
	return(nullptr);
}

UTGOR_Dimension* UTGOR_WorldData::GetDimensionContent(const FName& Identifier) const
{
	const FTGOR_DimensionInstance* Dimension = Dimensions.Find(Identifier);
	if (Dimension && IsValid(Dimension->Content))
	{
		return(Dimension->Content);
	}
	return(nullptr);
}

FVector UTGOR_WorldData::GetDimensionLocation(const FName& Identifier) const
{
	const FTGOR_DimensionInstance* Dimension = Dimensions.Find(Identifier);
	if (Dimension && IsValid(Dimension->Dimension))
	{
		return(Dimension->Location);
	}
	return(FVector::ZeroVector);
}


bool UTGOR_WorldData::IsOpen(const FName& Identifier) const
{
	const FTGOR_DimensionInstance* Dimension = Dimensions.Find(Identifier);
	if (Dimension && IsValid(Dimension->Dimension))
	{
		return(Dimension->Open);
	}
	return(false);
}

bool UTGOR_WorldData::HasBeenSpawned(const FName& Identifier) const
{
	const FTGOR_DimensionInstance* Dimension = Dimensions.Find(Identifier);
	if (Dimension && IsValid(Dimension->Dimension))
	{
		return(Dimension->Stage != ETGOR_DimensionLoadingStage::Unloaded);
	}
	return(false);
}

bool UTGOR_WorldData::HasFinishedLoading(const FName& Identifier) const
{
	const FTGOR_DimensionInstance* Dimension = Dimensions.Find(Identifier);
	if (Dimension && IsValid(Dimension->Dimension))
	{
		return(Dimension->Stage > ETGOR_DimensionLoadingStage::Loaded);
	}
	return(false);
}

bool UTGOR_WorldData::HasFinishedUnLoading(const FName& Identifier) const
{
	const FTGOR_DimensionInstance* Dimension = Dimensions.Find(Identifier);
	if (Dimension && IsValid(Dimension->Dimension))
	{
		return(Dimension->Stage == ETGOR_DimensionLoadingStage::Unloaded);
	}
	return(true);
}

bool UTGOR_WorldData::IsUnloading(const FName& Identifier) const
{
	const FTGOR_DimensionInstance* Dimension = Dimensions.Find(Identifier);
	if (Dimension && IsValid(Dimension->Dimension))
	{
		return(Dimension->Stage == ETGOR_DimensionLoadingStage::Cleanup || Dimension->Stage == ETGOR_DimensionLoadingStage::UnloadingClients);
	}
	return(true);
}

bool UTGOR_WorldData::ProgressLoadDimension(const FName& Identifier, bool IsServer)
{
	if (!HasIdentifier(Identifier))
	{
		return false;
	}

	FTGOR_DimensionInstance& Dimension = Dimensions[Identifier];
	if (!IsValid(Dimension.Content))
	{
		ERRET("Dimension has no content", Error, false)
	}

	// Only the last step of unloading actually destroys anything so we can skip it
	if (Dimension.Stage < ETGOR_DimensionLoadingStage::Unloaded)
	{
		Dimension.Stage = ETGOR_DimensionLoadingStage::Unloaded;
	}

	UWorld* World = GetWorld();
	if (!IsValid(World))
	{
		return false;
	}

	ATGOR_DimensionGameState* GameState = Cast<ATGOR_DimensionGameState>(World->GetGameState());
	if (!IsValid(GameState))
	{
		ERRET("GameState not available", Error, false)
	}

	switch (Dimension.Stage)
	{
		case ETGOR_DimensionLoadingStage::Unloaded:
		{
			// Make sure dimension is loaded on server and grab location
			if (!IsServer)
			{
				FVector* Ptr = GameState->DimensionRequestState.Dimensions.Find(Identifier);
				if (!Ptr)
				{
					break;
				}
				Dimension.Location = *Ptr;
			}

			// Make absolutely sure this dimension is closed during loading process
			Dimension.Open = false;

			// Get location for dimension
			if (IsServer)
			{
				if (!Dimension.Content->AllocateSpace(Dimension.Location))
				{
					break;
				}
			}

			// Spawn dimension
			Dimension.Dimension = Dimension.Content->Create(Identifier.ToString(), Dimension.Location);
			if (Dimension.Dimension)
			{
				Dimension.Dimension->Identifier = Identifier;
				Dimension.Stage = ETGOR_DimensionLoadingStage::Spawned;
			}
			else
			{
				break;
			}
		}
		case ETGOR_DimensionLoadingStage::Spawned:
		{
			// Wait until level is loaded
			ULevelStreaming* Streaming = Dimension.Dimension->LevelStreaming;
			if (IsValid(Streaming) && Streaming->IsLevelLoaded())
			{
				// Streaming levels are set invisible to start with so there is no flickering from effect actors
				Streaming->SetShouldBeVisible(true);
				Dimension.Content->SetCosmeticHidden(Dimension.Dimension, true);
				Dimension.Stage = ETGOR_DimensionLoadingStage::Loaded;
			}
			else
			{
				break;
			}
		}
		case ETGOR_DimensionLoadingStage::Loaded:
		{
			// Wait until level is visible
			ULevelStreaming* Streaming = Dimension.Dimension->LevelStreaming;
			if (IsValid(Streaming) && Streaming->IsLevelVisible())
			{
				// Link level to dimension
				Dimension.Dimension->LoadedLevel = Streaming->GetLoadedLevel();

				// BSPs don't teleport with the level automatically
				for (UModelComponent* ModelComponent : Dimension.Dimension->LoadedLevel->ModelComponents)
				{
					ModelComponent->AddWorldOffset(Dimension.Dimension->Origin);
				}

				if (IsServer)
				{
					// Load dimension
					TArray<int32> Options;
					Dimension.Content->Load(Dimension.Dimension, Dimension.Path, Options);

					// Update realities
					UpdateControllers();
				}
				else
				{
					Dimension.Dimension->ActorsAssembled = true;
				}

				// Save on server list
				if (GameState->HasAuthority())
				{
					GameState->DimensionRequestState.Dimensions.Add(Identifier, Dimension.Location);
				}

				// Open dimension
				Dimension.Open = true;
				Dimension.Stage = ETGOR_DimensionLoadingStage::CosmeticHidden;
				return true;
			}
			else
			{
				break;
			}
		}
		default: break;
	}
	return false;
}


bool UTGOR_WorldData::SetDimensionCosmeticsHidden(const FName& Identifier, bool Hidden)
{
	if (HasIdentifier(Identifier))
	{
		FTGOR_DimensionInstance& Dimension = Dimensions[Identifier];

		// Check for dimension not being loaded yet
		if (Dimension.Stage == ETGOR_DimensionLoadingStage::CosmeticHidden && !Hidden)
		{
			Dimension.Content->SetCosmeticHidden(Dimension.Dimension, false);
			Dimension.Stage = ETGOR_DimensionLoadingStage::CosmeticShown;
		}
		else if (Dimension.Stage == ETGOR_DimensionLoadingStage::CosmeticShown && Hidden)
		{
			Dimension.Content->SetCosmeticHidden(Dimension.Dimension, true);
			Dimension.Stage = ETGOR_DimensionLoadingStage::CosmeticHidden;
		}

		if ((Dimension.Stage == ETGOR_DimensionLoadingStage::CosmeticHidden && Hidden) ||
			(Dimension.Stage == ETGOR_DimensionLoadingStage::CosmeticShown && !Hidden))
		{
			return true;
		}
	}

	// Invalid dimensions count as hidden
	return Hidden;
}

void UTGOR_WorldData::CloseDimension(const FName& Identifier, ETGOR_ComputeEnumeration& Branches)
{
	Branches = ETGOR_ComputeEnumeration::Failed;
	if (HasIdentifier(Identifier))
	{
		// Check that dimension isn't loaded yet
		FTGOR_DimensionInstance& Dimension = Dimensions[Identifier];
		
		Dimension.Open = false;
		Branches = ETGOR_ComputeEnumeration::Success;
	}
}

bool UTGOR_WorldData::ProgressUnloadDimension(const FName& Identifier, bool IsServer)
{
	SINGLETON_RETCHK(false);
	if (!HasIdentifier(Identifier))
	{
		return(false);
	}

	FTGOR_DimensionInstance& Dimension = Dimensions[Identifier];
	if (!IsValid(Dimension.Content))
	{
		ERRET("Dimension has no content", Error, false)
	}

	// Start unloading if dimension is loaded or loading
	if (Dimension.Stage > ETGOR_DimensionLoadingStage::Unloaded)
	{
		// Unloading can reverse any step done by loading no matter the state
		Dimension.Stage = ETGOR_DimensionLoadingStage::Cleanup;
	}

	ATGOR_DimensionGameState* GameState = Cast<ATGOR_DimensionGameState>(GetWorld()->GetGameState());
	if (!IsValid(GameState))
	{
		ERRET("GameState not available", Error, false)
	}

	switch (Dimension.Stage)
	{
		case ETGOR_DimensionLoadingStage::Cleanup:
		{
			if (IsServer)
			{
				// Make sure noone enters this dimension anymore
				Dimension.Open = false;

				// Unload from all clients and server
				for (ATGOR_DimensionController* Controller : Controllers)
				{
					UTGOR_WorldComponent* WorldComponent = Controller->GetWorldManager();
					WorldComponent->RemoveDimensionRequest(Identifier);
				}

				UTGOR_WorldComponent* WorldComponent = GameState->GetWorldManager();
				WorldComponent->RemoveDimensionRequest(Identifier);
			}

			Dimension.Stage = ETGOR_DimensionLoadingStage::UnloadingClients;
		}
		case ETGOR_DimensionLoadingStage::UnloadingClients:
		{
			if (IsServer)
			{
				const FTGOR_Time GameTimestamp = Singleton->GetGameTimestamp();
				bool HasClientsUnloading = false;

				// Check unloading state of all clients but the host before unloading host
				for (ATGOR_DimensionController* Controller : Controllers)
				{
					if (!Controller->IsLocalController())
					{
						// Check whether removal is done on all clients or timed out
						UTGOR_WorldComponent* WorldComponent = Controller->GetWorldManager();
						const FTGOR_Time* Timestamp = WorldComponent->PendingDeletions.Find(Identifier);
						if (Timestamp && (*Timestamp) + GameState->LoadTimeout > GameTimestamp)
						{
							HasClientsUnloading = true;
						}
					}
				}

				if (HasClientsUnloading)
				{
					break;
				}
			}

			// Only unload what is already loaded
			if (IsValid(Dimension.Dimension) && Dimension.Dimension->LevelStreaming)
			{
				// Deallocate space from database
				Dimension.Content->DeallocateSpace(Dimension.Location);

				// Reset all
				Dimension.Dimension->LoadedLevel = nullptr;
				Dimension.Dimension->LevelStreaming->SetShouldBeLoaded(false);
				Dimension.Dimension = nullptr;

				// Remove volatile dimensions from the list alltogether
				if (Dimension.Content->Volatile)
				{
					Dimensions.Remove(Identifier);
				}

				// Remove from server list
				if (IsServer)
				{
					GameState->DimensionRequestState.Dimensions.Remove(Identifier);
				}

				// Forceupdate all levels
				PersistentWorld->UpdateLevelStreaming();
			}

			Dimension.Stage = ETGOR_DimensionLoadingStage::Unloaded;
			return(true);
		}
		default: break;
	}
	return false;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void UTGOR_WorldData::UnloadAllRealities()
{
	SaveWorldToFile();

	for (auto Iterator = Dimensions.CreateIterator(); Iterator; ++Iterator)
	{
		FTGOR_DimensionInstance& Instance = Iterator->Value;

		// Get all dimensions that are realities and loaded
		UTGOR_DimensionData*& Base = Instance.Dimension;
		if (IsValid(Base) && Base->LevelStreaming && Base->LevelStreaming->IsLevelLoaded() && Instance.Content->Reality)
		{
			// Reset all
			Base->LoadedLevel = nullptr;
			Base->LevelStreaming->SetShouldBeLoaded(false);
			Base = nullptr;

			// Remove volatile dimensions from the list alltogether
			if (Instance.Content->Volatile)
			{
				Iterator.RemoveCurrent();
			}
		}
	}
	
	PersistentWorld->UpdateLevelStreaming();
}

void UTGOR_WorldData::UpdateControllers()
{
	for (ATGOR_DimensionController* Controller : Controllers)
	{
		Controller->OnWorldUpdate();
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////

FName UTGOR_WorldData::GetActiveIdentifier() const
{
	return ActiveIdentifier; 
}

ATGOR_PhysicsVolume* UTGOR_WorldData::GetMainVolume() const
{
	return MainVolume; 
}

void UTGOR_WorldData::SwitchActiveDimension(const FName& Identifier)
{
	// Try hiding previous, try hiding new, affirm last
	if (SetDimensionCosmeticsHidden(ActiveIdentifier, true))
	{
		if (SetDimensionCosmeticsHidden(Identifier, false))
		{
			ActiveIdentifier = Identifier;
		}
	}
}

void UTGOR_WorldData::GetLoadedDimensions(TArray<FName>& Identifiers) const
{
	Dimensions.GenerateKeyArray(Identifiers);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void UTGOR_WorldData::LoginController(ATGOR_DimensionController* Controller)
{
	Controllers.Add(Controller);
}

void UTGOR_WorldData::LogoutController(ATGOR_DimensionController* Controller)
{
	Controllers.Remove(Controller);
}

bool UTGOR_WorldData::IsOnline(const ATGOR_DimensionController* Controller) const
{
	return Controllers.Contains(Controller);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

int32 UTGOR_WorldData::GetRandomTrackedIdentifier() const
{
	return FMath::RandRange(0, 0xFFFF);
}

int32 UTGOR_WorldData::GetUniqueTrackedIdentifier() const
{
	// Create new identifier from random numbers
	int32 TrackedIdentifier = INDEX_NONE;
	do
	{
		TrackedIdentifier = GetRandomTrackedIdentifier();
	} while (TrackedActors.Contains(TrackedIdentifier));
	return TrackedIdentifier;
}

int32 UTGOR_WorldData::RegisterTracked(UTGOR_IdentityComponent* Component, bool Create /*=false*/)
{
	ETGOR_FetchEnumeration State;
	const FTGOR_DimensionIdentifier DimensionIdentifier = GetIdentifier(Component->GetOwner(), State);
	if (State == ETGOR_FetchEnumeration::Found)
	{
		const int32 Identifier = Component->GetWorldIdentifier();
		if (FTGOR_DimensionIdentifier* Ptr = TrackedActors.Find(Identifier))
		{
			*Ptr = DimensionIdentifier;
			return Identifier;
		}
		else if (Create)
		{
			const int32 NewIdentifier = GetUniqueTrackedIdentifier();
			TrackedActors.Emplace(NewIdentifier, DimensionIdentifier);
			return NewIdentifier;
		}
	}
	return INDEX_NONE;
}

int32 UTGOR_WorldData::UnregisterTracked(int32 Identifier)
{
	if (TrackedActors.Contains(Identifier))
	{
		TrackedActors.Remove(Identifier);
		return Identifier;
	}
	return INDEX_NONE;
}

void UTGOR_WorldData::PossessTrackedWith(int32 Identifier, ATGOR_DimensionController* Controller)
{
	if (IsValid(Controller))
	{
		if (const FTGOR_DimensionIdentifier* Ptr = TrackedActors.Find(Identifier))
		{
			Controller->SetPossessionQueue(*Ptr);
		}
	}
}

UTGOR_IdentityComponent* UTGOR_WorldData::GetTracked(int32 Identifier, ETGOR_FetchEnumeration& Branches) const
{
	Branches = ETGOR_FetchEnumeration::Empty;
	if (const FTGOR_DimensionIdentifier* Ptr = TrackedActors.Find(Identifier))
	{
		return GetIdentity(*Ptr, Branches);
	}
	return nullptr;
}