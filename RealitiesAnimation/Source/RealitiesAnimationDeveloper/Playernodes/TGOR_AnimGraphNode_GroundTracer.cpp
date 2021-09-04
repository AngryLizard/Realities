// The Gateway of Realities: Planes of Existence.

#include "TGOR_AnimGraphNode_GroundTracer.h"

#include "AnimationGraphSchema.h"
#include "Kismet2/CompilerResultsLog.h"
#include "AnimNodeEditModes.h"

#define LOCTEXT_NAMESPACE "A3Nodes"

UTGOR_AnimGraphNode_GroundTracer::UTGOR_AnimGraphNode_GroundTracer(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}


FText UTGOR_AnimGraphNode_GroundTracer::GetTooltipText() const
{
	return LOCTEXT("GroundTracer_ToolTip", "Moves a bone to a trace target");
}
FText UTGOR_AnimGraphNode_GroundTracer::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
	return LOCTEXT("GroundTracer_NodeTitle", "Ground Tracer");
}



void UTGOR_AnimGraphNode_GroundTracer::ValidateAnimNodeDuringCompilation(USkeleton* ForSkeleton, FCompilerResultsLog& MessageLog)
{
	// Temporary fix where skeleton is not fully loaded during AnimBP compilation and thus virtual bone name check is invalid UE-39499 (NEED FIX) 
	if (ForSkeleton && !ForSkeleton->HasAnyFlags(RF_NeedPostLoad))
	{
		if (ForSkeleton->GetReferenceSkeleton().FindBoneIndex(Node.Bone.Bone.BoneName) == INDEX_NONE)
		{
			if (Node.Bone.Bone.BoneName == NAME_None)
			{
				MessageLog.Warning(*LOCTEXT("NoBoneSelectedToCompare", "@@ - You must pick a bone to compare with").ToString(), this);
			}
			else
			{
				FFormatNamedArguments Args;
				Args.Add(TEXT("BoneName"), FText::FromName(Node.Bone.Bone.BoneName));

				FText Msg = FText::Format(LOCTEXT("NoBoneSelectedToCompare", "@@ - Bone {BoneName} not found in Skeleton"), Args);

				MessageLog.Warning(*Msg.ToString(), this);
			}
		}

		if (ForSkeleton->GetReferenceSkeleton().FindBoneIndex(Node.Bone.Target.BoneName) == INDEX_NONE)
		{
			if (Node.Bone.Target.BoneName == NAME_None)
			{
				MessageLog.Warning(*LOCTEXT("NoBoneSelectedToTarget", "@@ - You must pick a bone to target").ToString(), this);
			}
			else
			{
				FFormatNamedArguments Args;
				Args.Add(TEXT("BoneName"), FText::FromName(Node.Bone.Target.BoneName));

				FText Msg = FText::Format(LOCTEXT("NoBoneSelectedToTarget", "@@ - Bone {BoneName} not found in Skeleton"), Args);

				MessageLog.Warning(*Msg.ToString(), this);
			}
		}

		if (ForSkeleton->GetReferenceSkeleton().FindBoneIndex(Node.Bone.Reference.BoneName) == INDEX_NONE)
		{
			if (Node.Bone.Reference.BoneName == NAME_None)
			{
				MessageLog.Warning(*LOCTEXT("NoBoneSelectedToReferene", "@@ - You must pick a bone to reference").ToString(), this);
			}
			else
			{
				FFormatNamedArguments Args;
				Args.Add(TEXT("BoneName"), FText::FromName(Node.Bone.Reference.BoneName));

				FText Msg = FText::Format(LOCTEXT("NoBoneSelectedToReferene", "@@ - Bone {BoneName} not found in Skeleton"), Args);

				MessageLog.Warning(*Msg.ToString(), this);
			}
		}
	}

	Super::ValidateAnimNodeDuringCompilation(ForSkeleton, MessageLog);
}

FEditorModeID UTGOR_AnimGraphNode_GroundTracer::GetEditorMode() const
{
	return AnimNodeEditModes::AnimNode;
}

void UTGOR_AnimGraphNode_GroundTracer::CopyNodeDataToPreviewNode(FAnimNode_Base* InPreviewNode)
{
	FTGOR_AnimNode_GroundTracer* GroundTracer = static_cast<FTGOR_AnimNode_GroundTracer*>(InPreviewNode);

	GroundTracer->MaxDistance = Node.MaxDistance;
	GroundTracer->TraceRadius = Node.TraceRadius;

	GroundTracer->Direction = Node.Direction;
	GroundTracer->Rotator = Node.Rotator;
	GroundTracer->Contact = Node.Contact;

}

void UTGOR_AnimGraphNode_GroundTracer::CopyPinDefaultsToNodeData(UEdGraphPin* InPin)
{
	if (InPin->GetName() == GET_MEMBER_NAME_STRING_CHECKED(FTGOR_AnimNode_GroundTracer, Direction))
	{
		GetDefaultValue(GET_MEMBER_NAME_STRING_CHECKED(FTGOR_AnimNode_GroundTracer, Direction), Node.Direction);
	}
	else if (InPin->GetName() == GET_MEMBER_NAME_STRING_CHECKED(FTGOR_AnimNode_GroundTracer, Rotator))
	{
		GetDefaultValue(GET_MEMBER_NAME_STRING_CHECKED(FTGOR_AnimNode_GroundTracer, Rotator), Node.Rotator);
	}
	else if (InPin->GetName() == GET_MEMBER_NAME_STRING_CHECKED(FTGOR_AnimNode_GroundTracer, Rotator))
	{
		GetDefaultValue(GET_MEMBER_NAME_STRING_CHECKED(FTGOR_AnimNode_GroundTracer, Rotator), Node.Rotator);
	}
}


FText UTGOR_AnimGraphNode_GroundTracer::GetControllerDescription() const
{
	return LOCTEXT("GroundTracer", "Transform (Modify) Bone");
}

#undef LOCTEXT_NAMESPACE