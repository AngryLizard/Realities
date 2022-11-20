// The Gateway of Realities: Planes of Existence.

#include "TGOR_ControlSkeletalMeshComponent.h"
#include "CustomisationSystem/Components/TGOR_ControlComponent.h"
#include "CustomisationSystem/Interfaces/TGOR_ControlInterface.h"
#include "Components/SkeletalMeshComponent.h"
#include "RealitiesUtility/Utility/TGOR_Error.h"

#include "ControlRig.h"
#include "Animation/NodeMappingContainer.h"

void FTGOR_ControlRigCache::InitialiseControls(TSubclassOf<UControlRig> ControlRigClass, UObject* Owner, USkinnedMeshComponent* SkinnedMesh, bool bForce)
{
	if (*ControlRigClass)
	{
		if (!IsValid(ControlRig) || !ControlRig->IsA(ControlRigClass) || bForce)
		{
			ControlRig = NewObject<UControlRig>(Owner, ControlRigClass);
			ControlRig->Initialize(true);
			ControlRig->RequestInit();
		}

		// Register skeletalmesh component
		ControlRig->GetDataSourceRegistry()->RegisterDataSource(UControlRig::OwnerComponent, Owner);

		USkinnedAsset* SkinnedMeshAsset = SkinnedMesh->GetSkinnedAsset();
		if (IsValid(SkinnedMeshAsset))
		{
			ControlRig->SetBoneInitialTransformsFromRefSkeleton(SkinnedMeshAsset->GetRefSkeleton());

			TArray<FName> NodeNames;
			TArray<FNodeItem> NodeItems;
			ControlRig->GetMappableNodeData(NodeNames, NodeItems);

			RigBoneMapping.Reset();
			const int32 NumTargetBones = SkinnedMeshAsset->GetRefSkeleton().GetNum();
			for (uint16 Index = 0; Index < NumTargetBones; ++Index)
			{
				const FName& BoneName = SkinnedMeshAsset->GetRefSkeleton().GetBoneName(Index);
				if (NodeNames.Contains(BoneName))
				{
					RigBoneMapping.Add(BoneName, Index);
				}
			}

			RigCurveMapping.Reset();

			/*
			// Not yet supported
			TArray<FName> const& CurveNames = BoneContainer.GetUIDToNameLookupTable();
			const FRigCurveContainer& RigCurveContainer = ControlRig->GetCurveContainer();
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

		// Register controls
		//InitialiseControlTransforms(SkinnedMesh);
		UpdateControlTransforms(SkinnedMesh, true);

		// re-init when LOD changes
		// and restore control values
		ControlRig->Execute(EControlRigState::Init, TEXT("Update"));
	}
	else
	{
		RigBoneMapping.Reset();
		RigCurveMapping.Reset();
		ControlMapping.Reset();
		ControlRig = nullptr;
	}
}

void FTGOR_ControlRigCache::InitialiseControlTransforms(USkinnedMeshComponent* SkinnedMesh)
{
	TArray<USceneComponent*> ChildComponents;
	SkinnedMesh->GetChildrenComponents(true, ChildComponents);
	for (USceneComponent* ChildComponent : ChildComponents)
	{
		if (TScriptInterface<ITGOR_ControlInterface> ControlComponent = TScriptInterface<ITGOR_ControlInterface>(ChildComponent))
		{
			URigHierarchy* Hierarchy = ControlRig->GetHierarchy();
			const int32 ControlIndex = Hierarchy->GetIndex(FRigElementKey(ControlComponent->GetControlName(), ERigElementType::Control));
			if (ControlIndex != INDEX_NONE)
			{
				ControlMapping.Emplace(ControlIndex, ControlComponent);
			}
		}
	}
}

void FTGOR_ControlRigCache::UpdateTransforms(TArray<FTransform>& Transforms, USkinnedMeshComponent* SkinnedMesh, float DeltaTime)
{
	if (IsValid(ControlRig))
	{
		// reset transforms here to prevent additive transforms from accumulating to INF
		// we only update transforms from the mesh pose for bones in the current LOD, 
		// so the reset here ensures excluded bones are also reset
		URigHierarchy* Hierarchy = ControlRig->GetHierarchy();
		//Hierarchy->ResetTransforms();

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

		ControlRig->SetDeltaTime(DeltaTime);

		UpdateControlTransforms(SkinnedMesh, false);

		ControlRig->Evaluate_AnyThread();

		/// /////////////////////////////////////////////////////////

		USkinnedAsset* SkinnedMeshAsset = SkinnedMesh->GetSkinnedAsset();
		if (IsValid(SkinnedMeshAsset))
		{
			// get component pose from control rig
			for (auto Iter = RigBoneMapping.CreateConstIterator(); Iter; ++Iter)
			{
				if (Transforms.IsValidIndex(Iter.Value()))
				{
					URigHierarchy* OtherHierarchy = ControlRig->GetHierarchy();
					Transforms[Iter.Value()] = OtherHierarchy->GetLocalTransform(FRigElementKey(Iter.Key(), ERigElementType::Bone));
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
	}
}

void FTGOR_ControlRigCache::UpdateControlTransforms(USkinnedMeshComponent* SkinnedMesh, bool bIsInitial)
{
	InitialiseControlTransforms(SkinnedMesh);

	for (const auto& Pair : ControlMapping)
	{
		if (!Pair.Value->IsInitialOnly() || bIsInitial)
		{
			URigHierarchy* Hierarchy = ControlRig->GetHierarchy();
			const FTransform Transform = Pair.Value->GetControlTransform(SkinnedMesh);
			Hierarchy->SetGlobalTransform(Pair.Key, Transform, true);
		}
	}
}

bool FTGOR_ControlRigCache::HasMapping() const
{
	return RigBoneMapping.Num() > 0 || ControlMapping.Num() > 0;
}


UTGOR_ControlSkeletalMeshComponent::UTGOR_ControlSkeletalMeshComponent()
	: Super()
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

void UTGOR_ControlSkeletalMeshComponent::SetSkinnedAssetAndUpdate(USkinnedAsset* NewMesh, bool bReinitPose)
{
	Super::SetSkinnedAssetAndUpdate(NewMesh, bReinitPose);
	InitAutoControlRig(true);
}

void UTGOR_ControlSkeletalMeshComponent::TickPose(float DeltaTime, bool bNeedsValidRootMotion)
{
	if (TargetCache.HasMapping())
	{
		UpdateTransforms(DeltaTime);
	}
	else
	{
		InitAutoControlRig();
	}

	Super::TickPose(DeltaTime, bNeedsValidRootMotion);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

FTransform UTGOR_ControlSkeletalMeshComponent::GetRelativeControlTransform(USkinnedMeshComponent* Parent, const USceneComponent* Child)
{
	FTransform Transform = Child->GetRelativeTransform();

	// Go up the hierarchy
	USceneComponent* ParentComponent = Child->GetAttachParent();
	while (ParentComponent != Parent)
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
	TargetCache.InitialiseControls(TargetRigClass, this, this, bForce);
	bNeedsRefreshTransform = true;
}

void UTGOR_ControlSkeletalMeshComponent::UpdateTransforms(float DeltaTime)
{
	TargetCache.UpdateTransforms(LocalSpaceTransforms, this, DeltaTime);
	bNeedsRefreshTransform = true;
}

void UTGOR_ControlSkeletalMeshComponent::RefreshBoneTransforms(FActorComponentTickFunction* TickFunction)
{
	// Can't do anything without a SkinnedAsset
	USkinnedAsset* SkinnedMeshAsset = GetSkinnedAsset();
	if (!SkinnedMeshAsset)
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
		USkinnedAsset* SkinnedMeshAsset = GetSkinnedAsset();
		LocalSpaceTransforms = SkinnedMeshAsset->GetRefSkeleton().GetRefBonePose();
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
	USkinnedAsset* SkinnedMeshAsset = GetSkinnedAsset();
	if (!SkinnedMeshAsset)
	{
		return;
	}

	if (LocalSpaceTransforms.Num() != SkinnedMeshAsset->GetRefSkeleton().GetNum())
	{
		LocalSpaceTransforms = SkinnedMeshAsset->GetRefSkeleton().GetRefBonePose();
	}

	// Compute component transforms of whole hierarchy
	const FTransform* LocalTransformsData = LocalSpaceTransforms.GetData();
	FTransform* SpaceBasesData = GetEditableComponentSpaceTransforms().GetData();

	SpaceBasesData[0] = LocalTransformsData[0];
	for (int32 BoneIndex = 1; BoneIndex < LocalSpaceTransforms.Num(); BoneIndex++)
	{
		FPlatformMisc::Prefetch(SpaceBasesData + BoneIndex);

		// For all bones below the root, final component-space transform is relative transform * component-space transform of parent.
		const int32 ParentIndex = SkinnedMeshAsset->GetRefSkeleton().GetParentIndex(BoneIndex);
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
