// The Gateway of Realities: Planes of Existence.

#include "TGOR_NamedSocketTask.h"
#include "SocketSystem/Components/TGOR_SocketComponent.h"
#include "SocketSystem/Content/TGOR_NamedSocket.h"

#include "Net/UnrealNetwork.h"
#include "Engine/ActorChannel.h"

FTGOR_NamedSocketParenting::FTGOR_NamedSocketParenting()
	: Socket(nullptr)
{
}

bool FTGOR_NamedSocketParenting::operator==(const FTGOR_NamedSocketParenting& Other) const
{
	return Component == Other.Component && Socket == Other.Socket;
}

bool FTGOR_NamedSocketParenting::operator!=(const FTGOR_NamedSocketParenting& Other) const
{
	return Component != Other.Component || Socket != Other.Socket;
}

UTGOR_NamedSocketTask::UTGOR_NamedSocketTask()
{
}

void UTGOR_NamedSocketTask::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(UTGOR_NamedSocketTask, Parenting, COND_None);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

UTGOR_MobilityComponent* UTGOR_NamedSocketTask::GetParent() const
{
	return Parenting.Component.Get();
}

FTGOR_MovementPosition UTGOR_NamedSocketTask::ComputePosition() const
{
	if (Parenting.Component.IsValid() && IsValid(Parenting.Socket))
	{
		const FTGOR_MovementPosition Position = Parenting.Component->ComputePosition();
		const FTGOR_MovementPosition Part = Parenting.Component->GetTransformOfSocket(Parenting.Socket->SocketName);
;
		const FTransform ParentTransform = Identifier.Component->GetComponentTransform();

		FTGOR_MovementPosition Local;
		Local.Linear = ParentTransform.InverseTransformPositionNoScale(Part.Linear);
		Local.Angular = ParentTransform.InverseTransformRotation(Part.Angular);
		return Local.BaseToPosition(Position);
	}
	return FTGOR_MovementPosition();
}

FTGOR_MovementSpace UTGOR_NamedSocketTask::ComputeSpace() const
{
	if (Parenting.Component.IsValid() && IsValid(Parenting.Socket))
	{
		FTGOR_MovementSpace Space = Parenting.Component->ComputeSpace();
		const FTGOR_MovementPosition Part = Parenting.Component->GetTransformOfSocket(Parenting.Socket->SocketName);

		const FTransform ParentTransform = Identifier.Component->GetComponentTransform();

		FTGOR_MovementDynamic Local;
		Local.Linear = ParentTransform.InverseTransformPositionNoScale(Part.Linear);
		Local.Angular = ParentTransform.InverseTransformRotation(Part.Angular);
		return Local.BaseToSpace(Space);
	}
	return FTGOR_MovementSpace();
}

void UTGOR_NamedSocketTask::Update(float DeltaTime)
{
	Super::Update(DeltaTime);
}

void UTGOR_NamedSocketTask::Unparent()
{
	Super::Unparent();

	Parent(nullptr, nullptr);
}

void UTGOR_NamedSocketTask::Register()
{
	Super::Register();
	if (Parenting.Component.IsValid())
	{
		Parenting.Component->RegisterAttachToSocket(this, Parenting.Socket);
	}
}

void UTGOR_NamedSocketTask::Unregister()
{
	Super::Unregister();
	if (Parenting.Component.IsValid())
	{
		Parenting.Component->RegisterDetachFromSocket(this, Parenting.Socket);
	}
}

UTGOR_Socket* UTGOR_NamedSocketTask::GetSocket() const
{
	return Parenting.Socket;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void UTGOR_NamedSocketTask::RecachePrimitive()
{
	Cache.Reset();
	if (IsValid(Parenting.Socket) && !Parenting.Socket->SocketName.IsNone() && Parenting.Component.IsValid())
	{
		TArray<UPrimitiveComponent*> Primitives = Parenting.Component->GetOwnerComponents<UPrimitiveComponent>();
		for (UPrimitiveComponent* Primitive : Primitives)
		{
			if (Primitive->DoesSocketExist(Parenting.Socket->SocketName))
			{
				Cache = Primitive;
			}
		}
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void UTGOR_NamedSocketTask::Parent(UTGOR_SocketComponent* Parent, UTGOR_NamedSocket* Socket)
{
	if (Parenting.Component != Parent || Parenting.Socket != Socket)
	{
		const bool Reregister = IsRegistered();
		if (Reregister)
		{
			Unregister();
		}

		Parenting.Component = Parent;
		Parenting.Socket = Socket;
		RecachePrimitive();

		if (Reregister)
		{
			Register();
		}
	}
}

bool UTGOR_NamedSocketTask::CanParent(const UTGOR_SocketComponent* Parent, UTGOR_NamedSocket* Socket) const
{
	return IsValid(Parent) && Identifier.Component->CanParent(Parent) && Parent->CanAttachToSocket(Identifier.Component, Socket);
}
