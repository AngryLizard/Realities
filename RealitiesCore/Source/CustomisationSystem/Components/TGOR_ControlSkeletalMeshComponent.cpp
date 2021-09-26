// The Gateway of Realities: Planes of Existence.

#include "TGOR_ControlSkeletalMeshComponent.h"
#include "CustomisationSystem/Components/TGOR_ControlComponent.h"
#include "CustomisationSystem/Interfaces/TGOR_ControlInterface.h"
#include "Components/SkeletalMeshComponent.h"
#include "RealitiesUtility/Utility/TGOR_Error.h"

#include "ControlRig.h"
#include "Animation/NodeMappingContainer.h"


UTGOR_ControlSkeletalMeshComponent::UTGOR_ControlSkeletalMeshComponent()
	: Super()
	, TargetRig(nullptr)
	, bUpdateAnimationInEditor(true)
	, bNeedsRefreshTransform(false)
{
	bAutoActivate = true;
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bTickEvenWhenPaused = false;
	PrimaryComponentTick.TickGroup = TG_PrePhysics;

	bTickInEditor = true;
}

void UTGOR_ControlSkeletalMeshComponent::InitializeComponent()
{
	Super::InitializeComponent();

	InitAutoControlRig();
}

void UTGOR_ControlSkeletalMeshComponent::OnRegister()
{
	Super::OnRegister();

	InitAutoControlRig();
}

void UTGOR_ControlSkeletalMeshComponent::SetSkeletalMesh(class USkeletalMesh* NewMesh, bool bReinitPose)
{
	Super::SetSkeletalMesh(NewMesh, bReinitPose);

	InitAutoControlRig(true);
}

