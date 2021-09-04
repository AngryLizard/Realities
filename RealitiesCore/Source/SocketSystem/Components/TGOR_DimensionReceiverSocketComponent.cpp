// The Gateway of Realities: Planes of Existence.


#include "TGOR_DimensionReceiverSocketComponent.h"
#include "DimensionSystem/Data/TGOR_DimensionData.h"
#include "SocketSystem/Components/TGOR_SocketComponent.h"
#include "SocketSystem/Content/TGOR_NamedSocket.h"

#include "CoreSystem/TGOR_Singleton.h"

UTGOR_DimensionReceiverSocketComponent::UTGOR_DimensionReceiverSocketComponent()
:	Super()
{
	ConnectionName = "Socket";
	Parent = nullptr;
	TargetSocket = UTGOR_NamedSocket::StaticClass();
}

void UTGOR_DimensionReceiverSocketComponent::BeginPlay()
{
	Super::BeginPlay();

	AActor* Actor = GetOwner();
	Parent = Actor->FindComponentByClass<UTGOR_SocketComponent>();
}

void UTGOR_DimensionReceiverSocketComponent::EnterSelf(UTGOR_PilotComponent* Component, const FTransform& Offset, ETGOR_ComputeEnumeration& Branches)
{
	Branches = ETGOR_ComputeEnumeration::Failed;
	if (!IsValid(Parent)) return;

	UTGOR_NamedSocket* Socket = Parent->GetSOfType<UTGOR_NamedSocket>(TargetSocket);
	if (IsValid(Socket))
	{
		IsMidTeleport = true;
		if (Parent->ParentNamedSocket(Component, Socket))
		{
			Branches = ETGOR_ComputeEnumeration::Success;
		}

		OnReceivedComponent.Broadcast(Component);
		IsMidTeleport = false;
	}
}

bool UTGOR_DimensionReceiverSocketComponent::IsOccupied(UTGOR_PilotComponent* Component) const
{
	if (!IsValid(Parent)) return true;
	UTGOR_NamedSocket* Socket = Parent->GetSOfType<UTGOR_NamedSocket>(TargetSocket);

	if (!IsValid(Socket)) return true;
	return Super::IsOccupied(Component) || !Parent->CanParentNamedSocket(Component, Socket);
}