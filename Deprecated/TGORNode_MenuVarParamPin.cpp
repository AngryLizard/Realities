// The Gateway of Realities: Planes of Existence.


#include "TGORNode_MenuVarParamPin.h"

#include "KismetEditorUtilities.h"
#include "UI/TGOR_HUD.h"

#include "STextComboBox.h"
#include "EdGraph/EdGraphPin.h"
#include "EdGraph/EdGraphSchema.h"

#include "Editor/PropertyEditor/Public/PropertyHandle.h"
#include "Editor/PropertyEditor/Public/DetailWidgetRow.h"
#include "Editor/PropertyEditor/Public/DetailLayoutBuilder.h"

void SGATGORNode_MenuParamPin::Construct(const FArguments& InArgs, UEdGraphPin* InGraphPinObj)
{
	UpdateList(InGraphPinObj);

	SGraphPin::Construct(SGraphPin::FArguments(), InGraphPinObj);
}

void SGATGORNode_MenuParamPin::Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime)
{
	SGraphPin::Tick(AllottedGeometry, InCurrentTime, InDeltaTime);

}

SGATGORNode_MenuParamPin::SGATGORNode_MenuParamPin()
	: SGraphPin()
{
	bShowLabel = false;
}

TSharedRef<SWidget>	SGATGORNode_MenuParamPin::GetDefaultValueWidget()
{
	const int32 Selected = GetSelected(GraphPinObj->GetDefaultAsString(), NamesList);

	if (AttributesList.IsValidIndex(Selected))
	{
		// Get pin entry
		TSharedPtr<FString>& Default = AttributesList[Selected];

		// Create list
		return	SNew(STextComboBox)
			.InitiallySelectedItem(Default)
			.OptionsSource(&AttributesList)
			.OnComboBoxOpening(this, &SGATGORNode_MenuParamPin::OnComboBoxOpening)
			.OnSelectionChanged(this, &SGATGORNode_MenuParamPin::OnAttributeSelected);
	}
	else
	{
		// Create list
		return	SNew(STextComboBox)
			.OptionsSource(&AttributesList)
			.OnComboBoxOpening(this, &SGATGORNode_MenuParamPin::OnComboBoxOpening)
			.OnSelectionChanged(this, &SGATGORNode_MenuParamPin::OnAttributeSelected);
	}
}


int32 SGATGORNode_MenuParamPin::GetSelected(const FString& Value, const TArray<FName>& NamesList)
{
	// Get pin value
	FString CurrentDefaultValue = Value;
	if (!CurrentDefaultValue.IsEmpty())
	{
		// Parse "(Name = *)", can appear with or without quotes (Thanks Obama)
		CurrentDefaultValue.RemoveFromStart("(Name=");
		CurrentDefaultValue.RemoveFromEnd(")");
		CurrentDefaultValue.RemoveFromStart("\"");
		CurrentDefaultValue.RemoveFromEnd("\"");

		// Find attribute index
		for (int i = 0; i < NamesList.Num(); i++)
		{
			const FString Reference = NamesList[i].ToString();
			if (Reference.Equals(CurrentDefaultValue, ESearchCase::CaseSensitive))
			{
				return(i);
			}
		}
	}

	return(0);
}

void SGATGORNode_MenuParamPin::PopulateList(ATGOR_HUD* Hud, TArray<TSharedPtr<FString>>& AttributesList, TArray<FName>& NamesList)
{
	AttributesList.Empty();
	NamesList.Empty();

	NamesList.Add("None");
	AttributesList.Add(MakeShareable(new FString("None")));

	if (IsValid(Hud))
	{
		for (auto& Pair : Hud->Menus)
		{
			NamesList.Add(Pair.Key);
			AttributesList.Add(MakeShareable(new FString(Pair.Key.GetPlainNameString())));
		}
	}
}

void SGATGORNode_MenuParamPin::OnComboBoxOpening()
{
	if (GraphPinObj)
	{
		//UpdateList(GraphPinObj);
	}
}


FString SGATGORNode_MenuParamPin::SetSelected(const FString& Value, int32 Index, const TArray<FName>& NamesList)
{
	FString CurrentDefaultValue = Value;
	if (CurrentDefaultValue.IsEmpty())
	{
		CurrentDefaultValue = FString(TEXT("()"));
	}

	// Get var index
	if (NamesList.IsValidIndex(Index))
	{
		// Set attribute
		FString AttributeString = TEXT("(Name=");
		AttributeString += NamesList[Index].ToString();
		AttributeString += TEXT(")");

		// Set pin value
		if (!CurrentDefaultValue.Equals(AttributeString))
		{
			return(AttributeString);
		}
	}
	return(CurrentDefaultValue);
}

void SGATGORNode_MenuParamPin::OnAttributeSelected(TSharedPtr<FString> ItemSelected, ESelectInfo::Type SelectInfo)
{
	FString CurrentDefaultValue = GraphPinObj->GetDefaultAsString();
	uint32 Index = AttributesList.IndexOfByKey(ItemSelected);

	FString AttributeString = SetSelected(CurrentDefaultValue, Index, NamesList);

	GraphPinObj->GetSchema()->TrySetDefaultValue(*GraphPinObj, AttributeString);
}

void SGATGORNode_MenuParamPin::UpdateList(UEdGraphPin* InGraphPinObj)
{
	// Display all variables depending on output pin type				
	ATGOR_HUD* Hud = AcquireRelevantHud(InGraphPinObj);
	PopulateList(Hud, AttributesList, NamesList);
}

ATGOR_HUD* SGATGORNode_MenuParamPin::AcquireRelevantHud(UEdGraphPin* InGraphPinObj)
{
	// Get owning blueprint
	UK2Node* Node = Cast<UK2Node>(InGraphPinObj->GetOwningNode());
	if (IsValid(Node))
	{
		for (UEdGraphPin* Pin : Node->Pins)
		{
			if (Pin->Direction == EEdGraphPinDirection::EGPD_Input)
			{
				// Get action in input pin
				if (Pin->PinType.PinCategory.IsEqual("class"))
				{
					UClass* Result = FindObject<UClass>(ANY_PACKAGE, *Pin->GetDefaultAsString());
					if (IsValid(Result) && Result->IsChildOf(ATGOR_HUD::StaticClass()))
					{
						return(Result->GetDefaultObject<ATGOR_HUD>());
					}
				}
			}
		}

		// Check if own class is a hud
		UBlueprint* Blueprint = Node->GetBlueprint();
		if (IsValid(Blueprint))
		{
			UClass* Class = Blueprint->GeneratedClass;
			if (IsValid(Class) && Class->IsChildOf(ATGOR_HUD::StaticClass()))
			{
				// Display all variables depending on output pin type
				return(Class->GetDefaultObject<ATGOR_HUD>());
			}
		}
	}

	return(nullptr);
}



FSlateColor SGATGORNode_MenuParamPin::GetPinColor() const
{
	return(FSlateColor(FLinearColor(0.0f, 0.0f, 0.0f, 0.0f)));
}

bool SGATGORNode_MenuParamPin::GetDefaultValueIsReadOnly() const
{
	return GraphPinObj->bDefaultValueIsReadOnly;
}
