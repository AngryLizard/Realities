// The Gateway of Realities: Planes of Existence.

#include "TGOR_MontageMovementTask.h"
#include "DimensionSystem/Components/TGOR_PilotComponent.h"
#include "MovementSystem/Components/TGOR_MovementComponent.h"
#include "DimensionSystem/Tasks/TGOR_AttachedPilotTask.h"
#include "RealitiesUtility/Utility/TGOR_Math.h"

#include "MotionWarpingComponent.h"
#include "AnimationSystem/Notifies/TGOR_MotionWarpingNotifyState.h"
#include "DrawDebugHelpers.h"

UTGOR_MontageMovementTask::UTGOR_MontageMovementTask()
	: Super(),
	RootComponent(nullptr),
	AttachedTask(nullptr)
{
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void FTGOR_RootMotionModifier::Update(UTGOR_MontageMovementTask* Task)
{
	const FAnimMontageInstance* RootMotionMontageInstance = Task->GetMontageInstance();
	const UAnimMontage* Montage = RootMotionMontageInstance ? ToRawPtr(RootMotionMontageInstance->Montage) : nullptr;

	// Mark for removal if our animation is not relevant anymore
	if (Montage == nullptr || Montage != Animation)
	{
		State = ETGOR_RootMotionModifierState::MarkedForRemoval;
		return;
	}

	// Update playback times and weight
	PreviousPosition = RootMotionMontageInstance->GetPreviousPosition();
	CurrentPosition = RootMotionMontageInstance->GetPosition();
	Weight = RootMotionMontageInstance->GetWeight();

	// Mark for removal if the animation already passed the warping window
	if (PreviousPosition >= EndTime)
	{
		State = ETGOR_RootMotionModifierState::MarkedForRemoval;
		return;
	}

	// Check if we are inside the warping window
	if (PreviousPosition >= StartTime && PreviousPosition < EndTime)
	{
		// If we were waiting, switch to active
		if (State == ETGOR_RootMotionModifierState::Waiting)
		{
			State = ETGOR_RootMotionModifierState::Active;
		}
	}

	// Cache sync point transform and trigger OnSyncPointChanged if needed
	if (State == ETGOR_RootMotionModifierState::Active)
	{
		const FTGOR_MovementPosition* SyncPointPtr = Task->FindSyncPoint(SyncPointName);

		// Disable if there is no sync point for us
		if (SyncPointPtr == nullptr)
		{
			State = ETGOR_RootMotionModifierState::Disabled;
			return;
		}

		if (!SyncPointPtr->Linear.Equals(CachedSyncPoint.Linear) || !SyncPointPtr->Angular.Equals(CachedSyncPoint.Angular))
		{
			CachedSyncPoint = *SyncPointPtr;
		}
	}
}

FTransform FTGOR_RootMotionModifier::ProcessRootMotion(UTGOR_MontageMovementTask* Task, UTGOR_AnimationComponent* Component, const FTransform& InRootMotion, float DeltaSeconds) const
{
	FTGOR_MovementPosition Position;
	Position.Linear = Component->GetComponentLocation();
	Position.Angular = Component->GetComponentQuat();

	FTransform FinalRootMotion = InRootMotion;
	const FTransform RootMotionTotal = UMotionWarpingUtilities::ExtractRootMotionFromAnimation(Animation.Get(), PreviousPosition, EndTime);

	FVector DeltaTranslation = InRootMotion.GetTranslation();
	const FTransform RootMotionDelta = UMotionWarpingUtilities::ExtractRootMotionFromAnimation(Animation.Get(), PreviousPosition, FMath::Min(CurrentPosition, EndTime));

	const float Delta = RootMotionDelta.GetTranslation().Size();
	const float Target = FVector::Dist(Position.Linear, CachedSyncPoint.Linear);
	const float Original = RootMotionTotal.GetTranslation().Size();
	const float TranslationWarped = Original != 0.f ? ((Delta * Target) / Original) : 0.f;

	DeltaTranslation = (CachedSyncPoint.Linear - Position.Linear).GetSafeNormal() * TranslationWarped;

	/*
	const FQuat DeltaRotation = Position.Angular * Task->RootComponent->ComputePosition().Angular.Inverse();
	DeltaRotation = DeltaRotation * DeltaTranslation;
	*/

	FinalRootMotion.SetTranslation(DeltaTranslation);

	const FQuat WarpedRotation = WarpRotation(Position, InRootMotion, RootMotionTotal, DeltaSeconds);
	FinalRootMotion.SetRotation(WarpedRotation);

	return FinalRootMotion;
}

FQuat FTGOR_RootMotionModifier::WarpRotation(const FTGOR_MovementPosition& Position, const FTransform& RootMotionDelta, const FTransform& RootMotionTotal, float DeltaSeconds) const
{
	/*
	const FQuat CurrentRotation = Position.Angular;
	const FQuat RemainingRootRotationInWorld = RootMotionTotal.GetRotation();
	const FQuat CurrentPlusRemainingRootMotion = RemainingRootRotationInWorld * CurrentRotation;
	const FQuat TargetRotation = CachedSyncPoint.Angular;

	return CurrentRotation.Inverse() * TargetRotation;
	*/


	const FQuat CurrentRotation = Position.Angular;
	const FQuat TargetRotation = CachedSyncPoint.Angular;
	const float TimeRemaining = (EndTime - PreviousPosition); //WarpRotationTimeMultiplier
	const FQuat RemainingRootRotationInWorld = RootMotionTotal.GetRotation();
	const FQuat CurrentPlusRemainingRootMotion = RemainingRootRotationInWorld * CurrentRotation;
	const float PercentThisStep = FMath::Clamp(DeltaSeconds / TimeRemaining, 0.f, 1.f);
	const FQuat TargetRotThisFrame = FQuat::Slerp(CurrentPlusRemainingRootMotion, TargetRotation, PercentThisStep);
	const FQuat DeltaOut = TargetRotThisFrame * CurrentPlusRemainingRootMotion.Inverse();

	return (DeltaOut * RootMotionDelta.GetRotation());
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void UTGOR_MontageMovementTask::Initialise()
{
	Super::Initialise();

	RootComponent = Identifier.Component->GetRootPilot();
	if (!RootComponent.IsValid())
	{
		ERROR("RootComponent invalid", Error);
	}

	AttachedTask.Reset();
}

bool UTGOR_MontageMovementTask::Invariant(const FTGOR_MovementSpace& Space, const FTGOR_MovementExternal& External) const
{
	if (!Super::Invariant(Space, External))
	{
		return false;
	}

	UTGOR_AttachedPilotTask* Task = RootComponent->GetCurrentPOfType<UTGOR_AttachedPilotTask>();
	if (!RootComponent.IsValid() || !IsValid(Task))
	{
		return false;
	}

	return true;
}

void UTGOR_MontageMovementTask::Update(const FTGOR_MovementSpace& Space, const FTGOR_MovementExternal& External, const FTGOR_MovementTick& Tick, FTGOR_MovementOutput& Output)
{
	FTGOR_MovementSpace Out = Space;

	const FTGOR_MovementPosition Offset = TickAnimationRootMotion(Out, Tick.DeltaTime);

	/*
	// TODO: Motionwarping is very iffy right now, just hard-set it to the syncpoints
	for (const TSharedPtr<FTGOR_RootMotionModifier>& Modifier : RootMotionModifiers)
	{
		const FTGOR_MovementPosition* SyncPointPtr = FindSyncPoint(Modifier->SyncPointName);

		// Disable if there is no sync point for us
		TScriptInterface<ITGOR_AnimationInterface> Owner = GetAnimationOwner();
		if (Owner && SyncPointPtr)
		{
			UTGOR_AnimationComponent* Component = Owner->GetAnimationComponent();
			if (Component)
			{
				const float StepTime = FMath::Clamp((Modifier->CurrentPosition - Modifier->PreviousPosition) / (Modifier->EndTime - Modifier->PreviousPosition), 0.0f, 1.0f) * Modifier->Weight;

				const FVector DeltaLocation = RootComponent->GetComponentLocation() - Component->GetComponentLocation();
				const FQuat DeltaRotation = RootComponent->GetComponentQuat().Inverse() * Component->GetComponentQuat();

				if (Modifier->State == ETGOR_RootMotionModifierState::Active)
				{
					VPORT(StepTime);
					//Out.Linear = DeltaLocation * StepTime + SyncPointPtr->Linear;
					//Out.Angular = FQuat::Slerp(FQuat::Identity, DeltaRotation, StepTime) * SyncPointPtr->Angular;
				}
				else if (Modifier->State == ETGOR_RootMotionModifierState::MarkedForRemoval)
				{
					Out.Linear = SyncPointPtr->Linear + DeltaLocation;
					Out.Angular = DeltaRotation * SyncPointPtr->Angular;
				}
			}
		}
	}
	*/

	// Simulate move
	FTGOR_MovementImpact MovementImpact;
	RootComponent->SimulateMove(Out, Offset, Tick.DeltaTime, false, MovementImpact);
	AttachedTask->SimulateDynamic(Out);

	Super::Update(Space, External, Tick, Output);
}

void UTGOR_MontageMovementTask::PrepareStart()
{
	Super::PrepareStart();

	AttachedTask = RootComponent->GetCurrentPOfType<UTGOR_AttachedPilotTask>();
	if (!AttachedTask.IsValid())
	{
		ERROR("AttachedTask invalid", Fatal);
	}
}

void UTGOR_MontageMovementTask::Interrupt()
{
	AttachedTask.Reset();
	Super::Interrupt();
}

///////////////////////////////////////////////////////////////////////

bool UTGOR_MontageMovementTask::ContainsModifier(const UAnimSequenceBase* Animation, float StartTime, float EndTime) const
{
	return RootMotionModifiers.ContainsByPredicate([=](const TSharedPtr<FTGOR_RootMotionModifier>& Modifier)
	{
		return (Modifier.IsValid() && Modifier->Animation == Animation && Modifier->StartTime == StartTime && Modifier->EndTime == EndTime);
	});
}

void UTGOR_MontageMovementTask::AddRootMotionModifier(TSharedPtr<FTGOR_RootMotionModifier> Modifier)
{
	if (ensureAlways(Modifier.IsValid()))
	{
		RootMotionModifiers.Add(Modifier);
	}
}

void UTGOR_MontageMovementTask::DisableAllRootMotionModifiers()
{
	if (RootMotionModifiers.Num() > 0)
	{
		for (TSharedPtr<FTGOR_RootMotionModifier>& Modifier : RootMotionModifiers)
		{
			Modifier->State = ETGOR_RootMotionModifierState::Disabled;
		}
	}
}

void UTGOR_MontageMovementTask::Update()
{
	const FAnimMontageInstance* RootMotionMontageInstance = GetMontageInstance();
	UAnimMontage* Montage = RootMotionMontageInstance ? ToRawPtr(RootMotionMontageInstance->Montage) : nullptr;
	if (Montage)
	{
		const float PreviousPosition = RootMotionMontageInstance->GetPreviousPosition();
		const float CurrentPosition = RootMotionMontageInstance->GetPosition();

		// Loop over notifies directly in the montage, looking for Motion Warping windows
		for (const FAnimNotifyEvent& NotifyEvent : Montage->Notifies)
		{
			const UTGOR_MotionWarpingNotifyState* MotionWarpingNotify = NotifyEvent.NotifyStateClass ? Cast<UTGOR_MotionWarpingNotifyState>(NotifyEvent.NotifyStateClass) : nullptr;
			if (MotionWarpingNotify)
			{
				const float StartTime = FMath::Clamp(NotifyEvent.GetTriggerTime(), 0.f, Montage->GetPlayLength());
				const float EndTime = FMath::Clamp(NotifyEvent.GetEndTriggerTime(), 0.f, Montage->GetPlayLength());

				if (PreviousPosition >= StartTime && PreviousPosition < EndTime)
				{
					if (!ContainsModifier(Montage, StartTime, EndTime))
					{
						TSharedPtr<FTGOR_RootMotionModifier> Modifier = MakeShared<FTGOR_RootMotionModifier>();
						Modifier->Animation = Montage;
						Modifier->StartTime = StartTime;
						Modifier->EndTime = EndTime;
						Modifier->SyncPointName = MotionWarpingNotify->SyncPointName;
						RootMotionModifiers.Add(Modifier);
					}
				}
			}
		}

		// Same as before but scanning all animation within the montage
		for (int32 SlotIdx = 0; SlotIdx < Montage->SlotAnimTracks.Num(); SlotIdx++)
		{
			const FAnimTrack& AnimTrack = Montage->SlotAnimTracks[SlotIdx].AnimTrack;
			const FAnimSegment* AnimSegment = AnimTrack.GetSegmentAtTime(PreviousPosition);
			if (AnimSegment && AnimSegment->AnimReference)
			{
				for (const FAnimNotifyEvent& NotifyEvent : AnimSegment->AnimReference->Notifies)
				{
					const UTGOR_MotionWarpingNotifyState* MotionWarpingNotify = NotifyEvent.NotifyStateClass ? Cast<UTGOR_MotionWarpingNotifyState>(NotifyEvent.NotifyStateClass) : nullptr;
					if (MotionWarpingNotify)
					{
						const float NotifyStartTime = FMath::Clamp(NotifyEvent.GetTriggerTime(), 0.f, AnimSegment->AnimReference->GetPlayLength());
						const float NotifyEndTime = FMath::Clamp(NotifyEvent.GetEndTriggerTime(), 0.f, AnimSegment->AnimReference->GetPlayLength());

						// Convert notify times from AnimSequence times to montage times
						const float StartTime = (NotifyStartTime - AnimSegment->AnimStartTime) + AnimSegment->StartPos;
						const float EndTime = (NotifyEndTime - AnimSegment->AnimStartTime) + AnimSegment->StartPos;

						if (PreviousPosition >= StartTime && PreviousPosition < EndTime)
						{
							if (!ContainsModifier(Montage, StartTime, EndTime))
							{
								TSharedPtr<FTGOR_RootMotionModifier> Modifier = MakeShared<FTGOR_RootMotionModifier>();
								Modifier->Animation = Montage;
								Modifier->StartTime = StartTime;
								Modifier->EndTime = EndTime;
								Modifier->SyncPointName = MotionWarpingNotify->SyncPointName;
								RootMotionModifiers.Add(Modifier);
							}
						}
					}
				}
			}
		}
	}

	// Update the state of all the modifiers
	if (RootMotionModifiers.Num() > 0)
	{
		for (TSharedPtr<FTGOR_RootMotionModifier>& Modifier : RootMotionModifiers)
		{
			Modifier->Update(this);
		}
	}

	// Remove the modifiers that have been marked for removal
	RootMotionModifiers.RemoveAll([this](const TSharedPtr<FTGOR_RootMotionModifier>& Modifier)
	{
		if (Modifier->State == ETGOR_RootMotionModifierState::MarkedForRemoval)
		{
			return true;
		}

		return false;
	});
}

FTransform UTGOR_MontageMovementTask::ProcessRootMotionPreConvertToWorld(const FTransform& InRootMotion, UTGOR_AnimationComponent* Component, float DeltaSeconds)
{
	// Check for warping windows and update modifier states
	Update();
	return InRootMotion;
}

FTransform UTGOR_MontageMovementTask::ProcessRootMotionPostConvertToWorld(const FTransform& InRootMotion, UTGOR_AnimationComponent* Component, float DeltaSeconds)
{
	FTransform FinalRootMotion = InRootMotion;

	// Apply World Space Modifiers
	for (TSharedPtr<FTGOR_RootMotionModifier>& RootMotionModifier : RootMotionModifiers)
	{
		if (RootMotionModifier->State == ETGOR_RootMotionModifierState::Active)
		{
			FinalRootMotion = RootMotionModifier->ProcessRootMotion(this, Component, FinalRootMotion, DeltaSeconds);
		}
	}
	return FinalRootMotion;
}

void UTGOR_MontageMovementTask::AddOrUpdateSyncPoint(FName Name, const FTGOR_MovementPosition& SyncPoint)
{
	if (Name != NAME_None)
	{
		FTGOR_MovementPosition& MotionWarpingSyncPoint = SyncPoints.FindOrAdd(Name);
		MotionWarpingSyncPoint = SyncPoint;
	}
}

int32 UTGOR_MontageMovementTask::RemoveSyncPoint(FName Name)
{
	return SyncPoints.Remove(Name);
}

const FAnimMontageInstance* UTGOR_MontageMovementTask::GetMontageInstance() const
{
	TScriptInterface<ITGOR_AnimationInterface> AnimationOwner = GetAnimationOwner();
	check(AnimationOwner);

	UTGOR_AnimationComponent* AnimationComponent = AnimationOwner->GetAnimationComponent();
	check(AnimationComponent);

	UAnimInstance* AnimationInstance = AnimationComponent->GetAnimInstance();
	check(AnimationInstance);

	return AnimationInstance->GetRootMotionMontageInstance();
}

FTransform UTGOR_MontageMovementTask::ConvertLocalRootMotionToWorld(const FTransform& LocalRootMotionTransform, UTGOR_AnimationComponent* Component, float DeltaSeconds)
{
	const FTransform PreProcessedRootMotion = ProcessRootMotionPreConvertToWorld(LocalRootMotionTransform, Component, DeltaSeconds);
	const FTransform WorldSpaceRootMotion = Super::ConvertLocalRootMotionToWorld(PreProcessedRootMotion, Component, DeltaSeconds);
	return ProcessRootMotionPostConvertToWorld(WorldSpaceRootMotion, Component, DeltaSeconds);
}
