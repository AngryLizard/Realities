// The Gateway of Realities: Planes of Existence.


#include "TGOR_SequencerPilotTask.h"
#include "DimensionSystem/Components/TGOR_PilotComponent.h"

#include "Net/UnrealNetwork.h"
#include "Engine/ActorChannel.h"

UTGOR_SequencerPilotTask::UTGOR_SequencerPilotTask()
{
}

void UTGOR_SequencerPilotTask::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}

UTGOR_MobilityComponent* UTGOR_SequencerPilotTask::GetParent() const
{
	return Parenting.Component.Get();
}


void UTGOR_SequencerPilotTask::Unparent()
{
	Super::Unparent();

	Parent(nullptr, nullptr);
}

void UTGOR_SequencerPilotTask::Register()
{
	Super::Register();
	if (Parenting.Component.IsValid())
	{
		Parenting.Component->RegisterAttachToSequencer(this);
	}
}

void UTGOR_SequencerPilotTask::Unregister()
{
	Super::Unregister();
	if (Parenting.Component.IsValid())
	{
		Parenting.Component->RegisterDetachFromSequencer(this);
	}
}

FTGOR_MovementPosition UTGOR_SequencerPilotTask::ComputePosition() const
{
	FTGOR_MovementPosition Position;
	if (Parenting.Pose.IsValid())
	{
		FTransform Transform = Parenting.Pose->GetComponentTransform();
		Position.Linear = Transform.TransformPosition(LocalOffset.Linear);
		Position.Angular = Transform.TransformRotation(LocalOffset.Angular);
	}
	else
	{
		Position.Linear = Identifier.Component->GetComponentLocation();
		Position.Angular = Identifier.Component->GetComponentQuat();
	}
	return Position;
}

FTGOR_MovementSpace UTGOR_SequencerPilotTask::ComputeSpace() const
{
	// To retain proper relative dynamics we compute relative dynamics from what values we can get from Sequencer
	// TODO: This could be way more optimized. we do a bunch of computations twice
	FTGOR_MovementSpace Space;
	if (Parenting.Pose.IsValid())
	{
		FTransform Transform = Parenting.Pose->GetComponentTransform();
		Space.Linear = Transform.TransformPosition(LocalOffset.Linear);
		Space.Angular = Transform.TransformRotation(LocalOffset.Angular);
		Space.LinearVelocity = Parenting.Pose->ComponentVelocity;
	}
	else
	{
		Space.Linear = Identifier.Component->GetComponentLocation();
		Space.Angular = Identifier.Component->GetComponentQuat();
		Space.LinearVelocity = Identifier.Component->ComponentVelocity;
	}

	if (Parenting.Component.IsValid() && ComputeRelativeDynamics)
	{
		const FTGOR_MovementSpace Parent = Parenting.Component->ComputeSpace();

		FTGOR_MovementDynamic Local;
		Local.DynamicToBase(Parent, Space);
		return Local.BaseToSpace(Parent);
	}
	return Space;
}

void UTGOR_SequencerPilotTask::Parent(UTGOR_SequencerComponent* Parent, USkeletalMeshComponent* Pose)
{
	if (Parenting.Component != Parent)
	{
		const bool Reregister = IsRegistered();
		if (Reregister)
		{
			Unregister();
		}

		// Set parenting
		Parenting.Component = Parent;
		Parenting.Pose = Pose;

		if (Reregister)
		{
			Register();
		}
	}
}

bool UTGOR_SequencerPilotTask::CanParent(const UTGOR_SequencerComponent* Parent, const USkeletalMeshComponent* Pose) const
{
	return IsValid(Parent) && IsValid(Pose) && Identifier.Component->CanParent(Parent);
}

USkeletalMeshComponent* UTGOR_SequencerPilotTask::GetPose() const
{
	return Parenting.Pose.Get();
}

void UTGOR_SequencerPilotTask::SetPoseOffset(const FTGOR_MovementPosition& Offset)
{
	LocalOffset = Offset;
	if (IsValid(Identifier.Component))
	{
		Identifier.Component->OnPositionChange(ComputePosition());
	}
}