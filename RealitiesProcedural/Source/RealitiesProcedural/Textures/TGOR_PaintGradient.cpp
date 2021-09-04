// Copyright Epic Games, Inc. All Rights Reserved.

#include "TGOR_PaintGradient.h"
#include "RealitiesUtility/Utility/TGOR_Error.h"


FTGOR_GradientFilter::FTGOR_GradientFilter()
{
}

FLinearColor FTGOR_GradientFilter::HSVFilter(const FLinearColor& HSV, float Time) const
{
	FLinearColor Out;

	/*
	// Apply "vibrancy" adjustment
	const float VibranceSample = Vibrance;
	if (!FMath::IsNearlyZero(VibranceSample, (float)KINDA_SMALL_NUMBER))
	{
		const float Half = FMath::Clamp(VibranceSample, 0.0f, 1.0f) * 0.5f;
		Sat += Half * UTGOR_Math::IntPow(1.0f - Sat, 5);
	}
	*/

	const float HueSample = Hue.Sample(Time) * 180.0f;
	Out.R = HSV.R + HueSample;

	const float SaturationSample = Saturation.Sample(Time);
	Out.G = HSV.G * (SaturationSample + 1.0);

	const float ValueSample = Value.Sample(Time);
	Out.B = HSV.B * (ValueSample + 1.0);

	Out.R = FMath::Fmod(Out.R, 360.0f);
	if (Out.R < 0.0f) Out.R += 360.0f;
	Out.G = FMath::Clamp(Out.G, 0.0f, 1.0f);
	Out.B = FMath::Clamp(Out.B, 0.0f, 1.0f);

	Out.A = HSV.A;
	return Out;
}

FLinearColor FTGOR_GradientFilter::RGBFilter(const FLinearColor& RGB, float Time) const
{
	const float MagentaGreenSample = MagentaGreen.Sample(Time);
	const float TemperatureSample = Temperature.Sample(Time);

	FLinearColor ColorOffset;
	ColorOffset.R = -1.335087f * MagentaGreenSample + -0.907013f * TemperatureSample;
	ColorOffset.G = 0.5070821f * MagentaGreenSample + 0.1607365f * TemperatureSample;
	ColorOffset.B = -1.210441f * MagentaGreenSample + 1.0677497f * TemperatureSample;
	ColorOffset.A = 0.0f;

	const float Dot = (RGB.R * 0.21f + RGB.G * 0.72f + RGB.B * 0.07f);

	FLinearColor Out = RGB + Dot * ColorOffset;
	Out.R = FMath::Clamp(Out.R, 0.0f, 1.0f);
	Out.G = FMath::Clamp(Out.G, 0.0f, 1.0f);
	Out.B = FMath::Clamp(Out.B, 0.0f, 1.0f);
	Out.A = FMath::Clamp(Out.A, 0.0f, 1.0f);
	return Out;
}


////////////////////////////////////////////////////////////////////////////////////////////////////

FTGOR_GradientSection::FTGOR_GradientSection()
{
}

FTGOR_GradientCache::FTGOR_GradientCache()
:	SizeX(1),
	SizeY(1),
	Height(0),
	Offset(0)
{
}


////////////////////////////////////////////////////////////////////////////////////////////////////

UTGOR_PaintGradient::UTGOR_PaintGradient(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer), CacheDirty(false)
{
}


#if WITH_EDITOR
void UTGOR_PaintGradient::UpdateCurveSlot(UCurveBase* Curve, EPropertyChangeType::Type ChangeType)
{
	CacheDirty = true;
	UpdateResource();
}
#endif

