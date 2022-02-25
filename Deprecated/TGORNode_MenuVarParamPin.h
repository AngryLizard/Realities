// The Gateway of Realities: Planes of Existence.

#pragma once

#include "CoreMinimal.h"
#include "Base/Instances/System/TGOR_MenuInstance.h"
#include "Editor/BlueprintGraph/Classes/K2Node.h"

#include "Widgets/DeclarativeSyntaxSupport.h"
#include "Widgets/SWidget.h"
#include "SGraphPin.h"

class ATGOR_HUD;

/**
 * 
 */
class SGATGORNode_MenuParamPin : public SGraphPin
{
	public:
		SLATE_BEGIN_ARGS(SGATGORNode_MenuParamPin) {}
		SLATE_END_ARGS()

		SGATGORNode_MenuParamPin();
		void Construct(const FArguments& InArgs, UEdGraphPin* InGraphPinObj);
		void OnComboBoxOpening();
		void OnAttributeSelected(TSharedPtr<FString> ItemSelected, ESelectInfo::Type SelectInfo);

		static int32 GetSelected(const FString& Value, const TArray<FName>& NamesList);
		static FString SetSelected(const FString& Value, int32 Index, const TArray<FName>& NamesList);
		static void PopulateList(ATGOR_HUD* Hud, TArray<TSharedPtr<FString>>& AttributesList, TArray<FName>& NamesList);

		void UpdateList(UEdGraphPin* InGraphPinObj);
		ATGOR_HUD* AcquireRelevantHud(UEdGraphPin* InGraphPinObj);

		virtual void Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime) override;
	protected:
		//~ Begin SGraphPin Interface
		virtual TSharedRef<SWidget>	GetDefaultValueWidget() override;
		//~ End SGraphPin Interface
		
		virtual FSlateColor GetPinColor() const override;

		/** @return True if the pin default value field is read-only */
		bool GetDefaultValueIsReadOnly() const;
		
	private:
		TArray<TSharedPtr<FString>> AttributesList;
		TArray<FName> NamesList;
};
