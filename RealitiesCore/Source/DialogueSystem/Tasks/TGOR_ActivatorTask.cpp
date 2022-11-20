// The Gateway of Realities: Planes of Existence.

#include "TGOR_ActivatorTask.h"
#include "DialogueSystem/Components/TGOR_ActivatorComponent.h"
#include "AttributeSystem/Components/TGOR_AttributeComponent.h"
#include "DialogueSystem/Content/TGOR_Participant.h"
#include "DialogueSystem/Content/TGOR_Activator.h"
#include "AttributeSystem/Content/TGOR_Attribute.h"

UTGOR_ActivatorTask::UTGOR_ActivatorTask()
: Super()
{
}

void UTGOR_ActivatorTask::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(UTGOR_ActivatorTask, Identifier, COND_InitialOnly);
}

TScriptInterface<ITGOR_AnimationInterface> UTGOR_ActivatorTask::GetAnimationOwner() const
{
	return GetOwnerComponent();
}

////////////////////////////////////////////////////////////////////////////////////////////////////

UTGOR_Activator* UTGOR_ActivatorTask::GetActivator() const
{
	return Identifier.Content;
}

UTGOR_ActivatorComponent* UTGOR_ActivatorTask::GetOwnerComponent() const
{
	return Identifier.Component;
}

int32 UTGOR_ActivatorTask::GetSlotIndex() const
{
	return Identifier.Slot;
}

float UTGOR_ActivatorTask::GetProgress() const
{
	return CurrentProgress;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

bool UTGOR_ActivatorTask::Initialise()
{
	Identifier.Content->TaskInitialise(this);

	if (!InitialiseParticipants(Identifier.Component, Identifier.Content))
	{
		return false;
	}

	// Gather all participant values
	DomainValues.Reset();
	for (UTGOR_Participant* Participant : Identifier.Content->Instanced_ParticipantInsertions.Collection)
	{
		DomainValues.Emplace(Participant, FVector::ZeroVector);
	}

	return Super::Initialise();
}

void UTGOR_ActivatorTask::Prepare(FTGOR_SpectacleState& DialogueState)
{
	Super::Prepare(DialogueState);
	OnPrepare();
}

bool UTGOR_ActivatorTask::Update(FTGOR_SpectacleState& DialogueState, float DeltaTime)
{
	Super::Update(DialogueState, DeltaTime);
	ETGOR_SpectacleStateEnumeration State = ETGOR_SpectacleStateEnumeration::Continue;
	float Progress = 0.0f;
	OnUpdate(DeltaTime, State, Progress);
	if (Progress >= 1.0f && CurrentProgress < 1.0f)
	{
		OnCompleted();
	}
	CurrentProgress = Progress;
	return State == ETGOR_SpectacleStateEnumeration::Continue;
}

void UTGOR_ActivatorTask::Interrupt(FTGOR_SpectacleState& DialogueState)
{
	Super::Interrupt(DialogueState);
	OnInterrupt();
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void UTGOR_ActivatorTask::SetDomain(UTGOR_Participant* Participant, const FVector& Local)
{
	if (FVector* Ptr = DomainValues.Find(Participant))
	{
		(*Ptr) = Local;
	}
}

FVector UTGOR_ActivatorTask::GetDomain(TSubclassOf<UTGOR_Participant> Participant) const
{
	for (const auto& Pair : DomainValues)
	{
		if (Pair.Key && Pair.Key->IsA(Participant))
		{
			return Pair.Value;
		}
	}
	return FVector::ZeroVector;
}