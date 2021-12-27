// The Gateway of Realities: Planes of Existence.


#include "TGOR_AnimationInstanceVariableSet.h"

#include "KismetCompiler.h"
#include "BlueprintNodeSpawner.h"
#include "EditorCategoryUtils.h"
#include "Kismet2/BlueprintEditorUtils.h"
#include "BlueprintActionDatabaseRegistrar.h"
#include "Widgets/Text/STextBlock.h"
#include "Widgets/Input/SCheckBox.h"
#include "AnimationSystem/Instances/TGOR_AnimInstance.h"
#include "AnimationSystem/Instances/TGOR_SubAnimInstance.h"
#include "AnimationSystem/Interfaces/TGOR_AnimationInterface.h"
#include "AnimationSystem/Components/TGOR_AnimationComponent.h"
#include "AnimationSystem/Content/TGOR_Performance.h"
#include "AnimationSystem/Content/TGOR_Animation.h"
#include "AnimationSystem/Content/TGOR_Archetype.h"
#include "AnimationSystem/Tasks/TGOR_AnimatedTask.h"
#include "DetailLayoutBuilder.h"
#include "DetailCategoryBuilder.h"
#include "PropertyCustomizationHelpers.h"
#include "K2Node_CallFunction.h"
#include "Editor/UnrealEd/Public/Subsystems/AssetEditorSubsystem.h"

//#include "Engine.h"

#define LOCTEXT_NAMESPACE "AnimationVariableSet"

struct FTGOR_AnimationVariableSetHelper
{
	static FName TargetPinName;
};

FName FTGOR_AnimationVariableSetHelper::TargetPinName(TEXT("Target"));


UTGOR_AnimationInstanceVariableSet::UTGOR_AnimationInstanceVariableSet(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer) 
{
}


void UTGOR_AnimationInstanceVariableSet::ReconstructNode()
{
	Super::ReconstructNode();
}

void UTGOR_AnimationInstanceVariableSet::PostReconstructNode()
{
	Super::PostReconstructNode();
}

UEdGraphPin* UTGOR_AnimationInstanceVariableSet::CreateTargetPin()
{
	return CreatePin(EGPD_Input, UEdGraphSchema_K2::PC_Interface, UTGOR_AnimationInterface::StaticClass(), FTGOR_AnimationVariableSetHelper::TargetPinName);
}

void UTGOR_AnimationInstanceVariableSet::AllocateDefaultPins()
{
	// Add execution pins
	CreatePin(EGPD_Input, UEdGraphSchema_K2::PC_Exec, UEdGraphSchema_K2::PN_Execute);
	CreatePin(EGPD_Output, UEdGraphSchema_K2::PC_Exec, UEdGraphSchema_K2::PN_Then);
	
	// Add blueprint pin
	if (HasTargetPin())
	{
		CreateTargetPin();
	}

	Super::AllocateDefaultPins();
}

FText UTGOR_AnimationInstanceVariableSet::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
	if (TitleType == ENodeTitleType::ListView || TitleType == ENodeTitleType::MenuTitle)
	{
		return LOCTEXT("AnimationVariableSet_ShortTitle", "Set Animation Data");
	}
	else
	{
		if (CachedNodeTitle.IsOutOfDate(this))
		{
			UClass* Class = GetAnimInstanceClass();
			if (Class)
			{
				FFormatNamedArguments Args;
				Args.Add(TEXT("ClassName"), Class->GetDisplayNameText());
				FText NodeTitleFormat = NSLOCTEXT("K2Node", "Set Variables", "Set {ClassName} Data");
				CachedNodeTitle.SetCachedText(FText::Format(NodeTitleFormat, Args), this);
			}
		}
		return CachedNodeTitle;
	}
	return NSLOCTEXT("K2Node", "AnimationVariableSet_None", "Set NONE Data");
}

void UTGOR_AnimationInstanceVariableSet::PinDefaultValueChanged(UEdGraphPin* ChangedPin)
{
	Super::PinDefaultValueChanged(ChangedPin);
}

