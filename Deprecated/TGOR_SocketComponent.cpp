// The Gateway of Realities: Planes of Existence.


#include "TGOR_SocketComponent.h"
#include "GameFramework/Actor.h"

#include "Realities/Components/Movement/TGOR_PilotComponent.h"
#include "Realities/Base/Instances/Dimension/TGOR_DimensionDataInstance.h"
#include "Realities/Base/System/Data/TGOR_WorldData.h"
#include "Realities/Mod/Pipelines/TGOR_Pipeline.h"
#include "Realities/Mod/Sockets/TGOR_StaticSocket.h"
#include "Realities/Base/TGOR_Singleton.h"
#include "Realities/Utility/Storage/TGOR_Package.h"
#include "Realities/Actors/Dimension/Volumes/TGOR_PhysicsVolume.h"
#include "Realities/Base/Content/TGOR_ContentManager.h"

#include "DrawDebugHelpers.h"
#include "Net/UnrealNetwork.h"

SERIALISE_INIT_SOURCE_IMPLEMENT(FTGOR_StaticAttachment)
SERIALISE_INIT_SOURCE_IMPLEMENT(FTGOR_StaticAttachments)

void FTGOR_StaticAttachment::Write(FTGOR_GroupWritePackage& Package, UTGOR_Singleton* Context) const
{
	Package.WriteEntry("Component", Component);
	Package.WriteEntry("Socket", Socket);
}

void FTGOR_StaticAttachment::Read(FTGOR_GroupReadPackage& Package, UTGOR_Singleton* Context)
{
	Package.ReadEntry("Component", Component);
	Package.ReadEntry("Socket", Socket);
}

void FTGOR_StaticAttachment::Send(FTGOR_NetworkWritePackage& Package, UTGOR_Singleton* Context) const
{
	Package.WriteEntry(Component);
	Package.WriteEntry(Socket);
}

void FTGOR_StaticAttachment::Recv(FTGOR_NetworkReadPackage& Package, UTGOR_Singleton* Context)
{
	Package.ReadEntry(Component);
	Package.ReadEntry(Socket);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void FTGOR_StaticAttachments::Write(FTGOR_GroupWritePackage& Package, UTGOR_Singleton* Context) const
{
	Package.WriteEntry("Parts", Parts);
}

void FTGOR_StaticAttachments::Read(FTGOR_GroupReadPackage& Package, UTGOR_Singleton* Context)
{
	Package.ReadEntry("Parts", Parts);
}

void FTGOR_StaticAttachments::Send(FTGOR_NetworkWritePackage& Package, UTGOR_Singleton* Context) const
{
	Package.WriteEntry(Parts);
}

void FTGOR_StaticAttachments::Recv(FTGOR_NetworkReadPackage& Package, UTGOR_Singleton* Context)
{
	Package.ReadEntry(Parts);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

UTGOR_SocketComponent::UTGOR_SocketComponent()
:	Super()
{
	SetIsReplicatedByDefault(true);
}

void UTGOR_SocketComponent::BeginPlay()
{
	// Get sockets
	SINGLETON_CHK;
	UTGOR_ContentManager* ContentManager = Singleton->GetContentManager();
	for (TSubclassOf<UTGOR_StaticSocket> SocketClass : SocketClasses)
	{
		Sockets.Append(ContentManager->GetTListFromType(SocketClass));
	}

	Super::BeginPlay();
}

void UTGOR_SocketComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	for (auto It = Attachments.Parts.CreateIterator(); It; ++It)
	{
		// Make sure not to keep track of already detached mobility
		if (!It->Value.Component->IsAttachedTo(this, It->Key.Index))
		{
			if (GetOwnerRole() == ENetRole::ROLE_Authority)
			{
				It.RemoveCurrent();
			}
		}
	}
}

void UTGOR_SocketComponent::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// Replicate attachment status to everyone
	DOREPLIFETIME_CONDITION(UTGOR_SocketComponent, Attachments, COND_None);
}
bool UTGOR_SocketComponent::IsValidIndex(int32 Index) const
{
	if (Super::IsValidIndex(Index)) return false;
	return Index >= 0;
}

