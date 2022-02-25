// The Gateway of Realities: Planes of Existence.
#include "TGOR_SocketComponent.h"
#include "TGOR_PilotComponent.h"

#include "Realities/Base/TGOR_Singleton.h"
#include "Realities/Base/System/Data/TGOR_WorldData.h"
#include "Realities/Actors/Dimension/Volumes/TGOR_PhysicsVolume.h"
#include "Realities/Mod/Targets/Sockets/TGOR_NamedSocket.h"
#include "Realities/Utility/Storage/TGOR_Package.h"

#include "Realities/Base/Content/TGOR_ContentManager.h"
#include "Net/UnrealNetwork.h"


SERIALISE_INIT_SOURCE_IMPLEMENT(FTGOR_NamedAttachment)
SERIALISE_INIT_SOURCE_IMPLEMENT(FTGOR_NamedAttachments)

void FTGOR_NamedAttachment::Write(FTGOR_GroupWritePackage& Package, UTGOR_Singleton* Context) const
{
	Package.WriteEntry("Component", Component);
	Package.WriteEntry("Socket", Socket);
}

void FTGOR_NamedAttachment::Read(FTGOR_GroupReadPackage& Package, UTGOR_Singleton* Context)
{
	Package.ReadEntry("Component", Component);
	Package.ReadEntry("Socket", Socket);
}

void FTGOR_NamedAttachment::Send(FTGOR_NetworkWritePackage& Package, UTGOR_Singleton* Context) const
{
	Package.WriteEntry(Component);
	Package.WriteEntry(Socket);
}

void FTGOR_NamedAttachment::Recv(FTGOR_NetworkReadPackage& Package, UTGOR_Singleton* Context)
{
	Package.ReadEntry(Component);
	Package.ReadEntry(Socket);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void FTGOR_NamedAttachments::Write(FTGOR_GroupWritePackage& Package, UTGOR_Singleton* Context) const
{
	Package.WriteEntry("Parts", Parts);
}

void FTGOR_NamedAttachments::Read(FTGOR_GroupReadPackage& Package, UTGOR_Singleton* Context)
{
	Package.ReadEntry("Parts", Parts);
}

void FTGOR_NamedAttachments::Send(FTGOR_NetworkWritePackage& Package, UTGOR_Singleton* Context) const
{
	Package.WriteEntry(Parts);
}

void FTGOR_NamedAttachments::Recv(FTGOR_NetworkReadPackage& Package, UTGOR_Singleton* Context)
{
	Package.ReadEntry(Parts);
}


UTGOR_SocketComponent::UTGOR_SocketComponent()
:	Super()
{
	SetIsReplicatedByDefault(true);
}


void UTGOR_SocketComponent::BeginPlay()
{
	Super::BeginPlay();
}


void UTGOR_SocketComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);


	if (GetOwnerRole() != ENetRole::ROLE_Authority) return;
	for (auto It = Attachments.Parts.CreateIterator(); It; ++It)
	{
		UTGOR_NamedSocket* Socket = It->Value.Socket;
		if (It->Value.Component.IsValid())
		{
			// Make sure not to keep track of already detached mobility
			if (!It->Value.Component->IsAttachedTo(this, It->Key, INDEX_NONE))
			{
				It.RemoveCurrent();
				OnSocketChanged.Broadcast(Socket);
			}
		}
		else
		{
			It.RemoveCurrent();
			OnSocketChanged.Broadcast(Socket);
		}
	}
}

void UTGOR_SocketComponent::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// Replicate to everyone
	DOREPLIFETIME_CONDITION(UTGOR_SocketComponent, Attachments, COND_None);
}


