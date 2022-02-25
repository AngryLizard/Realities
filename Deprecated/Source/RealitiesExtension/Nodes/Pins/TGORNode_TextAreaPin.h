// The Gateway of Realities: Planes of Existence.

#pragma once

#include "CoreMinimal.h"
#include "Realities/Utility/Structs/TGOR_TextAreaStruct.h"
#include "Widgets/DeclarativeSyntaxSupport.h"
#include "Widgets/SWidget.h"
#include "SGraphPin.h"


/**
 * 
 */
class SGATGORNode_TextAreaPin : public SGraphPin
{
	public:
		SLATE_BEGIN_ARGS(SGATGORNode_TextAreaPin) {}
		SLATE_END_ARGS()

		void Construct(const FArguments& InArgs, UEdGraphPin* InGraphPinObj);

		SGATGORNode_TextAreaPin();

	protected:
		//~ Begin SGraphPin Interface
		virtual TSharedRef<SWidget>	GetDefaultValueWidget() override;
		//~ End SGraphPin Interface

		FText GetTypeInValue() const;
		virtual void SetTypeInValue(const FText& NewTypeInValue, ETextCommit::Type CommitInfo);

		virtual FSlateColor GetPinColor() const override;

		/** @return True if the pin default value field is read-only */
		bool GetDefaultValueIsReadOnly() const;
};
