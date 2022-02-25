// The Gateway of Realities: Planes of Existence.


#include "TGOR_Normal.h"
#include "Realities/Utility/Storage/TGOR_Package.h"
#include "Realities/Base/TGOR_GameState.h"

SERIALISE_INIT_IMPLEMENT(FTGOR_Normal)

FTGOR_Normal::FTGOR_Normal()
	: Value(0.0f)
{
}

FTGOR_Normal::FTGOR_Normal(float V)
	: Value(V)
{
}

bool FTGOR_Normal::operator==(const FTGOR_Normal& Normal) const
{
	return Value == Normal.Value;
}

void FTGOR_Normal::Write(FTGOR_GroupWritePackage& Package, UTGOR_Singleton* Context) const
{
	Package.WriteEntry("Value", Value);
}

void FTGOR_Normal::Read(FTGOR_GroupReadPackage& Package, UTGOR_Singleton* Context)
{
	Package.ReadEntry("Value", Value);
}

void FTGOR_Normal::Send(FTGOR_NetworkWritePackage& Package, UTGOR_Singleton* Context) const
{
	const uint8 Normal = (uint8)(FMath::Clamp((Value + 1.0f) / 2, 0.0f, 1.0f) * 0xFF);
	Package.WriteEntry(Normal);
}

void FTGOR_Normal::Recv(FTGOR_NetworkReadPackage& Package, UTGOR_Singleton* Context)
{
	uint8 Normal; 
	Package.ReadEntry(Normal);
	Value = FMath::Clamp(((float)Normal) / 0xFF, 0.0f, 1.0f) * 2 - 1.0f;
}