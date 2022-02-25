// The Gateway of Realities: Planes of Existence.


#include "TGORNode_PortalSelection.h"
#include "Realities/Mod/Dimensions/TGOR_Dimension.h"
#include "Widgets/Input/STextComboBox.h"

#include "Editor/PropertyEditor/Public/PropertyHandle.h"
#include "Editor/PropertyEditor/Public/DetailWidgetRow.h"
#include "Editor/PropertyEditor/Public/DetailLayoutBuilder.h"
#include "ClassViewerModule.h"
#include "ClassViewerFilter.h"

#include "Editor/EditorWidgets/Public/SAssetDropTarget.h"
#include "Editor/PropertyEditor/Public/PropertyCustomizationHelpers.h"

#include "UnrealEd.h"

#define LOCTEXT_NAMESPACE "PortalSelection"


class FPortalSlectionClassFilter : public IClassViewerFilter
{
public:
	virtual bool IsClassAllowed(const FClassViewerInitializationOptions& InInitOptions, const UClass* InClass, TSharedRef< FClassViewerFilterFuncs > InFilterFuncs) override
	{
		bool bMatchesFlags = !InClass->HasAnyClassFlags(CLASS_Hidden | CLASS_HideDropDown | CLASS_Deprecated);
		return  bMatchesFlags && InClass->IsChildOf(UTGOR_Dimension::StaticClass());
	}

	virtual bool IsUnloadedClassAllowed(const FClassViewerInitializationOptions& InInitOptions, const TSharedRef< const IUnloadedBlueprintData > InClass, TSharedRef< FClassViewerFilterFuncs > InFilterFuncs) override
	{
		bool bMatchesFlags = !InClass->HasAnyClassFlags(CLASS_Hidden | CLASS_HideDropDown | CLASS_Deprecated);
		return  bMatchesFlags && InClass->IsChildOf(UTGOR_Dimension::StaticClass());
	}
};



TSharedRef<IPropertyTypeCustomization> FTGORNode_PortalSelection::MakeInstance()
{
	return MakeShareable(new FTGORNode_PortalSelection());
}

void FTGORNode_PortalSelection::CustomizeHeader(TSharedRef<class IPropertyHandle> StructPropertyHandle, class FDetailWidgetRow& HeaderRow, IPropertyTypeCustomizationUtils& StructCustomizationUtils)
{
	PortalSelectionUPropertyHandle = StructPropertyHandle;
	check(PortalSelectionUPropertyHandle.IsValid());
	
	ParseValue();

	HeaderRow.NameContent()
		[
			PortalSelectionUPropertyHandle->CreatePropertyNameWidget(PortalSelectionUPropertyHandle->GetPropertyDisplayName(), PortalSelectionUPropertyHandle->GetPropertyDisplayName(), false)
		]
		.ValueContent()
		.MinDesiredWidth(500)
		[
			SNew(SVerticalBox)
			+ SVerticalBox::Slot()
			[
				OnCreateDimensionCombo()
			]
			+ SVerticalBox::Slot()
			[
				SAssignNew(PortalSelection, STextComboBox)
				.OptionsSource(&AttributesList)
				.OnComboBoxOpening(this, &FTGORNode_PortalSelection::OnComboBoxOpening)
				.OnSelectionChanged(this, &FTGORNode_PortalSelection::OnAttributeSelected)
			]
		];

	PopulateList();
}

void FTGORNode_PortalSelection::ParseValue()
{
	FString Formatted;
	PortalSelectionUPropertyHandle->GetValueAsFormattedString(Formatted);

	FJsonSerializableArray Array;
	Formatted.ParseIntoArray(Array, L"\"", false);
	if (Array.Num() >= 6)
	{
		ClassProperty = FindObject<UClass>(ANY_PACKAGE, *Array[1]);
		SuffixProperty = Array[3];
		PortalProperty = FName(*Array[5]);
	}
	else
	{
		ClassProperty = nullptr;
		SuffixProperty = "";
		PortalProperty = "No Portal available";
	}
}

void FTGORNode_PortalSelection::WriteValue()
{
	FString ClassName = TEXT("None");
	if (ClassProperty)
	{
		ClassName = ClassProperty->GetPathName();
	}

	FString Formatted = "(Dimension=\"" + ClassName + "\",Suffix=\"" + SuffixProperty + "\",Portal=\"" + PortalProperty.ToString() + "\")";
	PortalSelectionUPropertyHandle->SetValueFromFormattedString(Formatted);
}


void FTGORNode_PortalSelection::PopulateList()
{
	NamesList.Empty();
	AttributesList.Empty();
	if (IsValid(ClassProperty))
	{
		UTGOR_Dimension* Dimension = ClassProperty->GetDefaultObject<UTGOR_Dimension>();
		if (IsValid(Dimension))
		{
			for (const FName& Portal : Dimension->PublicPortals)
			{
				NamesList.Add(Portal);
				AttributesList.Add(MakeShareable(new FString(Portal.GetPlainNameString())));
			}
		}
	}

	const int32 Selected = GetSelected();
	if (AttributesList.IsValidIndex(Selected))
	{
		// Get pin entry
		PortalSelection->SetSelectedItem(AttributesList[Selected]);
	}
	else if (AttributesList.Num() > 0)
	{
		PortalSelection->SetSelectedItem(AttributesList[0]);
		OnAttributeSelected(AttributesList[0], ESelectInfo::Type::Direct);
	}
	else
	{
		TSharedPtr<FString> NoPortal = TSharedPtr<FString>(new FString("No Portal available"));
		PortalSelection->SetSelectedItem(NoPortal);
		OnAttributeSelected(NoPortal, ESelectInfo::Type::Direct);
	}
}

