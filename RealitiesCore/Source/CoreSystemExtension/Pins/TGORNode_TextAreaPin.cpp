// The Gateway of Realities: Planes of Existence.


#include "TGORNode_TextAreaPin.h"

#include "Widgets/Layout/SBox.h"
#include "Widgets/Input/SMultiLineEditableTextBox.h"
#include "ScopedTransaction.h"

//#include "Editor/IntroTutorials/Private/STutorialEditableText.h"

void SGATGORNode_TextAreaPin::Construct(const FArguments& InArgs, UEdGraphPin* InGraphPinObj)
{
	SGraphPin::Construct(SGraphPin::FArguments(), InGraphPinObj);
}


SGATGORNode_TextAreaPin::SGATGORNode_TextAreaPin()
	: SGraphPin()
{
	bShowLabel = false;
}

TSharedRef<SWidget>	SGATGORNode_TextAreaPin::GetDefaultValueWidget()
{
	return SNew(SBox)
		.MinDesiredWidth(150)
		.MinDesiredHeight(100)
		.MaxDesiredWidth(210)
		.MaxDesiredHeight(200)
		[
			SNew(SMultiLineEditableTextBox)
			//.Style(FEditorStyle::Get(), "Graph.EditableTextBox")
			.BackgroundColor(FSlateColor(FLinearColor::Black))
			.Text(this, &SGATGORNode_TextAreaPin::GetTypeInValue)
			.SelectAllTextWhenFocused(true)
			.Visibility(this, &SGraphPin::GetDefaultValueVisibility)
			.IsReadOnly(this, &SGATGORNode_TextAreaPin::GetDefaultValueIsReadOnly)
			.OnTextCommitted(this, &SGATGORNode_TextAreaPin::SetTypeInValue)
			.ForegroundColor(FSlateColor::UseForeground())
			.WrapTextAt(200)
			.ModiferKeyForNewLine(EModifierKey::Shift)
		];
}

FText SGATGORNode_TextAreaPin::GetTypeInValue() const
{
	return FText::FromString(GraphPinObj->GetDefaultAsString());
}

void SGATGORNode_TextAreaPin::SetTypeInValue(const FText& NewTypeInValue, ETextCommit::Type /*CommitInfo*/)
{
	const FString Text = NewTypeInValue.ToString();
	if (!GraphPinObj->GetDefaultAsString().Equals(Text))
	{
		const FScopedTransaction Transaction(NSLOCTEXT("GraphEditor", "ChangeStringPinValue", "Change String Pin Value"));
		GraphPinObj->Modify();

		GraphPinObj->GetSchema()->TrySetDefaultValue(*GraphPinObj, Text);
	}
}


FSlateColor SGATGORNode_TextAreaPin::GetPinColor() const
{
	return(FSlateColor(FLinearColor(0.0f, 0.0f, 0.0f, 0.0f)));
}

bool SGATGORNode_TextAreaPin::GetDefaultValueIsReadOnly() const
{
	return GraphPinObj->bDefaultValueIsReadOnly;
}