FTGOR_MovementDynamic UTGOR_SocketComponent::GetIndexTransform(int32 Index) const
{
	FTGOR_MovementDynamic Dynamic;
	if (Index != INDEX_NONE)
	{
		USceneComponent* Basis = GetMovementBasis();
		if (const FTGOR_StaticAttachment* Ptr = Attachments.Parts.Find(Index))
		{
			// TODO: Cache in Attachments for faster access
			// Get socket transform
			TArray<UPrimitiveComponent*> Components = GetOwnerComponents<UPrimitiveComponent>();
			for(UPrimitiveComponent* Component : Components)
			{
				if(Component->DoesSocketExist(Ptr->Socket->SocketName))
				{
					const FTransform& BoneTransform = Component->GetSocketTransform(Ptr->Socket->SocketName, ERelativeTransformSpace::RTS_World);
					Dynamic.Linear = BoneTransform.GetLocation();
					Dynamic.Angular = BoneTransform.GetRotation();
				}
			}
		}
		else
		{
			return Dynamic;
		}

		const FTransform ParentTransform = Basis->GetComponentTransform();
		Dynamic.Linear = ParentTransform.InverseTransformPositionNoScale(Dynamic.Linear);
		Dynamic.Angular = ParentTransform.InverseTransformRotation(Dynamic.Angular);
	}
	return Dynamic;
}

TSet<UTGOR_Content*> UTGOR_SocketComponent::GetActiveContent_Implementation() const
{
	TSet<UTGOR_Content*> ContentContext = Super::GetActiveContent_Implementation();

	for (const auto Pair : Attachments.Parts)
	{
		ContentContext.Emplace(Pair.Value.Socket);
	}
	return ContentContext;
}


bool UTGOR_SocketComponent::QuerySocketObjects(UTGOR_PilotComponent* Component, FTGOR_MovementVisuals& Visuals) const
{
	for (const auto Pair : Attachments.Parts)
	{
		if (Pair.Value.Component == Component)
		{
			Visuals.Blend = 1.0f;
			Visuals.Radius = Pair.Value.Socket->Radius;
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

bool UTGOR_SocketComponent::CanAttachToStatic(UTGOR_PilotComponent* Attachee, UTGOR_StaticSocket* Socket) const
{
	if (!IsValid(Socket)) return false;
	const int32 Index = GetSocketIndex(Socket);
	return !Attachments.Parts.Contains(Index) && Index != INDEX_NONE && Attachee->CanAttachTo(this) && Socket->CheckDimensions(this, Attachee);
}

void UTGOR_SocketComponent::AttachToStatic(UTGOR_PilotComponent* Attachee, UTGOR_StaticSocket* Socket, ETGOR_ComputeEnumeration& Branches)
{
	if(GetOwnerRole() != ENetRole::ROLE_Authority) return;

	if (!IsValid(Socket)) return;
	Branches = ETGOR_ComputeEnumeration::Failed;

	// Find unique identifier
	int32 Identifier;
	do
	{
		Identifier = FMath::RandRange(0, 0xFFFF);
	} while (Attachments.Parts.Contains(Identifier));

	// Create attachment
	FTGOR_StaticAttachment Attachment;
	Attachment.Component = Attachee;
	Attachment.Socket = Socket;

	// Check validity
	const FTGOR_MovementShape& Shape = Attachee->GetBody();
	const int32 Index = GetSocketIndex(Socket);
	if (Attachments.Parts.Contains(Index) || Index == INDEX_NONE || !Socket->CheckDimensions(this, Attachee)) return;

	// Register on mobility for attachment
	Attachments.Parts.Add(Identifier, Attachment);
	Attachee->RegisterHandle(this, Socket, this, Identifier);
	Branches = ETGOR_ComputeEnumeration::Success;
}


void UTGOR_SocketComponent::DetachFromStatic(UTGOR_PilotComponent* Attachee)
{
	if (GetOwnerRole() != ENetRole::ROLE_Authority) return;

	for (auto It = Attachments.Parts.CreateIterator(); It; ++It)
	{
		// Make sure not to keep track of already detached mobility
		if (It->Value.Component == Attachee)
		{
			Attachee->UnregisterHandle(It->Value.Socket);
			It.RemoveCurrent();
		}
	}
}

void UTGOR_SocketComponent::DetachFromStaticAt(FTGOR_Serial Index)
{
	if (GetOwnerRole() != ENetRole::ROLE_Authority) return;

	const FTGOR_StaticAttachment* Ptr = Attachments.Parts.Find(Index);
	if (Ptr)
	{
		Ptr->Component->UnregisterHandle(Ptr->Socket);
		Attachments.Parts.Remove(Index);
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void UTGOR_SocketComponent::RepNotifyAttachments()
{
	for (const auto Pair : Attachments.Parts)
	{
		// Force-attach if current attachment doesn't match
		if (!Pair.Value.Component->IsAttachedTo(this, Pair.Key.Index))
		{
			// Make sure no other handles with this parent exist
			Pair.Value.Component->UnregisterHandleWith(this);
			Pair.Value.Component->RegisterHandle(this, Pair.Value.Socket, this, Pair.Key.Index);
		}
	}
}
