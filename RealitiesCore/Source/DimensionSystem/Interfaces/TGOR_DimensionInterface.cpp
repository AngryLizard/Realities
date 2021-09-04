// The Gateway of Realities: Planes of Existence.


#include "TGOR_DimensionInterface.h"

#include "DimensionSystem/Data/TGOR_DimensionData.h"

// Add default functionality here for any ITGOR_DimensionInterface functions that are not pure virtual.


bool ITGOR_DimensionInterface::PreAssemble(UTGOR_DimensionData* Dimension)
{
	return(true);
}

bool ITGOR_DimensionInterface::Assemble(UTGOR_DimensionData* Dimension)
{
	DimensionData = Dimension;
	DIMENSIONBASE_RETCHK(false);
	return(true);
}

bool ITGOR_DimensionInterface::PostAssemble(UTGOR_DimensionData* Dimension)
{
	//DIMENSIONBASE_RETCHK(false);
	if (!DimensionData.IsValid())
	{
		ERRET(DIMENSIONBASE_ERR, Error, false);
	}
	return(true);
}

FName ITGOR_DimensionInterface::GetDimensionIdentifier(ETGOR_FetchEnumeration& Branches) const
{
	if (DimensionData.IsValid())
	{
		Branches = ETGOR_FetchEnumeration::Found;
		return(DimensionData->GetIdentifier());
	}
	Branches = ETGOR_FetchEnumeration::Empty;
	return("");
}

UTGOR_DimensionData* ITGOR_DimensionInterface::GetDimension(ETGOR_FetchEnumeration& Branches) const
{
	if (DimensionData.IsValid())
	{
		Branches = ETGOR_FetchEnumeration::Found;
		return DimensionData.Get();
	}
	Branches = ETGOR_FetchEnumeration::Empty;
	return nullptr;
}