FTGOR_MovementDynamic UTGOR_SocketComponent::GetIndexTransform(UTGOR_Socket* Socket, int32 Index) const
{
	if (IsValid(Socket))
	{
		if (const FTGOR_NamedAttachment* Ptr = Attachments.Parts.Find(Socket))
		{
			USceneComponent* BaseComponent = GetMovementBasis();
			const FTransform ParentTransform = BaseComponent->GetComponentTransform();
			FTGOR_MovementPosition Position = GetTransformOfSocket(Ptr->Socket);

			FTGOR_MovementDynamic Dynamic;
			Dynamic.Linear = ParentTransform.InverseTransformPositionNoScale(Position.Linear);
			Dynamic.Angular = ParentTransform.InverseTransformRotation(Position.Angular);
			return Dynamic;
		}
	}
	return Super::GetIndexTransform(Socket, Index);
}

bool UTGOR_SocketComponent::IsSocketOccupied(UTGOR_Socket* Socket) const
{
	if (!IsValid(Socket)) return true;
	return Attachments.Parts.Contains(Socket);
}

bool UTGOR_SocketComponent::QuerySocketObjects(UTGOR_PilotComponent* Component, FTGOR_MovementVisuals& Visuals) const
{
	for (const auto Pair : Attachments.Parts)
	{
		if (Pair.Value.Component == Component)
		{
			Visuals.Radius = Pair.Value.Radius;
			Visuals.Blend = Pair.Value.Blend;
			Visuals.Content = Pair.Value.Socket;
			return true;
		}
	}
	return false;
}

