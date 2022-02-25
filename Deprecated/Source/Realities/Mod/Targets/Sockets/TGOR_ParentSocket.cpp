// The Gateway of Realities: Planes of Existence.

#include "TGOR_ParentSocket.h"
#include "Realities/Base/Inventory/TGOR_SocketStorage.h"
#include "Realities/Base/Inventory/Modules/TGOR_ParentModule.h"

#include "Realities/Components/Dimension/Interactable/TGOR_InteractableComponent.h"
#include "Realities/Components/Movement/TGOR_SocketComponent.h"

UTGOR_ParentSocket::UTGOR_ParentSocket()
	: Super()
{
	Modules.Modules.Emplace("Parent", UTGOR_ParentModule::StaticClass());
}

void UTGOR_ParentSocket::BuildStorage(UTGOR_SocketStorage* Storage)
{
	Super::BuildStorage(Storage);

	//UTGOR_ParentModule* Module = Storage->GetModule<UTGOR_ParentModule>();
}

void UTGOR_ParentSocket::Tick(UTGOR_DimensionComponent* Owner, UTGOR_SocketStorage* Storage, float DeltaSeconds)
{

}

void UTGOR_ParentSocket::Attach(UTGOR_SocketStorage* Storage, USceneComponent* Attachee)
{
	UTGOR_ParentModule* Module = Storage->GetModule<UTGOR_ParentModule>();
	if (IsValid(Module))
	{
		const FName SocketName = Module->GetSocketName();
		UTGOR_MobilityComponent* Parent = Module->GetParent();
		if (IsValid(Parent))
		{
			USceneComponent* Basis = Parent->GetMovementBasis();
			check(!Basis->IsAttachedTo(Attachee) && "Attachment loop detected");
			Attachee->AttachToComponent(Basis, FAttachmentTransformRules::KeepWorldTransform, SocketName);
		}
	}
}

UTGOR_MobilityComponent* UTGOR_ParentSocket::GetParent(UTGOR_SocketStorage* Storage) const
{
	UTGOR_ParentModule* Module = Storage->GetModule<UTGOR_ParentModule>();
	if (IsValid(Module))
	{
		return Module->GetParent();
	}
	return nullptr;
}

FTGOR_MovementDynamic UTGOR_ParentSocket::GetTransform(UTGOR_MobilityComponent* Owner, UTGOR_SocketStorage* Storage) const
{
	FTGOR_MovementDynamic Dynamic;
	UTGOR_ParentModule* Module = Storage->GetModule<UTGOR_ParentModule>();
	if (IsValid(Module))
	{
		UTGOR_MobilityComponent* Parent = Module->GetParent();
		USceneComponent* BaseComponent = Parent->GetMovementBasis();
		const FTransform ParentTransform = BaseComponent->GetComponentTransform();

		FTGOR_MovementPosition Position = Module->GetTransform();
		Dynamic.Linear = ParentTransform.InverseTransformPositionNoScale(Position.Linear);
		Dynamic.Angular = ParentTransform.InverseTransformRotation(Position.Angular);
	}
	return Dynamic;
}

