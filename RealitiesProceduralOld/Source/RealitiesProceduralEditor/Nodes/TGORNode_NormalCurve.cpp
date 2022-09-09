// The Gateway of Realities: Planes of Existence.


#include "TGORNode_NormalCurve.h"
#include "Widgets/Input/STextComboBox.h"
#include "Widgets/Input/SNumericEntryBox.h"

#include "Editor/PropertyEditor/Public/PropertyHandle.h"
#include "Editor/PropertyEditor/Public/DetailWidgetRow.h"
#include "RealitiesUtility/Utility/TGOR_Math.h"
#include "ClassViewerModule.h"
#include "ClassViewerFilter.h"

#include "RealitiesProceduralEditor/Widget/TGOR_NormalCurveGradient.h"


#include "UnrealEd.h"

#define LOCTEXT_NAMESPACE "NormalCurve"


class FPortalSlectionClassFilter : public IClassViewerFilter
{
public:
	virtual bool IsClassAllowed(const FClassViewerInitializationOptions& InInitOptions, const UClass* InClass, TSharedRef< FClassViewerFilterFuncs > InFilterFuncs) override
	{
		bool bMatchesFlags = !InClass->HasAnyClassFlags(CLASS_Hidden | CLASS_HideDropDown | CLASS_Deprecated);
		return  bMatchesFlags;//&& InClass->IsChildOf(UTGOR_Dimension::StaticClass());
	}

	virtual bool IsUnloadedClassAllowed(const FClassViewerInitializationOptions& InInitOptions, const TSharedRef< const IUnloadedBlueprintData > InClass, TSharedRef< FClassViewerFilterFuncs > InFilterFuncs) override
	{
		bool bMatchesFlags = !InClass->HasAnyClassFlags(CLASS_Hidden | CLASS_HideDropDown | CLASS_Deprecated);
		return  bMatchesFlags;// && InClass->IsChildOf(UTGOR_Dimension::StaticClass());
	}
};



TSharedRef<IPropertyTypeCustomization> FTGORNode_NormalCurve::MakeInstance()
{
	return MakeShareable(new FTGORNode_NormalCurve());
}

struct FNormalCurveTypeInterface : public TDefaultNumericTypeInterface<float>
{
	virtual FString ToString(const float& Value) const override
	{
		static const FNumberFormattingOptions NumberFormattingOptions = FNumberFormattingOptions()
			.SetUseGrouping(false)
			.SetMinimumFractionalDigits(0)
			.SetMaximumFractionalDigits(3);
		FString NumberString = FastDecimalFormat::NumberToString(Value, ExpressionParser::GetLocalizedNumberFormattingRules(), NumberFormattingOptions);
		return NumberString;
	}

	virtual TOptional<float> FromString(const FString& InString, const float& ExistingValue)
	{
		TOptional<float> ParsedValue = TDefaultNumericTypeInterface<float>::FromString(InString, ExistingValue);
		if (ParsedValue.IsSet())
		{
			return ParsedValue.GetValue();
		}
		return ParsedValue;
	}
};

