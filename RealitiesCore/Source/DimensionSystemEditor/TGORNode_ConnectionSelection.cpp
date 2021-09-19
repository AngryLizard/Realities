// The Gateway of Realities: Planes of Existence.


#include "TGORNode_ConnectionSelection.h"
#include "DimensionSystem/Content/TGOR_Dimension.h"
#include "Widgets/Input/STextComboBox.h"

#include "Editor/PropertyEditor/Public/PropertyHandle.h"
#include "Editor/PropertyEditor/Public/DetailWidgetRow.h"
#include "ClassViewerModule.h"
#include "ClassViewerFilter.h"

#include "Editor/PropertyEditor/Public/PropertyCustomizationHelpers.h"

#include "UnrealEd.h"

#define LOCTEXT_NAMESPACE "ConnectionSelection"


class FConnectionSlectionClassFilter : public IClassViewerFilter
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



TSharedRef<IPropertyTypeCustomization> FTGORNode_ConnectionSelection::MakeInstance()
{
	return MakeShareable(new FTGORNode_ConnectionSelection());
}

void FTGORNode_ConnectionSelection::CustomizeHeader(TSharedRef<class IPropertyHandle> StructPropertyHandle, class FDetailWidgetRow& HeaderRow, IPropertyTypeCustomizationUtils& StructCustomizationUtils)
{
	ConnectionSelectionUPropertyHandle = StructPropertyHandle;
	check(ConnectionSelectionUPropertyHandle.IsValid());
	
	ParseValue();

	HeaderRow.NameContent()
		[
			ConnectionSelectionUPropertyHandle->CreatePropertyNameWidget(ConnectionSelectionUPropertyHandle->GetPropertyDisplayName(), ConnectionSelectionUPropertyHandle->GetPropertyDisplayName(), false)
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
				SAssignNew(ConnectionSelection, STextComboBox)
				.OptionsSource(&AttributesList)
				.OnComboBoxOpening(this, &FTGORNode_ConnectionSelection::OnComboBoxOpening)
				.OnSelectionChanged(this, &FTGORNode_ConnectionSelection::OnAttributeSelected)
			]
		];

	PopulateList();
}

void FTGORNode_ConnectionSelection::ParseValue()
{
	FString Formatted;
	ConnectionSelectionUPropertyHandle->GetValueAsFormattedString(Formatted);

	FJsonSerializableArray Array;
	Formatted.ParseIntoArray(Array, L"\"", false);
	if (Array.Num() >= 6)
	{
		ClassProperty = FindObject<UClass>(ANY_PACKAGE, *Array[1]);
		SuffixProperty = Array[3];
		ConnectionProperty = FName(*Array[5]);
	}
	else
	{
		ClassProperty = nullptr;
		SuffixProperty = "";
		ConnectionProperty = "No Connection available";
	}
}

void FTGORNode_ConnectionSelection::WriteValue()
{
	FString ClassName = TEXT("None");
	if (ClassProperty)
	{
		ClassName = ClassProperty->GetPathName();
	}

	FString Formatted = "(Dimension=\"" + ClassName + "\",Suffix=\"" + SuffixProperty + "\",Connection=\"" + ConnectionProperty.ToString() + "\")";
	ConnectionSelectionUPropertyHandle->SetValueFromFormattedString(Formatted);
}


void FTGORNode_ConnectionSelection::PopulateList()
{
	NamesList.Empty();
	AttributesList.Empty();
	if (IsValid(ClassProperty))
	{
		UTGOR_Dimension* Dimension = ClassProperty->GetDefaultObject<UTGOR_Dimension>();
		if (IsValid(Dimension))
		{
			for (const FName& Connection : Dimension->PublicConnections)
			{
				NamesList.Add(Connection);
				AttributesList.Add(MakeShareable(new FString(Connection.GetPlainNameString())));
			}
		}
	}

	const int32 Selected = GetSelected();
	if (AttributesList.IsValidIndex(Selected))
	{
		// Get pin entry
		ConnectionSelection->SetSelectedItem(AttributesList[Selected]);
	}
	else if (AttributesList.Num() > 0)
	{
		ConnectionSelection->SetSelectedItem(AttributesList[0]);
		OnAttributeSelected(AttributesList[0], ESelectInfo::Type::Direct);
	}
	else
	{
		TSharedPtr<FString> NoConnection = TSharedPtr<FString>(new FString("No Connection available"));
		ConnectionSelection->SetSelectedItem(NoConnection);
		OnAttributeSelected(NoConnection, ESelectInfo::Type::Direct);
	}
}

