// The Gateway of Realities: Planes of Existence.


#include "TGOR_SaveInterface.h"

// Add default functionality here for any ITGOR_DimensionInterface functions that are not pure virtual.


void ITGOR_SaveInterface::Write_Implementation(FTGOR_GroupWritePackage& Package, UTGOR_Singleton* Context) const
{
}

bool ITGOR_SaveInterface::Read_Implementation(FTGOR_GroupReadPackage& Package, UTGOR_Singleton* Context)
{
	return true;
}

void ITGOR_NetInterface::Send_Implementation(FTGOR_NetworkWritePackage& Package, UTGOR_Singleton* Context) const
{
}

bool ITGOR_NetInterface::Recv_Implementation(FTGOR_NetworkReadPackage& Package, UTGOR_Singleton* Context)
{
	return true;
}