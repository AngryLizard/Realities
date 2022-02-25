// The Gateway of Realities: Planes of Existence.


#include "TGOR_StaticMeshLoader.h"

#include "Realities/Components/Dimension/TGOR_IdentityComponent.h"
#include "Realities/Base/TGOR_GameInstance.h"


#if WITH_EDITOR
#include "UnrealEd.h"
#endif // WITH_EDITOR

FTGOR_StaticMeshLoaderInstance::FTGOR_StaticMeshLoaderInstance()
{
}

FTGOR_StaticMeshLoaderInstance::FTGOR_StaticMeshLoaderInstance(const FTransform& Transform)
{
	Transforms.Emplace(Transform);
}

FTGOR_StaticMeshLoaderActors::FTGOR_StaticMeshLoaderActors()
{
}

FTGOR_StaticMeshLoaderActors::FTGOR_StaticMeshLoaderActors(AActor* Actor)
{
	Actors.Emplace(Actor);
}

FTGOR_StaticMeshLoaderIndices::FTGOR_StaticMeshLoaderIndices()
{
}

FTGOR_StaticMeshLoaderIndices::FTGOR_StaticMeshLoaderIndices(int32 Index)
{
	Indices.Emplace(Index);
}

UTGOR_StaticMeshLoader::UTGOR_StaticMeshLoader()
	: Super()
{
	Optional = false;
}


bool UTGOR_StaticMeshLoader::CanLoad(UWorld* World, const FTransform& Origin, UTGOR_Dimension* Dimension) const
{
	return true;
}

bool UTGOR_StaticMeshLoader::Load(UWorld* World, const FTransform& Origin, UTGOR_Dimension* Dimension)
{
	TMap<UStaticMesh*, FTGOR_StaticMeshLoaderActors> Loaded;
	TMap<UStaticMesh*, FTGOR_StaticMeshLoaderIndices> Unloaded;
	DetectBuildingBlocks(World, Origin, Loaded, Unloaded);
	
	for (const TPair<UStaticMesh*, FTGOR_StaticMeshLoaderIndices>& Pair : Unloaded)
	{
		const FTGOR_StaticMeshLoaderInstance& Instance = BuildingBlocks[Pair.Key];
		for (int32 Index : Pair.Value.Indices)
		{
			const FTransform& Transform = Instance.Transforms[Index];
			AStaticMeshActor* Actor = World->SpawnActor<AStaticMeshActor>(AStaticMeshActor::StaticClass(), Transform, FActorSpawnParameters());
			if (IsValid(Actor))
			{
				Actor->SetMobility(EComponentMobility::Movable);
				UStaticMeshComponent* Component = Actor->GetStaticMeshComponent();
				Component->SetStaticMesh(Pair.Key);
			}
		}
	}
	return true;
}

bool UTGOR_StaticMeshLoader::IsLoaded(UWorld* World, const FTransform& Origin, UTGOR_Dimension* Dimension) const
{
	TMap<UStaticMesh*, FTGOR_StaticMeshLoaderActors> Loaded;
	TMap<UStaticMesh*, FTGOR_StaticMeshLoaderIndices> Unloaded;
	DetectBuildingBlocks(World, Origin, Loaded, Unloaded);
	return Unloaded.Num() == 0;
}

