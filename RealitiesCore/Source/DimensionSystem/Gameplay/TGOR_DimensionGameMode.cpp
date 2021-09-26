// The Gateway of Realities: Planes of Existence.


#include "TGOR_DimensionGameMode.h"

#include "RealitiesUGC/Mod/TGOR_ContentManager.h"

#include "CoreSystem/TGOR_Singleton.h"

#include "DimensionSystem/Data/TGOR_WorldData.h"
#include "DimensionSystem/Data/TGOR_DimensionData.h"
#include "DimensionSystem/Content/TGOR_Spawner.h"
#include "DimensionSystem/Components/TGOR_IdentityComponent.h"

ATGOR_DimensionGameMode::ATGOR_DimensionGameMode(const FObjectInitializer& ObjectInitializer)
:	Super(ObjectInitializer)
{
}

APawn* ATGOR_DimensionGameMode::SpawnDefaultPawnAtTransform_Implementation(AController* NewPlayer, const FTransform& SpawnTransform)
{
	// If singleton (or the dimension system) isn't available yet we know any spawned object is swept up by the initialisation process later
	if (IsValid(EnsureSingleton(this)))
	{
		UTGOR_ContentManager* ContentManager = Singleton->GetContentManager();
		UTGOR_WorldData* WorldData = Singleton->GetData<UTGOR_WorldData>();
		if (IsValid(WorldData) && IsValid(ContentManager))
		{
			const FName PersistentIdentifier = WorldData->GetPersistentIdentifier();

			ETGOR_FetchEnumeration State;
			UTGOR_DimensionData* PersistentDimension = WorldData->GetDimension(PersistentIdentifier, State);
			if (IsValid(PersistentDimension))
			{
				UTGOR_Spawner* Spawner = ContentManager->GetTFromType<UTGOR_Spawner>(PlayerSpawner);
				if (IsValid(Spawner))
				{
					const int32 ActorIdentifier = PersistentDimension->GetUniqueActorIdentifier();
					UTGOR_IdentityComponent* Identity = PersistentDimension->AddDimensionObject(ActorIdentifier, Spawner, SpawnTransform.GetLocation(), SpawnTransform.GetRotation().Rotator(), State);
					if (IsValid(Identity))
					{
						APawn* Pawn = Cast<APawn>(Identity->GetOwner());
						if (!IsValid(Pawn))
						{
							ERRET("Default Player spawner didn't produce a pawn!", Error, Pawn);
						}
						return Pawn;
					}
				}
			}
			else
			{
				// Spawn default and try to register with the dimension anyway
				APawn* Pawn = Super::SpawnDefaultPawnAtTransform_Implementation(NewPlayer, SpawnTransform);
				if (IsValid(Pawn))
				{
					UTGOR_IdentityComponent* Identity = Pawn->FindComponentByClass<UTGOR_IdentityComponent>();
					PersistentDimension->RegisterDimensionObject(Identity);
				}
				return Pawn;
			}

		}
	}

	return Super::SpawnDefaultPawnAtTransform_Implementation(NewPlayer, SpawnTransform);
}
