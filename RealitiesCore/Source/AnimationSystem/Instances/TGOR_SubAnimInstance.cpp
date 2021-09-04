// Fill out your copyright notice in the Description page of Project Settings.


#include "TGOR_SubAnimInstance.h"
#include "RealitiesUtility/Utility/TGOR_Error.h"
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

				const FSyncGroupMap& GroupRead = GetSyncGroupMapRead();
				const FAnimGroupInstance* Ptr = GroupRead.Find(SyncGroup);
				if (Ptr)
				{
					for (const FAnimTickRecord& Record : Ptr->ActivePlayers)
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
	ParentSlot(nullptr)
{
}

UTGOR_SubAnimInstance::UTGOR_SubAnimInstance(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer),
	Animation(nullptr),
	ParentInstance(nullptr),
	ParentSlot(nullptr)
{
}
