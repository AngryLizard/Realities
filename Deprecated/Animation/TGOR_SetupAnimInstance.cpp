
#include "TGOR_SetupAnimInstance.h"
#include "RealitiesUtility/Utility/TGOR_Error.h"
#include "Rigs/RigControlHierarchy.h"
#include "AnimNode_ControlRig.h"
#include "Engine.h"

#include "Realities/CustomisationSystem/Components/TGOR_ControlComponent.h"
#include "Realities/AnimationSystem/Content/TGOR_Archetype.h"
#include "Realities/AnimationSystem/Components/TGOR_AttachComponent.h"
#include "Realities/AnimationSystem/Components/TGOR_HandleComponent.h"


UTGOR_SetupAnimInstance::UTGOR_SetupAnimInstance(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UTGOR_SetupAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();
}

void UTGOR_SetupAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	// TODO: This doesn't have to be done every tick
	UpdateControls();

	Super::NativeUpdateAnimation(DeltaSeconds);
}

void UTGOR_SetupAnimInstance::UpdateControls()
{
	// Grab ControlRig
	if (IAnimClassInterface* AnimBlueprintClass = IAnimClassInterface::GetFromClass(GetClass()))
	{
		const TArray<FStructProperty*>& AnimGraphNodeProperties = AnimBlueprintClass->GetAnimNodeProperties();
		for (const FStructProperty* AnimGraphNodeProperty : AnimGraphNodeProperties)
		{
			if (AnimGraphNodeProperty->Struct->IsChildOf(FAnimNode_Base::StaticStruct()) && 
				AnimGraphNodeProperty->Struct == FAnimNode_ControlRig::StaticStruct())
			{
				if (FAnimNode_ControlRig* LinkedControlRig = AnimGraphNodeProperty->ContainerPtrToValuePtr<FAnimNode_ControlRig>(this))
				{
					if (UControlRig* ControlRig = LinkedControlRig->GetControlRig())
					{
						UpdateControlsFromHierarchy(ControlRig->GetControlHierarchy());
					}
				}
			}
		}
	}
}

void UTGOR_SetupAnimInstance::UpdateControlsFromHierarchy(FRigControlHierarchy& Hierarchy)
{
	TArray<UTGOR_ControlComponent*> ControlComponents;
	if (USkeletalMeshComponent* OwnerComponent = GetSkelMeshComponent())
	{
		OwnerComponent->GetOwner()->GetComponents(ControlComponents);
		for (UTGOR_ControlComponent* ControlComponent : ControlComponents)
		{
			FTransform Transform = ControlComponent->GetRelativeTransform();

			// Go up the hierarchy
			USceneComponent* ParentComponent = ControlComponent->GetAttachParent();
			while (ParentComponent != OwnerComponent)
			{
				if (!ParentComponent)
				{
					return;
				}

				Transform = Transform * ParentComponent->GetRelativeTransform();
				ParentComponent = ParentComponent->GetAttachParent();
			}

			// Set control in Controlrig
			int32 ControlIndex = Hierarchy.GetIndex(ControlComponent->GetFName());
			if (ControlIndex != INDEX_NONE)
			{
				Hierarchy.SetGlobalTransform(ControlIndex, Transform);
			}
		}
	}
}
