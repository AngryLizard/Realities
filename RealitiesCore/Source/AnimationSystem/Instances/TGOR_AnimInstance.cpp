// Fill out your copyright notice in the Description page of Project Settings.


#include "TGOR_AnimInstance.h"
#include "RealitiesUtility/Utility/TGOR_Error.h"
#include "Rigs/RigControlHierarchy.h"
#include "AnimNode_ControlRig.h"
#include "Engine.h"

#include "AnimationSystem/Instances/TGOR_SubAnimInstance.h"
#include "AnimationSystem/Content/TGOR_Animation.h"
#include "AnimationSystem/Content/TGOR_Performance.h"
#include "AnimationSystem/Content/TGOR_Archetype.h"
#include "AnimationSystem/Content/TGOR_RigParam.h"
#include "AnimationSystem/Tasks/TGOR_AnimatedTask.h"

#include "AnimationSystem/Components/TGOR_AttachComponent.h"
#include "AnimationSystem/Components/TGOR_HandleComponent.h"
#include "DimensionSystem/Content/TGOR_Primitive.h"


FTGOR_SubAnimationInstance::FTGOR_SubAnimationInstance()
: IsSwitched(false), 
Previous(nullptr), 
Current(nullptr), 
DefaultOn(nullptr), 
DefaultOff(nullptr)
{
}

FTGOR_SubAnimBlend::FTGOR_SubAnimBlend()
: IsSwitched(false),
OnBlend(0.5f),
OffBlend(0.5f)
{
}

FTGOR_AnimInstanceProxy::FTGOR_AnimInstanceProxy()
: Super()
{
}

FTGOR_AnimInstanceProxy::FTGOR_AnimInstanceProxy(UAnimInstance* Instance)
	: Super(Instance)
{
}

void FTGOR_AnimInstanceProxy::PrintSyncGroups(const TArray<FName>& SyncGroups) const
{
	const FSyncGroupMap& GroupRead = GetSyncGroupMapRead();
	for (const auto& Pair : GroupRead)
	{
		for (const FAnimTickRecord& Record : Pair.Value.ActivePlayers)
		{
			VPORT(Record.SourceAsset);
		}
	}
}

bool FTGOR_AnimInstanceProxy::HasSyncGroupInstance(const FName& SyncGroup) const
{
	const FSyncGroupMap& GroupRead = GetSyncGroupMapRead();
	const int32 Index = GetSyncGroupIndexFromName(SyncGroup);
	return GroupRead.Contains(SyncGroup);
}

const FAnimGroupInstance& FTGOR_AnimInstanceProxy::GetSyncGroupInstance(const FName& SyncGroup) const
{
	const FSyncGroupMap& GroupRead = GetSyncGroupMapRead();
	return GroupRead[SyncGroup];
}

UTGOR_AnimInstance::UTGOR_AnimInstance(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer),
	DefaultBlendTime(0.25f)
{
}

void UTGOR_AnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

}

void UTGOR_AnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	UpdateHandles();
	AnimationTaskLock = false;

	Super::NativeUpdateAnimation(DeltaSeconds);
}

bool UTGOR_AnimInstance::HasQueue(UTGOR_Performance* Performance) const
{
	return AnimationInstanceQueues.Contains(Performance);
}

UTGOR_AnimatedTask* UTGOR_AnimInstance::GetQueue(UTGOR_Performance* Performance) const
{
	if (const FTGOR_SubAnimationInstance* Ptr = AnimationInstanceQueues.Find(Performance))
	{
		return Ptr->Current.IsValid() ? Ptr->Current->AnimatedTask.Get() : nullptr;
	}
	return nullptr;
}

