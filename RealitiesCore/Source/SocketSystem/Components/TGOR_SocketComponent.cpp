// The Gateway of Realities: Planes of Existence.
#include "TGOR_SocketComponent.h"
#include "DimensionSystem/Components/TGOR_PilotComponent.h"

#include "CoreSystem/TGOR_Singleton.h"
#include "SocketSystem/Tasks/TGOR_NamedSocketTask.h"
#include "AttributeSystem/Components/TGOR_AttributeComponent.h"
#include "SocketSystem/Content/TGOR_NamedSocket.h"
#include "AttributeSystem/Content/TGOR_Attribute.h"
#include "CoreSystem/Storage/TGOR_Package.h"
#include "DimensionSystem/Content/TGOR_Spawner.h"

#include "RealitiesUGC/Mod/TGOR_ContentManager.h"
#include "Net/UnrealNetwork.h"



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
}

void UTGOR_SocketComponent::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

}

void UTGOR_SocketComponent::UpdateContent_Implementation(FTGOR_SpawnerDependencies& Dependencies)
{
	Super::UpdateContent_Implementation(Dependencies);

	Attachments.Empty();

	// Get sockets from spawner
	const TArray<TObjectPtr<UTGOR_NamedSocket>> Sockets = Dependencies.Spawner->GetMListFromType<UTGOR_NamedSocket>(SpawnSockets);
	for (UTGOR_NamedSocket* Socket : Sockets)
	{
		Attachments.Emplace(Socket, TWeakObjectPtr<UTGOR_NamedSocketTask>());
	}

	UpdatePrimitiveCache();
}

