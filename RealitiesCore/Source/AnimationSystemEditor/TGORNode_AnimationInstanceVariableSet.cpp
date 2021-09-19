// The Gateway of Realities: Planes of Existence.


#include "TGORNode_AnimationInstanceVariableSet.h"

#include "BlueprintNodeSpawner.h"
#include "AnimationSystem/Instances/TGOR_SubAnimInstance.h"
#include "AnimationSystem/Content/TGOR_Animation.h"
#include "DetailLayoutBuilder.h"

#include "AnimationSystemExtension/TGOR_AnimationInstanceVariableSet.h"
//#include "Engine.h"

#define LOCTEXT_NAMESPACE "AnimationInstanceVariableSet"


TSharedRef<IDetailCustomization> FAnimationInstanceVariableSetDetails::MakeInstance()
{
	return MakeShareable(new FAnimationInstanceVariableSetDetails());
}

void FAnimationInstanceVariableSetDetails::CustomizeDetails(class IDetailLayoutBuilder& DetailBuilder)
{
	TArray< TWeakObjectPtr<UObject> > SelectedObjectsList = DetailBuilder.GetSelectedObjects();

	// get first animgraph nodes
	UTGOR_AnimationInstanceVariableSet* Node = Cast<UTGOR_AnimationInstanceVariableSet>(SelectedObjectsList[0].Get());
	if (Node == NULL)
	{
		return;
	}

	// customize node's own details if needed
	Node->CustomizeDetails(DetailBuilder);
}


#undef LOCTEXT_NAMESPACE