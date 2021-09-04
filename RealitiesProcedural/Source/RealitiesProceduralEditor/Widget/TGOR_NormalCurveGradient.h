#pragma once

#include "CoreMinimal.h"
#include "Misc/Attribute.h"
#include "Widgets/DeclarativeSyntaxSupport.h"
#include "Styling/SlateColor.h"
#include "Widgets/SLeafWidget.h"

class FPaintArgs;
class FSlateWindowElementList;

/**
 * Implements a widget that displays an image with a desired width and height.
 */
class REALITIESPROCEDURALEDITOR_API STGOR_NormalCurveGradient : public SLeafWidget
{
public:
	SLATE_BEGIN_ARGS(STGOR_NormalCurveGradient)
		: _Curve(FVector::ZeroVector), _Size(FVector2D(100.0f, 100.0f))
	{}

	/** Curve to be displayed */
	SLATE_ATTRIBUTE(FVector, Curve)

	/** Color to be displayed */
	SLATE_ATTRIBUTE(FLinearColor, OffColor)

	/** Color to be displayed */
	SLATE_ATTRIBUTE(FLinearColor, OnColor)

	/** Size to be displayed */
	SLATE_ATTRIBUTE(FVector2D, Size)

	/** Invoked when the mouse is pressed in the widget. */
	SLATE_EVENT(FPointerEventHandler, OnMouseButtonDown)
	SLATE_END_ARGS()

	/** Constructor */
	STGOR_NormalCurveGradient()
	{
		SetCanTick(false);
		bCanSupportFocus = false;
	}

	/**
	 * Construct this widget
	 *
	 * @param	InArgs	The declaration data for this widget
	 */
	void Construct(const FArguments& InArgs);

public:

	/** See the Curve attribute */
	void SetCurve(const TAttribute<FVector>& InCurve);

	/** See the Color attribute */
	void SetOffColor(const TAttribute<FLinearColor>& InOffColor);

	/** See the Color attribute */
	void SetOnColor(const TAttribute<FLinearColor>& InOnColor);

	/** See the Size attribute */
	void SetSize(const TAttribute<FVector2D>& InSize);

public:

	// SWidget overrides
	virtual int32 OnPaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const override;
#if WITH_ACCESSIBILITY
	virtual TSharedRef<FSlateAccessibleWidget> CreateAccessibleWidget() override;
#endif

protected:
	// Begin SWidget overrides.
	virtual FVector2D ComputeDesiredSize(float) const override;
	// End SWidget overrides.

protected:

	/** The slate curve to draw that we can invalidate. */
	TAttribute<FVector> Curve;

	/** The slate size to draw that we can invalidate. */
	TAttribute<FVector2D> Size;

	/** The slate color to draw that we can invalidate. */
	TAttribute<FLinearColor> OffColor;

	/** The slate color to draw that we can invalidate. */
	TAttribute<FLinearColor> OnColor;

	/** Invoked when the mouse is pressed in the image */
	FPointerEventHandler OnMouseButtonDownHandler;
};
