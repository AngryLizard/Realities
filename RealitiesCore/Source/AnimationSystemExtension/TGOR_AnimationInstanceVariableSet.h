// The Gateway of Realities: Planes of Existence.

#pragma once

#include "K2Node.h"
#include "IDetailCustomization.h"
#include "K2Node_ConstructObjectFromClass.h"
#include "TGOR_AnimationInstanceVariableSet.generated.h"

class IDetailLayoutBuilder;
class UTGOR_SubAnimInstance;
class UK2Node_CallFunction;
class UTGOR_Animation;

UCLASS(Abstract)
class ANIMATIONSYSTEMEXTENSION_API UTGOR_AnimationInstanceVariableSet : public UK2Node
{
	GENERATED_UCLASS_BODY()

	virtual UK2Node_CallFunction* CreateCallFunction(class FKismetCompilerContext& CompilerContext, UEdGraph* SourceGraph) PURE_VIRTUAL(UTGOR_AnimationInstanceVariableSet::CreateCallFunction, return nullptr; );
	virtual bool IsActionFilteredOut(class FBlueprintActionFilter const& Filter) PURE_VIRTUAL(UTGOR_AnimationInstanceVariableSet::IsActionFilteredOut, return false; );
	virtual UClass* GetAnimInstanceClass() const PURE_VIRTUAL(UTGOR_AnimationInstanceVariableSet::GetAnimInstanceClass, return nullptr; );


	// customize pin data based on the input
	virtual UEdGraphPin* CreateTargetPin();
	virtual void CustomizePinData(UEdGraphPin* Pin, FName SourcePropertyName, int32 ArrayIndex) const {}

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
	virtual FText GetMenuCategory() const override;

	virtual UObject* GetJumpTargetForDoubleClick() const override;
	virtual bool CanJumpToDefinition() const override;
	virtual void JumpToDefinition() const override;
	//~ End UK2Node Interface

	/** Get the then output pin */
	UEdGraphPin* GetThenPin() const;
	/** Get the animation input pin */
	UEdGraphPin* GetTargetPin() const;

	// can customize details tab 
	virtual void CustomizeDetails(IDetailLayoutBuilder& DetailBuilder);

	// has target pin at all
	virtual bool HasTargetPin() const { return false; };

protected:
	void SetPinToolTip(UEdGraphPin& MutatablePin, const FText& PinDescription) const;

	// Searches the instance class for properties that we can expose (public and BP visible)
	void GetExposableProperties(TArray<FProperty*>& OutExposableProperties) const;
	// Gets a property's type as FText (for UI)
	FText GetPropertyTypeText(FProperty* Property);
	// Given a new class, rebuild the known property list (for tracking class changes and moving pins)
	void RebuildExposedProperties(UClass* InNewClass);

	// If given property exposed on this node
	ECheckBoxState IsPropertyExposed(FName PropertyName) const;
	// User chose to expose, or unexpose a property
	void OnPropertyExposeCheckboxChanged(ECheckBoxState NewState, FName PropertyName);

	/** List of property names we know to exist on the target class, so we can detect when
	*  Properties are added or removed on reconstruction
	*/
	UPROPERTY()
		TArray<FName> KnownExposableProperties;

	/** Names of properties the user has chosen to expose */
	UPROPERTY()
		TArray<FName> ExposedPropertyNames;

	UPROPERTY()
		TSubclassOf<UTGOR_SubAnimInstance> TargetAnimInstanceClass;
	
	/** Constructing FText strings can be costly, so we cache the node's title */
	FNodeTextCache CachedNodeTitle;
};

UCLASS(BlueprintType, Blueprintable)
class ANIMATIONSYSTEMEXTENSION_API UTGOR_TaskVariableSet : public UTGOR_AnimationInstanceVariableSet
{
	GENERATED_UCLASS_BODY()

	virtual UK2Node_CallFunction* CreateCallFunction(class FKismetCompilerContext& CompilerContext, UEdGraph* SourceGraph) override;
	virtual bool IsActionFilteredOut(class FBlueprintActionFilter const& Filter) override;
	virtual UClass* GetAnimInstanceClass() const override;
	virtual bool HasTargetPin() const override { return false; };
};

UCLASS(BlueprintType, Blueprintable)
class ANIMATIONSYSTEMEXTENSION_API UTGOR_AnimationVariableSet : public UTGOR_AnimationInstanceVariableSet
{
	GENERATED_UCLASS_BODY()

	virtual UK2Node_CallFunction* CreateCallFunction(class FKismetCompilerContext& CompilerContext, UEdGraph* SourceGraph) override;
	virtual bool IsActionFilteredOut(class FBlueprintActionFilter const& Filter) override;
	virtual UClass* GetAnimInstanceClass() const override;
	virtual bool HasTargetPin() const override { return true; };
};
