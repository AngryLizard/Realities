// Copyright Epic Games, Inc. All Rights Reserved.

#include "TGOR_NormalCurveGradient.h"
#include "Rendering/DrawElements.h"
#if WITH_ACCESSIBILITY
#include "Widgets/Accessibility/SlateCoreAccessibleWidgets.h"
#endif

void STGOR_NormalCurveGradient::Construct(const FArguments& InArgs)
{
	Curve = InArgs._Curve;
	Size = InArgs._Size;
	OffColor = InArgs._OffColor;
	OnColor = InArgs._OnColor;
	SetOnMouseButtonDown(InArgs._OnMouseButtonDown);
}

int32 STGOR_NormalCurveGradient::OnPaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const
{
	const FVector& Props = Curve.Get();
	const FLinearColor& OffCol = OffColor.Get();
	const FLinearColor& OnCol = OnColor.Get();
	
	TArray<FSlateGradientStop> Stops;
	const FVector2D& Dims = AllottedGeometry.GetLocalSize();

	const int32 Steps = 16;
	for (int32 Step = 0; Step < Steps; Step++)
	{
		const float Time = ((float)Step) / Steps;

		const float A = FMath::Lerp(Props.X, Props.Y, Time);
		const float B = FMath::Lerp(Props.Y, Props.Z, Time);
		const float Alpha = FMath::Lerp(A, B, Time);
		
		Stops.Emplace(FSlateGradientStop(Dims * Time, FMath::Lerp(OffCol, OnCol, Alpha)));
	}
	
	FSlateDrawElement::MakeGradient(OutDrawElements, LayerId, AllottedGeometry.ToPaintGeometry(), Stops, EOrientation::Orient_Vertical, ESlateDrawEffect::None);
	return LayerId;
}

FVector2D STGOR_NormalCurveGradient::ComputeDesiredSize(float) const
{
	return Size.Get();
}


void STGOR_NormalCurveGradient::SetCurve(const TAttribute<FVector>& InCurve)
{
	SetAttribute(Curve, InCurve, EInvalidateWidgetReason::Paint);
}

void STGOR_NormalCurveGradient::SetSize(const TAttribute<FVector2D>& InSize)
{
	SetAttribute(Size, InSize, EInvalidateWidgetReason::Paint);
}

void STGOR_NormalCurveGradient::SetOffColor(const TAttribute<FLinearColor>& InColor)
{
	SetAttribute(OffColor, InColor, EInvalidateWidgetReason::Paint);
}

void STGOR_NormalCurveGradient::SetOnColor(const TAttribute<FLinearColor>& InColor)
{
	SetAttribute(OnColor, InColor, EInvalidateWidgetReason::Paint);
}

#if WITH_ACCESSIBILITY
TSharedRef<FSlateAccessibleWidget> STGOR_NormalCurveGradient::CreateAccessibleWidget()
{
	return MakeShareable<FSlateAccessibleWidget>(new FSlateAccessibleImage(SharedThis(this)));
}
#endif