void UTGOR_PaintGradient::SampleGradientCurves()
{
	const int32 SizeX = GetSizeX();
	const int32 SizeY = GetSizeY();

	// Clear cache if empty
	const int32 SectionNum = Sections.Num();
	if (PixelCache.Num() != SectionNum)
	{
		PixelCache.Empty();
	}

	// Build cache
	for (int32 Index = 0; Index < SectionNum; Index++)
	{
		const FTGOR_GradientSection& Section = Sections[Index];
		if (PixelCache.Num() <= Index)
		{
			PixelCache.Emplace(FTGOR_GradientCache());
		}

		// Check whether cache is valid
		FTGOR_GradientCache& Cache = PixelCache[Index];
		if (Section.Curves != Cache.Curves || SizeX != Cache.SizeX || SizeY != Cache.SizeY || CacheDirty)
		{

#if WITH_EDITOR
			// Remove previous delegates in case curves got removed
			for (UCurveLinearColor* Curve : Cache.Curves)
			{
				if (Curve != nullptr)
				{
					Curve->OnUpdateCurve.RemoveAll(this);
				}
			}
#endif

			Cache.SizeX = SizeX;
			Cache.SizeY = SizeY;
			Cache.Height = SizeY / SectionNum;
			Cache.Offset = Index * Cache.Height;
			Cache.Curves = Section.Curves;
			Cache.Pixels.SetNum(Cache.Height * SizeX);

#if WITH_EDITOR
			// Make sure we update when curve changes
			for (UCurveLinearColor* Curve : Cache.Curves)
			{
				if (Curve != nullptr)
				{
					Curve->OnUpdateCurve.AddUObject(this, &UTGOR_PaintGradient::UpdateCurveSlot);
				}
			}
#endif
			
			for (int32 Y = 0; Y < Cache.Height; Y++)
			{
				if (Section.Curves.Num() == 0)
				{
					for (int32 X = 0; X < SizeX; X++)
					{
						Cache.Pixels[SizeX * Y + X] = FLinearColor::White.LinearRGBToHSV();
					}
				}
				else
				{
					const float RatioY = ((float)Y) / Cache.Height;
					const float Ratio = (Section.Curves.Num() - 1) * RatioY;
					const int32 Curve = FMath::FloorToInt((Section.Curves.Num() - 1) * RatioY);

					const UCurveLinearColor* From = Section.Curves[Curve];
					const UCurveLinearColor* To = Section.Curves[FMath::Min(Curve + 1, Section.Curves.Num() - 1)];

					for (int32 X = 0; X < SizeX; X++)
					{
						const float RatioX = ((float)X) / SizeX;
						const FLinearColor FromSample = IsValid(From) ? From->GetClampedLinearColorValue(RatioX) : FLinearColor::White;
						const FLinearColor ToSample = IsValid(To) ? To->GetClampedLinearColorValue(RatioX) : FLinearColor::White;
						const float Alpha = FMath::SmoothStep(0.0f, 1.0f, Ratio - (float)Curve);
						Cache.Pixels[SizeX * Y + X] = FMath::Lerp(FromSample, ToSample, Alpha).LinearRGBToHSV();
					}
				}
			}
		}
	}
	CacheDirty = false;
}

void UTGOR_PaintGradient::GenerateGradient(int32 SizeX, int32 SizeY, TArray<FLinearColor>& Pixels)
{
	SampleGradientCurves();

	const int32 SectionNum = Sections.Num();
	for (int32 Index = 0; Index < SectionNum; Index++)
	{
		const FTGOR_GradientSection& Section = Sections[Index];
		const FTGOR_GradientCache& Cache = PixelCache[Index];

		for (int32 Y = 0; Y < Cache.Height; Y++)
		{
			const float TimeY = ((float)Y) / Cache.Height;

			for (int32 X = 0; X < SizeX; X++)
			{
				const float TimeX = ((float)X) / SizeX;

				FLinearColor HSV = Cache.Pixels[SizeX * Y + X];
				HSV = Section.Horizontal.HSVFilter(HSV, TimeX);
				HSV = Section.Vertical.HSVFilter(HSV, TimeY);

				FLinearColor RGB = HSV.HSVToLinearRGB();
				RGB = Section.Horizontal.RGBFilter(RGB, TimeX);
				RGB = Section.Vertical.RGBFilter(RGB, TimeY);

				Pixels[SizeX * (Cache.Offset + Y) + X] = RGB;
			}
		}
	}
}

