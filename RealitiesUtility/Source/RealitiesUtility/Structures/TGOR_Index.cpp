// The Gateway of Realities: Planes of Existence.


#include "TGOR_Index.h"


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

bool FTGOR_Serial::NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess)
{
	uint16 Serial = (uint16)Index;
	Ar << Serial;
	Index = (int32)Serial;
	return bOutSuccess = true;
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

bool FTGOR_Index::NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess)
{
	bool IsValid = (Index > INDEX_NONE);
	Ar.SerializeBits(&IsValid, 1);

	if (IsValid)
	{
		return FTGOR_Serial::NetSerialize(Ar, Map, bOutSuccess);
	}
	else
	{
		Index = INDEX_NONE;
	}
	return bOutSuccess = true;
}
