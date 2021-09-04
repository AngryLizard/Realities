// The Gateway of Realities: Planes of Existence.


#include "TGOR_Percentage.h"

FTGOR_Percentage::FTGOR_Percentage()
	: Value(0.0f)
{
}

FTGOR_Percentage::FTGOR_Percentage(float V)
	: Value(V)
{
}

bool FTGOR_Percentage::operator==(const FTGOR_Percentage& Normal) const
{
	return Value == Normal.Value;
}

uint16 FTGOR_Percentage::Pack() const
{
	return (uint8)(FMath::Clamp((Value + 1.0f) / 2, 0.0f, 1.0f) * 0xFF);
}

void FTGOR_Percentage::Unpack(uint16 Byte)
{
	Value = FMath::Clamp(((float)Byte) / 0xFF, 0.0f, 1.0f) * 2 - 1.0f;
}

bool FTGOR_Percentage::NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess)
{
	uint8 Short = Pack();
	Ar << Short;
	Unpack(Short);
	return bOutSuccess = true;
}