void FTGORNode_NormalCurve::CustomizeHeader(TSharedRef<class IPropertyHandle> StructPropertyHandle, class FDetailWidgetRow& HeaderRow, IPropertyTypeCustomizationUtils& StructCustomizationUtils)
{
	NormalCurveUPropertyHandle = StructPropertyHandle;
	check(NormalCurveUPropertyHandle.IsValid());

	const FString ColorName = NormalCurveUPropertyHandle->GetMetaData("Color");
	const FLinearColor OnColor = GetColor(ColorName);
	const FLinearColor OffColor = OnColor * FLinearColor(0.1f, 0.1f, 0.1f, 1.0f);

	ParseValue();

	HeaderRow.NameContent()
		[
			NormalCurveUPropertyHandle->CreatePropertyNameWidget(NormalCurveUPropertyHandle->GetPropertyDisplayName(), NormalCurveUPropertyHandle->GetPropertyDisplayName(), false)
		]
		.ValueContent()
		.MinDesiredWidth(500)
		[
			SNew(SSplitter)
			+ SSplitter::Slot()
			[
				SNew(SHorizontalBox)
				+ SHorizontalBox::Slot()
				.HAlign(EHorizontalAlignment::HAlign_Left)
				.AutoWidth()
				.Padding(10.0f, 0.0f)
				[
					SNew(SVerticalBox)
					+ SVerticalBox::Slot()
					[
						SNew(SNumericEntryBox<float>)
						.MinValue(0.0f)
						.MinSliderValue(0.0f)
						.MaxValue(1.0f)
						.MaxSliderValue(1.0f)
						.AllowSpin(true)
						.MinDesiredValueWidth(50.0f)
						.TypeInterface(MakeShareable(new FNormalCurveTypeInterface))
						.OnValueChanged(this, &FTGORNode_NormalCurve::OnXChanged, ENormalCurveDirection::Left)
						.Value(this, &FTGORNode_NormalCurve::GetXOptional, ENormalCurveDirection::Left)
					]
					+ SVerticalBox::Slot()
					[
						SNew(SNumericEntryBox<float>)
						.MinValue(0.0f)
						.MinSliderValue(0.0f)
						.MaxValue(1.0f)
						.MaxSliderValue(1.0f)
						.AllowSpin(true)
						.MinDesiredValueWidth(50.0f)
						.TypeInterface(MakeShareable(new FNormalCurveTypeInterface))
						.OnValueChanged(this, &FTGORNode_NormalCurve::OnYChanged, ENormalCurveDirection::Left)
						.Value(this, &FTGORNode_NormalCurve::GetYOptional, ENormalCurveDirection::Left)
					]
					+ SVerticalBox::Slot()
					[
						SNew(SNumericEntryBox<float>)
						.MinValue(0.0f)
						.MinSliderValue(0.0f)
						.MaxValue(1.0f)
						.MaxSliderValue(1.0f)
						.AllowSpin(true)
						.MinDesiredValueWidth(50.0f)
						.TypeInterface(MakeShareable(new FNormalCurveTypeInterface))
						.OnValueChanged(this, &FTGORNode_NormalCurve::OnZChanged, ENormalCurveDirection::Left)
						.Value(this, &FTGORNode_NormalCurve::GetZOptional, ENormalCurveDirection::Left)
					]
				]
				+ SHorizontalBox::Slot()
				.HAlign(EHorizontalAlignment::HAlign_Fill)
				[
					SNew(SOverlay)
					+ SOverlay::Slot()
				[
					SNew(STGOR_NormalCurveGradient)
					.Size(FVector2D(16, 4))
					.Curve(this, &FTGORNode_NormalCurve::GetCurve, ENormalCurveDirection::Left)
					.OffColor(OffColor)
					.OnColor(OnColor)
				]
				+ SOverlay::Slot()
					.HAlign(EHorizontalAlignment::HAlign_Fill)
					[
						SNew(SHorizontalBox)
						+ SHorizontalBox::Slot()
						.HAlign(EHorizontalAlignment::HAlign_Left)
						.Padding(5.0f, 0.0f)
						.AutoWidth()
						[
							SNew(SSlider)
							.Orientation(EOrientation::Orient_Vertical)
							.MinValue(0.0)
							.MaxValue(1.0)
							.StepSize(0.01)
							.OnValueChanged(this, &FTGORNode_NormalCurve::OnXChanged, ENormalCurveDirection::Left)
							.Value(this, &FTGORNode_NormalCurve::GetXValue, ENormalCurveDirection::Left)
						]
						+ SHorizontalBox::Slot()
						.HAlign(EHorizontalAlignment::HAlign_Center)
						.Padding(5.0f, 0.0f)
						.AutoWidth()
						[
							SNew(SSlider)
							.Orientation(EOrientation::Orient_Vertical)
							.MinValue(0.0)
							.MaxValue(1.0)
							.StepSize(0.01)
							.OnValueChanged(this, &FTGORNode_NormalCurve::OnYChanged, ENormalCurveDirection::Left)
							.Value(this, &FTGORNode_NormalCurve::GetYValue, ENormalCurveDirection::Left)
						]
						+ SHorizontalBox::Slot()
						.HAlign(EHorizontalAlignment::HAlign_Right)
						.Padding(5.0f, 0.0f)
						.AutoWidth()
						[
							SNew(SSlider)
							.Orientation(EOrientation::Orient_Vertical)
							.MinValue(0.0)
							.MaxValue(1.0)
							.StepSize(0.01)
							.OnValueChanged(this, &FTGORNode_NormalCurve::OnZChanged, ENormalCurveDirection::Left)
							.Value(this, &FTGORNode_NormalCurve::GetZValue, ENormalCurveDirection::Left)
						]
					]
				]
			]

			+ SSplitter::Slot()
			[
				SNew(SHorizontalBox)
				+ SHorizontalBox::Slot()
				.HAlign(EHorizontalAlignment::HAlign_Fill)
				[
					SNew(SOverlay)
					+ SOverlay::Slot()
					[
						SNew(STGOR_NormalCurveGradient)
						.Size(FVector2D(16, 4))
						.Curve(this, &FTGORNode_NormalCurve::GetCurve, ENormalCurveDirection::Right)
						.OffColor(OffColor)
						.OnColor(OnColor)
					]
					+ SOverlay::Slot()
					.HAlign(EHorizontalAlignment::HAlign_Fill)
					[
						SNew(SHorizontalBox)
						+ SHorizontalBox::Slot()
						.HAlign(EHorizontalAlignment::HAlign_Left)
						.Padding(5.0f, 0.0f)
						.AutoWidth()
						[
							SNew(SSlider)
							.Orientation(EOrientation::Orient_Vertical)
							.MinValue(0.0)
							.MaxValue(1.0)
							.StepSize(0.01)
							.OnValueChanged(this, &FTGORNode_NormalCurve::OnXChanged, ENormalCurveDirection::Right)
							.Value(this, &FTGORNode_NormalCurve::GetXValue, ENormalCurveDirection::Right)
						]
						+ SHorizontalBox::Slot()
						.HAlign(EHorizontalAlignment::HAlign_Center)
						.Padding(5.0f, 0.0f)
						.AutoWidth()
						[
							SNew(SSlider)
							.Orientation(EOrientation::Orient_Vertical)
							.MinValue(0.0)
							.MaxValue(1.0)
							.StepSize(0.01)
							.OnValueChanged(this, &FTGORNode_NormalCurve::OnYChanged, ENormalCurveDirection::Right)
							.Value(this, &FTGORNode_NormalCurve::GetYValue, ENormalCurveDirection::Right)
						]
						+ SHorizontalBox::Slot()
						.HAlign(EHorizontalAlignment::HAlign_Right)
						.Padding(5.0f, 0.0f)
						.AutoWidth()
						[
							SNew(SSlider)
							.Orientation(EOrientation::Orient_Vertical)
							.MinValue(0.0)
							.MaxValue(1.0)
							.StepSize(0.01)
							.OnValueChanged(this, &FTGORNode_NormalCurve::OnZChanged, ENormalCurveDirection::Right)
							.Value(this, &FTGORNode_NormalCurve::GetZValue, ENormalCurveDirection::Right)
						]

					]
				]
				+ SHorizontalBox::Slot()
				.HAlign(EHorizontalAlignment::HAlign_Left)
				.AutoWidth()
				.Padding(10.0f, 0.0f)
				[
					SNew(SVerticalBox)
					+ SVerticalBox::Slot()
					[
						SNew(SNumericEntryBox<float>)
						.MinValue(0.0f)
						.MinSliderValue(0.0f)
						.MaxValue(1.0f)
						.MaxSliderValue(1.0f)
						.AllowSpin(true)
						.MinDesiredValueWidth(50.0f)
						.TypeInterface(MakeShareable(new FNormalCurveTypeInterface))
						.OnValueChanged(this, &FTGORNode_NormalCurve::OnXChanged, ENormalCurveDirection::Right)
						.Value(this, &FTGORNode_NormalCurve::GetXOptional, ENormalCurveDirection::Right)
					]
					+ SVerticalBox::Slot()
					[
						SNew(SNumericEntryBox<float>)
						.MinValue(0.0f)
						.MinSliderValue(0.0f)
						.MaxValue(1.0f)
						.MaxSliderValue(1.0f)
						.AllowSpin(true)
						.MinDesiredValueWidth(50.0f)
						.TypeInterface(MakeShareable(new FNormalCurveTypeInterface))
						.OnValueChanged(this, &FTGORNode_NormalCurve::OnYChanged, ENormalCurveDirection::Right)
						.Value(this, &FTGORNode_NormalCurve::GetYOptional, ENormalCurveDirection::Right)
					]
					+ SVerticalBox::Slot()
					[
						SNew(SNumericEntryBox<float>)
						.MinValue(0.0f)
						.MinSliderValue(0.0f)
						.MaxValue(1.0f)
						.MaxSliderValue(1.0f)
						.AllowSpin(true)
						.MinDesiredValueWidth(50.0f)
						.TypeInterface(MakeShareable(new FNormalCurveTypeInterface))
						.OnValueChanged(this, &FTGORNode_NormalCurve::OnZChanged, ENormalCurveDirection::Right)
						.Value(this, &FTGORNode_NormalCurve::GetZOptional, ENormalCurveDirection::Right)
					]
				]
			]
		];

}


