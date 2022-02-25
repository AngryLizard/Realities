// The Gateway of Realities: Planes of Existence.

#pragma once

#include "CoreMinimal.h"
#include "Realities/Base/Instances/System/TGOR_ActionInstance.h"
#include "Editor/BlueprintGraph/Classes/K2Node.h"

#include "Widgets/DeclarativeSyntaxSupport.h"
#include "Widgets/SWidget.h"
#include "SGraphPin.h"

/**
 * 
 */
class SGATGORNode_ActionPin : public SGraphPin
{
	public:
		SLATE_BEGIN_ARGS(SGATGORNode_ActionPin) {}
		SLATE_END_ARGS()

		SGATGORNode_ActionPin();
		void Construct(const FArguments& InArgs, UEdGraphPin* InGraphPinObj);
		void OnComboBoxOpening();
		void OnAttributeSelected(TSharedPtr<FString> ItemSelected, ESelectInfo::Type SelectInfo);

		void UpdateList(UEdGraphPin* InGraphPinObj);
		UTGOR_Action* AcquireRelevantAction(UEdGraphPin* InGraphPinObj);

		// Creates list from action
		virtual void PopulateAttributeList(UK2Node* Node, UTGOR_Action* Action);

		virtual void Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime) override;

	protected:
		//~ Begin SGraphPin Interface
		virtual TSharedRef<SWidget>	GetDefaultValueWidget() override;
		//~ End SGraphPin Interface
		
		virtual FSlateColor GetPinColor() const override;

		/** @return True if the pin default value field is read-only */
		bool GetDefaultValueIsReadOnly() const;
		
	protected:
		UTGOR_Action* ListAction;
		TArray<TSharedPtr<FString>> AttributesList;
		TArray<uint8> IndicesList;
};