FText UTGOR_AnimationInstanceVariableSet::GetTooltipText() const
{
	return LOCTEXT("AnimationVariableSet_ShortTitle", "Set Animation Variables");
}

bool UTGOR_AnimationInstanceVariableSet::HasExternalDependencies(TArray<class UStruct*>* OptionalOutput) const
{
	UClass* InstanceClassToUse = *TargetAnimInstanceClass;

	// Add our instance class... If that changes we need a recompile
	if (InstanceClassToUse && OptionalOutput)
	{
		OptionalOutput->AddUnique(InstanceClassToUse);
	}

	bool bSuperResult = Super::HasExternalDependencies(OptionalOutput);
	return InstanceClassToUse || bSuperResult;
}


bool UTGOR_AnimationInstanceVariableSet::IsCompatibleWithGraph(const UEdGraph* TargetGraph) const
{
	UBlueprint* Blueprint = FBlueprintEditorUtils::FindBlueprintForGraph(TargetGraph);
	return Super::IsCompatibleWithGraph(TargetGraph) && (!Blueprint || FBlueprintEditorUtils::FindUserConstructionScript(Blueprint) != TargetGraph);
}


void UTGOR_AnimationInstanceVariableSet::PinConnectionListChanged(UEdGraphPin* Pin)
{
	Super::PinConnectionListChanged(Pin);
}

void UTGOR_AnimationInstanceVariableSet::GetPinHoverText(const UEdGraphPin& Pin, FString& HoverTextOut) const
{
	if (UEdGraphPin* TargetPin = GetTargetPin())
	{
		SetPinToolTip(*TargetPin, LOCTEXT("AnimationVariableSet_Target", "Target to grab the animation instance from"));
	}
	return Super::GetPinHoverText(Pin, HoverTextOut);
}

UObject* UTGOR_AnimationInstanceVariableSet::GetJumpTargetForDoubleClick() const
{
	UClass* TargetClass = GetAnimInstanceClass();
	if (TargetClass)
	{
		return TargetClass->ClassGeneratedBy;
	}

	return nullptr;
}

bool UTGOR_AnimationInstanceVariableSet::CanJumpToDefinition() const
{
	return GetJumpTargetForDoubleClick() != nullptr;
}

void UTGOR_AnimationInstanceVariableSet::JumpToDefinition() const
{
	if (UObject* HyperlinkTarget = GetJumpTargetForDoubleClick())
	{
		UAssetEditorSubsystem* AssetEditorSubsystem = GEditor->GetEditorSubsystem<UAssetEditorSubsystem>();
		AssetEditorSubsystem->OpenEditorForAsset(HyperlinkTarget);
	}
}

void UTGOR_AnimationInstanceVariableSet::PostPlacedNewNode()
{
	Super::PostPlacedNewNode();

}


