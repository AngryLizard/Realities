// The Gateway of Realities: Planes of Existence.


#include "TGOR_DimensionData.h"

#include "Engine/World.h"
#include "CoreSystem/TGOR_Singleton.h"
#include "CoreSystem/Gameplay/TGOR_GameInstance.h"

#include "DimensionSystem/Regions/TGOR_MainRegionActor.h"
#include "DimensionSystem/Interfaces/TGOR_DimensionInterface.h"
#include "DimensionSystem/Components/TGOR_ConnectionComponent.h"
#include "DimensionSystem/Components/TGOR_IdentityComponent.h"
#include "DimensionSystem/Components/TGOR_DimensionComponent.h"
#include "DimensionSystem/Data/TGOR_WorldData.h"
#include "DimensionSystem/Actors/TGOR_GateActor.h"

#include "CoreSystem/Storage/TGOR_Package.h"

#include "DimensionSystem/Content/TGOR_Dimension.h"
#include "DimensionSystem/Content/TGOR_Spawner.h"

UTGOR_DimensionData::UTGOR_DimensionData()
{
	ActorsAssembled = false;

	MainRegion = nullptr;
	LevelStreaming = nullptr;
	LoadedLevel = nullptr;
	Origin = FVector::ZeroVector;

	Folder = "Dimensions";
}


UWorld* UTGOR_DimensionData::GetWorld() const
{
	if (IsValid(LoadedLevel))
	{
		return LoadedLevel->GetWorld();
	}
	return nullptr;
}

void UTGOR_DimensionData::Write_Implementation(FTGOR_GroupWritePackage& Package, UTGOR_Singleton* Context) const
{
	// Make copy of register so we can filter and adapt spawns
	TMap<int32, FTGOR_SpawnInstance> Cache;
	for (const auto& Pair : DimensionObjects)
	{
		FTGOR_SpawnInstance Spawn;
		if (IsValid(Pair.Value.Identity) && !Pair.Value.Identity->IgnoreStorage)
		{
			TScriptInterface<ITGOR_SaveInterface> Interface = Pair.Value.Identity;
			Package.WriteEntry(FString::FromInt(Pair.Key), Interface);

			// Create cache
			AActor* Actor = Pair.Value.Identity->GetOwner();
			Spawn.SpawnLocation = Actor->GetActorLocation() - Origin;
			Spawn.SpawnRotation = Actor->GetActorRotation();
			Spawn.Identity = Pair.Value.Identity;
			Spawn.SpawnActor = Pair.Value.SpawnActor;

			Cache.Add(Pair.Key, Spawn);
		}
	}

	Package.WriteEntry("DimensionObjects", Cache);
	Package.WriteEntry("DeletedObjects", DeletedObjects);
}

bool UTGOR_DimensionData::Read_Implementation(FTGOR_GroupReadPackage& Package, UTGOR_Singleton* Context)
{
	// Despawn all despawned map actors
	Package.ReadEntry("DeletedObjects", DeletedObjects);
	for (int32 ActorIdentifier : DeletedObjects)
	{
		FTGOR_SpawnInstance* Current = DimensionObjects.Find(ActorIdentifier);
		if (Current && IsValid(Current->Identity) && !IsValid(Current->SpawnActor))
		{
			UTGOR_GameInstance::DespawnActor(Current->Identity->GetOwner());
		}
	}
	
	// Spawn all spawned actors, move all map actors
	Package.ReadEntry("DimensionObjects", DimensionObjects);
	UTGOR_WorldData* WorldData = Singleton->GetData<UTGOR_WorldData>();
	if (IsValid(WorldData))
	{
		UTGOR_Dimension* Dimension = WorldData->GetDimensionContent(Identifier);
		for (auto& Pair : DimensionObjects)
		{
			if (IsValid(Pair.Value.SpawnActor))
			{
				// Spawn actor
				Pair.Value.Identity = SpawnIdentity(Pair.Value.SpawnActor, Origin + Pair.Value.SpawnLocation, Pair.Value.SpawnRotation);
				if (IsValid(Pair.Value.Identity))
				{
					Pair.Value.Identity->DimensionIdentity.Identifier = Pair.Key;
					Pair.Value.Identity->SetActorSpawner(Pair.Value.SpawnActor);
					Pair.Value.Identity->DimensionIdentity.Dimension = Dimension;
					Dimension->SetCullDistance(Pair.Value.Identity->GetOwner());

					Pair.Value.Identity->PreAssemble(this);
					Pair.Value.Identity->Assemble(this);
					Pair.Value.Identity->PostAssemble(this);
				}
			}
			else
			{
				// Find map actor
				ETGOR_FetchEnumeration State;
				Pair.Value.Identity = GetDimensionObject(Pair.Key, State);
				if (State == ETGOR_FetchEnumeration::Found)
				{
					AActor* Actor = Pair.Value.Identity->GetOwner();
					if (IsValid(Actor) && Actor->GetRootComponent()->Mobility == EComponentMobility::Movable)
					{
						Actor->SetActorLocation(Origin + Pair.Value.SpawnLocation);
						Actor->SetActorRotation(Pair.Value.SpawnRotation);
					}
				}
			}
		}

		// Load objects after all have spawned so references can be loaded
		for (auto& Pair : DimensionObjects)
		{
			// Read identity
			if (IsValid(Pair.Value.Identity))
			{
				TScriptInterface<ITGOR_SaveInterface> Interface = Pair.Value.Identity;
				Package.ReadEntry(FString::FromInt(Pair.Key), Interface);
			}
		}
		return true;
	}
	return false;
}


