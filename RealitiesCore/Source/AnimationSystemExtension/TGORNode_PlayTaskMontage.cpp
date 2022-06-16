// Copyright Epic Games, Inc. All Rights Reserved.
// Code copied from UK2Node_PlayMontage

#include "TGORNode_PlayTaskMontage.h"
#include "AnimationSystem/TGOR_PlayTaskMontageCallbackProxy.h"

#define LOCTEXT_NAMESPACE "TGORAnimatedTask"

UTGORNode_PlayTaskMontage::UTGORNode_PlayTaskMontage(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	ProxyFactoryFunctionName = GET_FUNCTION_NAME_CHECKED(UTGOR_PlayTaskMontageCallbackProxy, CreateProxyObjectForPlayMontage);
	ProxyFactoryClass = UTGOR_PlayTaskMontageCallbackProxy::StaticClass();
	ProxyClass = UTGOR_PlayTaskMontageCallbackProxy::StaticClass();
}

FText UTGORNode_PlayTaskMontage::GetTooltipText() const
{
	return LOCTEXT("TGORNode_PlayTaskMontage_Tooltip", "Plays a Montage on a Task");
}

FText UTGORNode_PlayTaskMontage::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
	return LOCTEXT("PlayMontage", "Play Task Montage");
}

FText UTGORNode_PlayTaskMontage::GetMenuCategory() const
{
	return LOCTEXT("PlayMontageCategory", "Animation|Montage");
}

void UTGORNode_PlayTaskMontage::GetPinHoverText(const UEdGraphPin& Pin, FString& HoverTextOut) const
{
	Super::GetPinHoverText(Pin, HoverTextOut);

	static const FName NAME_OnNotifyBegin = FName(TEXT("OnNotifyBegin"));
	static const FName NAME_OnNotifyEnd = FName(TEXT("OnNotifyEnd"));

	if (Pin.PinName == NAME_OnNotifyBegin)
	{
		FText ToolTipText = LOCTEXT("TGORNode_PlayTaskMontage_OnNotifyBegin_Tooltip", "Event called when using a PlayMontageNotify or PlayMontageNotifyWindow Notify in a Montage.");
		HoverTextOut = FString::Printf(TEXT("%s\n%s"), *ToolTipText.ToString(), *HoverTextOut);
	}
	else if (Pin.PinName == NAME_OnNotifyEnd)
	{
		FText ToolTipText = LOCTEXT("TGORNode_PlayTaskMontage_OnNotifyEnd_Tooltip", "Event called when using a PlayMontageNotifyWindow Notify in a Montage.");
		HoverTextOut = FString::Printf(TEXT("%s\n%s"), *ToolTipText.ToString(), *HoverTextOut);
	}
}

#undef LOCTEXT_NAMESPACE
