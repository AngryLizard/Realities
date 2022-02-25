// The Gateway of Realities: Planes of Existence.


#include "TGORNode_MenuContentTypes.h"

#include "Editor/PropertyEditor/Public/PropertyHandle.h"
#include "Editor/PropertyEditor/Public/DetailWidgetRow.h"
#include "Editor/PropertyEditor/Public/DetailLayoutBuilder.h"


#define LOCTEXT_NAMESPACE "MenuParamCustomization"

TSharedRef<IPropertyTypeCustomization> FTGORNode_MenuContentTypes::MakeInstance()
{
	return MakeShareable(new FTGORNode_MenuContentTypes());
}

void FTGORNode_MenuContentTypes::CustomizeHeader(TSharedRef<class IPropertyHandle> StructPropertyHandle, class FDetailWidgetRow& HeaderRow, IPropertyTypeCustomizationUtils& StructCustomizationUtils)
{
	MenuContentTypesHandle = StructPropertyHandle;

	check(MenuContentTypesHandle.IsValid());

}

void FTGORNode_MenuContentTypes::CustomizeChildren(TSharedRef<class IPropertyHandle> StructPropertyHandle, class IDetailChildrenBuilder& StructBuilder, IPropertyTypeCustomizationUtils& StructCustomizationUtils)
{
	//Create further customization here
}

#undef LOCTEXT_NAMESPACE