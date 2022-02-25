// The Gateway of Realities: Planes of Existence.


#include "TGORNode_ActionEventPin.h"

#include "Kismet2/KismetEditorUtilities.h"

#include "Widgets/Input/STextComboBox.h"
#include "EdGraph/EdGraphPin.h"
#include "EdGraph/EdGraphSchema.h"

#include "Realities/Mod/Actions/TGOR_Action.h"

void SGATGORNode_ActionEventPin::Construct(const FArguments& InArgs, UEdGraphPin* InGraphPinObj)
{
	SGATGORNode_ActionPin::Construct(SGATGORNode_ActionPin::FArguments(), InGraphPinObj);
}

void SGATGORNode_ActionEventPin::PopulateAttributeList(UK2Node* Node, UTGOR_Action* Action)
{
	PopulateAttributeList(Action->Events);
}

void SGATGORNode_ActionEventPin::PopulateAttributeList(const TArray<FTGOR_ActionEvent>& Events)
{
	AttributesList.Empty();
	IndicesList.Empty();

	const int32 Num = Events.Num();
	for (int i = 0; i < Num; i++)
	{
		const FTGOR_ActionEvent& Event = Events[i];
		IndicesList.Add(i);
		AttributesList.Add(MakeShareable(new FString(Event.Name.GetPlainNameString())));
	}
}


