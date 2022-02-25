// The Gateway of Realities: Planes of Existence.


#include "TGOR_AnimGraphNode_HingeInverseKinematics.h"

#define LOCTEXT_NAMESPACE "A3Nodes"

UTGOR_AnimGraphNode_HingeInverseKinematics::UTGOR_AnimGraphNode_HingeInverseKinematics(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

FText UTGOR_AnimGraphNode_HingeInverseKinematics::GetControllerDescription() const
{
	return LOCTEXT("HingeGradientDescent_IK_Description", "Hinge IK");
}

FText UTGOR_AnimGraphNode_HingeInverseKinematics::GetTooltipText() const
{
	return LOCTEXT("HingeGradientDescent_IK_Tooltip", "IK node using gradient descent on a chain of hinges.");
}

FText UTGOR_AnimGraphNode_HingeInverseKinematics::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
	return GetControllerDescription();
}

#undef LOCTEXT_NAMESPACE