void UTGOR_ControlSkeletalMeshComponent::TickPose(float DeltaTime, bool bNeedsValidRootMotion)
{
	if (TargetRigBoneMapping.Num() > 0)
	{
		UpdateTransforms(0.0f);
	}
	else
	{
		InitAutoControlRig();
	}

	Super::TickPose(DeltaTime, bNeedsValidRootMotion);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

FTransform UTGOR_ControlSkeletalMeshComponent::GetRelativeControlTransform(const USceneComponent* Child) const
{
	FTransform Transform = Child->GetRelativeTransform();

	// Go up the hierarchy
	USceneComponent* ParentComponent = Child->GetAttachParent();
	while (ParentComponent != this)
	{
		if (!ParentComponent)
		{
			return Transform;
		}

		Transform = Transform * ParentComponent->GetRelativeTransform();
		ParentComponent = ParentComponent->GetAttachParent();
	}

	return Transform;
}

void UTGOR_ControlSkeletalMeshComponent::InitialiseControls(bool bForce)
{
	if (*TargetRigClass)
	{
		if (!IsValid(TargetRig) || !TargetRig->IsA(TargetRigClass) || bForce)
		{
			TargetRig = NewObject<UControlRig>(this, TargetRigClass);
			TargetRig->Initialize(true);
			TargetRig->RequestInit();
		}

		// register skeletalmesh component
		TargetRig->GetDataSourceRegistry()->RegisterDataSource(UControlRig::OwnerComponent, this);

		if (IsValid(SkeletalMesh))
		{
			TargetRig->SetBoneInitialTransformsFromSkeletalMesh(SkeletalMesh);

			TArray<FName> NodeNames;
			TArray<FNodeItem> NodeItems;
			TargetRig->GetMappableNodeData(NodeNames, NodeItems);

			TargetRigBoneMapping.Reset();
			const int32 NumTargetBones = SkeletalMesh->GetRefSkeleton().GetNum();
			for (uint16 Index = 0; Index < NumTargetBones; ++Index)
			{
				const FName& BoneName = SkeletalMesh->GetRefSkeleton().GetBoneName(Index);
				if (NodeNames.Contains(BoneName))
				{
					TargetRigBoneMapping.Add(BoneName, Index);
				}
			}

			TargetRigCurveMapping.Reset();
			/*
			// Not yet supported
			TArray<FName> const& CurveNames = BoneContainer.GetUIDToNameLookupTable();
			const FRigCurveContainer& RigCurveContainer = TargetRig->GetCurveContainer();
			for (uint16 Index = 0; Index < CurveNames.Num(); ++Index)
			{
				// see if the curve name exists in the control rig
				if (RigCurveContainer.GetIndex(CurveNames[Index]) != INDEX_NONE)
				{
					SourceRigCurveMapping.Add(CurveNames[Index], Index);
				}
			}
			*/
		}

		UpdateControlTransforms(true);

		// re-init when LOD changes
		// and restore control values
		TargetRig->Execute(EControlRigState::Init, TEXT("Update"));
	}

	bNeedsRefreshTransform = true;
}

void UTGOR_ControlSkeletalMeshComponent::UpdateTransforms(float DeltaTime)
{
	if (IsValid(TargetRig))
	{
		// reset transforms here to prevent additive transforms from accumulating to INF
		// we only update transforms from the mesh pose for bones in the current LOD, 
		// so the reset here ensures excluded bones are also reset
		TargetRig->GetBoneHierarchy().ResetTransforms();

		/*
		// we just do name mapping 
		for (auto Iter = SourceRigCurveMapping.CreateConstIterator(); Iter; ++Iter)
		{
			const FName& Name = Iter.Key();
			const uint16 Index = Iter.Value();

			TargetRig->SetCurveValue(Name, InOutput.Curve.Get(Index));
		}
		*/

		/// /////////////////////////////////////////////////////////

		TargetRig->SetDeltaTime(DeltaTime);

		UpdateControlTransforms(false);

		TargetRig->Evaluate_AnyThread();

		/// /////////////////////////////////////////////////////////

		if (IsValid(SkeletalMesh))
		{
			// get component pose from control rig
			for (auto Iter = TargetRigBoneMapping.CreateConstIterator(); Iter; ++Iter)
			{
				if (LocalSpaceTransforms.IsValidIndex(Iter.Value()))
				{
					LocalSpaceTransforms[Iter.Value()] = TargetRig->GetBoneHierarchy().GetLocalTransform(Iter.Key());
				}
			}
		}

		/*
		if (OutputSettings.bUpdateCurves)
		{
			// update curve
			for (auto Iter = ControlRigCurveMapping.CreateConstIterator(); Iter; ++Iter)
			{
				const FName& Name = Iter.Key();
				const uint16 Index = Iter.Value();

				const float Value = TargetRig->GetCurveValue(Name);
				InOutput.Curve.Set(Index, Value);
			}
		}
		*/

		bNeedsRefreshTransform = true;
	}
}

void UTGOR_ControlSkeletalMeshComponent::RefreshBoneTransforms(FActorComponentTickFunction* TickFunction)
{
	// Can't do anything without a SkeletalMesh
	if (!SkeletalMesh)
	{
		return;
	}

	// Do nothing more if no bones in skeleton.
	if (GetNumComponentSpaceTransforms() == 0)
	{
		return;
	}

	// We need the mesh space bone transforms now for renderer to get delta from ref pose:
	FillComponentSpaceTransforms();

	UpdateChildTransforms();
	UpdateBounds();
	MarkRenderTransformDirty();
	MarkRenderDynamicDataDirty();

	bNeedsRefreshTransform = false;
}

bool UTGOR_ControlSkeletalMeshComponent::AllocateTransformData()
{
	// Allocate transforms if not present.
	if (Super::AllocateTransformData())
	{
		LocalSpaceTransforms = SkeletalMesh->GetRefSkeleton().GetRefBonePose();
		FillComponentSpaceTransforms();

		return true;
	}

	LocalSpaceTransforms.Empty();

	return false;
}

bool UTGOR_ControlSkeletalMeshComponent::ShouldUpdateTransform(bool bLODHasChanged) const
{
#if WITH_EDITOR	
	if (GetWorld()->WorldType == EWorldType::Editor)
	{
		if (bUpdateAnimationInEditor)
		{
			return true;
		}
	}
#endif

	// we don't always update transform - each function when they changed will update
	return Super::ShouldUpdateTransform(bLODHasChanged) || bNeedsRefreshTransform;
}

void UTGOR_ControlSkeletalMeshComponent::FillComponentSpaceTransforms()
{
	if (!SkeletalMesh)
	{
		return;
	}

	if (LocalSpaceTransforms.Num() != SkeletalMesh->GetRefSkeleton().GetNum())
	{
		LocalSpaceTransforms = SkeletalMesh->GetRefSkeleton().GetRefBonePose();
	}

	// Compute component transforms of whole hierarchy
	const FTransform* LocalTransformsData = LocalSpaceTransforms.GetData();
	FTransform* SpaceBasesData = GetEditableComponentSpaceTransforms().GetData();

	SpaceBasesData[0] = LocalTransformsData[0];
	for (int32 BoneIndex = 1; BoneIndex < LocalSpaceTransforms.Num(); BoneIndex++)
	{
		FPlatformMisc::Prefetch(SpaceBasesData + BoneIndex);

		// For all bones below the root, final component-space transform is relative transform * component-space transform of parent.
		const int32 ParentIndex = SkeletalMesh->GetRefSkeleton().GetParentIndex(BoneIndex);
		FPlatformMisc::Prefetch(SpaceBasesData + ParentIndex);

		FTransform::Multiply(SpaceBasesData + BoneIndex, LocalTransformsData + BoneIndex, SpaceBasesData + ParentIndex);

		checkSlow(GetEditableComponentSpaceTransforms()[BoneIndex].IsRotationNormalized());
		checkSlow(!GetEditableComponentSpaceTransforms()[BoneIndex].ContainsNaN());
	}

	bNeedToFlipSpaceBaseBuffers = true;
	FinalizeBoneTransform();
}


#if WITH_EDITOR
void UTGOR_ControlSkeletalMeshComponent::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	FProperty* PropertyThatChanged = PropertyChangedEvent.Property;

	if (PropertyThatChanged != nullptr)
	{
		// if the blueprint has changed, recreate the AnimInstance
		if (PropertyThatChanged->GetFName() == GET_MEMBER_NAME_CHECKED(UTGOR_ControlSkeletalMeshComponent, TargetRigClass))
		{
			InitAutoControlRig();
		}
	}
}
#endif // WITH_EDITOR


void UTGOR_ControlSkeletalMeshComponent::InitAutoControlRig(bool bForceReinit)
{
	InitialiseControls(bForceReinit);
}

void UTGOR_ControlSkeletalMeshComponent::UpdateControlTransforms(bool bIsInitial)
{
	TArray<USceneComponent*> ChildComponents;
	GetChildrenComponents(true, ChildComponents);
	for (USceneComponent* ChildComponent : ChildComponents)
	{
		if (TScriptInterface<ITGOR_ControlInterface> ControlComponent = TScriptInterface<ITGOR_ControlInterface>(ChildComponent))
		{
			if (!ControlComponent->IsInitialOnly() || bIsInitial)
			{
				const int32 ControlIndex = TargetRig->GetControlHierarchy().GetIndex(ControlComponent->GetControlName());
				if (ControlIndex != INDEX_NONE)
				{
					const FTransform Transform = ControlComponent->GetControlTransform(this);
					TargetRig->GetControlHierarchy().SetGlobalTransform(ControlIndex, Transform);
				}
			}
		}
	}
}
