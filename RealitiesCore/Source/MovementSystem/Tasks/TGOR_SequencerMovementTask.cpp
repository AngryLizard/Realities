// The Gateway of Realities: Planes of Existence.

#include "TGOR_SequencerMovementTask.h"
#include "DimensionSystem/Components/TGOR_PilotComponent.h"
#include "MovementSystem/Components/TGOR_MovementComponent.h"
#include "AnimationSystem/Tasks/TGOR_SequencerPilotTask.h"
#include "RealitiesUtility/Utility/TGOR_Math.h"

UTGOR_SequencerMovementTask::UTGOR_SequencerMovementTask()
	: Super(),
	RootComponent(nullptr),
	SequencerTask(nullptr)
{
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void UTGOR_SequencerMovementTask::Initialise()
{
	Super::Initialise();

	RootComponent = Identifier.Component->GetRootPilot();
	if (!RootComponent.IsValid())
	{
		ERROR("RootComponent invalid", Error);
	}

	SequencerTask.Reset();
}

bool UTGOR_SequencerMovementTask::Invariant(const FTGOR_MovementSpace& Space, const FTGOR_MovementExternal& External) const
{
	if (!Super::Invariant(Space, External))
	{
		return false;
	}

	UTGOR_SequencerPilotTask* Task = RootComponent->GetCurrentPOfType<UTGOR_SequencerPilotTask>();
	if (!RootComponent.IsValid() || !IsValid(Task))
	{
		return false;
	}

	return true;
}

void UTGOR_SequencerMovementTask::Update(const FTGOR_MovementSpace& Space, const FTGOR_MovementExternal& External, const FTGOR_MovementTick& Tick, FTGOR_MovementOutput& Output)
{
	Super::Update(Space, External, Tick, Output);
}

void UTGOR_SequencerMovementTask::PrepareStart()
{
	Super::PrepareStart();

	SequencerTask = RootComponent->GetCurrentPOfType<UTGOR_SequencerPilotTask>();
	if (!SequencerTask.IsValid())
	{
		ERROR("SequencerTask invalid", Fatal);
	}

	UTGOR_AnimationComponent* AnimationComponent = Identifier.Component->GetAnimationComponent();
	if (IsValid(AnimationComponent))
	{
		const FTransform TargetTransform = AnimationComponent->GetComponentTransform();
		const FTransform RootTransform = AnimationComponent->GetOwner()->GetActorTransform();

		const FTransform Local = RootTransform * TargetTransform.Inverse();
		SequencerTask->SetPoseOffset(FTGOR_MovementPosition(Local.GetLocation(), Local.GetRotation()));
	}
}

void UTGOR_SequencerMovementTask::Interrupt()
{
	SequencerTask.Reset();
	Super::Interrupt();
}

///////////////////////////////////////////////////////////////////////

USkeletalMeshComponent* UTGOR_SequencerMovementTask::GetPose() const
{
	if (SequencerTask.IsValid())
	{
		return SequencerTask->GetPose();
	}
	return nullptr;
}