int32 FTGORNode_PortalSelection::GetSelected()
{
	// Get pin value
	if (!PortalProperty.IsNone())
	{
		// Find attribute index
		for (int i = 0; i < NamesList.Num(); i++)
		{
			if (NamesList[i].IsEqual(PortalProperty))
			{
				return i;
			}
		}
	}
	return 0;
}


void FTGORNode_PortalSelection::CustomizeChildren(TSharedRef<class IPropertyHandle> StructPropertyHandle, class IDetailChildrenBuilder& StructBuilder, IPropertyTypeCustomizationUtils& StructCustomizationUtils)
{
	//Create further customization here
}


TSharedRef<SWidget> FTGORNode_PortalSelection::OnCreateDimensionCombo()
{
	return 
		SNew(SHorizontalBox)
		+ SHorizontalBox::Slot()
		.FillWidth(0.7f)
		[
			SAssignNew(DimensionSelection, SComboButton)
			.OnGetMenuContent(this, &FTGORNode_PortalSelection::OnGetDimensionSelection)
			.ContentPadding(FMargin(2.0f, 2.0f))
			.ToolTipText(FText::FromString(FString("Select asset")))
			.ButtonContent()
			[
				SNew(STextBlock)
				.Text(this, &FTGORNode_PortalSelection::GetDisplayValueAsString)
			]
		]
		+ SHorizontalBox::Slot()
		.FillWidth(0.3f)
		[
			SAssignNew(DimensionSuffix, SEditableTextBox)
			.ToolTipText(FText::FromString("Suffix"))
			.Text(FText::FromString(SuffixProperty))
			.OnTextChanged(this, &FTGORNode_PortalSelection::OnSuffixChanged)
		];
}


FText FTGORNode_PortalSelection::GetDisplayValueAsString() const
{
	static bool bIsReentrant = false;
	if (!bIsReentrant)
	{
		TGuardValue<bool>(bIsReentrant, true);
		if (IsValid(ClassProperty))
		{
			if (ClassProperty != NULL)
			{
				UBlueprint* BP = UBlueprint::GetBlueprintFromClass(ClassProperty);
				if (BP != NULL)
				{
					return FText::FromString(BP->GetName());
				}
			}
			return FText::FromString((ClassProperty) ? ClassProperty->GetName() : "None");
		}
	}
	return FText::GetEmpty();
}

TSharedRef<SWidget> FTGORNode_PortalSelection::OnGetDimensionSelection() const
{
	FClassViewerInitializationOptions Options;
	Options.bShowUnloadedBlueprints = true;
	Options.bShowNoneOption = true;

	//Options.PropertyHandle = ClassUPropertyHandle;

	TSharedPtr<FPortalSlectionClassFilter> ClassFilter = MakeShareable(new FPortalSlectionClassFilter);
	Options.ClassFilter = ClassFilter;
	Options.bIsBlueprintBaseOnly = true;
	Options.bIsPlaceableOnly = false;
	Options.NameTypeToDisplay = EClassViewerNameTypeToDisplay::ClassName;
	Options.DisplayMode = EClassViewerDisplayMode::ListView;
	Options.bAllowViewOptions = false;

	FOnClassPicked OnPicked(FOnClassPicked::CreateRaw(const_cast<FTGORNode_PortalSelection*>(this), &FTGORNode_PortalSelection::OnDimensionSelected));

	return SNew(SBox)
		.WidthOverride(280)
		[
			SNew(SVerticalBox)
			+ SVerticalBox::Slot()
			.AutoHeight()
			.MaxHeight(500)
			[
				FModuleManager::LoadModuleChecked<FClassViewerModule>("ClassViewer").CreateClassViewer(Options, OnPicked)
			]
		];
}


void FTGORNode_PortalSelection::OnMenuOpenChanged(bool bOpen)
{
	if (bOpen == false)
	{
		DimensionSelection->SetMenuContent(SNullWidget::NullWidget);
	}
}

bool FTGORNode_PortalSelection::OnShouldFilterAsset(const FAssetData& InAssetData) const
{
	UObject* Object = InAssetData.GetAsset();
	UBlueprint* Blueprint = Cast<UBlueprint>(Object);
	if (IsValid(Blueprint))
	{
		UClass* Class = Blueprint->GeneratedClass;
		if (IsValid(Class) && Class->IsChildOf(UTGOR_Dimension::StaticClass()))
		{
			return false;
		}
	}
	return true;
}

void FTGORNode_PortalSelection::OnDimensionSelected(UClass* InClass)
{
	if (ClassProperty != InClass)
	{
		ClassProperty = InClass;
		WriteValue();
	}
	
	DimensionSelection->SetIsOpen(false);
	PopulateList();
}


void FTGORNode_PortalSelection::OnComboBoxOpening()
{
	
}

void FTGORNode_PortalSelection::OnAttributeSelected(TSharedPtr<FString> ItemSelected, ESelectInfo::Type SelectInfo)
{
	if (ItemSelected.IsValid())
	{
		FName PortalName = FName(**ItemSelected.Get());
		if (!PortalName.IsEqual(PortalProperty))
		{
			PortalProperty = PortalName;
			WriteValue();
		}
	}
}

void FTGORNode_PortalSelection::OnSuffixChanged(const FText& Text)
{
	const FString Suffix = Text.ToString();

	if (!Suffix.Equals(SuffixProperty))
	{
		SuffixProperty = Suffix;
		WriteValue();
	}
}

#undef LOCTEXT_NAMESPACE