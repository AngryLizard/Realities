// The Gateway of Realities: Planes of Existence.


#include "TGORNode_ActionPin.h"

#include "Kismet2/KismetEditorUtilities.h"

#include "Widgets/Input/STextComboBox.h"
#include "EdGraph/EdGraphPin.h"
#include "EdGraph/EdGraphSchema.h"

#include "Realities/Mod/Actions/TGOR_Action.h"

void SGATGORNode_ActionPin::Construct(const FArguments& InArgs, UEdGraphPin* InGraphPinObj)
{
	ListAction = nullptr;
	UpdateList(InGraphPinObj);

	SGraphPin::Construct(SGraphPin::FArguments(), InGraphPinObj);
}

void SGATGORNode_ActionPin::Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime)
{
	SGraphPin::Tick(AllottedGeometry, InCurrentTime, InDeltaTime);
}

SGATGORNode_ActionPin::SGATGORNode_ActionPin()
	: SGraphPin()
{
	bShowLabel = false;
}

TSharedRef<SWidget>	SGATGORNode_ActionPin::GetDefaultValueWidget()
{
	// Default to 0
	uint8 Selected = 0xFF;

	// Get pin value
	FString CurrentDefaultValue = GraphPinObj->GetDefaultAsString();
	if (!CurrentDefaultValue.IsEmpty())
	{
		// Parse "(Index = *)"
		CurrentDefaultValue.RemoveFromStart("(Index=");
		CurrentDefaultValue.RemoveFromEnd(")");
		int32 Index = FCString::Atoi(*CurrentDefaultValue);

		// Find attribute index
		for (int i = 0; i < IndicesList.Num(); i++)
		{
			if (IndicesList[i] == Index)
			{
				Selected = i;
				break;
			}
		}
	}

	if (AttributesList.IsValidIndex(Selected))
	{
		// Get pin entry
		TSharedPtr<FString>& Default = AttributesList[Selected];

		// Create list
		return	SNew(STextComboBox)
			.InitiallySelectedItem(Default)
			.OptionsSource(&AttributesList)
			.OnComboBoxOpening(this, &SGATGORNode_ActionPin::OnComboBoxOpening)
			.OnSelectionChanged(this, &SGATGORNode_ActionPin::OnAttributeSelected);
	}
	else
	{
		if (AttributesList.Num() > 0)
		{
			OnAttributeSelected(AttributesList[0], ESelectInfo::Type::Direct);
		}

		// Create list
		return	SNew(STextComboBox)
			.OptionsSource(&AttributesList)
			.OnComboBoxOpening(this, &SGATGORNode_ActionPin::OnComboBoxOpening)
			.OnSelectionChanged(this, &SGATGORNode_ActionPin::OnAttributeSelected);
	}
}


UTGOR_Action* SGATGORNode_ActionPin::AcquireRelevantAction(UEdGraphPin* InGraphPinObj)
{
	// Get owning blueprint
	UK2Node* Node = Cast<UK2Node>(InGraphPinObj->GetOwningNode());
	if (IsValid(Node))
	{
		// Check if another pin is an action
		for (UEdGraphPin* Pin : Node->Pins)
		{
			if (Pin->Direction == EEdGraphPinDirection::EGPD_Input)
			{
				// Get action in input pin
				if (Pin->PinType.PinCategory.IsEqual("class"))
				{
					UClass* Result = FindObject<UClass>(ANY_PACKAGE, *Pin->GetDefaultAsString());
					if (IsValid(Result) && Result->IsChildOf(UTGOR_Action::StaticClass()))
					{
						return(Result->GetDefaultObject<UTGOR_Action>());
					}
				}
			}
		}

		// Check if own class is an action
		UBlueprint* Blueprint = Node->GetBlueprint();
		if (IsValid(Blueprint))
		{
			UClass* Class = Blueprint->GeneratedClass;
			if (IsValid(Class))
			{
				// Display all variables depending on output pin type
				return(Cast<UTGOR_Action>(Class->GetDefaultObject()));
			}
		}
	}

	return(nullptr);
}


void SGATGORNode_ActionPin::PopulateAttributeList(UK2Node* Node, UTGOR_Action* Action)
{

}

void SGATGORNode_ActionPin::OnComboBoxOpening()
{
	if (GraphPinObj)
	{
		UpdateList(GraphPinObj);
	}
}

void SGATGORNode_ActionPin::OnAttributeSelected(TSharedPtr<FString> ItemSelected, ESelectInfo::Type SelectInfo)
{
	FString CurrentDefaultValue = GraphPinObj->GetDefaultAsString();
	if (CurrentDefaultValue.IsEmpty())
	{
		CurrentDefaultValue = FString(TEXT("()"));
	}

	// Get var index
	uint8 Index = AttributesList.IndexOfByKey(ItemSelected);
	if (IndicesList.IsValidIndex(Index))
	{
		// Set attribute
		FString AttributeString = TEXT("(Index=");
		AttributeString += FString::FromInt(IndicesList[Index]);
		AttributeString += TEXT(")");

		// Set pin value
		if (!CurrentDefaultValue.Equals(AttributeString))
		{
			GraphPinObj->GetSchema()->TrySetDefaultValue(*GraphPinObj, AttributeString);
		}
	}
}

void SGATGORNode_ActionPin::UpdateList(UEdGraphPin* InGraphPinObj)
{
	// Get relevant action
	UTGOR_Action* Action = AcquireRelevantAction(InGraphPinObj);

	// Change list only if action changed
	if (IsValid(Action) && ListAction != Action)
	{
		ListAction = Action;

		UK2Node* Node = Cast<UK2Node>(InGraphPinObj->GetOwningNode());
		if (IsValid(Node))
		{
			// Change to relevant type
			PopulateAttributeList(Node, Action);
		}
	}
}

FSlateColor SGATGORNode_ActionPin::GetPinColor() const
{
	return(FSlateColor(FLinearColor(0.0f, 0.0f, 0.0f, 0.0f)));
}

bool SGATGORNode_ActionPin::GetDefaultValueIsReadOnly() const
{
	return GraphPinObj->bDefaultValueIsReadOnly;
}