bool UTGOR_StaticMeshLoader::Unload(UWorld* World, const FTransform& Origin, UTGOR_Dimension* Dimension)
{
	TMap<UStaticMesh*, FTGOR_StaticMeshLoaderActors> Loaded;
	TMap<UStaticMesh*, FTGOR_StaticMeshLoaderIndices> Unloaded;
	DetectBuildingBlocks(World, Origin, Loaded, Unloaded);
	
	for (const TPair<UStaticMesh*, FTGOR_StaticMeshLoaderActors>& Pair : Loaded)
	{
		const FTGOR_StaticMeshLoaderInstance& Instance = BuildingBlocks[Pair.Key];
		for (AActor* Actor : Pair.Value.Actors)
		{
			World->DestroyActor(Actor);
		}
	}
	return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void UTGOR_StaticMeshLoader::DetectBuildingBlocks(UWorld* World, const FTransform& Origin, TMap<UStaticMesh*, FTGOR_StaticMeshLoaderActors>& Loaded, TMap<UStaticMesh*, FTGOR_StaticMeshLoaderIndices>& Unloaded) const
{
	// Fill Missing with all meshes
	for (const TPair<UStaticMesh*, FTGOR_StaticMeshLoaderInstance>& Pair : BuildingBlocks)
	{
		// Can add all indices if no mesh was found
		FTGOR_StaticMeshLoaderIndices Indices;
		for (int32 Index = 0; Index < Pair.Value.Transforms.Num(); Index++)
		{
			Indices.Indices.Emplace(Index);
		}
		Unloaded.Add(Pair.Key, Indices);
	}

	// Iterate through all candidates
	for (TActorIterator<AStaticMeshActor> a(World); a; ++a)
	{
		const UStaticMeshComponent* Component = (*a)->GetStaticMeshComponent();
		if (IsValid(Component))
		{
			// Look for mesh
			UStaticMesh* Mesh = Component->GetStaticMesh();
			const FTGOR_StaticMeshLoaderInstance* Instance = BuildingBlocks.Find(Mesh);
			if (Instance)
			{
				// Find actor location relative to origin so we can compare
				const FVector Location = Origin.InverseTransformPosition((*a)->GetActorLocation());

				// Look for matching
				const int32 Num = Instance->Transforms.Num();
				for (int32 Index = 0; Index < Num; Index++)
				{
					const FTransform& InstanceTransform = Instance->Transforms[Index];
					if (InstanceTransform.GetLocation().Equals(Location, KINDA_SMALL_NUMBER))
					{
						// Remove from missing list
						TArray<int32>& Indices = Unloaded[Mesh].Indices;
						Indices.Remove(Index);
						if (Indices.Num() == 0)
						{
							Unloaded.Remove(Mesh);
						}

						// Add to loaded
						FTGOR_StaticMeshLoaderActors* Actors = Loaded.Find(Mesh);
						if (Actors)
						{
							Actors->Actors.Emplace(*a);
						}
						else
						{
							Loaded.Emplace(Mesh, FTGOR_StaticMeshLoaderActors(*a));
						}
					}
				}
			}
		}
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////

bool UTGOR_StaticMeshLoader::AddBuildingBlock(UStaticMesh* Mesh, const FTransform& Transform)
{
	FTGOR_StaticMeshLoaderInstance* Instance = BuildingBlocks.Find(Mesh);
	if (Instance)
	{
		for (const FTransform& InstanceTransform : Instance->Transforms)
		{
			if (InstanceTransform.Equals(Transform, SMALL_NUMBER))
			{
				return false;
			}
		}
		Instance->Transforms.Emplace(Transform);
		return true;
	}

	BuildingBlocks.Add(Mesh, FTGOR_StaticMeshLoaderInstance(Transform));
	return true;
}

bool UTGOR_StaticMeshLoader::MoveBuildingBlock(UStaticMesh* Mesh, const FTransform& OldTransform, const FTransform& NewTransform)
{
	FTGOR_StaticMeshLoaderInstance* Instance = BuildingBlocks.Find(Mesh);
	if (Instance)
	{
		for (FTransform& InstanceTransform : Instance->Transforms)
		{
			if (InstanceTransform.Equals(OldTransform, SMALL_NUMBER))
			{
				InstanceTransform = NewTransform;
				return true;
			}
		}
	}
	return false;
}



/*

#if WITH_EDITOR

		// Add to editor cache
		ATGOR_WorldSettings* Settings = Cast<ATGOR_WorldSettings>(World->GetWorldSettings());
		if (IsValid(Settings))
		{
			UTGOR_Dimension* Dimension = Settings->Dimension->GetDefaultObject<UTGOR_Dimension>();
			if (IsValid(Dimension))
			{
				Dimension->AddBuildingBlock(BuildingBlock);
				Dimension->MarkPackageDirty();
			}
		}

#endif

*/