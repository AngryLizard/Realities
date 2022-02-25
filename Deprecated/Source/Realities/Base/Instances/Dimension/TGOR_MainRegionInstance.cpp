// The Gateway of Realities: Planes of Existence.


#include "TGOR_MainRegionInstance.h"

#include "Realities/Base/TGOR_GameInstance.h"
#include "Realities/Actors/Dimension/TGOR_RegionActor.h"

FTGOR_MainRegionInstance::FTGOR_MainRegionInstance()
{
}

/*
bool FTGOR_MainRegionInstance::Write(FilePack& Pack) const
{
	int32 Size = Regions.Num();

	// Set lengths
	Pack.Generate<0>(Size);
	
	// Read every Region
	bool Success = true;
	for (int i = 0; i < Size; i++)
	{
		ATGOR_RegionActor* Region = Regions[i];
		Success = Region->RegionInstance.Write(Pack.Pack<0>(i)) && Success;
	}

	return(Success);
}

bool FTGOR_MainRegionInstance::Read(const FilePack& Pack)
{
	int32 Size = Regions.Num();

	// Check for length
	if (Pack.GetLength<0>() != Size) { return(false); }
		
	// Write every Region
	bool Success = true;
	for (int i = 0; i < Size; i++)
	{
		ATGOR_RegionActor* Region = Regions[i];
		Success = Region->RegionInstance.Read(Pack.Pack<0>(i)) && Success;
	}

	return(Success);
}
*/
