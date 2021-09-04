// The Gateway of Realities: Planes of Existence.


#include "TGOR_DimensionLoaderComponent.h"

#include "CoreSystem/TGOR_Singleton.h"
#include "DimensionSystem/Data/TGOR_WorldData.h"
#include "DimensionSystem/Content/TGOR_Dimension.h"
#include "DimensionSystem/Data/TGOR_DimensionData.h"
#include "DimensionSystem/Components/TGOR_ConnectionComponent.h"

#include "Net/UnrealNetwork.h"
#include "CoreSystem/Storage/TGOR_Package.h"

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
		UTGOR_WorldData* WorldData = Singleton->GetData<UTGOR_WorldData>();
		if (IsValid(WorldData))
		{
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
}

void UTGOR_DimensionLoaderComponent::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(UTGOR_DimensionLoaderComponent, LoaderLevel, COND_None);
	DOREPLIFETIME_CONDITION(UTGOR_DimensionLoaderComponent, DimensionIdentifier, COND_None);
	DOREPLIFETIME_CONDITION(UTGOR_DimensionLoaderComponent, DimensionConnections, COND_None);
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
	UTGOR_WorldData* WorldData = Singleton->GetData<UTGOR_WorldData>();
	if (IsValid(WorldData))
	{
		/*
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
		*/

		ETGOR_PushEnumeration State;
		DimensionIdentifier = WorldData->EnsureIdentifier(Dimension, Suffix, State);
		if (State != ETGOR_PushEnumeration::Failed)
		{
			ChangeLoaderLevel(ETGOR_DimensionLoaderLevel::Loading);
		}
	}
}

void UTGOR_DimensionLoaderComponent::OpenDimension(UTGOR_Dimension* Dimension, const FString& Identifier)
{
	SINGLETON_CHK;
	UTGOR_WorldData* WorldData = Singleton->GetData<UTGOR_WorldData>();
	if (IsValid(WorldData))
	{
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
}

bool UTGOR_DimensionLoaderComponent::HasDial() const
{
	return LoaderLevel != ETGOR_DimensionLoaderLevel::Idle;
}

UTGOR_DimensionData* UTGOR_DimensionLoaderComponent::GetLinkedDimension() const
{
	SINGLETON_RETCHK(nullptr);
	if (DimensionIdentifier.IsNone()) return nullptr;

	UTGOR_WorldData* WorldData = Singleton->GetData<UTGOR_WorldData>();
	if (IsValid(WorldData))
	{
		ETGOR_FetchEnumeration State;
		return WorldData->GetDimension(DimensionIdentifier, State);
	}
	return nullptr;
}

void UTGOR_DimensionLoaderComponent::ChangeLoaderLevel(ETGOR_DimensionLoaderLevel Level)
{
	LoaderLevel = Level;
	OnLoaderStateChange.Broadcast(Level);

	if (HasServerAuthority())
	{
		DimensionConnections.Components.Empty();
		if (Level == ETGOR_DimensionLoaderLevel::Loaded)
		{
			UTGOR_DimensionData* Dimension = GetLinkedDimension();
			if (IsValid(Dimension))
			{
				for (const FTGOR_ConnectionSelection& ConnectionRequest : ConnectionRequests)
				{
					UTGOR_ConnectionComponent* Connection = Dimension->GetConnection(ConnectionRequest.Connection);
					if (IsValid(Connection))
					{
						DimensionConnections.Components.Emplace(ConnectionRequest.Connection, Connection);
					}
				}
			}
		}
	}
}


void UTGOR_DimensionLoaderComponent::RepNotifyLoad()
{
	OnLoaderStateChange.Broadcast(LoaderLevel);

}

////////////////////////////////////////////////////////////////////////////////////////////////////

FName UTGOR_DimensionLoaderComponent::GetLoadedIdentifier() const
{
	return DimensionIdentifier;
}

UTGOR_ConnectionComponent* UTGOR_DimensionLoaderComponent::GetConnection(const FName& ConnectionName) const
{
	const TWeakObjectPtr<UTGOR_ConnectionComponent>* Ptr = DimensionConnections.Components.Find(ConnectionName);
	if (Ptr)
	{
		return((*Ptr).Get());
	}
	return(nullptr);
}

UTGOR_ConnectionComponent* UTGOR_DimensionLoaderComponent::GetConnectionOfType(const FName& ConnectionName, TSubclassOf<UTGOR_ConnectionComponent> Class) const
{
	UTGOR_ConnectionComponent* Connection = GetConnection(ConnectionName);
	if (IsValid(Connection) && Connection->IsA(Class))
	{
		return Connection;
	}
	return nullptr;
}
