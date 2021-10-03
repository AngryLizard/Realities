// The Gateway of Realities: Planes of Existence.

#include "TGOR_MatterComponent.h"

#include "InventorySystem/Content/TGOR_Segment.h"
#include "InventorySystem/Content/TGOR_Resource.h"

#include "CoreSystem/TGOR_Singleton.h"
#include "CoreSystem/Gameplay/TGOR_GameState.h"
#include "CoreSystem/Storage/TGOR_Package.h"
#include "Net/UnrealNetwork.h"


////////////////////////////////////////////////////////////////////////////////////////////////////

UTGOR_MatterComponent::UTGOR_MatterComponent()
	: Super()
{
	PrimaryComponentTick.bCanEverTick = true;
	SetIsReplicatedByDefault(true);
}

void UTGOR_MatterComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UTGOR_MatterComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UTGOR_MatterComponent::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(UTGOR_MatterComponent, MatterContainers, COND_None);
}

void UTGOR_MatterComponent::RepNotifyMatterContainers()
{
	OnMatterChanged.Broadcast();
}

void UTGOR_MatterComponent::UpdateContent_Implementation(FTGOR_SpawnerDependencies& Dependencies)
{
	SINGLETON_CHK;

	FTGOR_MatterContainers Old = MatterContainers;

	MatterContainers.Containers.Empty();

	UTGOR_Resource* Resource = Cast<UTGOR_Resource>(Dependencies.Spawner);
	if (IsValid(Resource))
	{
		const TArray<UTGOR_Segment*>& Segments = Resource->Instanced_SegmentInsertions.Collection;// ->GetIListFromType<UTGOR_Segment>();
		for (UTGOR_Segment* Segment : Segments)
		{
			FTGOR_MatterContainer Container;
			Container.Segment = Segment;

			// Move matter from old container if available
			// TODO: Do we want that?
			for (const FTGOR_MatterContainer& OldContainer : Old.Containers)
			{
				if (OldContainer.Segment == Segment)
				{
					for (const auto& Pair : OldContainer.Slots)
					{
						Container.AddMatter(Pair.Key, Pair.Value);
					}
				}
			}

			MatterContainers.Containers.Emplace(Container);
		}
	}
	OnMatterChanged.Broadcast();
}

////////////////////////////////////////////////////////////////////////////////////////////////////

UTGOR_Segment* UTGOR_MatterComponent::GetSegment(TSubclassOf<UTGOR_Segment> Filter) const
{
	for (const FTGOR_MatterContainer& Container : MatterContainers.Containers)
	{
		if (Container.Segment->IsA(Filter))
		{
			return Container.Segment;
		}
	}
	return nullptr;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

TMap<UTGOR_Matter*, int32> UTGOR_MatterComponent::GetComposition(TSubclassOf<UTGOR_Segment> Filter)
{
	TMap<UTGOR_Matter*, int32> Composition;
	MatterContainers.AddComposition(Filter, Composition);
	return Composition;
}

TMap<UTGOR_Matter*, int32> UTGOR_MatterComponent::GetMissing(TSubclassOf<UTGOR_Segment> Filter)
{
	TMap<UTGOR_Matter*, int32> Missing;
	MatterContainers.AddMissing(Filter, Missing);
	return Missing;
}

TMap<UTGOR_Matter*, int32> UTGOR_MatterComponent::GetCapacity(TSubclassOf<UTGOR_Segment> Filter)
{
	TMap<UTGOR_Matter*, int32> Capacity;
	MatterContainers.AddCapacity(Filter, Capacity);
	return Capacity;
}

TSet<UTGOR_Matter*> UTGOR_MatterComponent::GetSupported(TSubclassOf<UTGOR_Segment> Filter)
{
	TSet<UTGOR_Matter*> Supported;
	MatterContainers.AddSupported(Filter, Supported);
	return Supported;
}

TMap<UTGOR_Matter*, int32> UTGOR_MatterComponent::PayStorageMatter(TSubclassOf<UTGOR_Segment> Filter, const TMap<UTGOR_Matter*, int32>& Composition)
{
	TMap<UTGOR_Matter*, int32> Payment;
	for (const auto& Pair : Composition)
	{
		const int32 Quantity = MatterContainers.RemoveMatter(Filter, Pair.Key, Pair.Value);
		Payment.FindOrAdd(Pair.Key) += Quantity;
	}

	OnMatterChanged.Broadcast();
	return Payment;
}

TMap<UTGOR_Matter*, int32> UTGOR_MatterComponent::InvestStorageMatter(TSubclassOf<UTGOR_Segment> Filter, const TMap<UTGOR_Matter*, int32>& Composition)
{
	TMap<UTGOR_Matter*, int32> Investment;
	for (const auto& Pair : Composition)
	{
		const int32 Quantity = MatterContainers.AddMatter(Filter, Pair.Key, Pair.Value);
		Investment.FindOrAdd(Pair.Key) += Quantity;
	}

	OnMatterChanged.Broadcast();
	return Investment;
}
