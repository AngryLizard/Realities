// The Gateway of Realities: Planes of Existence.


#include "TGOR_GetContent.h"

#include "Realities/Base/Content/TGOR_Content.h"

#include "KismetCompiler.h"
#include "BlueprintNodeSpawner.h"
#include "EditorCategoryUtils.h"
#include "Kismet2/BlueprintEditorUtils.h"
#include "BlueprintActionDatabaseRegistrar.h"
#include "Widgets/Text/STextBlock.h"
#include "Widgets/Input/SCheckBox.h"
#include "PropertyHandle.h"
#include "DetailLayoutBuilder.h"
#include "DetailCategoryBuilder.h"
#include "PropertyCustomizationHelpers.h"
#include "Toolkits/AssetEditorManager.h"
#include "K2Node_CallFunction.h"

#include "Realities/Base/TGOR_Singleton.h"

#define LOCTEXT_NAMESPACE "AnimationVariableSet"

struct FTGOR_GetContentHelper
{
	static FName ContentPinName;
	static FName WorldContextPinName;
};

FName FTGOR_GetContentHelper::ContentPinName(TEXT("Content"));
FName FTGOR_GetContentHelper::WorldContextPinName(TEXT("WorldContextObject"));

UTGOR_GetContent::UTGOR_GetContent(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer) 
{
}

UClass* UTGOR_GetContent::GetContentClass(const TArray<UEdGraphPin*>& SearchPins) const
{
	UClass* UseSpawnClass = nullptr;

	UEdGraphPin* ClassPin = GetContentPin(SearchPins);
	if (ClassPin && ClassPin->DefaultObject && ClassPin->LinkedTo.Num() == 0)
	{
		return(CastChecked<UClass>(ClassPin->DefaultObject));
	}
	else if (ClassPin && ClassPin->LinkedTo.Num())
	{
		UEdGraphPin* ClassSource = ClassPin->LinkedTo[0];
		return(ClassSource ? Cast<UClass>(ClassSource->PinType.PinSubCategoryObject.Get()) : nullptr);
	}

	return nullptr;
}

bool UTGOR_GetContent::UseWorldContext() const
{
	UBlueprint* BP = GetBlueprint();
	const UClass* ParentClass = BP ? BP->ParentClass : nullptr;
	return ParentClass ? ParentClass->HasMetaDataHierarchical(FBlueprintMetadata::MD_ShowWorldContextPin) != nullptr : false;
}

void UTGOR_GetContent::ReconstructNode()
{
	Super::ReconstructNode();
}

void UTGOR_GetContent::PostReconstructNode()
{
	Super::PostReconstructNode();
}

void UTGOR_GetContent::AllocateDefaultPins()
{
	// Add execution pins
	CreatePin(EGPD_Input, UEdGraphSchema_K2::PC_Exec, UEdGraphSchema_K2::PN_Execute);
	CreatePin(EGPD_Output, UEdGraphSchema_K2::PC_Exec, UEdGraphSchema_K2::PN_Then);

	// Add blueprint pins
	if (UseWorldContext())
	{
		CreatePin(EGPD_Input, UEdGraphSchema_K2::PC_Object, UTGOR_Singleton::StaticClass(), FTGOR_GetContentHelper::WorldContextPinName);
	}
	CreatePin(EGPD_Input, UEdGraphSchema_K2::PC_Class, UTGOR_Content::StaticClass(), FTGOR_GetContentHelper::ContentPinName);

	// Result pin
	CreatePin(EGPD_Output, UEdGraphSchema_K2::PC_Object, UTGOR_Content::StaticClass(), UEdGraphSchema_K2::PN_ReturnValue);
	
	Super::AllocateDefaultPins();
}

