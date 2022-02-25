// The Gateway of Realities: Planes of Existence. By Salireths.


#include "TGOR_Math.h"
#include "HAL/PlatformMisc.h"


bool UTGOR_Math::IsPrime(int32 Number)
{
	// exclude most common divisors
	if (Number == 2 || Number == 3) return true;
	if (Number % 2 == 0 || Number % 3 == 0) return false;
	// math magic
	int d = 6;
	while (d * d - 2 * d + 1 <= Number)
	{
		if (Number % (d - 1) == 0)return false;
		if (Number % (d + 1) == 0)return false;
		d += 6;
	}
	return true;
}

int UTGOR_Math::NextPrime(int32 Number)
{
	while (!IsPrime(++Number)) {}
	return Number;
}

int32 UTGOR_Math::PosMod(int32 X, int32 N)
{
	if (N == 0) return(0);
	return((X % N + N) % N);
}

int32 UTGOR_Math::Log2(int32 X)
{
	// From: https://stackoverflow.com/questions/994593/how-to-do-an-integer-log2-in-c
#define S(k) if (X >= (1 << k)) { i += k; X >>= k; }
	int32 i = -(X == 0); S(16); S(8); S(4); S(2); S(1); return i;
#undef S
}

float UTGOR_Math::SafeInv(float X)
{
	if (X * X < SMALL_NUMBER)
	{
		return(0.0f);
	}
	return(1.0f / X);
}

float UTGOR_Math::SafeDiv(float X, float Div)
{
	if (Div < SMALL_NUMBER)
	{
		return(0.0f);
	}
	return(X / Div);
}

float UTGOR_Math::InterpolateParabola(float X)
{
	// a*x^2 + b*x
	return(CURVEA * X * X + CURVEB * X);
}

float UTGOR_Math::ThresholdClamp(float X, float Threshold)
{
	if (FMath::Abs(X) < Threshold) return(0.0f);
	return( X - FMath::Sign(X) * Threshold);
}


FRotator UTGOR_Math::AddRotation(const FRotator& Rotator, float Pitch, float Yaw)
{
	FRotator Result = Rotator;
	Result.Pitch += Pitch;
	Result.Yaw += Yaw;

	Result.Pitch = FMath::Clamp(Result.Pitch, -89.0f, 89.0f);

	return(Result);
}

float UTGOR_Math::ByteToRatio(uint8 Byte)
{
	return((float)Byte / 0xFF);
}

uint8 UTGOR_Math::RatioToByte(float Ratio)
{
	return((uint8)(Ratio * 0xFF));
}

float UTGOR_Math::SafeRatio(float X, float Max)
{
	if (Max < SMALL_NUMBER)
	{
		return(1.0f);
	}
	else
	{
		return(FMath::Clamp(X / Max, 0.0f, 1.0f));
	}
}

FLinearColor UTGOR_Math::LerpColours(const FLinearColor& Dark, const FLinearColor& Medium, const FLinearColor& Light, uint8 Byte)
{
	float Ratio = ByteToRatio(Byte);
	if (Ratio < 0.5f) return(FMath::Lerp(Dark, Medium, Ratio * 2));
	else return(FMath::Lerp(Medium, Light, (Ratio - 0.5) * 2));
}

FLinearColor UTGOR_Math::SolidifyColour(const FLinearColor& Colour)
{
	return(FLinearColor(Colour.R, Colour.G, Colour.B, 1.0f));
}

FLinearColor UTGOR_Math::BuildColour(int32 DwColour)
{
	return(FColor(DwColour).ReinterpretAsLinear());
}


float UTGOR_Math::GetTimeValue(const FTGOR_Time& Time)
{
	return Time.Value();
}

float UTGOR_Math::GetDuration(const FTGOR_Time& From, const FTGOR_Time& To)
{
	return To - From;
}

float UTGOR_Math::GetTimeMod(const FTGOR_Time& Time, float Period, float Speed, float Phase)
{
	return Time.Modf(Period, Speed, Phase);
}



FVector UTGOR_Math::AddLimited(const FVector& Vector, const FVector& Delta, float Limit)
{
	const FVector Result = Vector + Delta;

	const FVector Normal = UTGOR_Math::Normalize(Result);
	const float Before = Vector.Size();
	const float After = Result.Size();

	if (After <= Limit || After < Before)
	{
		// Change if no limit reached
		return(Result);
	}
	else if (Before <= Limit)
	{
		// Change if limit just got reached
		return(Normal * Limit);
	}
	else
	{
		// Only change direction
		return(Normal * Before);
	}
}

int32 UTGOR_Math::GetClassDistance(UClass* Base, UClass* Super)
{
	int32 Distance = 0;
	for (const UStruct* Struct = Base; Struct; Struct = Struct->GetSuperStruct())
	{
		if (Struct == Super)
			return Distance;
		Distance++;
	}

	return Distance;
}

FRotator UTGOR_Math::Conv_QuatToRotator(FQuat InQuat)
{
	return(InQuat.Rotator());
}

FVector UTGOR_Math::Normalize(const FVector& Vector)
{
	/*
	const FVector Normal = Vector.GetSafeNormal();
	if (Normal.Size() > 1.1f)
	{
		ERRET("Normal computation borked!", Error, FVector::ZeroVector);
	}
	return Normal;
	*/

	const float Sqr = Vector.SizeSquared();
	if (Sqr < KINDA_SMALL_NUMBER) return FVector::ZeroVector;
	return Vector / FMath::Sqrt(Sqr);
}

FVector UTGOR_Math::ClampToSize(const FVector& Vector, float Limit)
{
	if (Limit < KINDA_SMALL_NUMBER)
	{
		return FVector::ZeroVector;
	}

	const float Sqr = Vector.SizeSquared();
	if (Sqr > Limit * Limit)
	{
		const float Scale = Limit / FMath::Sqrt(Sqr);
		return Vector * Scale;
	}
	return Vector;
}

float UTGOR_Math::SmoothStep(float X, int32 Order)
{
	// See https://en.wikipedia.org/wiki/Smoothstep
	float Out = 0.0f;
	for (int i = 0; i <= Order; i++)
	{
		const float A = PascalTriangle(-Order - 1, i);
		const float B = PascalTriangle(2 * Order + 1, Order - i);
		Out += A * B * IntPow(X, Order + i + 1);
	}
	return Out;
}

float UTGOR_Math::PascalTriangle(int32 A, int32 B)
{
	float Out = 1;
	for (int32 i = 0; i < B; ++i)
	{
		Out *= float(A - i) / (i + 1);
	}
	return Out;
}

float UTGOR_Math::IntPow(float Base, int32 Exp)
{
	// See https://stackoverflow.com/questions/101439/the-most-efficient-way-to-implement-an-integer-based-power-function-powint-int
	float Out = 1.0f;
	for (;;)
	{
		if (Exp & 1) Out *= Base;
		Exp >>= 1;
		if (!Exp) break;
		Base *= Base;
	}
	return Out;
}