FTGOR_SubAnimBlend UTGOR_AnimInstance::GetBlendInfo(TSubclassOf<UTGOR_Performance> Type) const
{
	FTGOR_SubAnimBlend Blend;
	if (IsValid(Archetype))
	{
		UTGOR_Performance* Performance = Archetype->Instanced_PerformanceInsertions.GetOfType<UTGOR_Performance>(Type);
		if (IsValid(Performance))
		{
			if (const FTGOR_SubAnimationInstance* Ptr = AnimationInstanceQueues.Find(Performance))
			{
				Blend.IsSwitched = Ptr->IsSwitched;

				float CurrBlend = DefaultBlendTime;
				if (Ptr->Current.IsValid() && Ptr->Current->AnimatedTask.IsValid())
				{
					CurrBlend = Ptr->Current->AnimatedTask->BlendTime;
				}

				float PrevBlend = DefaultBlendTime;
				if (Ptr->Previous.IsValid() && Ptr->Previous->AnimatedTask.IsValid())
				{
					PrevBlend = Ptr->Previous->AnimatedTask->BlendTime;
				}

				Blend.OnBlend = Blend.IsSwitched ? PrevBlend : CurrBlend;
				Blend.OffBlend = Blend.IsSwitched ? CurrBlend : PrevBlend;
			}
		}
	}
	return Blend;
}


UTGOR_SubAnimInstance* UTGOR_AnimInstance::GetSubAnimInstance(UTGOR_Performance* Performance) const
{
	if (IsValid(Performance))
	{
		const FTGOR_SubAnimationInstance* Ptr = AnimationInstanceQueues.Find(Performance);
		if (Ptr)
		{
			return Ptr->Current.Get();
		}
	}
	return nullptr;
}

FName UTGOR_AnimInstance::GetSubAnimName(UTGOR_Performance* Performance) const
{
	const FTGOR_SubAnimationInstance* Ptr = AnimationInstanceQueues.Find(Performance);
	if (Ptr && Ptr->IsSwitched)
	{
		return FName(*(Performance->SubAnimIdentifier + "_On"));
	}
	return FName(*(Performance->SubAnimIdentifier + "_Off"));
}

void UTGOR_AnimInstance::AddAnimationInstance(UTGOR_Performance* Performance, UTGOR_AnimatedTask* AnimatedTask)
{
	if (!IsValid(Performance))
	{
		return;
	}

	// Wait on parallel tasks to finish (This is arguably an engine bug)
	// DO NOT DELETE UNTIL LinkAnimGraphByTag IS MADE THREAD-SAFE
	GetProxyOnAnyThread<FAnimInstanceProxy>();

	if (IsValid(AnimatedTask))
	{
		// Get or add instance to queue
		FTGOR_SubAnimationInstance& Instance = AnimationInstanceQueues.FindOrAdd(Performance);
		if (!AnimationTaskLock)
		{
			Instance.IsSwitched = !Instance.IsSwitched;
			AnimationTaskLock = true;
		}
		Instance.Previous = Instance.Current;
		if (Instance.Previous.IsValid())
		{
			Instance.Previous->OnRemovedFromParent();
		}

		const FName AnimationKey = GetSubAnimName(Performance);
		TSubclassOf<UTGOR_SubAnimInstance>& Default = Instance.IsSwitched ? Instance.DefaultOn : Instance.DefaultOff;
		if (!IsValid(Default))
		{
			// Set default to current state if not already set
			UAnimInstance* AnimInstance = GetLinkedAnimGraphInstanceByTag(AnimationKey);
			UTGOR_SubAnimInstance* CurrentInstance = Cast<UTGOR_SubAnimInstance>(AnimInstance);
			if (IsValid(CurrentInstance))
			{
				Default = CurrentInstance->GetClass();
			}
		}

		// Check that instance can produce an animation instance
		if (IAnimClassInterface::GetFromClass(AnimatedTask->InstanceClass))
		{
			// Set anim instance to graph and queue
			LinkAnimGraphByTag(AnimationKey, AnimatedTask->InstanceClass);
			Instance.Current = Cast<UTGOR_SubAnimInstance>(GetLinkedAnimGraphInstanceByTag(AnimationKey));
			if (Instance.Current.IsValid())
			{
				Instance.Current->ParentInstance = this;
				Instance.Current->AnimatedTask = AnimatedTask;
				Instance.Current->ParentSlot = Performance;
				Instance.Current->OnAddedToParent();
			}
		}
	}
	else
	{
		// TODO: For now instances cannot be layered, implement a queue in case multiple tasks are active per peformance
		RemoveAnimationInstance(Performance, AnimatedTask);
	}
}

