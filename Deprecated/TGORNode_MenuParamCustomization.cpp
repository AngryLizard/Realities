// The Gateway of Realities: Planes of Existence.


#include "TGORNode_MenuParamCustomization.h"
#include "UI/TGOR_HUD.h"
#include "STextComboBox.h"

#include "Editor/PropertyEditor/Public/PropertyHandle.h"
#include "Editor/PropertyEditor/Public/DetailWidgetRow.h"
#include "Editor/PropertyEditor/Public/DetailLayoutBuilder.h"

#include "Nodes/Pins/TGORNode_MenuVarParamPin.h"


#define LOCTEXT_NAMESPACE "MenuParamCustomization"

TSharedRef<IPropertyTypeCustomization> FTGORNode_MenuParamCustomization::MakeInstance()
{
	return MakeShareable(new FTGORNode_MenuParamCustomization());
}

void FTGORNode_MenuParamCustomization::CustomizeHeader(TSharedRef<class IPropertyHandle> StructPropertyHandle, class FDetailWidgetRow& HeaderRow, IPropertyTypeCustomizationUtils& StructCustomizationUtils)
{
	MenuParamUPropertyHandle = StructPropertyHandle;

	check(MenuParamUPropertyHandle.IsValid());

	FString Value;
	MenuParamUPropertyHandle->GetValueAsFormattedString(Value);
	
	ATGOR_HUD* Hud = AcquireRelevantHud(StructPropertyHandle);
	SGATGORNode_MenuParamPin::PopulateList(Hud, AttributesList, NamesList);
	const int32 Selected = SGATGORNode_MenuParamPin::GetSelected(Value, NamesList);

	if (AttributesList.IsValidIndex(Selected))
	{
		// Get pin entry
		TSharedPtr<FString>& Default = AttributesList[Selected];

		HeaderRow.NameContent()
			[
				MenuParamUPropertyHandle->CreatePropertyNameWidget(MenuParamUPropertyHandle->GetPropertyDisplayName(), MenuParamUPropertyHandle->GetPropertyDisplayName(), false)
			]
			.ValueContent()
			.MinDesiredWidth(500)
			[
				SNew(STextComboBox)
				.InitiallySelectedItem(Default)
				.OptionsSource(&AttributesList)
				.OnComboBoxOpening(this, &FTGORNode_MenuParamCustomization::OnComboBoxOpening)
				.OnSelectionChanged(this, &FTGORNode_MenuParamCustomization::OnAttributeSelected)
			];
	}
	else
	{
		HeaderRow.NameContent()
			[
				MenuParamUPropertyHandle->CreatePropertyNameWidget(MenuParamUPropertyHandle->GetPropertyDisplayName(), MenuParamUPropertyHandle->GetPropertyDisplayName(), false)
			]
			.ValueContent()
			.MinDesiredWidth(500)
			[
				SNew(STextComboBox)
				.OptionsSource(&AttributesList)
				.OnComboBoxOpening(this, &FTGORNode_MenuParamCustomization::OnComboBoxOpening)
				.OnSelectionChanged(this, &FTGORNode_MenuParamCustomization::OnAttributeSelected)
			];

		if (AttributesList.Num() > 0)
		{
			OnAttributeSelected(AttributesList[0], ESelectInfo::Type::Direct);
		}
	}
}

void FTGORNode_MenuParamCustomization::CustomizeChildren(TSharedRef<class IPropertyHandle> StructPropertyHandle, class IDetailChildrenBuilder& StructBuilder, IPropertyTypeCustomizationUtils& StructCustomizationUtils)
{
	//Create further customization here
}


ATGOR_HUD* FTGORNode_MenuParamCustomization::AcquireRelevantHud(TSharedRef<class IPropertyHandle> StructPropertyHandle)
{
	TArray<UObject*> Objects;
	StructPropertyHandle->GetOuterObjects(Objects);
	for (UObject* Object : Objects)
	{
		if (Object->IsA(ATGOR_HUD::StaticClass()))
		{
			return(Object->GetClass()->GetDefaultObject<ATGOR_HUD>());
		}
	}
	return(nullptr);
}

void FTGORNode_MenuParamCustomization::OnComboBoxOpening()
{
	
}

void FTGORNode_MenuParamCustomization::OnAttributeSelected(TSharedPtr<FString> ItemSelected, ESelectInfo::Type SelectInfo)
{
	FString CurrentDefaultValue = "";
	MenuParamUPropertyHandle->GetValueAsFormattedString(CurrentDefaultValue);
	uint32 Index = AttributesList.IndexOfByKey(ItemSelected);

	FString AttributeString = SGATGORNode_MenuParamPin::SetSelected(CurrentDefaultValue, Index, NamesList);
	MenuParamUPropertyHandle->SetValueFromFormattedString(AttributeString);
}


#undef LOCTEXT_NAMESPACE