FText UTGOR_GetContent::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
	if (TitleType == ENodeTitleType::ListView || TitleType == ENodeTitleType::MenuTitle)
	{
		return LOCTEXT("GetContent_ShortTitle", "Get Content class");
	}
	else if (UClass* ContentClass = GetContentClass(Pins))
	{
		if (CachedNodeTitle.IsOutOfDate(this))
		{
			FFormatNamedArguments Args;
			Args.Add(TEXT("ClassName"), ContentClass->GetDisplayNameText());
			// FText::Format() is slow, so we cache this to save on performance
			CachedNodeTitle.SetCachedText(FText::Format(NSLOCTEXT("K2Node", "Get ", "Get {ClassName}"), Args), this);
		}
		return CachedNodeTitle;
	}
	return NSLOCTEXT("K2Node", "ContentPin_None", "Get Nothing");
}

void UTGOR_GetContent::PinDefaultValueChanged(UEdGraphPin* ChangedPin)
{
	Super::PinDefaultValueChanged(ChangedPin);

	if (ChangedPin && (ChangedPin->PinName == FTGOR_GetContentHelper::ContentPinName))
	{
		OnContentPinChanged();
	}
}

FText UTGOR_GetContent::GetTooltipText() const
{
	return LOCTEXT("ContentPin_ShortTitle", "Get content");
}

bool UTGOR_GetContent::HasExternalDependencies(TArray<class UStruct*>* OptionalOutput) const
{
	UClass* SourceClass = GetContentClass(Pins);
	const UBlueprint* SourceBlueprint = GetBlueprint();
	const bool bResult = (SourceClass && (SourceClass->ClassGeneratedBy != SourceBlueprint));
	if (bResult && OptionalOutput)
	{
		OptionalOutput->AddUnique(SourceClass);
	}
	const bool bSuperResult = Super::HasExternalDependencies(OptionalOutput);
	return bSuperResult || bResult;
}


bool UTGOR_GetContent::IsCompatibleWithGraph(const UEdGraph* TargetGraph) const
{
	UBlueprint* Blueprint = FBlueprintEditorUtils::FindBlueprintForGraph(TargetGraph);
	return Super::IsCompatibleWithGraph(TargetGraph) && (!Blueprint || FBlueprintEditorUtils::FindUserConstructionScript(Blueprint) != TargetGraph);
}


void UTGOR_GetContent::PinConnectionListChanged(UEdGraphPin* Pin)
{
	Super::PinConnectionListChanged(Pin);

	if (Pin && (Pin->PinName == FTGOR_GetContentHelper::ContentPinName))
	{
		OnContentPinChanged();
	}
}

void UTGOR_GetContent::GetPinHoverText(const UEdGraphPin& Pin, FString& HoverTextOut) const
{
	if (UEdGraphPin* ContentPin = GetContentPin(Pins))
	{
		SetPinToolTip(*ContentPin, LOCTEXT("ContentPinDescription", "Content class to query database for"));
	}

	return Super::GetPinHoverText(Pin, HoverTextOut);
}


void UTGOR_GetContent::PostPlacedNewNode()
{
	Super::PostPlacedNewNode();

}


