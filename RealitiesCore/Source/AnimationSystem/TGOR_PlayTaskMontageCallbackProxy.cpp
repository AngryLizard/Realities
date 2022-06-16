// Copyright Epic Games, Inc. All Rights Reserved.
// Code copied from UPlayMontageCallbackProxy

#include "TGOR_PlayTaskMontageCallbackProxy.h"
#include "AnimationSystem/Tasks/TGOR_AnimatedTask.h"

//////////////////////////////////////////////////////////////////////////
// UTGOR_PlayTaskMontageCallbackProxy

UTGOR_PlayTaskMontageCallbackProxy::UTGOR_PlayTaskMontageCallbackProxy(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, MontageInstanceID(INDEX_NONE)
	, bInterruptedCalledBeforeBlendingOut(false)
{
}

UTGOR_PlayTaskMontageCallbackProxy* UTGOR_PlayTaskMontageCallbackProxy::CreateProxyObjectForPlayMontage(
	class UTGOR_AnimatedTask* InTask,
	class UAnimMontage* MontageToPlay,
	float PlayRate,
	float StartingPosition,
	FName StartingSection)
{
	UTGOR_PlayTaskMontageCallbackProxy* Proxy = NewObject<UTGOR_PlayTaskMontageCallbackProxy>();
	Proxy->SetFlags(RF_StrongRefOnFrame);
	Proxy->PlayMontage(InTask, MontageToPlay, PlayRate, StartingPosition, StartingSection);
	return Proxy;
}


void UTGOR_PlayTaskMontageCallbackProxy::PlayMontage(
	class UTGOR_AnimatedTask* InTask,
	class UAnimMontage* MontageToPlay,
	float PlayRate,
	float StartingPosition,
	FName StartingSection)
{
	bool bPlayedSuccessfully = false;
	if (InTask)
	{
		if (UAnimInstance* AnimInstance = InTask->GetAnimationInstance())
		{
			const float MontageLength = AnimInstance->Montage_Play(MontageToPlay, PlayRate, EMontagePlayReturnType::MontageLength, StartingPosition);
			bPlayedSuccessfully = (MontageLength > 0.f);

			if (bPlayedSuccessfully)
			{
				AnimInstancePtr = AnimInstance;
				if (FAnimMontageInstance* MontageInstance = AnimInstance->GetActiveInstanceForMontage(MontageToPlay))
				{
					MontageInstanceID = MontageInstance->GetInstanceID();
				}

				if (StartingSection != NAME_None)
				{
					AnimInstance->Montage_JumpToSection(StartingSection, MontageToPlay);
				}

				BlendingOutDelegate.BindUObject(this, &UTGOR_PlayTaskMontageCallbackProxy::OnMontageBlendingOut);
				AnimInstance->Montage_SetBlendingOutDelegate(BlendingOutDelegate, MontageToPlay);

				MontageEndedDelegate.BindUObject(this, &UTGOR_PlayTaskMontageCallbackProxy::OnMontageEnded);
				AnimInstance->Montage_SetEndDelegate(MontageEndedDelegate, MontageToPlay);

				AnimInstance->OnPlayMontageNotifyBegin.AddDynamic(this, &UTGOR_PlayTaskMontageCallbackProxy::OnNotifyBeginReceived);
				AnimInstance->OnPlayMontageNotifyEnd.AddDynamic(this, &UTGOR_PlayTaskMontageCallbackProxy::OnNotifyEndReceived);
			}
		}
	}

	if (!bPlayedSuccessfully)
	{
		OnInterrupted.Broadcast(NAME_None);
	}
}

bool UTGOR_PlayTaskMontageCallbackProxy::IsNotifyValid(FName NotifyName, const FBranchingPointNotifyPayload& BranchingPointNotifyPayload) const
{
	return ((MontageInstanceID != INDEX_NONE) && (BranchingPointNotifyPayload.MontageInstanceID == MontageInstanceID));
}


void UTGOR_PlayTaskMontageCallbackProxy::OnNotifyBeginReceived(FName NotifyName, const FBranchingPointNotifyPayload& BranchingPointNotifyPayload)
{
	if (IsNotifyValid(NotifyName, BranchingPointNotifyPayload))
	{
		OnNotifyBegin.Broadcast(NotifyName);
	}
}


void UTGOR_PlayTaskMontageCallbackProxy::OnNotifyEndReceived(FName NotifyName, const FBranchingPointNotifyPayload& BranchingPointNotifyPayload)
{
	if (IsNotifyValid(NotifyName, BranchingPointNotifyPayload))
	{
		OnNotifyEnd.Broadcast(NotifyName);
	}
}


void UTGOR_PlayTaskMontageCallbackProxy::OnMontageBlendingOut(UAnimMontage* Montage, bool bInterrupted)
{
	if (bInterrupted)
	{
		OnInterrupted.Broadcast(NAME_None);
		bInterruptedCalledBeforeBlendingOut = true;
	}
	else
	{
		OnBlendOut.Broadcast(NAME_None);
	}
}

void UTGOR_PlayTaskMontageCallbackProxy::OnMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	if (!bInterrupted)
	{
		OnCompleted.Broadcast(NAME_None);
	}
	else if (!bInterruptedCalledBeforeBlendingOut)
	{
		OnInterrupted.Broadcast(NAME_None);
	}

	UnbindDelegates();
}

void UTGOR_PlayTaskMontageCallbackProxy::UnbindDelegates()
{
	if (UAnimInstance* AnimInstance = AnimInstancePtr.Get())
	{
		AnimInstance->OnPlayMontageNotifyBegin.RemoveDynamic(this, &UTGOR_PlayTaskMontageCallbackProxy::OnNotifyBeginReceived);
		AnimInstance->OnPlayMontageNotifyEnd.RemoveDynamic(this, &UTGOR_PlayTaskMontageCallbackProxy::OnNotifyEndReceived);
	}
}

void UTGOR_PlayTaskMontageCallbackProxy::BeginDestroy()
{
	UnbindDelegates();

	Super::BeginDestroy();
}