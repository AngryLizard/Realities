// The Gateway of Realities: Planes of Existence.


#include "TGOR_MatterInterface.h"

#include "Realities/Base/System/Data/TGOR_DimensionData.h"

// Add default functionality here for any ITGOR_MatterInterface functions that are not pure virtual.

FTGOR_MatterContainers& ITGOR_MatterInterface::GetMatterContainers()
{
}


TMap<UTGOR_Matter*, int32> ITGOR_MatterInterface::GetComposition(TSubclassOf<UTGOR_Section> Filter)
{
	return TMap<UTGOR_Matter*, int32>();
}

TMap<UTGOR_Matter*, int32> ITGOR_MatterInterface::GetMissing(TSubclassOf<UTGOR_Section> Filter)
{
	return TMap<UTGOR_Matter*, int32>();
}

TMap<UTGOR_Matter*, int32> ITGOR_MatterInterface::GetCapacity(TSubclassOf<UTGOR_Section> Filter)
{
	return TMap<UTGOR_Matter*, int32>();
}

TSet<UTGOR_Matter*> ITGOR_MatterInterface::GetSupported(TSubclassOf<UTGOR_Section> Filter)
{
	return TSet<UTGOR_Matter*>();
}

TMap<UTGOR_Matter*, int32> ITGOR_MatterInterface::PayStorageMatter(TSubclassOf<UTGOR_Section> Filter, const TMap<UTGOR_Matter*, int32>& Composition)
{
	return TMap<UTGOR_Matter*, int32>();
}

TMap<UTGOR_Matter*, int32> ITGOR_MatterInterface::InvestStorageMatter(TSubclassOf<UTGOR_Section> Filter, const TMap<UTGOR_Matter*, int32>& Composition)
{
	return TMap<UTGOR_Matter*, int32>();
}