// The Gateway of Realities: Planes of Existence.


#include "TGOR_DimensionLoaderComponent.h"

#include "Realities/Base/TGOR_Singleton.h"
#include "Realities/Base/Content/TGOR_ContentManager.h"
#include "Realities/Base/System/Data/TGOR_WorldData.h"
#include "Realities/Mod/Dimensions/TGOR_Dimension.h"
#include "Realities/Actors/Pawns/TGOR_Pawn.h"
#include "Realities/Base/System/Data/TGOR_DimensionData.h"
#include "Realities/Base/Controller/TGOR_OnlineController.h"
#include "Realities/Base/TGOR_WorldSettings.h"

#include "Net/UnrealNetwork.h"
#include "Realities/Utility/Storage/TGOR_Package.h"

UTGOR_DimensionLoaderComponent::UTGOR_DimensionLoaderComponent()
:	Super(),
	LoaderLevel(ETGOR_DimensionLoaderLevel::Idle)
{
	PrimaryComponentTick.bCanEverTick = true;
	SetIsReplicatedByDefault(true);
}

void UTGOR_DimensionLoaderComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (HasServerAuthority())
	{
		// TODO: Update this to be reactive (look at current state of dimensionData), not active

		SINGLETON_CHK;
		UTGOR_WorldData* WorldData = Singleton->GetWorldData();

		// Determine current state
		ETGOR_DimensionLoaderLevel NewLevel = ETGOR_DimensionLoaderLevel::Idle;
		if (WorldData->HasIdentifier(DimensionIdentifier))
		{
			if (WorldData->IsUnloading(DimensionIdentifier))
			{
				NewLevel = ETGOR_DimensionLoaderLevel::Unloading;
			}
			else if (WorldData->HasFinishedLoading(DimensionIdentifier))
			{
				NewLevel = ETGOR_DimensionLoaderLevel::Loaded;
			}
			else
			{
				NewLevel = ETGOR_DimensionLoaderLevel::Loading;
			}
		}

		// Notify on changes
		if (NewLevel != LoaderLevel)
		{
			ChangeLoaderLevel(NewLevel);
		}
	}
}

void UTGOR_DimensionLoaderComponent::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(UTGOR_DimensionLoaderComponent, LoaderLevel, COND_None);
}

void UTGOR_DimensionLoaderComponent::Write_Implementation(FTGOR_GroupWritePackage& Package, UTGOR_Singleton* Context) const
{
	Package.WriteEntry("OtherDimension", DimensionIdentifier);
}

bool UTGOR_DimensionLoaderComponent::Read_Implementation(FTGOR_GroupReadPackage& Package, UTGOR_Singleton* Context)
{
	Package.ReadEntry("OtherDimension", DimensionIdentifier);
	return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

bool UTGOR_DimensionLoaderComponent::IsReady() const
{
	return LoaderLevel == ETGOR_DimensionLoaderLevel::Loaded;
}


void UTGOR_DimensionLoaderComponent::DialDimension(TSubclassOf<UTGOR_Dimension> Dimension, const FString& Suffix)
{
	SINGLETON_CHK;
	UTGOR_WorldData* WorldData = Singleton->GetWorldData();
	UTGOR_ContentManager* ContentManager = Singleton->GetContentManager();

	FString Identifier = "Dimension";
	UTGOR_Dimension* Content = ContentManager->GetTFromType<UTGOR_Dimension>(Dimension);
	if (IsValid(Content))
	{
		Identifier = Content->GetDefaultName() + Suffix;
	}

	DimensionIdentifier = WorldData->GetUniqueIdentifier(Identifier);


	// Don't reload if dimension class matches the currently persistent level (mostly for in-editor play)
	const FName PersistentIdentifier = WorldData->GetPersistentIdentifier();
	UTGOR_Dimension* PersistentDimension = WorldData->GetDimensionContent(PersistentIdentifier);
	if (PersistentDimension == Content)
	{
		DimensionIdentifier = PersistentIdentifier;
	}

	// Register to world
	WorldData->SetIdentifier(Content, DimensionIdentifier);
	ChangeLoaderLevel(ETGOR_DimensionLoaderLevel::Loading);
}

void UTGOR_DimensionLoaderComponent::OpenDimension(UTGOR_Dimension* Dimension, const FString& Identifier)
{
	SINGLETON_CHK;
	UTGOR_WorldData* WorldData = Singleton->GetWorldData();
	UTGOR_ContentManager* ContentManager = Singleton->GetContentManager();

	DimensionIdentifier = WorldData->GetUniqueIdentifier(Identifier);

	// Don't reload if dimension class matches the currently persistent level (mostly for in-editor play)
	const FName PersistentIdentifier = WorldData->GetPersistentIdentifier();
	UTGOR_Dimension* PersistentDimension = WorldData->GetDimensionContent(PersistentIdentifier);
	if (PersistentDimension == Dimension)
	{
		DimensionIdentifier = PersistentIdentifier;
	}

	// Register to world
	WorldData->SetIdentifier(Dimension, DimensionIdentifier);

	// Set dimension owner
	ETGOR_FetchEnumeration State;
	const FTGOR_DimensionIdentifier OwnerIdentifier = WorldData->GetIdentifier(GetOwner(), State);
	if (State == ETGOR_FetchEnumeration::Found)
	{
		WorldData->SetOwner(DimensionIdentifier, OwnerIdentifier);
	}

	ChangeLoaderLevel(ETGOR_DimensionLoaderLevel::Loading);
}

bool UTGOR_DimensionLoaderComponent::HasDial() const
{
	return LoaderLevel != ETGOR_DimensionLoaderLevel::Idle;
}

UTGOR_DimensionData* UTGOR_DimensionLoaderComponent::GetLinkedDimension() const
{
	if (!Singleton || DimensionIdentifier.IsNone()) return nullptr;

	ETGOR_FetchEnumeration State;
	UTGOR_WorldData* WorldData = Singleton->GetWorldData();
	return WorldData->GetDimension(DimensionIdentifier, State);
}

void UTGOR_DimensionLoaderComponent::ChangeLoaderLevel(ETGOR_DimensionLoaderLevel Level)
{
	LoaderLevel = Level;
	OnLoaderLevelChanged.Broadcast(Level);
}


void UTGOR_DimensionLoaderComponent::RepNotifyLoad()
{
	OnLoaderLevelChanged.Broadcast(LoaderLevel);

}

////////////////////////////////////////////////////////////////////////////////////////////////////

void UTGOR_DimensionLoaderComponent::LoadForPlayer(ATGOR_OnlineController* Player)
{
	if (IsValid(Player))
	{
		SINGLETON_CHK;
		UTGOR_WorldData* WorldData = Singleton->GetWorldData();
		if (WorldData->HasIdentifier(DimensionIdentifier) && !Player->HasDimensionUpdate(DimensionIdentifier))
		{
			// Add to client
			Player->AddExistingDimensionUpdate(DimensionIdentifier);
		}
	}
}
