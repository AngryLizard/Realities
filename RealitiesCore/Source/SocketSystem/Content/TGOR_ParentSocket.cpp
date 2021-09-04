// The Gateway of Realities: Planes of Existence.

#include "TGOR_ParentSocket.h"

#include "TargetSystem/Components/TGOR_InteractableComponent.h"

UTGOR_ParentSocket::UTGOR_ParentSocket()
	: Super()
{
	AllowsReparent = true;

	Abstract = false;
}


/*
void UTGOR_ParentSocket::AttachParenting(UTGOR_PilotComponent* Owner, UTGOR_SocketStorage* Storage)
{
	Super::AttachParenting(Owner, Storage);

	UTGOR_ParentModule* Module = Storage->GetModule<UTGOR_ParentModule>();
	if (IsValid(Module))
	{
		UTGOR_MobilityComponent* Parent = Module->GetParent();
		if (IsValid(Parent))
		{
			const FName SocketName = Parent->GetNameFromIndex(Module->GetParentIndex());

			USceneComponent* OwnerBasis = Owner->GetMovementBasis();
			USceneComponent* ParentBasis = Parent->GetMovementBasis();
			check(!ParentBasis->IsAttachedTo(OwnerBasis) && "Attachment loop detected");
			OwnerBasis->AttachToComponent(ParentBasis, FAttachmentTransformRules::KeepWorldTransform, SocketName);
		}
	}
}
*/

