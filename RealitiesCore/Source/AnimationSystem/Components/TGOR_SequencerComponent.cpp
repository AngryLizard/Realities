// The Gateway of Realities: Planes of Existence.
#include "TGOR_SequencerComponent.h"

#include "CoreSystem/TGOR_Singleton.h"
#include "AnimationSystem/Tasks/TGOR_SequencerPilotTask.h"
#include "DimensionSystem/Components/TGOR_PilotComponent.h"
#include "LevelSequencePlayer.h"

#include "RealitiesUGC/Mod/TGOR_ContentManager.h"
#include "DrawDebugHelpers.h"

UTGOR_SequencerComponent::UTGOR_SequencerComponent()
:	Super()
{
}

void UTGOR_SequencerComponent::BeginPlay()
{
	Super::BeginPlay();
	if (IsValid(SequenceActor))
	{
		SequenceActor->SequencePlayer->OnFinished.AddDynamic(this, &UTGOR_SequencerComponent::OnSequenceFinished);
	}
}

void UTGOR_SequencerComponent::PlaySequence()
{
	if (!SequenceActor->SequencePlayer->IsPlaying())
	{
		SequenceActor->SequencePlayer->Play();
	}
}

void UTGOR_SequencerComponent::DisengageSequencer()
{
	OnSequenceFinished();
}

bool UTGOR_SequencerComponent::ParentSequencerPose(UTGOR_PilotComponent* Attachee, FMovieSceneObjectBindingID Binding)
{
	USkeletalMeshComponent* Pose = GetMeshFromBinding(Binding);
	if (IsValid(Attachee) && IsValid(Pose))
	{
		TArray<UTGOR_SequencerPilotTask*> Tasks = Attachee->GetPListOfType<UTGOR_SequencerPilotTask>();
		for (UTGOR_SequencerPilotTask* Task : Tasks)
		{
			if (IsValid(Task) && Task->CanParent(this, Pose))
			{
				Task->Parent(this, Pose);
				Attachee->AttachWith(Task->Identifier.Slot);

				// Trigger movement events now
				Attachee->OnPositionChange(Attachee->ComputePosition());
				return true;
			}
		}
	}
	return false;
}

bool UTGOR_SequencerComponent::CanParentSequencerPose(UTGOR_PilotComponent* Attachee, FMovieSceneObjectBindingID Binding) const
{
	USkeletalMeshComponent* Pose = GetMeshFromBinding(Binding);
	if (IsValid(Attachee) && IsValid(SequenceActor) && IsValid(Pose))
	{
		TArray<UTGOR_SequencerPilotTask*> Tasks = Attachee->GetPListOfType<UTGOR_SequencerPilotTask>();
		for (UTGOR_SequencerPilotTask* Task : Tasks)
		{
			if (IsValid(Task) && Task->CanParent(this, Pose))
			{
				return true;
			}
		}
	}
	return false;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void UTGOR_SequencerComponent::RegisterAttachToSequencer(UTGOR_SequencerPilotTask* Attachee)
{
	SequencerAttachments.Add(Attachee);
}

void UTGOR_SequencerComponent::RegisterDetachFromSequencer(UTGOR_SequencerPilotTask* Attachee)
{
	SequencerAttachments.Remove(Attachee);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void UTGOR_SequencerComponent::OnSequenceFinished()
{
	// Reparent all children to us
	if (IsValid(SequenceActor))
	{
		TSet<TWeakObjectPtr<UTGOR_SequencerPilotTask>> Attachments = SequencerAttachments;
		for (TWeakObjectPtr<UTGOR_SequencerPilotTask> Attachment : Attachments)
		{
			UTGOR_PilotComponent* Pilot = Attachment->Identifier.Component;
			if (IsValid(Pilot))
			{
				// This can fail, will just stay attached
				ParentLinear(Pilot, INDEX_NONE, Attachment->ComputeSpace());
			}
		}
	}
}

USkeletalMeshComponent* UTGOR_SequencerComponent::GetMeshFromBinding(FMovieSceneObjectBindingID Binding) const
{
	if (!IsValid(SequenceActor))
	{
		return nullptr;
	}

	TArray<UObject*> BoundObjects = SequenceActor->SequencePlayer->GetBoundObjects(Binding);
	if (BoundObjects.Num() == 0)
	{
		return nullptr;
	}

	AActor* BoundActor = Cast<AActor>(BoundObjects[0]);
	if (!IsValid(BoundActor))
	{
		return nullptr;
	}

	return BoundActor->FindComponentByClass<USkeletalMeshComponent>();
}