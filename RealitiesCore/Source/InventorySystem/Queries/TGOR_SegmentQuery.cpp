// The Gateway of Realities: Planes of Existence.


#include "TGOR_SegmentQuery.h"
#include "InventorySystem/Components/TGOR_MatterComponent.h"

#include "InventorySystem/Content/TGOR_Item.h"
#include "InventorySystem/Content/TGOR_Segment.h"

#include "CoreSystem/TGOR_Singleton.h"

UTGOR_SegmentQuery::UTGOR_SegmentQuery()
	: Super(),
	HostComponent(nullptr)
{
}

TArray<UTGOR_CoreContent*> UTGOR_SegmentQuery::QueryContent() const
{
	return MigrateContentArray(Segments);
}

void UTGOR_SegmentQuery::AssignComponent(UTGOR_MatterComponent* MatterComponent, TSubclassOf<UTGOR_Segment> Segment)
{
	HostComponent = MatterComponent;
	
	Segments.Empty();
	for (const FTGOR_MatterContainer& Container : HostComponent->MatterContainers.Containers)
	{
		if (!*Segment || Container.Segment->IsA(Segment))
		{
			Segments.Emplace(Container.Segment);
		}
	}
}
