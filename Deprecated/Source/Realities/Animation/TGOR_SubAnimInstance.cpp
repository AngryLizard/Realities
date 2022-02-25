// Fill out your copyright notice in the Description page of Project Settings.


#include "TGOR_SubAnimInstance.h"
#include "Realities/Utility/Error/TGOR_Error.h"
#include "DrawDebugHelpers.h"
#include "Engine.h"


FTGOR_SubAnimInstanceProxy::FTGOR_SubAnimInstanceProxy()
: Super()
{
}

FTGOR_SubAnimInstanceProxy::FTGOR_SubAnimInstanceProxy(UAnimInstance* Instance)
	: Super(Instance)
{
}

void FTGOR_SubAnimInstanceProxy::MergeSyncGroupsFrom(const FTGOR_AnimInstanceProxy& Parent, const TArray<FName>& SyncGroups)
{
	for (const FName& SyncGroup : SyncGroups)
	{
		if (Parent.HasSyncGroupInstance(SyncGroup))
		{
			const FAnimGroupInstance& TheirInstance = Parent.GetSyncGroupInstance(SyncGroup);
			if (TheirInstance.ActivePlayers.IsValidIndex(TheirInstance.GroupLeaderIndex))
			{
				const FAnimTickRecord& Leader = TheirInstance.ActivePlayers[TheirInstance.GroupLeaderIndex];

				const TArray<FAnimGroupInstance>& GroupRead = GetSyncGroupRead();
				const int32 Index = GetSyncGroupIndexFromName(SyncGroup);
				if (GroupRead.IsValidIndex(Index))
				{
					const FAnimGroupInstance& OwnInstance = GroupRead[Index];
					for (const FAnimTickRecord& Record : OwnInstance.ActivePlayers)
					{
						if (Record.TimeAccumulator && Leader.TimeAccumulator)
						{
							(*Record.TimeAccumulator) = (*Leader.TimeAccumulator);
						}
					}
				}
			}
		}
	}
}

void FTGOR_SubAnimInstanceProxy::PreUpdate(UAnimInstance* InAnimInstance, float DeltaSeconds)
{
	Super::PreUpdate(InAnimInstance, DeltaSeconds);

	// Update sync groups from parent instance
	UTGOR_SubAnimInstance* Instance = Cast<UTGOR_SubAnimInstance>(InAnimInstance);
	if (IsValid(Instance) && IsValid(Instance->ParentInstance))
	{
		const FTGOR_AnimInstanceProxy& ParentProxy = Instance->ParentInstance->GetInstanceProxy();
		MergeSyncGroupsFrom(ParentProxy, Instance->SyncGroupInheritance);
	}
}

void FTGOR_SubAnimInstanceProxy::UpdateSubAnimation()
{
	UpdateAnimation();
}

UTGOR_SubAnimInstance::UTGOR_SubAnimInstance()
:	Super(),
	Animation(nullptr),
	ParentInstance(nullptr),
	ParentSlot("")
{
}

UTGOR_SubAnimInstance::UTGOR_SubAnimInstance(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer),
	Animation(nullptr),
	ParentInstance(nullptr),
	ParentSlot("")
{
}


UTGOR_Animation* UTGOR_SubAnimInstance::GetAnimationLink() const
{
	if (IsValid(ParentInstance))
	{
		return(ParentInstance->GetQueue(ParentSlot));
	}
	return(Animation);
}