void UTGOR_AnimationInstanceVariableSet::ExpandNode(class FKismetCompilerContext& CompilerContext, UEdGraph* SourceGraph)
{
	Super::ExpandNode(CompilerContext, SourceGraph);

	UK2Node_CallFunction* CallCreateNode = CreateCallFunction(CompilerContext, SourceGraph);

	// store off the class to spawn before we mutate pin connections:
	UClass* TargetClass = *TargetAnimInstanceClass;
	bool bSucceeded = true;
	//connect exe
	{
		UEdGraphPin* SpawnExecPin = GetExecPin();
		UEdGraphPin* CallExecPin = CallCreateNode->GetExecPin();
		bSucceeded &= SpawnExecPin && CallExecPin && CompilerContext.MovePinLinksToIntermediate(*SpawnExecPin, *CallExecPin).CanSafeConnect();
	}

	//connect outer
	if (HasTargetPin())
	{
		UEdGraphPin* TargetPin = GetTargetPin();
		UEdGraphPin* CallTargetPin = CallCreateNode->FindPin(FTGOR_AnimationVariableSetHelper::TargetPinName);
		bSucceeded &= TargetPin && CallTargetPin && CompilerContext.MovePinLinksToIntermediate(*TargetPin, *CallTargetPin).CanSafeConnect();
	}

	UEdGraphPin* CallResultPin = nullptr;
	//connect result
	{
		//UEdGraphPin* SpawnResultPin = GetResultPin();
		CallResultPin = CallCreateNode->GetReturnValuePin();
		/*
		// cast HACK. It should be safe. The only problem is native code generation.
		if (SpawnResultPin && CallResultPin)
		{
			CallResultPin->PinType = SpawnResultPin->PinType;
		}
		bSucceeded &= SpawnResultPin && CallResultPin && CompilerContext.MovePinLinksToIntermediate(*SpawnResultPin, *CallResultPin).CanSafeConnect();
		*/
	}

	//assign exposed values and connect then
	{
		UEdGraphPin* LastThen = FKismetCompilerUtilities::GenerateAssignmentNodes(CompilerContext, SourceGraph, CallCreateNode, this, CallResultPin, TargetClass);
		UEdGraphPin* SpawnNodeThen = GetThenPin();
		bSucceeded &= SpawnNodeThen && LastThen && CompilerContext.MovePinLinksToIntermediate(*SpawnNodeThen, *LastThen).CanSafeConnect();
	}

	BreakAllNodeLinks();

	if (!bSucceeded)
	{
		CompilerContext.MessageLog.Error(*LOCTEXT("AnimationVariableSet_Error", "ICE: AnimationVariableSet error @@").ToString(), this);
	}
}


void UTGOR_AnimationInstanceVariableSet::ReallocatePinsDuringReconstruction(TArray<UEdGraphPin*>& OldPins)
{
	AllocateDefaultPins();
	
	// Need the schema to extract pin types
	const UEdGraphSchema_K2* Schema = CastChecked<UEdGraphSchema_K2>(GetSchema());

	UClass* TargetClass = GetAnimInstanceClass();
	if (TargetClass && Schema)
	{
		// Cache for compilation
		TargetAnimInstanceClass = TargetClass;

		// Grab the list of properties we can expose
		TArray<FProperty*> ExposablePropeties;
		GetExposableProperties(ExposablePropeties);

		// We'll track the names we encounter by removing from this list, if anything remains the properties
		// have been removed from the target class and we should remove them too
		TArray<FName> BeginExposableNames = KnownExposableProperties;

		for (FProperty* Property : ExposablePropeties)
		{
			FName PropertyName = Property->GetFName();
			BeginExposableNames.Remove(PropertyName);

			if (!KnownExposableProperties.Contains(PropertyName))
			{
				// New property added to the target class
				KnownExposableProperties.Add(PropertyName);
			}

			if (ExposedPropertyNames.Contains(PropertyName) && FBlueprintEditorUtils::PropertyStillExists(Property))
			{
				FEdGraphPinType PinType;

				verify(Schema->ConvertPropertyToPinType(Property, PinType));

				UEdGraphPin* NewPin = CreatePin(EEdGraphPinDirection::EGPD_Input, PinType, Property->GetFName());
				NewPin->PinFriendlyName = Property->GetDisplayNameText();

				// Need to grab the default value for the property from the target generated class CDO
				FString CDODefaultValueString;
				uint8* ContainerPtr = reinterpret_cast<uint8*>(TargetClass->GetDefaultObject());

				if (FBlueprintEditorUtils::PropertyValueToString(Property, ContainerPtr, CDODefaultValueString))
				{
					// If we successfully pulled a value, set it to the pin
					Schema->TrySetDefaultValue(*NewPin, CDODefaultValueString);
				}

				CustomizePinData(NewPin, PropertyName, INDEX_NONE);
			}
		}

		// Remove any properties that no longer exist on the target class
		for (FName& RemovedPropertyName : BeginExposableNames)
		{
			KnownExposableProperties.Remove(RemovedPropertyName);
		}
	}

	RestoreSplitPins(OldPins);
}

