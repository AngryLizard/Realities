// The Gateway of Realities: Planes of Existence.


#include "TGOR_PocketDimensionComponent.h"

#include "Realities/Base/Controller/TGOR_OnlineController.h"
#include "Realities/Base/System/Data/TGOR_DimensionData.h"
#include "Realities/Base/TGOR_Singleton.h"

#include "Realities/Utility/Storage/TGOR_Package.h"
#include "Net/UnrealNetwork.h"

UTGOR_PocketDimensionComponent::UTGOR_PocketDimensionComponent()
:	Super(),
	PocketIdentifier("Pocket")
{
}

void UTGOR_PocketDimensionComponent::SetExternal(const FVector& Force)
{
	SINGLETON_CHK;
	UTGOR_WorldData* WorldData = Singleton->GetWorldData();

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

void UTGOR_PocketDimensionComponent::BuildDimension(UTGOR_Dimension* Dimension)
{
	SINGLETON_CHK;
	UTGOR_WorldData* WorldData = Singleton->GetWorldData();

	// Only switch dimension if not already exists
	if (!DimensionIdentifier.IsNone())
	{
		if (WorldData->GetDimensionContent(DimensionIdentifier) == Dimension)
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

	OpenDimension(Dimension, PocketIdentifier);

	// Automatically load for owner
	APawn* Pawn = Cast<APawn>(GetOwner());
	if (IsValid(Pawn))
	{
		ATGOR_OnlineController* OnlineController = Cast<ATGOR_OnlineController>(Pawn->GetController());
		if (IsValid(OnlineController))
		{
			OnlineController->AddExistingDimensionUpdate(DimensionIdentifier);
			return;
		}
	}

	// If there is no owner automatically load on server
	UWorld* World = GetWorld();
	ATGOR_OnlineController* HostController = World ->GetFirstPlayerController<ATGOR_OnlineController>();
	if (IsValid(HostController))
	{
		HostController->AddExistingDimensionUpdate(DimensionIdentifier);
	}

}
