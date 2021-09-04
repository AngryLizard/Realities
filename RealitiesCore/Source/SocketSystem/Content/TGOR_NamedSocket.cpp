// The Gateway of Realities: Planes of Existence.

#include "TGOR_NamedSocket.h"
#include "AttributeSystem/Content/TGOR_Attribute.h"

#include "TargetSystem/Components/TGOR_InteractableComponent.h"
#include "SocketSystem/Components/TGOR_SocketComponent.h"

UTGOR_NamedSocket::UTGOR_NamedSocket()
	: Super(), SocketName(""), Radius(1.0f), Height(1.0f)
{
	AllowsReparent = false;
}

UTGOR_MobilityComponent* UTGOR_NamedSocket::QuerySocket(UTGOR_InteractableComponent* Component, FTGOR_MovementPosition& Position) const
{
	AActor* Owner = Component->GetOwner();
	UTGOR_SocketComponent* Mobility = Owner->FindComponentByClass<UTGOR_SocketComponent>();
	if (IsValid(Mobility))
	{
		Position = Mobility->GetTransformOfSocket(SocketName);
	}
	return Mobility;
}

UTGOR_MobilityComponent* UTGOR_NamedSocket::QuerySocketWith(UActorComponent* Component, FTGOR_MovementPosition& Position) const
{
	UTGOR_SocketComponent* Mobility = Cast<UTGOR_SocketComponent>(Component);
	if (IsValid(Mobility))
	{
		Position = Mobility->GetTransformOfSocket(SocketName);
	}
	return Mobility;
}


/*
void UTGOR_NamedSocket::AttachParenting(UTGOR_PilotComponent* Owner, UTGOR_SocketStorage* Storage)
{
	Super::AttachParenting(Owner, Storage);

	UTGOR_SocketModule* Module = Storage->GetModule<UTGOR_SocketModule>();
	if (IsValid(Module))
	{
		UTGOR_MobilityComponent* Parent = Module->GetParent();
		if (IsValid(Parent))
		{
			USceneComponent* OwnerBasis = Owner->GetMovementBasis();
			USceneComponent* ParentBasis = Parent->GetMovementBasis();
			check(!ParentBasis->IsAttachedTo(OwnerBasis) && "Attachment loop detected");
			OwnerBasis->AttachToComponent(ParentBasis, FAttachmentTransformRules::KeepWorldTransform, SocketName);
		}
	}
}
*/

/*
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

*/
////////////////////////////////////////////////////////////////////////////////////////////////////

bool UTGOR_NamedSocket::CheckDimensions(const UTGOR_SocketComponent* Parent, const FTGOR_MovementShape& Shape) const
{
	if (IsValid(Parent))
	{
		UTGOR_Attribute* Attribute = Instanced_AttributeInsertions.GetOfType(SizeAttribute);
		const float Factor = Parent->Execute_GetAttribute(Parent, Attribute, 1.0f);

		const FTGOR_MovementShape& ParentShape = Parent->GetBody();
		return (Shape.Radius <= ParentShape.Radius * Radius * Factor && Shape.Height <= ParentShape.Height * Height * Factor);
	}
	return false;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
