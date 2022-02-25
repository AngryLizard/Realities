// The Gateway of Realities: Planes of Existence.

#include "TGOR_NamedSocket.h"
#include "Realities/Base/Inventory/TGOR_SocketStorage.h"
#include "Realities/Base/Inventory/Modules/TGOR_SocketModule.h"

#include "Realities/Components/Dimension/Interactable/TGOR_InteractableComponent.h"
#include "Realities/Components/Movement/TGOR_SocketComponent.h"

UTGOR_NamedSocket::UTGOR_NamedSocket()
	: Super(), SocketName(""), Radius(1.0f), Height(1.0f)
{
}

void UTGOR_NamedSocket::BuildStorage(UTGOR_SocketStorage* Storage)
{
	Super::BuildStorage(Storage);

	UTGOR_SocketModule* Module = Storage->GetModule<UTGOR_SocketModule>();
	Module->SetSocketName(SocketName);
}

UTGOR_MobilityComponent* UTGOR_NamedSocket::QuerySocket(UTGOR_InteractableComponent* Component, FTGOR_MovementPosition& Position) const
{
	AActor* Owner = Component->GetOwner();
	UTGOR_SocketComponent* Mobility = Owner->FindComponentByClass<UTGOR_SocketComponent>();
	if (IsValid(Mobility))
	{
		Position = Mobility->GetTransformOfSocket(const_cast<UTGOR_NamedSocket*>(this));
	}
	return Mobility;
}

UTGOR_MobilityComponent* UTGOR_NamedSocket::QuerySocketWith(USceneComponent* Component, FTGOR_MovementPosition& Position) const
{
	UTGOR_SocketComponent* Mobility = Cast<UTGOR_SocketComponent>(Component);
	if (IsValid(Mobility))
	{
		Position = Mobility->GetTransformOfSocket(const_cast<UTGOR_NamedSocket*>(this));
	}
	return Mobility;
}

bool UTGOR_NamedSocket::AttachToSocket(UTGOR_MobilityComponent* Parent, UTGOR_PilotComponent* Component)
{
	UTGOR_SocketComponent* MovementParent = Cast<UTGOR_SocketComponent>(Parent);
	if (IsValid(MovementParent))
	{
		ETGOR_ComputeEnumeration State;
		MovementParent->AttachToSocket(Component, this, State);
		return State == ETGOR_ComputeEnumeration::Success;
	}
	return Super::AttachToSocket(Parent, Component);
}

bool UTGOR_NamedSocket::CanAttachToSocket(UTGOR_MobilityComponent* Parent, UTGOR_PilotComponent* Component) const
{
	UTGOR_SocketComponent* MovementParent = Cast<UTGOR_SocketComponent>(Parent);
	if (IsValid(MovementParent))
	{
		return MovementParent->CanAttachToSocket(Component, const_cast<UTGOR_NamedSocket*>(this));
	}

	return Super::CanAttachToSocket(Parent, Component);
}

bool UTGOR_NamedSocket::CanFitToSocket(UTGOR_MobilityComponent* Parent, const FTGOR_MovementShape& Shape) const
{
	UTGOR_SocketComponent* MovementParent = Cast<UTGOR_SocketComponent>(Parent);
	if (IsValid(MovementParent))
	{
		return MovementParent->CanFitToSocket(Shape, const_cast<UTGOR_NamedSocket*>(this));
	}

	return Super::CanFitToSocket(Parent, Shape);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void UTGOR_NamedSocket::Tick(UTGOR_DimensionComponent* Owner, UTGOR_SocketStorage* Storage, float DeltaSeconds)
{

}

void UTGOR_NamedSocket::Attach(UTGOR_SocketStorage* Storage, USceneComponent* Attachee)
{
	UTGOR_MobilityComponent* Parent = GetParent(Storage);
	if (IsValid(Parent))
	{
		USceneComponent* Basis = Parent->GetMovementBasis();
		check(!Basis->IsAttachedTo(Attachee) && "Attachment loop detected");
		Attachee->AttachToComponent(Basis, FAttachmentTransformRules::KeepWorldTransform, SocketName);
	}
}

UTGOR_MobilityComponent* UTGOR_NamedSocket::GetParent(UTGOR_SocketStorage* Storage) const
{
	UTGOR_SocketModule* Module = Storage->GetModule<UTGOR_SocketModule>();
	if (IsValid(Module))
	{
		return Module->GetParent();
	}
	return nullptr;
}

FTGOR_MovementDynamic UTGOR_NamedSocket::GetTransform(UTGOR_MobilityComponent* Owner, UTGOR_SocketStorage* Storage) const
{
	FTGOR_MovementDynamic Dynamic;
	UTGOR_SocketModule* Module = Storage->GetModule<UTGOR_SocketModule>();
	if (IsValid(Module))
	{
		UTGOR_SocketComponent* Parent = Module->GetParent();
		USceneComponent* BaseComponent = Parent->GetMovementBasis();
		const FTransform ParentTransform = BaseComponent->GetComponentTransform();

		FTGOR_MovementPosition Position = Module->GetTransform();
		Dynamic.Linear = ParentTransform.InverseTransformPositionNoScale(Position.Linear);
		Dynamic.Angular = ParentTransform.InverseTransformRotation(Position.Angular);
	}
	return Dynamic;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

bool UTGOR_NamedSocket::CheckDimensions(const UTGOR_MobilityComponent* Parent, const FTGOR_MovementShape& Shape) const
{
	const FTGOR_MovementShape& ParentShape = Parent->GetBody();
	return (Shape.Radius <= ParentShape.Radius * Radius && Shape.Height <= ParentShape.Height * Height);
}