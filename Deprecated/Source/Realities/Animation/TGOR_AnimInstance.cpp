// Fill out your copyright notice in the Description page of Project Settings.


#include "TGOR_AnimInstance.h"
#include "Realities/Utility/Error/TGOR_Error.h"
#include "DrawDebugHelpers.h"
#include "Engine.h"

#include "Realities/Base/TGOR_Singleton.h"
#include "Realities/Base/Content/TGOR_ContentManager.h"
#include "Realities/Animation/TGOR_SubAnimInstance.h"
#include "Realities/Mod/Animations/TGOR_Animation.h"

#include "Realities/Base/TGOR_GameInstance.h"


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
	RPORT("++++++");
	const TArray<FAnimGroupInstance>& GroupRead = GetSyncGroupRead();
	for (const FAnimGroupInstance& Instance : GroupRead)
	{
		RPORT("-------");
		for (const FAnimTickRecord& Record : Instance.ActivePlayers)
		{
			VPORT(Record.SourceAsset);
		}
	}
}

bool FTGOR_AnimInstanceProxy::HasSyncGroupInstance(const FName& SyncGroup) const
{
	const TArray<FAnimGroupInstance>& GroupRead = GetSyncGroupRead();
	const int32 Index = GetSyncGroupIndexFromName(SyncGroup);
	return GroupRead.IsValidIndex(Index);
}

const FAnimGroupInstance& FTGOR_AnimInstanceProxy::GetSyncGroupInstance(const FName& SyncGroup) const
{
	const TArray<FAnimGroupInstance>& GroupRead = GetSyncGroupRead();
	const int32 Index = GetSyncGroupIndexFromName(SyncGroup);
	return GroupRead[Index];
}


UTGOR_AnimInstance::UTGOR_AnimInstance()
	: Super()
{
}

UTGOR_AnimInstance::UTGOR_AnimInstance(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UTGOR_AnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	/*
	Is done on assign and not every tick now
	for (const auto& Pair : AnimationInstanceQueues)
	{
		const FName AnimationKey = GetSubAnimName(Pair.Key);
		UTGOR_SubAnimInstance* Instance = Cast<UTGOR_SubAnimInstance>(GetLinkedAnimGraphInstanceByTag(AnimationKey));
		if (IsValid(Instance))
		{
			Instance->ParentSlot = Pair.Key;
			Instance->ParentInstance = this;
			Instance->Animation = Pair.Value.Animation;
		}
	}
	*/
}

bool UTGOR_AnimInstance::HasQueue(FName AnimationContentQueue) const
{
	return AnimationInstanceQueues.Contains(AnimationContentQueue);
}

UTGOR_Animation* UTGOR_AnimInstance::GetQueue(FName AnimationContentQueue) const
{
	if (const FTGOR_SubAnimationInstance* Ptr = AnimationInstanceQueues.Find(AnimationContentQueue))
	{
		return IsValid(Ptr->Current) ? Ptr->Current->Animation : nullptr;
	}
	return nullptr;
}

FTGOR_SubAnimBlend UTGOR_AnimInstance::GetBlendInfo(FName AnimationContentQueue) const
{
	FTGOR_SubAnimBlend Blend;
	if (const FTGOR_SubAnimationInstance* Ptr = AnimationInstanceQueues.Find(AnimationContentQueue))
	{
		Blend.IsSwitched = Ptr->IsSwitched;

		float CurrBlend = 0.0f;
		if (IsValid(Ptr->Current) && IsValid(Ptr->Current->Animation))
		{
			CurrBlend = Ptr->Current->Animation->BlendTime;
		}

		float PrevBlend = 0.0f;
		if (IsValid(Ptr->Previous) && IsValid(Ptr->Previous->Animation))
		{
			PrevBlend = Ptr->Previous->Animation->BlendTime;
		}

		Blend.OnBlend = Blend.IsSwitched ? PrevBlend : CurrBlend;
		Blend.OffBlend = Blend.IsSwitched ? CurrBlend : PrevBlend;
	}
	return Blend;
}


UTGOR_SubAnimInstance* UTGOR_AnimInstance::GetSubAnimInstance(FName AnimationContentQueue) const
{
	const FTGOR_SubAnimationInstance* Ptr = AnimationInstanceQueues.Find(AnimationContentQueue);
	if (Ptr)
	{
		return Ptr->Current;
	}
	return nullptr;
}

FName UTGOR_AnimInstance::GetSubAnimName(FName AnimationContentQueue) const
{
	const FTGOR_SubAnimationInstance* Ptr = AnimationInstanceQueues.Find(AnimationContentQueue);
	if (Ptr && Ptr->IsSwitched)
	{
		return FName(*(AnimationContentQueue.ToString() + "_On"));
	}
	return FName(*(AnimationContentQueue.ToString() + "_Off"));
}

void UTGOR_AnimInstance::AssignAnimationInstance(FName AnimationContentQueue, UTGOR_Animation* Animation)
{
	if (IsValid(Animation))
	{
		// Get or add instance to queue
		FTGOR_SubAnimationInstance& Instance = AnimationInstanceQueues.FindOrAdd(AnimationContentQueue);
		Instance.IsSwitched = !Instance.IsSwitched;
		Instance.Previous = Instance.Current;

		const FName AnimationKey = GetSubAnimName(AnimationContentQueue);
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

		// Set anim instance to graph and queue
		LinkAnimGraphByTag(AnimationKey, Animation->InstanceClass);
		Instance.Current = Cast<UTGOR_SubAnimInstance>(GetLinkedAnimGraphInstanceByTag(AnimationKey));
		Instance.Current->Animation = Animation;
	}
	else if (FTGOR_SubAnimationInstance* Ptr = AnimationInstanceQueues.Find(AnimationContentQueue))
	{
		// Set default value instead
		Ptr->IsSwitched = !Ptr->IsSwitched;
		Ptr->Previous = Ptr->Current;
		Ptr->Current = nullptr;

		const FName AnimationKey = GetSubAnimName(AnimationContentQueue);
		LinkAnimGraphByTag(AnimationKey, Ptr->IsSwitched ? Ptr->DefaultOn : Ptr->DefaultOff);
		Ptr->Current = Cast<UTGOR_SubAnimInstance>(GetLinkedAnimGraphInstanceByTag(AnimationKey));
		Ptr->Current->Animation = nullptr;
	}
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