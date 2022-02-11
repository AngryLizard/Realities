// The Gateway of Realities: Planes of Existence.

#pragma once

#include "CoreMinimal.h"
#include "Widgets/Input/SComboButton.h"
#include "Editor/PropertyEditor/Public/IPropertyTypeCustomization.h"

class UTGOR_Dimension;
class SEditableTextBox;

class FTGORNode_ConnectionSelection : public IPropertyTypeCustomization
{
public:
	static TSharedRef<IPropertyTypeCustomization> MakeInstance();

	/** IPropertyTypeCustomization interface */
	virtual void CustomizeHeader(TSharedRef<class IPropertyHandle> StructPropertyHandle, class FDetailWidgetRow& HeaderRow, IPropertyTypeCustomizationUtils& StructCustomizationUtils) override;
	virtual void CustomizeChildren(TSharedRef<class IPropertyHandle> StructPropertyHandle, class IDetailChildrenBuilder& StructBuilder, IPropertyTypeCustomizationUtils& StructCustomizationUtils) override;

	void ParseValue();
	void WriteValue();

	TSharedPtr<FString> PopulateList(bool UpdateValues);
	int32 GetSelected();

	TSharedRef<SWidget> OnCreateDimensionCombo();
	TSharedRef<SWidget> OnGetDimensionSelection() const;

	FText GetDisplayValueAsString() const;
	void OnMenuOpenChanged(bool bOpen);

	bool OnShouldFilterAsset(const FAssetData& InAssetData) const;
	void OnDimensionSelected(UClass* InClass);

	void OnComboBoxOpening();
	void OnAttributeSelected(TSharedPtr<FString> ItemSelected, ESelectInfo::Type SelectInfo);

	void OnSuffixChanged(const FText& Text);

private:
	TSharedPtr<IPropertyHandle> ConnectionSelectionUPropertyHandle;

	UClass* ClassProperty;
	FString SuffixProperty;
	FName ConnectionProperty;

	TSharedPtr<STextComboBox> ConnectionSelection;
	TSharedPtr<SComboButton> DimensionSelection;
	TSharedPtr<SEditableTextBox> DimensionSuffix;

	TArray<TSharedPtr<FString>> AttributesList;
	TArray<FName> NamesList;
};