void UTGOR_GetContent::ExpandNode(class FKismetCompilerContext& CompilerContext, UEdGraph* SourceGraph)
{
	Super::ExpandNode(CompilerContext, SourceGraph);

	UK2Node_CallFunction* CallCreateNode = CompilerContext.SpawnIntermediateNode<UK2Node_CallFunction>(this, SourceGraph);
	CallCreateNode->FunctionReference.SetExternalMember(GET_FUNCTION_NAME_CHECKED(UTGOR_Content, GetContentStatic), UTGOR_Content::StaticClass());
	CallCreateNode->AllocateDefaultPins();

	// store off the class to spawn before we mutate pin connections:
	UClass* ClassToSpawn = GetContentClass(Pins);

	bool bSucceeded = true;
	//connect exe
	{
		UEdGraphPin* SpawnExecPin = GetExecPin();
		UEdGraphPin* CallExecPin = CallCreateNode->GetExecPin();
		bSucceeded &= SpawnExecPin && CallExecPin && CompilerContext.MovePinLinksToIntermediate(*SpawnExecPin, *CallExecPin).CanSafeConnect();
	}

	//connect world context
	{
		UEdGraphPin* WorldContextPin = GetWorldContextPin();
		if (WorldContextPin)
		{
			UEdGraphPin* CallWorldContextPin = CallCreateNode->FindPin(TEXT("WorldContextObject"));
			bSucceeded &= WorldContextPin && CallWorldContextPin && CompilerContext.MovePinLinksToIntermediate(*WorldContextPin, *CallWorldContextPin).CanSafeConnect();
		}
	}

	//connect class
	{
		UEdGraphPin* SpawnClassPin = GetContentPin(Pins);
		UEdGraphPin* CallClassPin = CallCreateNode->FindPin(TEXT("Class"));
		bSucceeded &= SpawnClassPin && CallClassPin && CompilerContext.MovePinLinksToIntermediate(*SpawnClassPin, *CallClassPin).CanSafeConnect();
	}

	UEdGraphPin* CallResultPin = nullptr;
	//connect result
	{
		UEdGraphPin* SpawnResultPin = GetResultPin();
		CallResultPin = CallCreateNode->GetReturnValuePin();

		// cast HACK. It should be safe. The only problem is native code generation.
		if (SpawnResultPin && CallResultPin)
		{
			CallResultPin->PinType = SpawnResultPin->PinType;
		}
		bSucceeded &= SpawnResultPin && CallResultPin && CompilerContext.MovePinLinksToIntermediate(*SpawnResultPin, *CallResultPin).CanSafeConnect();
	}

	//assign exposed values and connect then
	{
		UEdGraphPin* LastThen = FKismetCompilerUtilities::GenerateAssignmentNodes(CompilerContext, SourceGraph, CallCreateNode, this, CallResultPin, ClassToSpawn);
		UEdGraphPin* SpawnNodeThen = GetThenPin();
		bSucceeded &= SpawnNodeThen && LastThen && CompilerContext.MovePinLinksToIntermediate(*SpawnNodeThen, *LastThen).CanSafeConnect();
	}

	BreakAllNodeLinks();

	if (!bSucceeded)
	{
		CompilerContext.MessageLog.Error(*LOCTEXT("GetContent_Error", "ICE: GetContent error @@").ToString(), this);
	}
}

void UTGOR_GetContent::ReallocatePinsDuringReconstruction(TArray<UEdGraphPin*>& OldPins)
{
	AllocateDefaultPins();
	if (UClass* Category = GetContentClass(OldPins))
	{
		// Change class of output pin
		UEdGraphPin* ResultPin = GetResultPin();
		UClass* Class = Category->GetAuthoritativeClass();
		ResultPin->PinType.PinSubCategoryObject = Class;
	}
	RestoreSplitPins(OldPins);
}

void UTGOR_GetContent::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
	
}

void UTGOR_GetContent::GetNodeAttributes(TArray<TKeyValuePair<FString, FString>>& OutNodeAttributes) const
{
	OutNodeAttributes.Add(TKeyValuePair<FString, FString>(TEXT("Type"), TEXT("GetContent")));
	OutNodeAttributes.Add(TKeyValuePair<FString, FString>(TEXT("Class"), GetClass()->GetName()));
	OutNodeAttributes.Add(TKeyValuePair<FString, FString>(TEXT("Name"), GetName()));
}

void UTGOR_GetContent::GetMenuActions(FBlueprintActionDatabaseRegistrar& ActionRegistrar) const
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

bool UTGOR_GetContent::IsActionFilteredOut(class FBlueprintActionFilter const& Filter)
{
	return(false);
}


FText UTGOR_GetContent::GetMenuCategory() const
{
	return FEditorCategoryUtils::GetCommonCategory(FCommonEditorCategory::Gameplay);
}