UTGOR_IdentityComponent* UTGOR_DimensionData::SpawnIdentity(UTGOR_Spawner* Spawner, const FVector& Location, const FRotator& Rotation)
{
	// Create new actor with said identifier
	AActor* Actor = UTGOR_GameInstance::SpawnActorForce<AActor>(Spawner->GetSpawnClass(), this, Location, Rotation);
	if (IsValid(Actor))
	{
		FTGOR_SpawnInstance Spawn;
		UTGOR_IdentityComponent* Identity = Cast<UTGOR_IdentityComponent>(Actor->GetComponentByClass(UTGOR_IdentityComponent::StaticClass()));
		if (IsValid(Identity))
		{
			return Identity;
		}
	}
	return nullptr;
}


bool UTGOR_DimensionData::IsDimensionAssembled() const
{
	return DimensionAssembled;
}

bool UTGOR_DimensionData::IsActorsAssembled() const
{
	return ActorsAssembled;
}



bool UTGOR_DimensionData::Construct(Voronoi::SearchActor& Actor)
{
	REGIONCONTROLLER_RETCHK(false)

	// Use Controller cell for construction
	Actor.reset(&MainRegion->GetCell());
	return(true);
}

void UTGOR_DimensionData::AssembleDimension()
{
	SINGLETON_CHK;
	UTGOR_WorldData* WorldData = Singleton->GetData<UTGOR_WorldData>();
	if (IsValid(WorldData))
	{
		UTGOR_Dimension* Dimension = WorldData->GetDimensionContent(Identifier);
		if (IsValid(Dimension))
		{
			// Find and set up Dimensionactors and DimensionComponents in the level
			for (AActor* Actor : LoadedLevel->Actors)
			{
				if (IsValid(Actor))
				{
					TArray<UTGOR_IdentityComponent*> Components;
					Actor->GetComponents(Components);
					for (UTGOR_IdentityComponent* Component : Components)
					{
						// Avoid collisions
						int32 ActorIdentifier = Component->GetActorIdentifier();
						if (DimensionObjects.Contains(ActorIdentifier) || ActorIdentifier == -1)
						{
							ActorIdentifier = GetUniqueActorIdentifier();
						}

						// Add spawn info
						FTGOR_SpawnInstance Spawn;
						Spawn.Identity = Component;
						DimensionObjects.Add(ActorIdentifier, Spawn);
						Component->DimensionIdentity.Identifier = ActorIdentifier;
						Component->DimensionIdentity.Dimension = Dimension;
						Component->PreAssemble(this);
						Spawn.SpawnActor = Component->DimensionIdentity.Spawner;
					}

					Dimension->SetCullDistance(Actor);
				}
			}

			DimensionAssembled = true;
		}
	}
}

