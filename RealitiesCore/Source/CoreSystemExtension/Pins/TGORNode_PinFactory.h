// The Gateway of Realities: Planes of Existence.

#pragma once
#include "SlateBasics.h"
#include "EdGraph/EdGraphPin.h"
#include "EdGraphSchema_K2.h"
//#include "ActionPin/TGORNode_ActionVarParamPin.h"
//#include "ActionPin/TGORNode_ActionEventPin.h"
//#include "TGORNode_MenuVarParamPin.h"
#include "TGORNode_TextAreaPin.h"
#include "EdGraphUtilities.h"
#include "CoreSystem/Utility/TGOR_TextAreaStruct.h"

class FGATGORNode_PanelGraphPinFactory : public FGraphPanelPinFactory
{
	virtual TSharedPtr<class SGraphPin> CreatePin(class UEdGraphPin* InPin) const override
	{
		const UEdGraphSchema_K2* K2Schema = GetDefault<UEdGraphSchema_K2>();
		/*
		Check if pin is struct, and then check if that pin is of struct type we want customize
		*/
		/*
		if (InPin->PinType.PinCategory == K2Schema->PC_Struct
			&& InPin->PinType.PinSubCategoryObject == FTGOR_ActionVarParam::StaticStruct())
		{
			return SNew(SGATGORNode_ActionParamPin, InPin); //and return our customized pin widget ;).
		}

		if (InPin->PinType.PinCategory == K2Schema->PC_Struct
			&& InPin->PinType.PinSubCategoryObject == FTGOR_ActionEvtParam::StaticStruct())
		{
			return SNew(SGATGORNode_ActionEventPin, InPin); //and return our customized pin widget ;).
		}

		if (InPin->PinType.PinCategory == K2Schema->PC_Struct
			&& InPin->PinType.PinSubCategoryObject == FTGOR_MenuVarParam::StaticStruct())
		{
			return SNew(SGATGORNode_MenuParamPin, InPin); //and return our customized pin widget ;).
		}
		*/

		if (InPin->PinType.PinCategory == K2Schema->PC_Struct
			&& InPin->PinType.PinSubCategoryObject == FTGOR_TextAreaStruct::StaticStruct())
		{
			return SNew(SGATGORNode_TextAreaPin, InPin); //and return our customized pin widget ;).
		}
		return nullptr;
	}
};