TMap<int32, UTGOR_SpawnModule*> UTGOR_SocketComponent::GetModuleType_Implementation() const
{
	TMap<int32, UTGOR_SpawnModule*> Modules;

	for (const auto& Pair : Attachments)
	{
		Modules.Emplace(Pair.Key->GetStorageIndex(), Pair.Key);
	}
	return Modules;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

FTGOR_MovementPosition UTGOR_SocketComponent::GetTransformOfSocket(const FName& Socket) const
{
	const TWeakObjectPtr<UPrimitiveComponent>* Ptr = PrimitiveCache.Find(Socket);
	if (Ptr)
	{
		if ((*Ptr)->DoesSocketExist(Socket))
		{
			const FTransform& BoneTransform = (*Ptr)->GetSocketTransform(Socket, ERelativeTransformSpace::RTS_World);

			FTGOR_MovementPosition Position;
			Position.Linear = BoneTransform.GetLocation();
			Position.Angular = BoneTransform.GetRotation();
			return Position;
		}
	}
	return ComputePosition();
}

bool UTGOR_SocketComponent::CanFitToSocket(const FTGOR_MovementShape& Shape, UTGOR_NamedSocket* Socket) const
{
	if (!IsValid(Socket) || !PrimitiveCache.Contains(Socket->SocketName) || !Attachments.Contains(Socket)) return false;
	return !Attachments[Socket].IsValid() && Socket->CheckDimensions(this, Shape);
}

bool UTGOR_SocketComponent::CanAttachToSocket(UTGOR_PilotComponent* Attachee, UTGOR_NamedSocket* Socket) const
{
	if (!IsValid(Attachee)) return false;
	return CanFitToSocket(Attachee->GetBody(), Socket) && Attachee->CanParent(this);
}

bool UTGOR_SocketComponent::IsAttachedToSocket(UTGOR_PilotComponent* Attachee, UTGOR_NamedSocket* Socket) const
{
	return GetAttached(Socket) == Attachee;
}

UTGOR_NamedSocket* UTGOR_SocketComponent::GetAttachedSocket(UTGOR_PilotComponent* Attachee) const
{
	for (const auto& Pair : Attachments)
	{
		if (Pair.Value.IsValid() && Pair.Value->Identifier.Component == Attachee)
		{
			return Pair.Key;
		}
	}
	return nullptr;
}

UTGOR_NamedSocket* UTGOR_SocketComponent::GetTargetedSocket(UTGOR_Target* Target) const
{
	for (const auto& Pair : Attachments)
	{
		if (Pair.Key->Instanced_TargetInsertions.Contains(Target))
		{
			return Pair.Key;
		}
	}
	return nullptr;
}

UTGOR_PilotComponent* UTGOR_SocketComponent::GetAttached(UTGOR_NamedSocket* Socket) const
{
	if (!IsValid(Socket)) return nullptr;
	const TWeakObjectPtr<UTGOR_NamedSocketTask>* Ptr = Attachments.Find(Socket);
	if (Ptr && Ptr->IsValid())
	{
		return (*Ptr)->Identifier.Component;
	}
	return nullptr;
}

TArray<UTGOR_PilotComponent*> UTGOR_SocketComponent::GetAttachedList(TSubclassOf<UTGOR_NamedSocket> Type) const
{
	TArray<UTGOR_PilotComponent*> List;
	if (*Type)
	{
		for (const auto& Pair : Attachments)
		{
			if (Pair.Key->IsA(Type) && Pair.Value.IsValid())
			{
				List.Emplace(Pair.Value->Identifier.Component);
			}
		}
	}
	return List;
}

UTGOR_NamedSocket* UTGOR_SocketComponent::GetSocketOfType(TSubclassOf<UTGOR_NamedSocket> Type) const
{
	if (*Type)
	{
		for (const auto& Pair : Attachments)
		{
			if (Pair.Key->IsA(Type))
			{
				return Pair.Key;
			}
		}
	}
	return nullptr;
}

TArray<UTGOR_NamedSocket*> UTGOR_SocketComponent::GetSocketListOfType(TSubclassOf<UTGOR_NamedSocket> Type) const
{
	TArray<UTGOR_NamedSocket*> List;
	if (*Type)
	{
		for (const auto& Pair : Attachments)
		{
			if (Pair.Key->IsA(Type))
			{
				List.Emplace(Pair.Key);
			}
		}
	}
	return List;
}

FTGOR_MovementShape UTGOR_SocketComponent::GetDisplacedShape(UTGOR_NamedSocket* Socket) const
{
	UTGOR_PilotComponent* Attachee = GetAttached(Socket);
	if (IsValid(Attachee))
	{
		return Attachee->GetBody();
	}
	return FTGOR_MovementShape();
}

void UTGOR_SocketComponent::SetSocketVisuals(UTGOR_NamedSocket* Socket, float Radius, float Blend)
{
	if (!IsValid(Socket)) return;
	const TWeakObjectPtr<UTGOR_NamedSocketTask>* Ptr = Attachments.Find(Socket);
	if (Ptr && Ptr->IsValid())
	{
		(*Ptr)->Radius = Radius;
		(*Ptr)->Blend = Blend;
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////

bool UTGOR_SocketComponent::ParentNamedSocket(UTGOR_PilotComponent* Attachee, UTGOR_NamedSocket* Socket)
{
	if (IsValid(Attachee))
	{
		TArray<UTGOR_NamedSocketTask*> Tasks = Attachee->GetPListOfType<UTGOR_NamedSocketTask>();
		for (UTGOR_NamedSocketTask* Task : Tasks)
		{
			if (IsValid(Task) && Task->CanParent(this, Socket))
			{
				Task->Parent(this, Socket);
				Attachee->AttachWith(Task->Identifier.Slot);
				return true;
			}
		}
	}
	return false;
}

bool UTGOR_SocketComponent::CanParentNamedSocket(UTGOR_PilotComponent* Attachee, UTGOR_NamedSocket* Socket) const
{
	if (IsValid(Attachee))
	{
		TArray<UTGOR_NamedSocketTask*> Tasks = Attachee->GetPListOfType<UTGOR_NamedSocketTask>();
		for (UTGOR_NamedSocketTask* Task : Tasks)
		{
			if (IsValid(Task) && Task->CanParent(this, Socket))
			{
				return true;
			}
		}
	}
	return nullptr;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void UTGOR_SocketComponent::UpdatePrimitiveCache()
{
	PrimitiveCache.Empty();

	TArray<UPrimitiveComponent*> Components = GetOwnerComponents<UPrimitiveComponent>();
	for (UPrimitiveComponent* Component : Components)
	{
		for (const auto& Pair : Attachments)
		{
			if (Component->DoesSocketExist(Pair.Key->SocketName))
			{
				PrimitiveCache.Emplace(Pair.Key->SocketName, Component);
			}
		}
	}

	UpdateCurrentAttachments();
}

void UTGOR_SocketComponent::UpdateCurrentAttachments()
{
	if (GetOwnerRole() != ENetRole::ROLE_Authority) return;
	for (auto& Pair : Attachments)
	{
		if (Pair.Value.IsValid())
		{
			if (!Pair.Key->CheckDimensions(this, Pair.Value->Identifier.Component->GetBody()))
			{
				ParentLinear(Pair.Value->Identifier.Component, INDEX_NONE, Pair.Value->Identifier.Component->ComputeSpace());
				Pair.Value.Reset();
				OnSocketChanged.Broadcast(Pair.Key);
			}
		}
		else
		{
			Pair.Value.Reset();
		}
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void UTGOR_SocketComponent::RegisterAttachToSocket(UTGOR_NamedSocketTask* Attachee, UTGOR_NamedSocket* Socket)
{
	if (IsValid(Socket) && Attachments.Contains(Socket))
	{
		Attachments[Socket] = Attachee;
		OnSocketChanged.Broadcast(Socket);
	}
}

void UTGOR_SocketComponent::RegisterDetachFromSocket(UTGOR_NamedSocketTask* Attachee, UTGOR_NamedSocket* Socket)
{
	TWeakObjectPtr<UTGOR_NamedSocketTask>* Ptr = Attachments.Find(Socket);
	if (Ptr && Ptr->Get() == Attachee)
	{
		Attachments[Socket].Reset();
		OnSocketChanged.Broadcast(Socket);
	}
}