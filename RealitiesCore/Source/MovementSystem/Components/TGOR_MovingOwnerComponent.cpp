// The Gateway of Realities: Planes of Existence.


#include "TGOR_MovingOwnerComponent.h"
#include "SocketSystem/Content/TGOR_NamedSocket.h"
#include "SocketSystem/Components/TGOR_SocketComponent.h"
#include "DimensionSystem/Components/TGOR_IdentityComponent.h"
#include "DimensionSystem/Data/TGOR_WorldData.h"
#include "TGOR_MovementComponent.h"

#include "CoreSystem/TGOR_Singleton.h"


UTGOR_MovingOwnerComponent::UTGOR_MovingOwnerComponent()
	: Super()
{

}

bool UTGOR_MovingOwnerComponent::IsReady() const
{
	if (!Super::IsReady())
	{
		return false;
	}

	UTGOR_SocketComponent* Parent = GetOwnerMovement();
	if (!IsValid(Parent))
	{
		return false;
	}

	ETGOR_FetchEnumeration State;
	FName DimensionName = Parent->GetDimensionIdentifier(State);
	if (State == ETGOR_FetchEnumeration::Found)
	{
		SINGLETON_RETCHK(false);
		UTGOR_WorldData* WorldData = Singleton->GetData<UTGOR_WorldData>();
		if (IsValid(WorldData) && WorldData->IsOpen(DimensionName))
		{
			return true;
		}
	}
	return false;
}


UTGOR_SocketComponent* UTGOR_MovingOwnerComponent::GetOwnerMovement() const
{
	SINGLETON_RETCHK(nullptr);
	UTGOR_WorldData* WorldData = Singleton->GetData<UTGOR_WorldData>();
	if (IsValid(WorldData))
	{
		FTGOR_DimensionIdentifier Identifier;
		Identifier.DimensionIdentifier = DimensionIdentifier;
		Identifier.ActorIdentifier = ActorIdentifier;

		ETGOR_FetchEnumeration State;
		UTGOR_IdentityComponent* Identity = WorldData->GetIdentity(Identifier, State);
		if (IsValid(Identity))
		{
			AActor* Owner = Identity->GetOwner();
			UTGOR_SocketComponent* Parent = Cast<UTGOR_SocketComponent>(Owner->GetRootComponent());
			if (IsValid(Parent))
			{
				return Parent;
			}
		}
	}
	return nullptr;
}


void UTGOR_MovingOwnerComponent::EnterOther(UTGOR_PilotComponent* Component, ETGOR_ComputeEnumeration& Branches)
{
	Branches = ETGOR_ComputeEnumeration::Failed;
	if (!IsValid(Component))
	{
		ERROR("Invalid component to teleport", Error);
	}

	IsMidTeleport = true;

	UTGOR_SocketComponent* Parent = GetOwnerMovement();
	if (IsValid(Parent))
	{
		UTGOR_NamedSocket* Socket = Parent->GetSOfType<UTGOR_NamedSocket>(TargetSocket);
		if (IsValid(Socket))
		{
			Parent->ParentNamedSocket(Component, Socket);
		}
	}
	else
	{
		//DrawDebugCoordinateSystem(GetWorld(), Target, Rotation.Rotator(), 10.0f, false, 10.0f, 0, 5.0f);
		ERROR("Actor couldn't teleport: Owner doesn't have a compatible movement component.", Error);
	}

	IsMidTeleport = false;
}


bool UTGOR_MovingOwnerComponent::CanTeleport(UTGOR_PilotComponent* Component) const
{
	if (!IsReady()) return false;
	UTGOR_SocketComponent* Parent = GetOwnerMovement();
	if (IsValid(Parent))
	{
		UTGOR_NamedSocket* Socket = Parent->GetSOfType<UTGOR_NamedSocket>(TargetSocket);
		if (IsValid(Socket))
		{
			return Parent->CanParentNamedSocket(Component, Socket);
		}
	}

	return false;
}