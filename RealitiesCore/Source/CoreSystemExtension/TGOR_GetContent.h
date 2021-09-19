// The Gateway of Realities: Planes of Existence.

#pragma once

#include "K2Node.h"
#include "IDetailCustomization.h"
#include "RealitiesUGC/Mod/TGOR_ContentManager.h"

#include "K2Node_ConstructObjectFromClass.h"
#include "TGOR_GetContent.generated.h"



UCLASS(BlueprintType, Blueprintable)
class CORESYSTEMEXTENSION_API UTGOR_GetContent : public UK2Node
{
	GENERATED_UCLASS_BODY()

	// customize pin data based on the input
	virtual void CustomizePinData(UEdGraphPin* Pin, FName SourcePropertyName, int32 ArrayIndex) const {}

	UClass* GetContentClass(const TArray<UEdGraphPin*>& SearchPins) const;
	bool UseWorldContext() const;

	//~ Begin UEdGraphNode Interface.
	virtual void ReconstructNode() override;
	virtual void PostReconstructNode() override;
	virtual void AllocateDefaultPins() override;
	virtual FText GetNodeTitle(ENodeTitleType::Type TitleType) const override;
	virtual void PinDefaultValueChanged(UEdGraphPin* Pin) override;
	virtual FText GetTooltipText() const override;
	virtual bool HasExternalDependencies(TArray<class UStruct*>* OptionalOutput) const override;
	virtual bool IsCompatibleWithGraph(const UEdGraph* TargetGraph) const override;
	virtual void PinConnectionListChanged(UEdGraphPin* Pin) override;
	virtual void GetPinHoverText(const UEdGraphPin& Pin, FString& HoverTextOut) const override;
	virtual void PostPlacedNewNode() override;
	virtual void ExpandNode(class FKismetCompilerContext& CompilerContext, UEdGraph* SourceGraph) override;
	//~ End UEdGraphNode Interface.

	//~ Begin UK2Node Interface
	virtual bool NodeCausesStructuralBlueprintChange() const override { return true; }
	virtual bool ShouldShowNodeProperties() const override { return true; }
	virtual void ReallocatePinsDuringReconstruction(TArray<UEdGraphPin*>& OldPins) override;
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
	virtual void GetNodeAttributes(TArray<TKeyValuePair<FString, FString>>& OutNodeAttributes) const override;
	virtual void GetMenuActions(FBlueprintActionDatabaseRegistrar& ActionRegistrar) const override;
	virtual bool IsActionFilteredOut(class FBlueprintActionFilter const& Filter) override;
	virtual FText GetMenuCategory() const override;
	//~ End UK2Node Interface

	/** Get the then output pin */
	UEdGraphPin* GetThenPin() const;
	/** Get the worldcontext input pin */
	UEdGraphPin* GetWorldContextPin() const;
	/** Get the content class pin */
	UEdGraphPin* GetContentPin(const TArray<UEdGraphPin*>& SearchPins) const;
	/** Get the result output pin */
	UEdGraphPin* GetResultPin() const;

protected:

	void SetPinToolTip(UEdGraphPin& MutatablePin, const FText& PinDescription) const;

	bool IsContentVarPin(UEdGraphPin* Pin) const;
	void OnContentPinChanged();
	
	/** Constructing FText strings can be costly, so we cache the node's title */
	FNodeTextCache CachedNodeTitle;
};