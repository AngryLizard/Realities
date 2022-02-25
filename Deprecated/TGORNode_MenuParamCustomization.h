// The Gateway of Realities: Planes of Existence.

#pragma once

#include "CoreMinimal.h"
#include "Editor/PropertyEditor/Public/IPropertyTypeCustomization.h"

class ATGOR_HUD;

class FTGORNode_MenuParamCustomization : public IPropertyTypeCustomization
{
public:
	static TSharedRef<IPropertyTypeCustomization> MakeInstance();

	/** IPropertyTypeCustomization interface */
	virtual void CustomizeHeader(TSharedRef<class IPropertyHandle> StructPropertyHandle, class FDetailWidgetRow& HeaderRow, IPropertyTypeCustomizationUtils& StructCustomizationUtils) override;
	virtual void CustomizeChildren(TSharedRef<class IPropertyHandle> StructPropertyHandle, class IDetailChildrenBuilder& StructBuilder, IPropertyTypeCustomizationUtils& StructCustomizationUtils) override;

	ATGOR_HUD* AcquireRelevantHud(TSharedRef<class IPropertyHandle> StructPropertyHandle);

	void OnComboBoxOpening();
	void OnAttributeSelected(TSharedPtr<FString> ItemSelected, ESelectInfo::Type SelectInfo);

private:
	TSharedPtr<IPropertyHandle> MenuParamUPropertyHandle;

	TArray<TSharedPtr<FString>> AttributesList;
	TArray<FName> NamesList;
};