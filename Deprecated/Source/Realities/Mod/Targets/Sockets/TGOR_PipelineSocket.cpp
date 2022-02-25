// The Gateway of Realities: Planes of Existence.

#include "TGOR_PipelineSocket.h"
#include "Realities/Base/Inventory/TGOR_SocketStorage.h"
#include "Realities/Base/Inventory/Modules/TGOR_PipelineModule.h"

#include "Realities/Components/Dimension/Interactable/TGOR_InteractableComponent.h"
#include "Realities/Components/Movement/TGOR_PipelineComponent.h"

UTGOR_PipelineSocket::UTGOR_PipelineSocket()
	: Super(), Forward(true)
{
}

void UTGOR_PipelineSocket::BuildStorage(UTGOR_SocketStorage* Storage)
{
	Super::BuildStorage(Storage);

	UTGOR_PipelineModule* Module = Storage->GetModule<UTGOR_PipelineModule>();
	Module->SetSocketDirection(Forward);
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

UTGOR_MobilityComponent* UTGOR_PipelineSocket::QuerySocketWith(USceneComponent* Component, FTGOR_MovementPosition& Position) const
{
	UTGOR_PipelineComponent* Mobility = Cast<UTGOR_PipelineComponent>(Component);
	if (IsValid(Mobility))
	{
		Position = Mobility->GetTransformOfPipeline(const_cast<UTGOR_PipelineSocket*>(this));
	}
	return Mobility;
}

bool UTGOR_PipelineSocket::AttachToSocket(UTGOR_MobilityComponent* Parent, UTGOR_PilotComponent* Component)
{
	UTGOR_PipelineComponent* PipelineParent = Cast<UTGOR_PipelineComponent>(Parent);
	if (IsValid(PipelineParent))
	{
		ETGOR_ComputeEnumeration State;
		PipelineParent->AttachToPipeline(Component, this, State);
		return State == ETGOR_ComputeEnumeration::Success;
	}
	return Super::AttachToSocket(Parent, Component);
}

bool UTGOR_PipelineSocket::CanAttachToSocket(UTGOR_MobilityComponent* Parent, UTGOR_PilotComponent* Component) const
{
	UTGOR_PipelineComponent* PipelineParent = Cast<UTGOR_PipelineComponent>(Parent);
	if (IsValid(PipelineParent))
	{
		return PipelineParent->CanAttachToPipeline(Component, const_cast<UTGOR_PipelineSocket*>(this));
	}

	return Super::CanAttachToSocket(Parent, Component);
}

bool UTGOR_PipelineSocket::CanFitToSocket(UTGOR_MobilityComponent* Parent, const FTGOR_MovementShape& Shape) const
{
	UTGOR_PipelineComponent* PipelineParent = Cast<UTGOR_PipelineComponent>(Parent);
	if (IsValid(PipelineParent))
	{
		return PipelineParent->CanFitToPipeline(Shape, const_cast<UTGOR_PipelineSocket*>(this));
	}

	return Super::CanFitToSocket(Parent, Shape);
}


////////////////////////////////////////////////////////////////////////////////////////////////////

void UTGOR_PipelineSocket::Tick(UTGOR_DimensionComponent* Owner, UTGOR_SocketStorage* Storage, float DeltaSeconds)
{

}

UTGOR_MobilityComponent* UTGOR_PipelineSocket::GetParent(UTGOR_SocketStorage* Storage) const
{
	UTGOR_PipelineModule* Module = Storage->GetModule<UTGOR_PipelineModule>();
	if (IsValid(Module))
	{
		return Module->GetParent();
	}
	return nullptr;
}

FTGOR_MovementDynamic UTGOR_PipelineSocket::GetTransform(UTGOR_MobilityComponent* Owner, UTGOR_SocketStorage* Storage) const
{
	FTGOR_MovementDynamic Dynamic;
	UTGOR_PipelineModule* Module = Storage->GetModule<UTGOR_PipelineModule>();
	if (IsValid(Module))
	{
		return Module->GetDynamic();
	}
	return Dynamic;
}

////////////////////////////////////////////////////////////////////////////////////////////////////