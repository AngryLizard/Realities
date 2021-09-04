// The Gateway of Realities: Planes of Existence.

#include "TGOR_PipelineSocket.h"

#include "TargetSystem/Components/TGOR_InteractableComponent.h"
#include "SocketSystem/Components/TGOR_PipelineComponent.h"
#include "DimensionSystem/Components/TGOR_PilotComponent.h"

UTGOR_PipelineSocket::UTGOR_PipelineSocket()
	: Super(), Forward(true)
{
	AllowsReparent = false;
}

UTGOR_MobilityComponent* UTGOR_PipelineSocket::QuerySocket(UTGOR_InteractableComponent* Component, FTGOR_MovementPosition& Position) const
{
	AActor* Owner = Component->GetOwner();
	UTGOR_PipelineComponent* Mobility = Owner->FindComponentByClass<UTGOR_PipelineComponent>();
	if (IsValid(Mobility))
	{
		Position = Mobility->GetTransformOfPipeline(const_cast<UTGOR_PipelineSocket*>(this));
	}
	return Mobility;
}

UTGOR_MobilityComponent* UTGOR_PipelineSocket::QuerySocketWith(UActorComponent* Component, FTGOR_MovementPosition& Position) const
{
	UTGOR_PipelineComponent* Mobility = Cast<UTGOR_PipelineComponent>(Component);
	if (IsValid(Mobility))
	{
		Position = Mobility->GetTransformOfPipeline(const_cast<UTGOR_PipelineSocket*>(this));
	}
	return Mobility;
}
