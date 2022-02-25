// The Gateway of Realities: Planes of Existence.


#include "TGORNode_ActionVarParamPin.h"

#include "Kismet2/KismetEditorUtilities.h"

#include "Widgets/Input/STextComboBox.h"
#include "EdGraph/EdGraphPin.h"
#include "EdGraph/EdGraphSchema.h"

#include "Realities/Mod/Actions/TGOR_Action.h"

void SGATGORNode_ActionParamPin::Construct(const FArguments& InArgs, UEdGraphPin* InGraphPinObj)
{
	SGATGORNode_ActionPin::Construct(SGATGORNode_ActionPin::FArguments(), InGraphPinObj);
}


void SGATGORNode_ActionParamPin::PopulateAttributeList(UK2Node* Node, UTGOR_Action* Action)
{
	// Change to relevant type
	ETGOR_VariableType Type = AcquireRelevantType(Node);
	PopulateAttributeList(Action->Variables, Type);
}

ETGOR_VariableType SGATGORNode_ActionParamPin::AcquireRelevantType(UK2Node* Node)
{
	// Find matching pin type
	for (UEdGraphPin* Pin : Node->Pins)
	{
		// Ignore self
		if (Pin != GraphPinObj)
		{
			ETGOR_VariableType Type = ConvertType(Pin->PinType, Node->GetNodeTitle(ENodeTitleType::MenuTitle).ToString());
			if (Type != ETGOR_VariableType::None)
			{
				return(Type);
			}
		}
	}
	return(ETGOR_VariableType::None);
}

ETGOR_VariableType SGATGORNode_ActionParamPin::ConvertType(const FEdGraphPinType& Type, const FString& Name)
{
	if (Type.PinCategory.IsEqual("byte"))
	{
		return(ETGOR_VariableType::Byte);
	}
	else if (Type.PinCategory.IsEqual("float"))
	{
		return(ETGOR_VariableType::Float);
	}
	else if (Type.PinCategory.IsEqual("int"))
	{
		return(ETGOR_VariableType::Integer);
	}
	else if (Type.PinCategory.IsEqual("bool"))
	{
		return(ETGOR_VariableType::Boolean);
	}
	else if (Type.PinCategory.IsEqual("struct"))
	{
		if (Type.PinSubCategoryObject->GetName().Equals("Vector"))
		{
			if (Name.Contains("normal", ESearchCase::IgnoreCase))
			{
				return(ETGOR_VariableType::Normal);
			}
			else
			{
				return(ETGOR_VariableType::Vector);
			}
		}
	}
	else if (Type.PinCategory.IsEqual("object"))
	{
		if (Type.PinSubCategoryObject->GetName().Equals("Actor"))
		{
			return(ETGOR_VariableType::Actor);
		}
		else if (Type.PinSubCategoryObject->GetName().Equals("ActorComponent"))
		{
			return(ETGOR_VariableType::Component);
		}
	}
	return(ETGOR_VariableType::None);
}

void SGATGORNode_ActionParamPin::PopulateAttributeList(const TArray<FTGOR_ActionVariable>& Variables, ETGOR_VariableType Type)
{
	AttributesList.Empty();
	IndicesList.Empty();

	const int32 Num = Variables.Num();
	for (int i = 0; i < Num; i++)
	{
		const FTGOR_ActionVariable& Variable = Variables[i];
		if (Variable.Type == Type)
		{
			IndicesList.Add(i);
			AttributesList.Add(MakeShareable(new FString(Variable.Name.GetPlainNameString())));
		}
	}
}

