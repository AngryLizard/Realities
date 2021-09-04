// The Gateway of Realities: Planes of Existence.


#include "TGOR_Polar.h"

FTGOR_Polar::FTGOR_Polar()
:	X(0.0f),
	Y(0.0f),
	R(0.0f)
{
}

FTGOR_Polar::FTGOR_Polar(float X, float Y, float R)
	: X(X), Y(Y), R(R)
{
}

bool FTGOR_Polar::operator==(const FTGOR_Polar& Other) const
{
	return X == Other.X && Y == Other.Y && R == Other.R;
}

uint16 FTGOR_Polar::Pack() const
{
	const int32 PX = FMath::FloorToInt(X * 0b11111);
	const int32 PY = FMath::FloorToInt(Y * 0b11111);
	const int32 PR = FMath::FloorToInt(R * 0b111111);
	return (uint16)(((PX & 0b11111) << 0) | ((PY & 0b11111) << 5) | ((PY & 0b111111) << 10));
}

void FTGOR_Polar::Unpack(uint16 Serial)
{
	X = (float)((Serial >> 0) & 0b11111) / 0b11111;
	Y = (float)((Serial >> 5) & 0b11111) / 0b11111;
	R = (float)((Serial >> 10) & 0b111111) / 0b111111;
}

bool FTGOR_Polar::NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess)
{
	uint16 Serial = Pack();
	Ar << Serial;
	Unpack(Serial);
	return bOutSuccess = true;
}