void UTGOR_AnimInstance::RemoveAnimationInstance(UTGOR_Performance* Performance, UTGOR_AnimatedTask* AnimatedTask)
{
	if (!IsValid(Performance))
	{
		return;
	}

	// Wait on parallel tasks to finish (This is arguably an engine bug)
	// DO NOT DELETE UNTIL LinkAnimGraphByTag IS MADE THREAD-SAFE
	GetProxyOnAnyThread<FAnimInstanceProxy>();

	if (FTGOR_SubAnimationInstance* Ptr = AnimationInstanceQueues.Find(Performance))
	{
		// Set default value instead
		if (!AnimationTaskLock)
		{
			Ptr->IsSwitched = !Ptr->IsSwitched;
			AnimationTaskLock = true;
		}
		Ptr->Previous = Ptr->Current;
		if (Ptr->Previous.IsValid())
		{
			Ptr->Previous->OnRemovedFromParent();
		}

		const FName AnimationKey = GetSubAnimName(Performance);
		const TSubclassOf<UTGOR_SubAnimInstance>& Default = Ptr->IsSwitched ? Ptr->DefaultOn : Ptr->DefaultOff;
		if (!IsValid(Default))
		{
			LinkAnimGraphByTag(AnimationKey, Default);
			Ptr->Current = Cast<UTGOR_SubAnimInstance>(GetLinkedAnimGraphInstanceByTag(AnimationKey));
			if (Ptr->Current.IsValid())
			{
				Ptr->Current->ParentInstance = this;
				Ptr->Current->AnimatedTask = nullptr;
				Ptr->Current->ParentSlot = Performance;
				Ptr->Current->OnAddedToParent();
			}
		}
	}
}

void UTGOR_AnimInstance::ClearQueues()
{
	// TODO: Properly clear queue with empty animations
	AnimationInstanceQueues.Empty();
}

void UTGOR_AnimInstance::UpdateHandles()
{
	// Change control transforms
	if (LinkedControlRig)
	{
		for (UTGOR_HandleComponent* Handle : HandleComponents)
		{
			if (IsValid(Handle) && IsValid(Handle->MovementCone))
			{
				FTransform Transform = Handle->GetRelativeTransform() * Handle->MovementCone->GetRelativeTransform();
				//Transform.SetLocation(Handle->MovementCone->ProjectInside(Transform.GetLocation()));
				Transform.SetRotation(Transform.GetRotation());
				Transform.SetScale3D(FVector(1.0f, 1.0f, 1.0f));

				UControlRig* ControlRig = LinkedControlRig->GetControlRig();
				if (IsValid(ControlRig))
				{
					URigHierarchy* Hierarchy = ControlRig->GetHierarchy();
					int32 ControlIndex = Hierarchy->GetIndex(FRigElementKey(Handle->ControlName, ERigElementType::Control));
					if (ControlIndex != INDEX_NONE)
					{
						Hierarchy->SetGlobalTransform(ControlIndex, Transform, false, true);
					}
				}
			}
		}
	}
}

