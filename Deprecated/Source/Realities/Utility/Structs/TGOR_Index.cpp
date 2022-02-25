// The Gateway of Realities: Planes of Existence.


#include "TGOR_Index.h"
#include "Realities/Utility/Storage/TGOR_Package.h"



FTGOR_Serial::FTGOR_Serial()
	: Index(INDEX_NONE)
{

}

FTGOR_Serial::FTGOR_Serial(int32 Index)
	: Index(Index)
{
}

bool FTGOR_Serial::operator==(int32 Other) const
{
	return Index == Other;
}

bool FTGOR_Serial::operator!=(int32 Other) const
{
	return Index != Other;
}

bool FTGOR_Serial::operator==(FTGOR_Serial Other) const
{
	return Index == Other.Index;
}

bool FTGOR_Serial::operator!=(FTGOR_Serial Other) const
{
	return Index != Other.Index;
}

void FTGOR_Serial::Write(FTGOR_GroupWritePackage& Package, UTGOR_Singleton* Context) const
{
	Package.WriteEntry("Index", Index);
}

void FTGOR_Serial::Read(FTGOR_GroupReadPackage& Package, UTGOR_Singleton* Context)
{
	Package.ReadEntry("Index", Index);
}

void FTGOR_Serial::Send(FTGOR_NetworkWritePackage& Package, UTGOR_Singleton* Context) const
{
	const uint16 Serial = (uint16)Index;
	Package.WriteEntry(Serial);
}

void FTGOR_Serial::Recv(FTGOR_NetworkReadPackage& Package, UTGOR_Singleton* Context)
{
	uint16 Serial;
	Package.ReadEntry(Serial);
	Index = (int32)Serial;
}

uint32 FTGOR_Serial::GetTypeHash() const
{
	return (uint32)Index;
}


////////////////////////////////////////////////////////////////////////////////////////////////////

FTGOR_Index::FTGOR_Index()
	: FTGOR_Serial(INDEX_NONE)
{

}

FTGOR_Index::FTGOR_Index(int32 Index)
	: FTGOR_Serial(Index)
{
}

FTGOR_Index::FTGOR_Index(FTGOR_Serial Serial)
	: FTGOR_Serial(Serial.Index)
{
}


bool FTGOR_Index::operator==(FTGOR_Index Other) const
{
	return Index == Other.Index;
}

bool FTGOR_Index::operator!=(FTGOR_Index Other) const
{
	return Index != Other.Index;
}

FTGOR_Index::operator bool() const
{
	return Index != INDEX_NONE;
}

void FTGOR_Index::Send(FTGOR_NetworkWritePackage& Package, UTGOR_Singleton* Context) const
{
	const bool IsValid = (Index > INDEX_NONE);
	Package.WriteEntry(IsValid);
	if (IsValid)
	{
		FTGOR_Serial::Send(Package, Context);
	}
}

void FTGOR_Index::Recv(FTGOR_NetworkReadPackage& Package, UTGOR_Singleton* Context)
{
	Index = INDEX_NONE;

	bool IsValid;
	Package.ReadEntry(IsValid);
	if (IsValid)
	{
		FTGOR_Serial::Recv(Package, Context);
	}
}