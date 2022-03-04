// The Gateway of Realities: Planes of Existence.

#include "TGOR_RetargetComponent.h"

#include "CoreSystem/TGOR_Singleton.h"
#include "RealitiesUtility/Utility/TGOR_Error.h"

#include "ControlRig.h"
#include "Animation/NodeMappingContainer.h"

UTGOR_RetargetComponent::UTGOR_RetargetComponent()
:	Super()
,	SourceComponent(nullptr)
{
}

void UTGOR_RetargetComponent::InitialiseControls(bool bForce)
{
	// Initialise target
	Super::InitialiseControls(bForce);

	if (IsValid(SourceComponent))
	{
		// Initialise source
		SourceCache.InitialiseControls(SourceRigClass, this, SourceComponent, bForce);

		// Find mappings
		RetargetRigControlMapping.Reset();
		if (SourceCache.ControlRig && TargetCache.ControlRig)
		{
			URigHierarchy* SourceHierarchy = SourceCache.ControlRig->GetHierarchy();
			TArray<FRigControlElement*> SourceControls = SourceHierarchy->GetControls();

			URigHierarchy* TargetHierarchy = TargetCache.ControlRig->GetHierarchy();
			TArray<FRigControlElement*> TargetControls = TargetHierarchy->GetControls();

			// even if not mapped, we map only nodes that exist in the TargetRig
			const int32 NumTargetControls = TargetControls.Num();
			for (int32 Target = 0; Target < NumTargetControls; ++Target)
			{
				FRigControlElement* TargetControl = TargetControls[Target];
				const int32 SourceIndex = SourceHierarchy->GetIndex(FRigElementKey(TargetControl->GetName(), ERigElementType::Control));
				if (SourceIndex != INDEX_NONE)
				{
					const int32 TargetIndex = TargetControls[Target]->GetIndex();
					RetargetRigControlMapping.Add(TargetIndex, SourceIndex);
				}
			}
		}
	}
}

void UTGOR_RetargetComponent::UpdateTransforms(float DeltaTime)
{
	SourceCache.UpdateTransforms(LocalSpaceTransforms, this, DeltaTime);

	if (IsValid(SourceCache.ControlRig))
	{
		URigHierarchy* SourceHierarchy = SourceCache.ControlRig->GetHierarchy();
		TArray<FRigControlElement*> SourceControls = SourceHierarchy->GetControls();
		if (IsValid(TargetCache.ControlRig))
		{
			URigHierarchy* TargetHierarchy = TargetCache.ControlRig->GetHierarchy();
			TArray<FRigControlElement*> TargetControls = TargetHierarchy->GetControls();
			for (auto Iter = RetargetRigControlMapping.CreateConstIterator(); Iter; ++Iter)
			{
				const FTransform& Transform = SourceHierarchy->GetGlobalTransform(Iter.Value());
				TargetHierarchy->SetGlobalTransform(Iter.Key(), Transform, false, true);
			}
		}
	}

	Super::UpdateTransforms(DeltaTime);
}

void UTGOR_RetargetComponent::InitAutoControlRig(bool bForceReinit)
{
#if WITH_EDITOR
	SourceComponent = Cast<USkeletalMeshComponent>(GetAttachParent());
	if (IsValid(SourceComponent))
	{
		// Update parent to animate same as the control
		SourceComponent->SetUpdateAnimationInEditor(bUpdateAnimationInEditor);
	}
#endif // WITH_EDITOR

	Super::InitAutoControlRig(bForceReinit);
}

#if WITH_EDITOR
void UTGOR_RetargetComponent::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	FProperty* PropertyThatChanged = PropertyChangedEvent.Property;
	if (PropertyThatChanged != nullptr)
	{
		// if the blueprint has changed, recreate the AnimInstance
		if (PropertyThatChanged->GetFName() == GET_MEMBER_NAME_CHECKED(UTGOR_RetargetComponent, SourceRigClass))
		{
			InitAutoControlRig();
		}
	}
}
#endif // WITH_EDITOR