void UTGOR_DimensionData::AssembleActors()
{
	SINGLETON_CHK;
	REGIONCONTROLLER_CHK;
	
	// Assemble objects in stages
	for (auto Itr = DimensionObjects.CreateIterator(); Itr; ++Itr)
	{
		if (IsValid(Itr->Value.Identity))
		{
			Itr->Value.Identity->Assemble(this);
		}
	}

	// Add to owning reality if this doesn't have a gate
	UTGOR_WorldData* WorldData = Singleton->GetData<UTGOR_WorldData>();
	if (IsValid(WorldData))
	{
		if (!IsValid(MainGate) && IsValid(WorldData))
		{
			ETGOR_FetchEnumeration State;
			const FName RealityIdentifier = WorldData->GetRealityIdentifier();
			UTGOR_DimensionData* Reality = WorldData->GetDimension(RealityIdentifier, State);
			if (State == ETGOR_FetchEnumeration::Found)
			{
				MainGate = Reality->GetMainGate();
				Reality->Children.Emplace(Identifier);
			}
		}

		for (auto Itr = DimensionObjects.CreateIterator(); Itr; ++Itr)
		{
			if (IsValid(Itr->Value.Identity))
			{
				Itr->Value.Identity->PostAssemble(this);
			}
		}

		ActorsAssembled = true;
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////


FVector UTGOR_DimensionData::GetDimensionOrigin() const
{
	return Origin;
}

int32 UTGOR_DimensionData::GetRandomActorIdentifier()
{
	return FMath::RandRange(0, 0xFFFF);
}

int32 UTGOR_DimensionData::GetUniqueActorIdentifier() const
{
	// Create new identifier from random numbers
	int32 ActorIdentifier = -1;
	do
	{
		ActorIdentifier = GetRandomActorIdentifier();
	} while (DimensionObjects.Contains(ActorIdentifier));
	return ActorIdentifier;
}

UTGOR_IdentityComponent* UTGOR_DimensionData::GetDimensionObject(int32 ActorIdentifier, ETGOR_FetchEnumeration& Branches) const
{
	Branches = ETGOR_FetchEnumeration::Empty;
	FTGOR_SpawnInstance const* Ptr = DimensionObjects.Find(ActorIdentifier);
	if (Ptr && IsValid(Ptr->Identity))
	{
		Branches = ETGOR_FetchEnumeration::Found;
		return Ptr->Identity;
	}
	return nullptr;
}

UTGOR_IdentityComponent* UTGOR_DimensionData::AddDimensionObject(int32 ActorIdentifier, UTGOR_Spawner* Spawner, const FVector& Location, const FRotator& Rotation, ETGOR_FetchEnumeration& Branches)
{
	Branches = ETGOR_FetchEnumeration::Empty;
	SINGLETON_RETCHK(nullptr);
	UTGOR_WorldData* WorldData = Singleton->GetData<UTGOR_WorldData>();
	if (IsValid(WorldData))
	{
		UTGOR_Dimension* Dimension = WorldData->GetDimensionContent(Identifier);

		if (IsValid(Spawner) && IsValid(WorldData))
		{
			// Make new identifier if already exists
			UTGOR_IdentityComponent* IdentityComponent = GetDimensionObject(ActorIdentifier, Branches);
			if (Branches == ETGOR_FetchEnumeration::Found || ActorIdentifier == INDEX_NONE)
			{
				ActorIdentifier = GetUniqueActorIdentifier();
			}

			FTGOR_SpawnInstance Spawn;
			Spawn.SpawnActor = Spawner;
			Spawn.Identity = SpawnIdentity(Spawner, Location, Rotation);
			if (IsValid(Spawn.Identity))
			{
				Spawn.Identity->DimensionIdentity.Identifier = ActorIdentifier;
				Spawn.Identity->DimensionIdentity.Dimension = Dimension;
				Spawn.Identity->SetActorSpawner(Spawner);
				DimensionObjects.Add(ActorIdentifier, Spawn);

				Dimension->SetCullDistance(Spawn.Identity->GetOwner());

				if (IsDimensionAssembled())
				{
					Spawn.Identity->PreAssemble(this);
				}

				if (IsActorsAssembled())
				{
					Spawn.SpawnActor = Spawn.Identity->GetActorSpawner();
					Spawn.Identity->Assemble(this);
					Spawn.Identity->PostAssemble(this);
				}

				// Let other worlds know
				WorldData->UpdateControllers();

				Branches = ETGOR_FetchEnumeration::Found;
				return Spawn.Identity;
			}
			else
			{
				ERRET("Spawner actor doesn't have identity component", Error, nullptr);
			}
		}
		else
		{
			ERRET("No Spawner defined", Error, nullptr);
		}
	}

	Branches = ETGOR_FetchEnumeration::Empty;
	return nullptr;
}

void UTGOR_DimensionData::RegisterDimensionObject(UTGOR_IdentityComponent* DimensionObject)
{
	SINGLETON_CHK;
	UTGOR_WorldData* WorldData = Singleton->GetData<UTGOR_WorldData>();
	if (IsValid(WorldData))
	{
		UTGOR_Dimension* Dimension = WorldData->GetDimensionContent(Identifier);

		if (IsValid(DimensionObject) && IsValid(WorldData))
		{
			// Make new identifier if already exists
			ETGOR_FetchEnumeration State;
			int32 ActorIdentifier = DimensionObject->GetActorIdentifier();
			UTGOR_IdentityComponent* IdentityComponent = GetDimensionObject(ActorIdentifier, State);
			if (State == ETGOR_FetchEnumeration::Found || ActorIdentifier == -1)
			{
				// Don't do anything if already added
				if (IdentityComponent == DimensionObject)
				{
					return;
				}

				ActorIdentifier = GetUniqueActorIdentifier();
			}

			FTGOR_SpawnInstance Spawn;
			Spawn.SpawnActor = DimensionObject->GetActorSpawner();
			Spawn.Identity = DimensionObject;

			Spawn.Identity->DimensionIdentity.Identifier = ActorIdentifier;
			Spawn.Identity->DimensionIdentity.Dimension = WorldData->GetDimensionContent(Identifier);
			// Reset spawn so we reassemble everything properly
			Spawn.Identity->SetActorSpawner(Spawn.SpawnActor, true);

			DimensionObjects.Add(ActorIdentifier, Spawn);
			Dimension->SetCullDistance(Spawn.Identity->GetOwner());

			if (IsDimensionAssembled())
			{
				Spawn.Identity->PreAssemble(this);
			}

			if (IsActorsAssembled())
			{
				Spawn.Identity->Assemble(this);
				Spawn.Identity->PostAssemble(this);
			}
		}
	}
}

void UTGOR_DimensionData::SwapDimensionObject(int32 ActorIdentifier, UTGOR_DimensionData* Other)
{
	SINGLETON_CHK;
	UTGOR_WorldData* WorldData = Singleton->GetData<UTGOR_WorldData>();
	if (IsValid(WorldData))
	{
		UTGOR_Dimension* Dimension = WorldData->GetDimensionContent(Identifier);
		FTGOR_SpawnInstance* Current = DimensionObjects.Find(ActorIdentifier);
		if (Current && IsValid(Current->Identity) && IsValid(WorldData))
		{
			DimensionObjects.Remove(ActorIdentifier);

			// Create new identifier if already exists
			if (Other->DimensionObjects.Contains(ActorIdentifier) || ActorIdentifier == -1)
			{
				ActorIdentifier = Other->GetUniqueActorIdentifier();
			}

			//Assign new identifier
			Current->Identity->DimensionIdentity.Identifier = ActorIdentifier;
			Current->Identity->DimensionIdentity.Dimension = WorldData->GetDimensionContent(Other->Identifier);
			Dimension->SetCullDistance(Current->Identity->GetOwner());
			//Current->Identity->SetActorSpawner(...); Does not change on swap

			// Copy to other dimension and assemble
			Other->DimensionObjects.Add(ActorIdentifier, *Current);
			Current->Identity->PreAssemble(Other);
			Current->Identity->Assemble(Other);
			Current->Identity->PostAssemble(Other);
		}

		// Let other worlds know
		WorldData->UpdateControllers();
	}
}

void UTGOR_DimensionData::RemoveDimensionObject(int32 ActorIdentifier)
{
	SINGLETON_CHK;

	// Remember deleted map actors
	FTGOR_SpawnInstance* Current = DimensionObjects.Find(ActorIdentifier);
	if (Current && IsValid(Current->Identity) && !IsValid(Current->SpawnActor) && !Current->Identity->IgnoreStorage)
	{
		DeletedObjects.Emplace(ActorIdentifier);
	}

	// Remove from list
	DimensionObjects.Remove(ActorIdentifier);

	// Let other worlds know
	UTGOR_WorldData* WorldData = Singleton->GetData<UTGOR_WorldData>();
	if (IsValid(WorldData))
	{
		WorldData->UpdateControllers();
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////


ATGOR_LevelVolume* UTGOR_DimensionData::GetLevelVolume() const
{
	return LevelVolume; 
}

ATGOR_GateActor* UTGOR_DimensionData::GetMainGate() const
{
	return MainGate;
}

ATGOR_MainRegionActor* UTGOR_DimensionData::GetMainRegion() const 
{ 
	return MainRegion; 
}

FName UTGOR_DimensionData::GetIdentifier() const 
{ 
	return Identifier; 
}


void UTGOR_DimensionData::AddConnection(const FName& ConnectionName, UTGOR_ConnectionComponent* ConnectionComponent)
{
	Connections.Add(ConnectionName, ConnectionComponent);
}

UTGOR_ConnectionComponent* UTGOR_DimensionData::GetConnection(const FName& ConnectionName) const
{
	UTGOR_ConnectionComponent* const * Connection = Connections.Find(ConnectionName);
	if (Connection)
	{
		return(*Connection);
	}
	return(nullptr);
}

UTGOR_ConnectionComponent* UTGOR_DimensionData::GetConnectionOfType(const FName& ConnectionName, TSubclassOf<UTGOR_ConnectionComponent> Class) const
{
	UTGOR_ConnectionComponent* Connection = GetConnection(ConnectionName);
	if (IsValid(Connection) && Connection->IsA(Class))
	{
		return Connection;
	}
	return nullptr;
}

TArray<UTGOR_ConnectionComponent*> UTGOR_DimensionData::GetConnectionsOfType(TSubclassOf<UTGOR_ConnectionComponent> Class) const
{
	TArray<UTGOR_ConnectionComponent*> Out;
	for (const auto& Pair : Connections)
	{
		if (Pair.Value->IsA(Class))
		{
			Out.Emplace(Pair.Value);
		}
	}
	return(Out);
}
