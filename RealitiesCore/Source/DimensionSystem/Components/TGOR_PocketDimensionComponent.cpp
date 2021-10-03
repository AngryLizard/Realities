// The Gateway of Realities: Planes of Existence.


#include "TGOR_PocketDimensionComponent.h"

#include "CoreSystem/TGOR_Singleton.h"
#include "DimensionSystem/Data/TGOR_DimensionData.h"
#include "DimensionSystem/Data/TGOR_WorldData.h"
#include "DimensionSystem/Volumes/TGOR_LevelVolume.h"

#include "Net/UnrealNetwork.h"

UTGOR_PocketDimensionComponent::UTGOR_PocketDimensionComponent()
:	Super(),
	PocketDimension(nullptr),
	PocketIdentifier("Pocket")
{
	SetIsReplicatedByDefault(true);
}

void UTGOR_PocketDimensionComponent::BeginPlay()
{
	Super::BeginPlay();

}

void UTGOR_PocketDimensionComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	//BuildDimension();
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void UTGOR_PocketDimensionComponent::UpdateContent_Implementation(FTGOR_SpawnerDependencies& Dependencies)
{
	ITGOR_SpawnerInterface::UpdateContent_Implementation(Dependencies);

	/*
	UTGOR_Creature* OwningCreature = Cast<UTGOR_Creature>(Spawner);
	if (IsValid(OwningCreature))
	{
		PocketDimension = OwningCreature->Instanced_StomachInsertion.Collection;
		BuildDimension();
	}
	*/
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void UTGOR_PocketDimensionComponent::SetExternal(const FVector& Force)
{
	SINGLETON_CHK;

	UTGOR_WorldData* WorldData = Singleton->GetData<UTGOR_WorldData>();
	if (IsValid(WorldData))
	{
		ETGOR_FetchEnumeration State;
		UTGOR_DimensionData* Dimension = WorldData->GetDimension(DimensionIdentifier, State);
		if (IsValid(Dimension))
		{
			ATGOR_LevelVolume* LevelVolume = Dimension->GetLevelVolume();
			if (IsValid(LevelVolume))
			{
				LevelVolume->SetExternal(Force);
			}
		}
	}
}

void UTGOR_PocketDimensionComponent::BuildDimension()
{
	SINGLETON_CHK;
	UTGOR_WorldData* WorldData = Singleton->GetData<UTGOR_WorldData>();
	if (IsValid(WorldData) && GetOwnerRole() == ENetRole::ROLE_Authority)
	{
		// Only switch dimension if not already exists
		if (!DimensionIdentifier.IsNone())
		{
			if (WorldData->GetDimensionContent(DimensionIdentifier) == PocketDimension)
			{
				return;
			}

			ETGOR_FetchEnumeration State;
			UTGOR_DimensionData* OldDimensionData = WorldData->GetDimension(DimensionIdentifier, State);
			if (IsValid(OldDimensionData))
			{
				// ... TODO: Rescue content from old dimension and delete dimension (Set to volatile or sth)
			}
		}

		OpenDimension(PocketDimension, PocketIdentifier);
	}

}
