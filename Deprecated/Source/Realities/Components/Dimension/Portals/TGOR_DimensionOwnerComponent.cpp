// The Gateway of Realities: Planes of Existence.


#include "TGOR_DimensionOwnerComponent.h"

#include "Realities/Base/System/Data/TGOR_DimensionData.h"
#include "Realities/Components/Movement/TGOR_MovementComponent.h"
#include "Realities/Mod/Targets/Sockets/TGOR_NamedSocket.h"

#include "Realities/Base/TGOR_Singleton.h"
#include "Realities/Actors/Dimension/TGOR_GateActor.h"
#include "Net/UnrealNetwork.h"

UTGOR_DimensionOwnerComponent::UTGOR_DimensionOwnerComponent()
:	Super()
{
}

void UTGOR_DimensionOwnerComponent::BeginPlay()
{
	Super::BeginPlay();
}


bool UTGOR_DimensionOwnerComponent::Assemble(UTGOR_DimensionData* Dimension)
{
	const bool Success = Super::Assemble(Dimension);
	DialOwner();

	return Success;
}

bool UTGOR_DimensionOwnerComponent::IsReady() const
{
	UTGOR_MovementComponent* Parent = GetOwnerMovement();
	if (!IsValid(Singleton) || !IsValid(Parent)) return false;

	// Don't allow teleportation during an ongoing teleportation
	if (!IsMidTeleport)
	{
		ETGOR_FetchEnumeration State;
		FName DimensionName = Parent->GetDimensionIdentifier(State);
		if (State == ETGOR_FetchEnumeration::Found)
		{
			UTGOR_WorldData* WorldData = Singleton->GetWorldData();
			if (WorldData->IsOpen(DimensionName))
			{
				return true;
			}
		}
	}
	return false;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void UTGOR_DimensionOwnerComponent::DialOwner()
{
	SINGLETON_CHK;
	DIMENSIONBASE_CHK;

	UTGOR_WorldData* WorldData = Singleton->GetWorldData();

	FTGOR_DimensionIdentifier Identifier = WorldData->GetOwner(DimensionData->GetIdentifier());
	DimensionIdentifier = Identifier.DimensionIdentifier;
	ActorIdentifier = Identifier.ActorIdentifier;
}

UTGOR_MovementComponent* UTGOR_DimensionOwnerComponent::GetOwnerMovement() const
{
	if (!IsValid(Singleton)) return nullptr;
	UTGOR_WorldData* WorldData = Singleton->GetWorldData();

	FTGOR_DimensionIdentifier Identifier;
	Identifier.DimensionIdentifier = DimensionIdentifier;
	Identifier.ActorIdentifier = ActorIdentifier;

	ETGOR_FetchEnumeration State;
	UTGOR_IdentityComponent* Identity = WorldData->GetIdentity(Identifier, State);
	if (IsValid(Identity))
	{
		UTGOR_MovementComponent* Parent = Identity->GetOwnerComponent<UTGOR_MovementComponent>();
		if (IsValid(Parent))
		{
			return Parent;
		}
	}
	return nullptr;
}


void UTGOR_DimensionOwnerComponent::EnterOther(UTGOR_PilotComponent* Component, ETGOR_ComputeEnumeration& Branches)
{
	Branches = ETGOR_ComputeEnumeration::Failed;
	if (!IsValid(Component))
	{
		ERROR("Invalid component to teleport", Error);
	}

	IsMidTeleport = true;

	UTGOR_MovementComponent* Parent = GetOwnerMovement();
	if (IsValid(Parent))
	{
		UTGOR_NamedSocket* Socket = Parent->GetSOfType<UTGOR_NamedSocket>(TargetSocket);
		if (IsValid(Socket))
		{
			Parent->AttachToSocket(Component, Socket, Branches);
		}
	}
	else
	{
		//DrawDebugCoordinateSystem(GetWorld(), Target, Rotation.Rotator(), 10.0f, false, 10.0f, 0, 5.0f);
		ERROR("Actor couldn't teleport: Owner doesn't have a compatible movement component.", Error);
	}
	
	IsMidTeleport = false;
}


bool UTGOR_DimensionOwnerComponent::CanTeleport(UTGOR_PilotComponent* Component) const
{
	if (!IsReady()) return false;
	UTGOR_MovementComponent* Parent = GetOwnerMovement();
	if (IsValid(Parent))
	{
		UTGOR_NamedSocket* Socket = Parent->GetSOfType<UTGOR_NamedSocket>(TargetSocket);
		if (IsValid(Socket))
		{
			return Parent->CanAttachToSocket(Component, Socket);
		}
	}

	return false;
}