// The Gateway of Realities: Planes of Existence.


#include "TGOR_Normal.h"

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

uint8 FTGOR_Normal::Pack() const
{
	return (uint8)(FMath::Clamp((Value + 1.0f) / 2, 0.0f, 1.0f) * 0xFF);
}

void FTGOR_Normal::Unpack(uint8 Byte)
{
	Value = FMath::Clamp(((float)Byte) / 0xFF, 0.0f, 1.0f) * 2 - 1.0f;
}

bool FTGOR_Normal::NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess)
{
	uint8 Byte = Pack();
	Ar << Byte;
	Unpack(Byte);
	return bOutSuccess = true;
}

