// The Gateway of Realities: Planes of Existence.


#include "TGOR_DimensionSocketComponent.h"
#include "Realities/Base/System/Data/TGOR_DimensionData.h"
#include "Realities/Components/Movement/TGOR_MobilityComponent.h"
#include "Realities/Mod/Targets/Sockets/TGOR_Socket.h"

#include "Realities/Base/TGOR_Singleton.h"
#include "Realities/Actors/Dimension/TGOR_GateActor.h"
#include "Net/UnrealNetwork.h"

UTGOR_DimensionSocketComponent::UTGOR_DimensionSocketComponent()
:	Super()
{
	PortalName = "Socket";
	Parent = nullptr;
	TargetSocket = UTGOR_Socket::StaticClass();
}

void UTGOR_DimensionSocketComponent::BeginPlay()
{
	Super::BeginPlay();

	AActor* Actor = GetOwner();
	Parent = Actor->FindComponentByClass<UTGOR_MobilityComponent>();

}

void UTGOR_DimensionSocketComponent::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	//DOREPLIFETIME_CONDITION(UTGOR_DimensionGateComponent, CurrentState, COND_None);
}

void UTGOR_DimensionSocketComponent::EnterSelf(UTGOR_PilotComponent* Component, const FTransform& Offset, ETGOR_ComputeEnumeration& Branches)
{
	Branches = ETGOR_ComputeEnumeration::Failed;
	if (!IsValid(Parent)) return;

	UTGOR_Socket* Socket = Parent->GetSocket(TargetSocket);
	if (IsValid(Socket))
	{
		IsMidTeleport = true;
		if (Socket->AttachToSocket(Parent, Component))
		{
			Branches = ETGOR_ComputeEnumeration::Success;
		}

		OnTeleportedComponent.Broadcast(Component);
		IsMidTeleport = false;
	}
}

bool UTGOR_DimensionSocketComponent::IsOccupied(UTGOR_PilotComponent* Component) const
{
	if (!IsValid(Parent)) return true;
	UTGOR_Socket* Socket = Parent->GetSocket(TargetSocket);
	return Super::IsOccupied(Component) || !Socket->CanAttachToSocket(Parent, Component);
}


void UTGOR_DimensionSocketComponent::TeleportSocket(TSubclassOf<UTGOR_Socket> SourceSocket, ETGOR_ComputeEnumeration& Branches)
{
	Branches = ETGOR_ComputeEnumeration::Failed;
	if (!IsValid(Parent)) return;

	const TArray<UTGOR_PilotComponent*> Objects = Parent->GetSocketObjects(SourceSocket);
	for (UTGOR_PilotComponent* Object : Objects)
	{
		EnterOther(Object, false, FTransform(), Branches);
		if (Branches == ETGOR_ComputeEnumeration::Failed) return;
	}
}