TArray<UTGOR_PilotComponent*> UTGOR_SocketComponent::GetSocketObjects(TSubclassOf<UTGOR_Socket> Type) const
{
	TArray<UTGOR_PilotComponent*> Objects;
	for (const auto Pair : Attachments.Parts)
	{
		if (Pair.Value.Socket->IsA(Type))
		{
			Objects.Emplace(Pair.Value.Component.Get());
		}
	}
	return Objects;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void UTGOR_SocketComponent::RepNotifyAttachments()
{
	for (const auto Pair : Attachments.Parts)
	{
		// Force-attach if current attachment doesn't match
		if (!Pair.Value.Component->IsAttachedTo(this, Pair.Key, INDEX_NONE))
		{
			// Make sure no other handles with this parent exist
			//Pair.Value.Component->UnregisterHandleWith(this);
			//Pair.Value.Component->RegisterHandle(this, Pair.Value.Socket, this, Pair.Key.Index);
			Pair.Value.Component->AttachTo(this, Pair.Key, INDEX_NONE);
		}
	}
}

FTGOR_MovementPosition UTGOR_SocketComponent::GetTransformOfSocket(UTGOR_NamedSocket* Socket) const
{
	if (IsValid(Socket))
	{
		// TODO: Cache target component in Attachments for faster access
		// Get socket transform
		TArray<UPrimitiveComponent*> Components = GetOwnerComponents<UPrimitiveComponent>();
		for (UPrimitiveComponent* Component : Components)
		{
			if (Component->DoesSocketExist(Socket->SocketName))
			{
				const FTransform& BoneTransform = Component->GetSocketTransform(Socket->SocketName, ERelativeTransformSpace::RTS_World);
				FTGOR_MovementPosition Position;
				Position.Linear = BoneTransform.GetLocation();
				Position.Angular = BoneTransform.GetRotation();
				return Position;
			}
		}
	}
	return ComputePosition();
}

bool UTGOR_SocketComponent::CanFitToSocket(const FTGOR_MovementShape& Shape, UTGOR_NamedSocket* Socket) const
{
	if (!IsValid(Socket)) return false;
	return !IsSocketOccupied(Socket) && Socket->CheckDimensions(this, Shape);
}

bool UTGOR_SocketComponent::CanAttachToSocket(UTGOR_PilotComponent* Attachee, UTGOR_NamedSocket* Socket) const
{
	return CanFitToSocket(Attachee->GetBody(), Socket) && Attachee->CanAttachTo(this);
}

bool UTGOR_SocketComponent::IsAttachedToSocket(UTGOR_PilotComponent* Attachee, UTGOR_NamedSocket* Socket) const
{
	if (!IsValid(Socket)) return false;
	const FTGOR_NamedAttachment* Ptr = Attachments.Parts.Find(Socket);
	if (Ptr)
	{
		return Ptr->Component == Attachee;
	}
	return false;
}

UTGOR_NamedSocket* UTGOR_SocketComponent::GetAttachedSocket(UTGOR_PilotComponent* Attachee) const
{
	for (const auto Pair : Attachments.Parts)
	{
		if (Pair.Value.Component == Attachee)
		{
			return Pair.Value.Socket;
		}
	}
	return nullptr;
}

UTGOR_PilotComponent* UTGOR_SocketComponent::GetAttached(UTGOR_NamedSocket* Socket) const
{
	if (!IsValid(Socket)) return nullptr;
	const FTGOR_NamedAttachment* Ptr = Attachments.Parts.Find(Socket);
	if (Ptr)
	{
		return Ptr->Component.Get();
	}
	return nullptr;
}

FTGOR_MovementShape UTGOR_SocketComponent::GetDisplacedShape(UTGOR_NamedSocket* Socket) const
{
	if (!IsValid(Socket)) return FTGOR_MovementShape();
	const FTGOR_NamedAttachment* Ptr = Attachments.Parts.Find(Socket);
	if (Ptr)
	{
		return Ptr->Component->GetBody();
	}
	return FTGOR_MovementShape();
}

void UTGOR_SocketComponent::SetSocketVisuals(UTGOR_NamedSocket* Socket, float Radius, float Blend)
{
	if (!IsValid(Socket)) return;
	FTGOR_NamedAttachment* Ptr = Attachments.Parts.Find(Socket);
	if (Ptr)
	{
		Ptr->Radius = Radius;
		Ptr->Blend = Blend;
	}
}

void UTGOR_SocketComponent::AttachToSocket(UTGOR_PilotComponent* Attachee, UTGOR_NamedSocket* Socket, ETGOR_ComputeEnumeration& Branches)
{
	if (GetOwnerRole() != ENetRole::ROLE_Authority) return;

	if (!IsValid(Socket)) return;
	Branches = ETGOR_ComputeEnumeration::Failed;

	// Create attachment
	FTGOR_NamedAttachment Attachment;
	Attachment.Component = Attachee;
	Attachment.Socket = Socket;

	// Check validity
	const FTGOR_MovementShape& Shape = Attachee->GetBody();
	if (Attachments.Parts.Contains(Socket) || !IsValid(Socket) || !Socket->CheckDimensions(this, Shape)) return;

	// Register on mobility for attachment
	Attachments.Parts.Add(Socket, Attachment);
	//Attachee->RegisterHandle(this, Socket, this, Index);
	Attachee->AttachTo(this, Socket, INDEX_NONE);
	OnSocketChanged.Broadcast(Socket);
	Branches = ETGOR_ComputeEnumeration::Success;
}

void UTGOR_SocketComponent::DetachFromSocket(UTGOR_PilotComponent* Attachee)
{
	if (GetOwnerRole() != ENetRole::ROLE_Authority) return;

	for (auto It = Attachments.Parts.CreateIterator(); It; ++It)
	{
		// Make sure not to keep track of already detached mobility
		if (It->Value.Component == Attachee)
		{
			UTGOR_NamedSocket* Socket = It->Value.Socket;
			//Attachee->UnregisterHandle(Socket);
			Attachee->DetachFrom(this, Socket, INDEX_NONE);

			It.RemoveCurrent();
			OnSocketChanged.Broadcast(Socket);
		}
	}
}

void UTGOR_SocketComponent::DetachFromSocketAt(UTGOR_NamedSocket* Socket)
{
	if (GetOwnerRole() != ENetRole::ROLE_Authority) return;

	if (!IsValid(Socket)) return;
	const FTGOR_NamedAttachment* Ptr = Attachments.Parts.Find(Socket);
	if (Ptr)
	{
		//Ptr->Component->UnregisterHandle(Socket);
		Ptr->Component->DetachFrom(this, Socket, INDEX_NONE);

		Attachments.Parts.Remove(Socket);
		OnSocketChanged.Broadcast(Socket);
	}
}
