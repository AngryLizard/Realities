// The Gateway of Realities: Planes of Existence.

#include "TGOR_RetargetComponent.h"

#include "CoreSystem/TGOR_Singleton.h"
#include "RealitiesUtility/Utility/TGOR_Error.h"

#include "ControlRig.h"
#include "Animation/NodeMappingContainer.h"

UTGOR_RetargetComponent::UTGOR_RetargetComponent()
:	Super()
,	SourceComponent(nullptr)
,	SourceRig(nullptr)
{
}

void UTGOR_RetargetComponent::InitialiseControls(bool bForce)
{
	if (*SourceRigClass && IsValid(SourceComponent))
	{
		if (!IsValid(SourceRig) || !SourceRig->IsA(SourceRigClass) || bForce)
		{
			SourceRig = NewObject<UControlRig>(this, SourceRigClass);
			SourceRig->Initialize(true);
			SourceRig->RequestInit();
		}

		// register skeletalmesh component
		SourceRig->GetDataSourceRegistry()->RegisterDataSource(UControlRig::OwnerComponent, this);

		if (IsValid(SourceComponent->SkeletalMesh))
		{
			SourceRig->SetBoneInitialTransformsFromSkeletalMesh(SourceComponent->SkeletalMesh);

			TArray<FName> NodeNames;
			TArray<FNodeItem> NodeItems;
			SourceRig->GetMappableNodeData(NodeNames, NodeItems);



			SourceRigBoneMapping.Reset();
			const FReferenceSkeleton& RefSkeleton = SourceComponent->SkeletalMesh->GetRefSkeleton();
			const int32 NumTargetBones = RefSkeleton.GetNum();
			for (uint16 Index = 0; Index < NumTargetBones; ++Index)
			{
				const FName& BoneName = RefSkeleton.GetBoneName(Index);
				if (NodeNames.Contains(BoneName))
				{
					SourceRigBoneMapping.Add(BoneName, Index);
				}
			}

			// Initialise target
			Super::InitialiseControls(bForce);

			// Find mappings
			RetargetRigControlMapping.Reset();
			if (TargetRig)
			{
				const FRigControlHierarchy& SourceControls = SourceRig->GetControlHierarchy();
				const FRigControlHierarchy& TargetControls = TargetRig->GetControlHierarchy();

				// even if not mapped, we map only node that exists in the TargetRig
				const int32 NumTargetControls = TargetControls.Num();
				for (int32 TargetIndex = 0; TargetIndex < NumTargetControls; ++TargetIndex)
				{
					const FName& BoneName = TargetControls.GetName(TargetIndex);
					const int32 SourceIndex = SourceControls.GetIndex(BoneName);
					if (SourceIndex != INDEX_NONE)
					{
						RetargetRigControlMapping.Add(TargetIndex, SourceIndex);
					}
				}
			}
		}

		SourceRig->Execute(EControlRigState::Init, TEXT("Update"));
	}
}


void UTGOR_RetargetComponent::UpdateTransforms(float DeltaTime)
{
	if (IsValid(SourceRig))
	{
		SourceRig->GetBoneHierarchy().ResetTransforms();

		if (SourceComponent && SourceComponent->SkeletalMesh->GetRefSkeleton().GetNum() == SourceRigBoneMapping.Num())
		{
			for (auto Iter = SourceRigBoneMapping.CreateConstIterator(); Iter; ++Iter)
			{
				const FTransform& Transform = SourceComponent->GetComponentSpaceTransforms()[Iter.Value()];
				SourceRig->SetGlobalTransform(Iter.Key(), Transform, false);
			}
		}
		/// /////////////////////////////////////////////////////////

		SourceRig->SetDeltaTime(DeltaTime);
		SourceRig->Evaluate_AnyThread();

		/// /////////////////////////////////////////////////////////

		const FRigControlHierarchy& SourceControls = SourceRig->GetControlHierarchy();
		if (IsValid(TargetRig))
		{
			FRigControlHierarchy& TargetControls = TargetRig->GetControlHierarchy();
			for (auto Iter = RetargetRigControlMapping.CreateConstIterator(); Iter; ++Iter)
			{
				const FTransform& Transform = SourceControls.GetGlobalTransform(Iter.Value());
				TargetControls.SetGlobalTransform(Iter.Key(), Transform);
			}
		}

		/// /////////////////////////////////////////////////////////

		Super::UpdateTransforms(DeltaTime);
	}
}

void UTGOR_RetargetComponent::InitAutoControlRig(bool bForceReinit)
{
	SourceComponent = Cast<USkeletalMeshComponent>(GetAttachParent());
	if (IsValid(SourceComponent))
	{
		// Update parent to animate same as the control
		SourceComponent->SetUpdateAnimationInEditor(bUpdateAnimationInEditor);
	}

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
