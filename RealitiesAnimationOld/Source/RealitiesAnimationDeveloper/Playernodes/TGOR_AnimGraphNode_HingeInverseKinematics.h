// The Gateway of Realities: Planes of Existence.

#pragma once

#include "CoreMinimal.h"
#include "RealitiesAnimation/Nodes/TGOR_AnimNode_HingeInverseKinematics.h"

#include "AnimGraphNode_SkeletalControlBase.h"
#include "TGOR_AnimGraphNode_HingeInverseKinematics.generated.h"


/**
 * 
 */
UCLASS()
class REALITIESANIMATIONDEVELOPER_API UTGOR_AnimGraphNode_HingeInverseKinematics : public UAnimGraphNode_SkeletalControlBase
{
	GENERATED_UCLASS_BODY()

	UPROPERTY(EditAnywhere, Category = Settings)
		FTGOR_AnimNode_HingeInverseKinematics Node;

public:
	virtual FText GetNodeTitle(ENodeTitleType::Type TitleType) const override;
	virtual FText GetTooltipText() const override;

protected:
	virtual FText GetControllerDescription() const override;
	virtual const FAnimNode_SkeletalControlBase* GetNode() const override { return &Node; }
	
};
