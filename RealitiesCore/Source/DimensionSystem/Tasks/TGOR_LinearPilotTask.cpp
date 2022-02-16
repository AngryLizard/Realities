// The Gateway of Realities: Planes of Existence.


#include "TGOR_LinearPilotTask.h"
#include "DimensionSystem/Components/TGOR_PilotComponent.h"

#include "Net/UnrealNetwork.h"
#include "Engine/ActorChannel.h"

bool FTGOR_LinearParenting::operator==(const FTGOR_LinearParenting& Other) const
{
	return Component == Other.Component && Index == Other.Index;
}

bool FTGOR_LinearParenting::operator!=(const FTGOR_LinearParenting& Other) const
{
	return Component != Other.Component || Index != Other.Index;
}


UTGOR_LinearPilotTask::UTGOR_LinearPilotTask()
{
}

void UTGOR_LinearPilotTask::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(UTGOR_LinearPilotTask, Parenting, COND_None);
}


UTGOR_MobilityComponent* UTGOR_LinearPilotTask::GetParent() const
{
	return Parenting.Component.Get();
}


void UTGOR_LinearPilotTask::Unparent()
{
	Super::Unparent();

	Parent(nullptr, INDEX_NONE);
}

void UTGOR_LinearPilotTask::Register()
{
	Super::Register();
	if (Parenting.Component.IsValid())
	{
		Parenting.Component->RegisterAttach(this);
	}
}

void UTGOR_LinearPilotTask::Unregister()
{
	Super::Unregister();
	if (Parenting.Component.IsValid())
	{
		Parenting.Component->RegisterDetach(this);
	}
}


////////////////////////////////////////////////////////////////////////////////////////////////////

void UTGOR_LinearPilotTask::Parent(UTGOR_MobilityComponent* Parent, int32 Index)
{
	if (Parenting.Component != Parent || Parenting.Index.Index != Index)
	{
		const bool Reregister = IsRegistered();
		if (Reregister)
		{
			Unregister();
		}

		// Set parenting
		Parenting.Component = Parent;
		Parenting.Index = Index;

		if (Reregister)
		{
			Register();
		}
	}
}

bool UTGOR_LinearPilotTask::CanParent(const UTGOR_MobilityComponent* Parent, int32 Index) const
{
	return IsValid(Parent) && Identifier.Component->CanParent(Parent) && Parent->IsValidIndex(Index);
}

void UTGOR_LinearPilotTask::ResetToComponent(UTGOR_MobilityComponent* Owner)
{
	SimulatePosition(Owner->GetComponentPosition());
}

////////////////////////////////////////////////////////////////////////////////////////////////////

FTGOR_MovementPosition UTGOR_LinearPilotTask::ComputeParentPosition() const
{
	if (Parenting.Component.IsValid())
	{
		const FTGOR_MovementPosition Position = Parenting.Component->ComputePosition();
		if (Parenting.Index)
		{
			const FTGOR_MovementPosition Part = Parenting.Component->GetIndexTransform(Parenting.Index.Index);
			return Part.BaseToPosition(Position);
		}
		return Position;
	}
	return FTGOR_MovementPosition();
}

FTGOR_MovementSpace UTGOR_LinearPilotTask::ComputeParentSpace() const
{
	if (Parenting.Component.IsValid())
	{
		FTGOR_MovementSpace Space = Parenting.Component->ComputeSpace();
		if (Parenting.Index)
		{
			const FTGOR_MovementDynamic Part = Parenting.Component->GetIndexTransform(Parenting.Index.Index);
			return Part.BaseToSpace(Space);
		}

		return Space;
	}
	return FTGOR_MovementSpace();
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void UTGOR_LinearPilotTask::InitDynamic(const FTGOR_MovementDynamic& Dynamic)
{

}

void UTGOR_LinearPilotTask::SimulateDynamic(const FTGOR_MovementDynamic& Dynamic)
{
	if (IsValid(Identifier.Component))
	{
		Identifier.Component->OnPositionChange(Dynamic);
	}
}

void UTGOR_LinearPilotTask::SimulatePosition(const FTGOR_MovementPosition& Position)
{
	if (IsValid(Identifier.Component))
	{
		Identifier.Component->OnPositionChange(Position);
	}
}