void UTGOR_AnimationInstanceVariableSet::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
	
}

void UTGOR_AnimationInstanceVariableSet::GetNodeAttributes(TArray<TKeyValuePair<FString, FString>>& OutNodeAttributes) const
{
	OutNodeAttributes.Add(TKeyValuePair<FString, FString>(TEXT("Type"), TEXT("SetAnimationData")));
	OutNodeAttributes.Add(TKeyValuePair<FString, FString>(TEXT("Class"), GetClass()->GetName()));
	OutNodeAttributes.Add(TKeyValuePair<FString, FString>(TEXT("Name"), GetName()));
}

void UTGOR_AnimationInstanceVariableSet::GetMenuActions(FBlueprintActionDatabaseRegistrar& ActionRegistrar) const
{
	// actions get registered under specific object-keys; the idea is that 
	// actions might have to be updated (or deleted) if their object-key is  
	// mutated (or removed)... here we use the node's class (so if the node 
	// type disappears, then the action should go with it)
	UClass* ActionKey = GetClass();
	// to keep from needlessly instantiating a UBlueprintNodeSpawner, first   
	// check to make sure that the registrar is looking for actions of this type
	// (could be regenerating actions for a specific asset, and therefore the 
	// registrar would only accept actions corresponding to that asset)
	if (ActionRegistrar.IsOpenForRegistration(ActionKey))
	{
		UBlueprintNodeSpawner* NodeSpawner = UBlueprintNodeSpawner::Create(GetClass());
		check(NodeSpawner != nullptr);

		ActionRegistrar.AddBlueprintAction(ActionKey, NodeSpawner);
	}
}


FText UTGOR_AnimationInstanceVariableSet::GetMenuCategory() const
{
	return FEditorCategoryUtils::GetCommonCategory(FCommonEditorCategory::Gameplay);
}



UEdGraphPin* UTGOR_AnimationInstanceVariableSet::GetThenPin() const
{
	UEdGraphPin* Pin = FindPinChecked(UEdGraphSchema_K2::PN_Then);
	check(Pin->Direction == EGPD_Output);
	return Pin;
}

UEdGraphPin* UTGOR_AnimationInstanceVariableSet::GetTargetPin() const
{
	UEdGraphPin* Pin = FindPin(FTGOR_AnimationVariableSetHelper::TargetPinName);
	check(Pin == nullptr || Pin->Direction == EGPD_Input);
	return Pin;
}

void UTGOR_AnimationInstanceVariableSet::CustomizeDetails(IDetailLayoutBuilder& DetailBuilder)
{
	// We dont allow multi-select here
	if (DetailBuilder.GetSelectedObjects().Num() > 1)
	{
		return;
	}

	TArray<FProperty*> ExposableProperties;
	GetExposableProperties(ExposableProperties);

	if (ExposableProperties.Num() > 0)
	{
		IDetailCategoryBuilder& CategoryBuilder = DetailBuilder.EditCategory(FName(TEXT("Instance Properties")));

		for (FProperty* Property : ExposableProperties)
		{
			FDetailWidgetRow& WidgetRow = CategoryBuilder.AddCustomRow(FText::FromString(Property->GetName()));

			FName PropertyName = Property->GetFName();
			FText PropertyTypeText = GetPropertyTypeText(Property);

			FFormatNamedArguments Args;
			Args.Add(TEXT("PropertyName"), FText::FromName(PropertyName));
			Args.Add(TEXT("PropertyType"), PropertyTypeText);

			FText TooltipText = FText::Format(LOCTEXT("PropertyTooltipText", "{PropertyName}\nType: {PropertyType}"), Args);

			WidgetRow.NameContent()
			[
				SNew(STextBlock)
				.Text(FText::FromString(Property->GetName()))
				.ToolTipText(TooltipText)
			];

			WidgetRow.ValueContent()
			[
				SNew(SHorizontalBox)
				+ SHorizontalBox::Slot()
				[
					SNew(STextBlock)
					.Text(LOCTEXT("ExposePropertyValue", "Expose: "))
				]
				+ SHorizontalBox::Slot()
				[
					SNew(SCheckBox)
					.IsChecked_UObject(this, &UTGOR_AnimationInstanceVariableSet::IsPropertyExposed, PropertyName)
					.OnCheckStateChanged_UObject(this, &UTGOR_AnimationInstanceVariableSet::OnPropertyExposeCheckboxChanged, PropertyName)
				]
			];
		}
	}
}

