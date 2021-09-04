// The Gateway of Realities: Planes of Existence.


#include "TGORNode_UnitCurve.h"
#include "Widgets/Input/STextComboBox.h"
#include "Widgets/Input/SNumericEntryBox.h"

#include "Editor/PropertyEditor/Public/PropertyHandle.h"
#include "Editor/PropertyEditor/Public/DetailWidgetRow.h"
#include "RealitiesUtility/Utility/TGOR_Math.h"
#include "SCurveEditor.h"

#include "RealitiesProceduralEditor/Widget/TGOR_NormalCurveGradient.h"


#include "UnrealEd.h"

#define LOCTEXT_NAMESPACE "UnitCurve"

TSharedRef<IPropertyTypeCustomization> FTGORNode_UnitCurve::MakeInstance()
{
	return MakeShareable(new FTGORNode_UnitCurve());
}

void FTGORNode_UnitCurve::CustomizeHeader(TSharedRef<class IPropertyHandle> StructPropertyHandle, class FDetailWidgetRow& HeaderRow, IPropertyTypeCustomizationUtils& StructCustomizationUtils)
{
	UnitCurveUPropertyHandle = StructPropertyHandle;
	check(UnitCurveUPropertyHandle.IsValid());


	TArray<UObject*> OuterObjects;
	UnitCurveUPropertyHandle->GetOuterObjects(OuterObjects);

	TArray<void*> StructPtrs;
	UnitCurveUPropertyHandle->AccessRawData(StructPtrs);

	if (StructPtrs.Num() == 1)
	{
		static const FName XAxisName(TEXT("XAxisName"));
		static const FName YAxisName(TEXT("YAxisName"));

		TOptional<FString> XAxisString;
		if (UnitCurveUPropertyHandle->HasMetaData(XAxisName))
		{
			XAxisString = UnitCurveUPropertyHandle->GetMetaData(XAxisName);
		}

		TOptional<FString> YAxisString;
		if (UnitCurveUPropertyHandle->HasMetaData(YAxisName))
		{
			YAxisString = UnitCurveUPropertyHandle->GetMetaData(YAxisName);
		}

		UnitCurve = reinterpret_cast<FTGOR_UnitCurve*>(StructPtrs[0]);

		if (OuterObjects.Num() == 1)
		{
			Owner = OuterObjects[0];
		}

		HeaderRow
			.NameContent()
			[
				UnitCurveUPropertyHandle->CreatePropertyNameWidget()
			]
			.ValueContent()
			.HAlign(HAlign_Fill)
			.MinDesiredWidth(200)
			[
				SNew(SBorder)
				.VAlign(VAlign_Fill)
				[
					SAssignNew(CurveWidget, SCurveEditor)
					.ViewMinInput(-0.1f)
					.ViewMaxInput(1.1f)
					.ViewMinOutput(-1.1f)
					.ViewMaxOutput(1.1f)
					.ZoomToFitVertical(false)
					.ZoomToFitHorizontal(false)
					.TimelineLength(1.0f)
					.XAxisName(XAxisString)
					.YAxisName(YAxisString)
					.HideUI(false)
					.AllowZoomOutput(false)
					.ShowZoomButtons(false)
					.DesiredSize(FVector2D(300, 100))
				]
			];

		check(CurveWidget.IsValid());
		CurveWidget->SetCurveOwner(this, UnitCurveUPropertyHandle->IsEditable());
	}
	else
	{
		HeaderRow
			.NameContent()
			[
				UnitCurveUPropertyHandle->CreatePropertyNameWidget()
			]
		.ValueContent()
			[
				SNew(SBorder)
				.VAlign(VAlign_Fill)
			[
				SNew(STextBlock)
				.Text(StructPtrs.Num() == 0 ? LOCTEXT("NoCurves", "No Curves - unable to modify") : LOCTEXT("MultipleCurves", "Multiple Curves - unable to modify"))
			]
			];
	}
}


void FTGORNode_UnitCurve::CustomizeChildren(TSharedRef<class IPropertyHandle> StructPropertyHandle, class IDetailChildrenBuilder& StructBuilder, IPropertyTypeCustomizationUtils& StructCustomizationUtils)
{
	//Create further customization here
}




TArray<FRichCurveEditInfoConst> FTGORNode_UnitCurve::GetCurves() const
{
	TArray<FRichCurveEditInfoConst> Curves;
	Curves.Add(FRichCurveEditInfoConst(&UnitCurve->CurveData));
	return Curves;
}

TArray<FRichCurveEditInfo> FTGORNode_UnitCurve::GetCurves()
{
	TArray<FRichCurveEditInfo> Curves;
	Curves.Add(FRichCurveEditInfo(&UnitCurve->CurveData));
	return Curves;
}

void FTGORNode_UnitCurve::ModifyOwner()
{
	if (Owner)
	{
		Owner->Modify(true);
	}
}

TArray<const UObject*> FTGORNode_UnitCurve::GetOwners() const
{
	TArray<const UObject*> Owners;
	if (Owner)
	{
		Owners.Add(Owner);
	}

	return Owners;
}

void FTGORNode_UnitCurve::MakeTransactional()
{
	if (Owner)
	{
		Owner->SetFlags(Owner->GetFlags() | RF_Transactional);
	}
}

void FTGORNode_UnitCurve::OnCurveChanged(const TArray<FRichCurveEditInfo>& ChangedCurveEditInfos)
{
	UnitCurveUPropertyHandle->NotifyPostChange();
}

bool FTGORNode_UnitCurve::IsValidCurve(FRichCurveEditInfo CurveInfo)
{
	return CurveInfo.CurveToEdit == &UnitCurve->CurveData;
}




#undef LOCTEXT_NAMESPACE