void FTGORNode_NormalCurve::CustomizeChildren(TSharedRef<class IPropertyHandle> StructPropertyHandle, class IDetailChildrenBuilder& StructBuilder, IPropertyTypeCustomizationUtils& StructCustomizationUtils)
{
	//Create further customization here
}


FLinearColor FTGORNode_NormalCurve::GetColor(const FString& Name) const
{
	if (Name == "Red") return FLinearColor(0.9f, 0.2f, 0.2f, 1.0f);
	if (Name == "Green") return FLinearColor(0.2f, 0.9f, 0.2f, 1.0f);
	if (Name == "Blue") return FLinearColor(0.2f, 0.2f, 0.9f, 1.0f);
	if (Name == "Yellow") return FLinearColor(0.9f, 0.9f, 0.2f, 1.0f);
	if (Name == "Cyan") return FLinearColor(0.2f, 0.9f, 0.9f, 1.0f);
	if (Name == "Violet") return FLinearColor(0.9f, 0.2f, 0.9f, 1.0f);
	return FLinearColor::White;
}

void FTGORNode_NormalCurve::ParseValue()
{
	FString Formatted;
	NormalCurveUPropertyHandle->GetValueAsFormattedString(Formatted);


	FJsonSerializableArray Array;
	const TCHAR* Delims[4] = { L"(", L"=", L",", L")" };
	Formatted.ParseIntoArray(Array, Delims, 4, false);

	/*
	VPORT(Formatted);
	VPORT(Array.Num());
	for (int32 Index = 0; Index < Array.Num(); Index++)
	{
		VPORT(Array[Index]);
	}
	*/

	if (Array.Num() >= 20)
	{
		Props.X.X = FCString::Atof(*Array[4]);
		Props.X.Y = FCString::Atof(*Array[6]);
		Props.X.Z = FCString::Atof(*Array[8]);

		Props.Y.X = FCString::Atof(*Array[13]);
		Props.Y.Y = FCString::Atof(*Array[15]);
		Props.Y.Z = FCString::Atof(*Array[17]);
	}
	else
	{
		Props.X = FVector::ZeroVector;
		Props.Y = FVector::ZeroVector;
	}
}

