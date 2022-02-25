// The Gateway of Realities: Planes of Existence.

#pragma once

#include "CoreMinimal.h"
#include "TGORNode_ActionPin.h"

/**
 * 
 */
class SGATGORNode_ActionEventPin : public SGATGORNode_ActionPin
{
	public:
		SLATE_BEGIN_ARGS(SGATGORNode_ActionEventPin) {}
		SLATE_END_ARGS()

		void Construct(const FArguments& InArgs, UEdGraphPin* InGraphPinObj);
		virtual void PopulateAttributeList(UK2Node* Node, UTGOR_Action* Action) override;
		
		void PopulateAttributeList(const TArray<FTGOR_ActionEvent>& Events);
};