UEdGraphPin* UTGOR_GetContent::GetThenPin() const
{
	UEdGraphPin* Pin = FindPinChecked(UEdGraphSchema_K2::PN_Then);
	check(Pin->Direction == EGPD_Output);
	return Pin;
}

UEdGraphPin* UTGOR_GetContent::GetWorldContextPin() const
{
	UEdGraphPin* Pin = FindPin(FTGOR_GetContentHelper::WorldContextPinName);
	check(Pin == nullptr || Pin->Direction == EGPD_Input);
	return Pin;
}

UEdGraphPin* UTGOR_GetContent::GetContentPin(const TArray<UEdGraphPin*>& SearchPins) const
{
	UEdGraphPin* Pin = nullptr;
	for (UEdGraphPin* TestPin : SearchPins)
	{
		if (TestPin && TestPin->PinName == FTGOR_GetContentHelper::ContentPinName)
		{
			Pin = TestPin;
			break;
		}
	}
	check(Pin == nullptr || Pin->Direction == EGPD_Input);
	return Pin;
}

UEdGraphPin* UTGOR_GetContent::GetResultPin() const
{
	UEdGraphPin* Pin = FindPinChecked(UEdGraphSchema_K2::PN_ReturnValue);
	check(Pin->Direction == EGPD_Output);
	return Pin;
}

void UTGOR_GetContent::SetPinToolTip(UEdGraphPin& MutatablePin, const FText& PinDescription) const
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


bool UTGOR_GetContent::IsContentVarPin(UEdGraphPin* Pin) const
{
	return(Pin->PinName != UEdGraphSchema_K2::PN_Execute &&
		Pin->PinName != UEdGraphSchema_K2::PN_Then &&
		Pin->PinName != UEdGraphSchema_K2::PN_ReturnValue &&
		Pin->PinName != FTGOR_GetContentHelper::ContentPinName &&
		Pin->PinName != FTGOR_GetContentHelper::WorldContextPinName);
}

void UTGOR_GetContent::OnContentPinChanged()
{
	// Remove all pins related to archetype variables
	TArray<UEdGraphPin*> OldPins = Pins;
	TArray<UEdGraphPin*> OldClassPins;

	for (UEdGraphPin* OldPin : OldPins)
	{
		if (IsContentVarPin(OldPin))
		{
			Pins.Remove(OldPin);
			OldClassPins.Add(OldPin);
		}
	}

	CachedNodeTitle.MarkDirty();

	if (UClass* Category = GetContentClass(OldPins))
	{
		// Change class of output pin
		UEdGraphPin* ResultPin = GetResultPin();
		UClass* Class = Category->GetAuthoritativeClass();
		ResultPin->PinType.PinSubCategoryObject = Class;
	}

	RestoreSplitPins(OldPins);

	UEdGraphPin* ResultPin = GetResultPin();
	// Cache all the pin connections to the ResultPin, we will attempt to recreate them
	TArray<UEdGraphPin*> ResultPinConnectionList = ResultPin->LinkedTo;
	// Because the archetype has changed, we break the output link as the output pin type will change
	ResultPin->BreakAllPinLinks(true);

	const UEdGraphSchema_K2* K2Schema = GetDefault<UEdGraphSchema_K2>();

	// Recreate any pin links to the Result pin that are still valid
	for (UEdGraphPin* Connections : ResultPinConnectionList)
	{
		K2Schema->TryCreateConnection(ResultPin, Connections);
	}

	// Rewire the old pins to the new pins so connections are maintained if possible
	RewireOldPinsToNewPins(OldClassPins, Pins, nullptr);

	// Refresh the UI for the graph so the pin changes show up
	GetGraph()->NotifyGraphChanged();

	// Mark dirty
	FBlueprintEditorUtils::MarkBlueprintAsModified(GetBlueprint());
}

#undef LOCTEXT_NAMESPACE