void UTGOR_AnimInstance::AssignArchetype(UTGOR_Archetype* InArchetype, const TMap<UTGOR_RigParam*, FTGOR_Normal>& Params)
{
	// Wait on parallel tasks to finish (This is arguable an engine bug)
	GetProxyOnAnyThread<FAnimInstanceProxy>();

	Archetype = InArchetype;

	// Grab ControlRig
	if (IAnimClassInterface* AnimBlueprintClass = IAnimClassInterface::GetFromClass(GetClass()))
	{
		const TArray<FStructProperty*>& AnimGraphNodeProperties = AnimBlueprintClass->GetAnimNodeProperties();
		for (const FStructProperty* AnimGraphNodeProperty : AnimGraphNodeProperties)
		{
			if (AnimGraphNodeProperty->Struct->IsChildOf(FAnimNode_Base::StaticStruct()))
			{
				if (AnimGraphNodeProperty->Struct == FAnimNode_ControlRig::StaticStruct())
				{
					LinkedControlRig = AnimGraphNodeProperty->ContainerPtrToValuePtr<FAnimNode_ControlRig>(this);
					if (LinkedControlRig)
					{
						break;
					}
				}
			}
		}
	}

	HandleComponents.Empty();
	AttachComponents.Empty();
	if (USkeletalMeshComponent* OwnerComponent = GetSkelMeshComponent())
	{
		OwnerComponent->GetOwner()->GetComponents(AttachComponents);
		OwnerComponent->GetOwner()->GetComponents(HandleComponents);

		if (IsValid(Archetype))
		{
			Archetype->UpdateAnimInstance(Params);
		}
	}

	OnSetupArchetype();
}


void UTGOR_AnimInstance::StopMontageBySlot(float InBlendOutTime, FName SlotNodeName)
{
	// stop temporary montage
	// when terminate (in the Montage_Advance), we have to lose reference to the temporary montage
	if (SlotNodeName != NAME_None)
	{
		for (int32 InstanceIndex = 0; InstanceIndex < MontageInstances.Num(); InstanceIndex++)
		{
			// check if this is playing
			FAnimMontageInstance* MontageInstance = MontageInstances[InstanceIndex];
			// make sure what is active right now is transient that we created by request
			if (MontageInstance && MontageInstance->IsActive() && MontageInstance->IsPlaying())
			{
				UAnimMontage* CurMontage = MontageInstance->Montage;
				if (CurMontage)
				{
					// Check each track, in practice there should only be one on these
					for (int32 SlotTrackIndex = 0; SlotTrackIndex < CurMontage->SlotAnimTracks.Num(); SlotTrackIndex++)
					{
						const FSlotAnimationTrack* AnimTrack = &CurMontage->SlotAnimTracks[SlotTrackIndex];
						if (AnimTrack && AnimTrack->SlotName == SlotNodeName)
						{
							// Found it
							MontageInstance->Stop(FAlphaBlend(InBlendOutTime));
							break;
						}
					}
				}
			}
		}
	}
	else
	{
		// Stop all
		Montage_Stop(InBlendOutTime);
	}
}

UAnimMontage* UTGOR_AnimInstance::GetMontageBySlot(FName SlotNodeName)
{
	if (SlotNodeName != NAME_None)
	{
		for (int32 InstanceIndex = 0; InstanceIndex < MontageInstances.Num(); InstanceIndex++)
		{
			// check if this is playing
			FAnimMontageInstance* MontageInstance = MontageInstances[InstanceIndex];
			// make sure what is active right now is transient that we created by request
			if (MontageInstance && MontageInstance->IsActive() && MontageInstance->IsPlaying())
			{
				UAnimMontage* CurMontage = MontageInstance->Montage;
				if (CurMontage)
				{
					// Check each track, in practice there should only be one on these
					for (int32 SlotTrackIndex = 0; SlotTrackIndex < CurMontage->SlotAnimTracks.Num(); SlotTrackIndex++)
					{
						const FSlotAnimationTrack* AnimTrack = &CurMontage->SlotAnimTracks[SlotTrackIndex];
						if (AnimTrack && AnimTrack->SlotName == SlotNodeName)
						{
							return(CurMontage);
						}
					}
				}
			}
		}
	}
	return(nullptr);
}
