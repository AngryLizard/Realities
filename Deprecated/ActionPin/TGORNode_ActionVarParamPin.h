// The Gateway of Realities: Planes of Existence.

#pragma once

#include "CoreMinimal.h"
#include "TGORNode_ActionPin.h"

/**
 * 
 */
class SGATGORNode_ActionParamPin : public SGATGORNode_ActionPin
{
	public:
		SLATE_BEGIN_ARGS(SGATGORNode_ActionParamPin) {}
		SLATE_END_ARGS()

		void Construct(const FArguments& InArgs, UEdGraphPin* InGraphPinObj);
		virtual void PopulateAttributeList(UK2Node* Node, UTGOR_Action* Action) override;

		ETGOR_VariableType AcquireRelevantType(UK2Node* Node);
		ETGOR_VariableType ConvertType(const FEdGraphPinType& Type, const FString& Name);
		void PopulateAttributeList(const TArray<FTGOR_ActionVariable>& Variables, ETGOR_VariableType Type);
};