int32 FTGORNode_ConnectionSelection::GetSelected()
{
	// Get pin value
	if (!ConnectionProperty.IsNone())
	{
		// Find attribute index
		for (int i = 0; i < NamesList.Num(); i++)
		{
			if (NamesList[i].IsEqual(ConnectionProperty))
			{
				return i;
			}
		}
	}
	return 0;
}


void FTGORNode_ConnectionSelection::CustomizeChildren(TSharedRef<class IPropertyHandle> StructPropertyHandle, class IDetailChildrenBuilder& StructBuilder, IPropertyTypeCustomizationUtils& StructCustomizationUtils)
{
	//Create further customization here
}


TSharedRef<SWidget> FTGORNode_ConnectionSelection::OnCreateDimensionCombo()
{
	return 
		SNew(SHorizontalBox)
		+ SHorizontalBox::Slot()
		.FillWidth(0.7f)
		[
			SAssignNew(DimensionSelection, SComboButton)
			.OnGetMenuContent(this, &FTGORNode_ConnectionSelection::OnGetDimensionSelection)
			.ContentPadding(FMargin(2.0f, 2.0f))
			.ToolTipText(FText::FromString(FString("Select asset")))
			.ButtonContent()
			[
				SNew(STextBlock)
				.Text(this, &FTGORNode_ConnectionSelection::GetDisplayValueAsString)
			]
		]
		+ SHorizontalBox::Slot()
		.FillWidth(0.3f)
		[
			SAssignNew(DimensionSuffix, SEditableTextBox)
			.ToolTipText(FText::FromString("Suffix"))
			.Text(FText::FromString(SuffixProperty))
			.OnTextChanged(this, &FTGORNode_ConnectionSelection::OnSuffixChanged)
		];
}


FText FTGORNode_ConnectionSelection::GetDisplayValueAsString() const
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

TSharedRef<SWidget> FTGORNode_ConnectionSelection::OnGetDimensionSelection() const
{
	FClassViewerInitializationOptions Options;
	Options.bShowUnloadedBlueprints = true;
	Options.bShowNoneOption = true;

	//Options.PropertyHandle = ClassUPropertyHandle;

	TSharedPtr<FConnectionSlectionClassFilter> ClassFilter = MakeShareable(new FConnectionSlectionClassFilter);
	Options.ClassFilter = ClassFilter;
	Options.bIsBlueprintBaseOnly = true;
	Options.bIsPlaceableOnly = false;
	Options.NameTypeToDisplay = EClassViewerNameTypeToDisplay::ClassName;
	Options.DisplayMode = EClassViewerDisplayMode::ListView;
	Options.bAllowViewOptions = false;

	FOnClassPicked OnPicked(FOnClassPicked::CreateRaw(const_cast<FTGORNode_ConnectionSelection*>(this), &FTGORNode_ConnectionSelection::OnDimensionSelected));

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


void FTGORNode_ConnectionSelection::OnMenuOpenChanged(bool bOpen)
{
	if (bOpen == false)
	{
		DimensionSelection->SetMenuContent(SNullWidget::NullWidget);
	}
}

bool FTGORNode_ConnectionSelection::OnShouldFilterAsset(const FAssetData& InAssetData) const
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

void FTGORNode_ConnectionSelection::OnDimensionSelected(UClass* InClass)
{
	if (ClassProperty != InClass)
	{
		ClassProperty = InClass;
		WriteValue();
	}
	
	DimensionSelection->SetIsOpen(false);
	PopulateList();
}


void FTGORNode_ConnectionSelection::OnComboBoxOpening()
{
	
}

void FTGORNode_ConnectionSelection::OnAttributeSelected(TSharedPtr<FString> ItemSelected, ESelectInfo::Type SelectInfo)
{
	if (ItemSelected.IsValid())
	{
		FName ConnectionName = FName(**ItemSelected.Get());
		if (!ConnectionName.IsEqual(ConnectionProperty))
		{
			ConnectionProperty = ConnectionName;
			WriteValue();
		}
	}
}

void FTGORNode_ConnectionSelection::OnSuffixChanged(const FText& Text)
{
	const FString Suffix = Text.ToString();

	if (!Suffix.Equals(SuffixProperty))
	{
		SuffixProperty = Suffix;
		WriteValue();
	}
}

#undef LOCTEXT_NAMESPACE