void UTGOR_AnimationInstanceVariableSet::SetPinToolTip(UEdGraphPin& MutatablePin, const FText& PinDescription) const
{
	MutatablePin.PinToolTip = UEdGraphSchema_K2::TypeToText(MutatablePin.PinType).ToString();

	UEdGraphSchema_K2 const* const K2Schema = Cast<const UEdGraphSchema_K2>(GetSchema());
	if (K2Schema != nullptr)
	{
		MutatablePin.PinToolTip += TEXT(" ");
		MutatablePin.PinToolTip += K2Schema->GetPinDisplayName(&MutatablePin).ToString();
	}

	MutatablePin.PinToolTip += FString(TEXT("\n")) + PinDescription.ToString();
}


void UTGOR_AnimationInstanceVariableSet::GetExposableProperties(TArray<FProperty*>& OutExposableProperties) const
{
	OutExposableProperties.Empty();

	UClass* TargetClass = GetAnimInstanceClass();
	if (TargetClass)
	{
		const UEdGraphSchema_K2* Schema = CastChecked<UEdGraphSchema_K2>(GetSchema());

		for (TFieldIterator<FProperty> It(TargetClass, EFieldIteratorFlags::IncludeSuper); It; ++It)
		{
			FProperty* CurProperty = *It;
			FEdGraphPinType PinType;

			if (CurProperty->HasAllPropertyFlags(CPF_Edit | CPF_BlueprintVisible) && CurProperty->HasAllFlags(RF_Public) && Schema->ConvertPropertyToPinType(CurProperty, PinType))
			{
				OutExposableProperties.Add(CurProperty);
			}
		}
	}
}


FText UTGOR_AnimationInstanceVariableSet::GetPropertyTypeText(FProperty* Property)
{
	FText PropertyTypeText;

	if (FStructProperty* StructProperty = CastField<FStructProperty>(Property))
	{
		PropertyTypeText = StructProperty->Struct->GetDisplayNameText();
	}
	else if (FStructProperty* ObjectProperty = CastField<FStructProperty>(Property))
	{
		PropertyTypeText = ObjectProperty->GetClass()->GetDisplayNameText(); //ObjectProperty->PropertyClass->GetDisplayNameText();
	}
	else if (FFieldClass* PropClass = Property->GetClass())//UClass* PropClass = Property->GetClass())
	{
		PropertyTypeText = PropClass->GetDisplayNameText();
	}
	else
	{
		PropertyTypeText = LOCTEXT("PropertyTypeUnknown", "Unknown");
	}

	return PropertyTypeText;
}

void UTGOR_AnimationInstanceVariableSet::RebuildExposedProperties(UClass* InNewClass)
{
	ExposedPropertyNames.Empty();
	KnownExposableProperties.Empty();
	if (InNewClass)
	{
		TArray<FProperty*> ExposableProperties;
		GetExposableProperties(ExposableProperties);

		for (FProperty* Property : ExposableProperties)
		{
			KnownExposableProperties.Add(Property->GetFName());
		}
	}
}


ECheckBoxState UTGOR_AnimationInstanceVariableSet::IsPropertyExposed(FName PropertyName) const
{
	return ExposedPropertyNames.Contains(PropertyName) ? ECheckBoxState::Checked : ECheckBoxState::Unchecked;
}

