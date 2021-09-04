// The Gateway of Realities: Planes of Existence.


#include "TGOR_DimensionSenderSocketComponent.h"
#include "DimensionSystem/Data/TGOR_DimensionData.h"
#include "SocketSystem/Components/TGOR_SocketComponent.h"
#include "SocketSystem/Content/TGOR_NamedSocket.h"

#include "CoreSystem/TGOR_Singleton.h"

UTGOR_DimensionSenderSocketComponent::UTGOR_DimensionSenderSocketComponent()
:	Super()
{
	//ConnectionName = "Socket";
	Parent = nullptr;
	//TargetSocket = UTGOR_Socket::StaticClass();
}

void UTGOR_DimensionSenderSocketComponent::BeginPlay()
{
	Super::BeginPlay();

	AActor* Actor = GetOwner();
	Parent = Actor->FindComponentByClass<UTGOR_SocketComponent>();
}

void UTGOR_DimensionSenderSocketComponent::TeleportSocket(TSubclassOf<UTGOR_NamedSocket> SourceSocket, ETGOR_ComputeEnumeration& Branches)
{
	Branches = ETGOR_ComputeEnumeration::Failed;
	if (!IsValid(Parent)) return;

	const TArray<UTGOR_PilotComponent*> Objects = Parent->GetAttachedList(SourceSocket);
	for (UTGOR_PilotComponent* Object : Objects)
	{
		EnterOther(Object, false, FTransform(), Branches);
		if (Branches == ETGOR_ComputeEnumeration::Failed) return;
	}
}