void FTGORNode_NormalCurve::WriteValue()
{
	//(X = (X = 0.000000, Y = 0.000000, Z = 0.000000), Y = (X = 0.334000, Y = 0.000000, Z = 0.000000))


	FString Formatted = FString::Printf(TEXT("(X=(X=%f,Y=%f,Z=%f),Y=(X=%f,Y=%f,Z=%f))"), Props.X.X, Props.X.Y, Props.X.Z, Props.Y.X, Props.Y.Y, Props.Y.Z);
	NormalCurveUPropertyHandle->SetValueFromFormattedString(Formatted);
}

TOptional<float> FTGORNode_NormalCurve::GetXOptional(ENormalCurveDirection Dir) const
{
	return GetXValue(Dir);
}

float FTGORNode_NormalCurve::GetXValue(ENormalCurveDirection Dir) const
{
	if (Dir == ENormalCurveDirection::Left)
		return Props.X.X;
	return Props.Y.X;
}

void FTGORNode_NormalCurve::OnXChanged(float Value, ENormalCurveDirection Dir)
{
	if (Dir == ENormalCurveDirection::Left)
		Props.X.X = FMath::Clamp(Value, 0.0f, 1.0f);
	else
		Props.Y.X = FMath::Clamp(Value, 0.0f, 1.0f);
	WriteValue();
}


TOptional<float> FTGORNode_NormalCurve::GetYOptional(ENormalCurveDirection Dir) const
{
	return GetYValue(Dir);
}

float FTGORNode_NormalCurve::GetYValue(ENormalCurveDirection Dir) const
{
	if (Dir == ENormalCurveDirection::Left)
		return Props.X.Y;
	return Props.Y.Y;
}

void FTGORNode_NormalCurve::OnYChanged(float Value, ENormalCurveDirection Dir)
{
	if (Dir == ENormalCurveDirection::Left)
		Props.X.Y = FMath::Clamp(Value, 0.0f, 1.0f);
	else
		Props.Y.Y = FMath::Clamp(Value, 0.0f, 1.0f);
	WriteValue();
}


TOptional<float> FTGORNode_NormalCurve::GetZOptional(ENormalCurveDirection Dir) const
{
	return GetZValue(Dir);
}

float FTGORNode_NormalCurve::GetZValue(ENormalCurveDirection Dir) const
{
	if (Dir == ENormalCurveDirection::Left)
		return Props.X.Z;
	return Props.Y.Z;
}

void FTGORNode_NormalCurve::OnZChanged(float Value, ENormalCurveDirection Dir)
{
	if (Dir == ENormalCurveDirection::Left)
		Props.X.Z = FMath::Clamp(Value, 0.0f, 1.0f);
	else
		Props.Y.Z = FMath::Clamp(Value, 0.0f, 1.0f);
	WriteValue();
}

FVector FTGORNode_NormalCurve::GetCurve(ENormalCurveDirection Dir) const
{
	if (Dir == ENormalCurveDirection::Left)
		return Props.X;
	return Props.Y;
}

#undef LOCTEXT_NAMESPACE