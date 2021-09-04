// The Gateway of Realities: Planes of Existence.


#include "TGOR_Time.h"

FTGOR_Time::FTGOR_Time()
	: Seconds(0.0f), Epochs(0)
{
}

void FTGOR_Time::Normalize()
{
	while (Seconds > SECONDS_PER_EPOCH)
	{
		Seconds -= SECONDS_PER_EPOCH;
		Epochs++;
	}
}

void FTGOR_Time::Clear()
{
	Seconds = 0.0f;
	Epochs = 0;
}

bool FTGOR_Time::IsZero() const
{
	return Seconds == 0.0f && Epochs == 0;
}

float FTGOR_Time::Modf(float Period, float Speed, float Phase) const
{
	// Take rest of epoch first to prevent big numbers.
	const float EpochNum = SECONDS_PER_EPOCH * Speed / Period;
	const float EpochDiv = FMath::FloorToFloat(EpochNum);

	const float Num = (EpochNum - EpochDiv) * Epochs + (Seconds * Speed + Phase) / Period;
	const float Div = FMath::FloorToFloat(Num);
	return (Num - Div) * Period;
}

float FTGOR_Time::Value() const
{
	return Seconds;
}

FString FTGOR_Time::Print() const
{
	return FString::FromInt(Epochs) + " : " + FString::SanitizeFloat(Seconds, 2);
}

bool FTGOR_Time::operator==(const FTGOR_Time& Other) const
{
	return Other.Epochs == Epochs && Other.Seconds == Seconds;
}

bool FTGOR_Time::operator!=(const FTGOR_Time& Other) const
{
	return Other.Epochs != Epochs || Other.Seconds != Seconds;
}

bool FTGOR_Time::operator>(const FTGOR_Time& Other) const
{
	return (Epochs > Other.Epochs) || ((Epochs == Other.Epochs) && (Seconds > Other.Seconds));
}

bool FTGOR_Time::operator<(const FTGOR_Time& Other) const
{
	return (Epochs < Other.Epochs) || ((Epochs == Other.Epochs) && (Seconds < Other.Seconds));
}


float FTGOR_Time::operator-(const FTGOR_Time& Other) const
{
	return (Epochs - Other.Epochs) * SECONDS_PER_EPOCH + (Seconds - Other.Seconds);
}

FTGOR_Time FTGOR_Time::operator+=(float DeltaSeconds)
{
	Seconds += DeltaSeconds;
	return *this;
}

FTGOR_Time FTGOR_Time::operator+(float DeltaSeconds) const
{
	FTGOR_Time Out = *this;
	Out += DeltaSeconds;
	return Out;
}


bool FTGOR_Time::NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess) 
{
	Ar << Seconds;
	Ar << Epochs;
	return bOutSuccess = true;
}