void UTGOR_AnimationInstanceVariableSet::OnPropertyExposeCheckboxChanged(ECheckBoxState NewState, FName PropertyName)
{
	if (NewState == ECheckBoxState::Checked)
	{
		ExposedPropertyNames.AddUnique(PropertyName);
	}
	else if (NewState == ECheckBoxState::Unchecked)
	{
		ExposedPropertyNames.Remove(PropertyName);
	}

	ReconstructNode();
}


UTGOR_TaskVariableSet::UTGOR_TaskVariableSet(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

UK2Node_CallFunction* UTGOR_TaskVariableSet::CreateCallFunction(class FKismetCompilerContext& CompilerContext, UEdGraph* SourceGraph)
{
	UK2Node_CallFunction* CallCreateNode = CompilerContext.SpawnIntermediateNode<UK2Node_CallFunction>(this, SourceGraph);
	CallCreateNode->FunctionReference.SetExternalMember(GET_FUNCTION_NAME_CHECKED(UTGOR_AnimatedTask, GetAnimationInstance), UTGOR_AnimatedTask::StaticClass());
	CallCreateNode->AllocateDefaultPins();
	return CallCreateNode;
}

bool UTGOR_TaskVariableSet::IsActionFilteredOut(class FBlueprintActionFilter const& Filter)
{
	for (const UBlueprint* Blueprint : Filter.Context.Blueprints)
	{
		UClass* BPClass = Blueprint->SkeletonGeneratedClass ? Blueprint->SkeletonGeneratedClass : Blueprint->GeneratedClass;
		if (BPClass && BPClass->IsChildOf(UTGOR_AnimatedTask::StaticClass()))
		{
			return false;
		}
	}
	return true;
}

UClass* UTGOR_TaskVariableSet::GetAnimInstanceClass() const
{
	UClass* Class = GetBlueprintClassFromNode();
	if (Class && Class->IsChildOf(UTGOR_AnimatedTask::StaticClass()))
	{
		UTGOR_AnimatedTask* Content = Class->GetAuthoritativeClass()->GetDefaultObject<UTGOR_AnimatedTask>();
		if (Content)
		{
			return *Content->InstanceClass;
		}
	}
	return nullptr;
}


UTGOR_AnimationVariableSet::UTGOR_AnimationVariableSet(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

UK2Node_CallFunction* UTGOR_AnimationVariableSet::CreateCallFunction(class FKismetCompilerContext& CompilerContext, UEdGraph* SourceGraph)
{
	UK2Node_CallFunction* CallCreateNode = CompilerContext.SpawnIntermediateNode<UK2Node_CallFunction>(this, SourceGraph);
	CallCreateNode->FunctionReference.SetExternalMember(GET_FUNCTION_NAME_CHECKED(UTGOR_Animation, GetAnimationInstance), UTGOR_Animation::StaticClass());
	CallCreateNode->AllocateDefaultPins();
	return CallCreateNode;
}

bool UTGOR_AnimationVariableSet::IsActionFilteredOut(class FBlueprintActionFilter const& Filter)
{
	for (const UBlueprint* Blueprint : Filter.Context.Blueprints)
	{
		UClass* BPClass = Blueprint->SkeletonGeneratedClass ? Blueprint->SkeletonGeneratedClass : Blueprint->GeneratedClass;
		if (BPClass && BPClass->IsChildOf(UTGOR_Animation::StaticClass()))
		{
			return false;
		}
	}
	return true;
}

UClass* UTGOR_AnimationVariableSet::GetAnimInstanceClass() const
{
	UClass* Class = GetBlueprintClassFromNode();
	if (Class && Class->IsChildOf(UTGOR_Animation::StaticClass()))
	{
		UTGOR_Animation* Content = Class->GetAuthoritativeClass()->GetDefaultObject<UTGOR_Animation>();
		if (Content)
		{
			return *Content->InstanceClass;
		}
	}
	return nullptr;
}

#undef LOCTEXT_NAMESPACE