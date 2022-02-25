// The Gateway of Realities: Planes of Existence.

#pragma once

#include "CoreMinimal.h"
#include "Animation/TGOR_AnimNode_AnimationContentInstance.h"

#include "AnimGraphNode_Base.h"
#include "TGOR_AnimGraphNode_AnimationContentInstance.generated.h"


/**
 * 
 */
UCLASS()
class REALITIESEXTENSION_API UTGOR_AnimGraphNode_AnimationContentInstance : public UAnimGraphNode_Base
{
	GENERATED_UCLASS_BODY()

	UPROPERTY(EditAnywhere, Category = Settings)
		FTGOR_AnimNode_AnimationContentInstance Node;

public:

	//~ Begin UEdGraphNode Interface.
	virtual FLinearColor GetNodeTitleColor() const override;
	virtual FText GetTooltipText() const override;
	virtual FText GetNodeTitle(ENodeTitleType::Type TitleType) const override;
	virtual void ValidateAnimNodeDuringCompilation(USkeleton* ForSkeleton, FCompilerResultsLog& MessageLog) override;
	virtual void ReallocatePinsDuringReconstruction(TArray<UEdGraphPin*>& OldPins) override;
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
	virtual bool HasExternalDependencies(TArray<class UStruct*>* OptionalOutput = NULL) const override;
	virtual UObject* GetJumpTargetForDoubleClick() const override;
	//~ End UEdGraphNode Interface.

	// Detail panel customizations
	virtual void CustomizeDetails(IDetailLayoutBuilder& DetailBuilder) override;
	// Input pose visibility
	ECheckBoxState IsInputPoseVisible() const;
	// If input pose visibility changed
	void OnInputPoseVisibleCheckboxChanged(ECheckBoxState NewState);

	/** Whether input pose pin is visible */
	UPROPERTY()
		bool InputPoseVisible;
};
