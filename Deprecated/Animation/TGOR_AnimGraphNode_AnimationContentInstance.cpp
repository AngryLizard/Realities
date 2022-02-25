// The Gateway of Realities: Planes of Existence.


#include "TGOR_AnimGraphNode_AnimationContentInstance.h"

#include "Mod/Animations/TGOR_Animation.h"
#include "Widgets/Text/STextBlock.h"
#include "Widgets/Input/SCheckBox.h"
#include "EdGraphSchema_K2.h"
#include "Kismet2/CompilerResultsLog.h"
#include "Kismet2/BlueprintEditorUtils.h"
#include "PropertyHandle.h"
#include "DetailLayoutBuilder.h"
#include "DetailCategoryBuilder.h"
#include "Animation/AnimNode_SubInput.h"
#include "PropertyCustomizationHelpers.h"
#include "ScopedTransaction.h"
#include "AnimationGraphSchema.h"

#define LOCTEXT_NAMESPACE "A3Nodes"

UTGOR_AnimGraphNode_AnimationContentInstance::UTGOR_AnimGraphNode_AnimationContentInstance(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}


FLinearColor UTGOR_AnimGraphNode_AnimationContentInstance::GetNodeTitleColor() const
{
	return FLinearColor(1.0f, 0.8f, 0.8f);
}

FText UTGOR_AnimGraphNode_AnimationContentInstance::GetTooltipText() const
{
	return LOCTEXT("AnimationContent_ToolTip", "Runs a sub-anim instance from a animation content class to process animation");
}
FText UTGOR_AnimGraphNode_AnimationContentInstance::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
	FText ContentSlot = FText::FromName(Node.AnimationContentQueue);

	FFormatNamedArguments Args;
	Args.Add(TEXT("NodeTitle"), LOCTEXT("Title", "Animation Content Instance"));
	Args.Add(TEXT("TargetClass"), Node.AnimationContentQueue.IsNone() ? LOCTEXT("ClassNone", "None") : ContentSlot);

	if (TitleType == ENodeTitleType::MenuTitle)
	{
		return LOCTEXT("AnimationContent_NodeTitle", "Animation Content Instance");
	}
	if (TitleType == ENodeTitleType::ListView)
	{
		return FText::Format(LOCTEXT("AnimationContent_TitleListFormat", "{NodeTitle} - Target Class: {TargetClass}"), Args);
	}
	else
	{
		return FText::Format(LOCTEXT("AnimationContent_TitleFormat", "{NodeTitle}\nTarget Class: {TargetClass}"), Args);
	}
}

void UTGOR_AnimGraphNode_AnimationContentInstance::ValidateAnimNodeDuringCompilation(USkeleton* ForSkeleton, FCompilerResultsLog& MessageLog)
{
	Super::ValidateAnimNodeDuringCompilation(ForSkeleton, MessageLog);
}

void UTGOR_AnimGraphNode_AnimationContentInstance::ReallocatePinsDuringReconstruction(TArray<UEdGraphPin*>& OldPins)
{
	Super::ReallocatePinsDuringReconstruction(OldPins);

	// Need the schema to extract pin types
	const UEdGraphSchema_K2* Schema = CastChecked<UEdGraphSchema_K2>(GetSchema());

	if (InputPoseVisible)
	{
		if (UProperty* PoseProperty = FindField<UProperty>(FTGOR_AnimNode_AnimationContentInstance::StaticStruct(), GET_MEMBER_NAME_CHECKED(FTGOR_AnimNode_AnimationContentInstance, InPose)))
		{
			FEdGraphPinType PinType;
			if (Schema->ConvertPropertyToPinType(PoseProperty, PinType))
			{
				UEdGraphPin* NewPin = CreatePin(EEdGraphPinDirection::EGPD_Input, PinType, PoseProperty->GetFName());
				NewPin->PinFriendlyName = PoseProperty->GetDisplayNameText();

				CustomizePinData(NewPin, PoseProperty->GetFName(), INDEX_NONE);
			}
		}
	}
}

void UTGOR_AnimGraphNode_AnimationContentInstance::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
}

bool UTGOR_AnimGraphNode_AnimationContentInstance::HasExternalDependencies(TArray<class UStruct*>* OptionalOutput /*= NULL*/) const
{
	UClass* DefaultClass = *Node.Default;

	// Add our instance class... If that changes we need a recompile
	if (DefaultClass && OptionalOutput)
	{
		OptionalOutput->AddUnique(DefaultClass);
	}

	bool bSuperResult = Super::HasExternalDependencies(OptionalOutput);
	return DefaultClass || bSuperResult;
}

UObject* UTGOR_AnimGraphNode_AnimationContentInstance::GetJumpTargetForDoubleClick() const
{
	if (*Node.Default)
	{
		return(Node.Default->ClassGeneratedBy);
	}

	return nullptr;
}


void UTGOR_AnimGraphNode_AnimationContentInstance::CustomizeDetails(IDetailLayoutBuilder& DetailBuilder)
{
	Super::CustomizeDetails(DetailBuilder);

	// We dont allow multi-select here
	if (DetailBuilder.GetSelectedObjects().Num() > 1)
	{
		return;
	}

	const FName Title = FName(TEXT("Sub Instance Properties"));
	IDetailCategoryBuilder& CategoryBuilder = DetailBuilder.EditCategory(Title);
	FDetailWidgetRow& WidgetRow = CategoryBuilder.AddCustomRow(FText::FromString("Pose input visible"));
	FText TooltipText = LOCTEXT("AnimationContentInstance_PoseInputVisible", "Pose input visible");

	WidgetRow.NameContent()
	[
		SNew(STextBlock)
		.Text(TooltipText)
		.ToolTipText(TooltipText)
	];

	WidgetRow.ValueContent()
	[
		SNew(SHorizontalBox)
		+ SHorizontalBox::Slot()
		[
			SNew(SCheckBox)
			.IsChecked_UObject(this, &UTGOR_AnimGraphNode_AnimationContentInstance::IsInputPoseVisible)
			.OnCheckStateChanged_UObject(this, &UTGOR_AnimGraphNode_AnimationContentInstance::OnInputPoseVisibleCheckboxChanged)
		]
	];
}

ECheckBoxState UTGOR_AnimGraphNode_AnimationContentInstance::IsInputPoseVisible() const
{
	return InputPoseVisible ? ECheckBoxState::Checked : ECheckBoxState::Unchecked;
}

void UTGOR_AnimGraphNode_AnimationContentInstance::OnInputPoseVisibleCheckboxChanged(ECheckBoxState NewState)
{
	if (NewState == ECheckBoxState::Checked)
	{
		InputPoseVisible = true;
	}
	else if (NewState == ECheckBoxState::Unchecked)
	{
		InputPoseVisible = false;
	}

	ReconstructNode();
}

#undef LOCTEXT_NAMESPACE