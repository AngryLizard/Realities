// The Gateway of Realities: Planes of Existence.

#include "TGOR_CustomMovementComponent.h"
#include "Components/PrimitiveComponent.h"
#include "GameFramework/Actor.h"
#include "Net/UnrealNetwork.h"



UTGOR_CustomMovementComponent::UTGOR_CustomMovementComponent()
:	Super()
{
	SlaveMode = ENetRole::ROLE_SimulatedProxy;
	MasterMode = ENetRole::ROLE_Authority;
	AuthoritiveSlave = false;
}

void UTGOR_CustomMovementComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

}

void UTGOR_CustomMovementComponent::PreComputePhysics(const FTGOR_MovementTick& Tick, bool Replay)
{
	OnPreComputePhysics(Tick, Replay);
}

void UTGOR_CustomMovementComponent::ComputePhysics(const FTGOR_MovementTick& Tick, const FTGOR_MovementSpace& Space, bool Replay, FTGOR_MovementOutput& Output)
{
	OnComputePhysics(Tick, Output, Space, Replay, Output);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void UTGOR_CustomMovementComponent::LoadFromBufferExternal(int32 Index)
{
	Super::LoadFromBufferExternal(Index);

	OnLoadFromBufferExternal(Index);
}

void UTGOR_CustomMovementComponent::LoadFromBufferWhole(int32 Index)
{
	Super::LoadFromBufferWhole(Index);

	OnLoadFromBufferWhole(Index);
}

void UTGOR_CustomMovementComponent::LerpToBuffer(int32 PrevIndex, int32 NextIndex, float Alpha)
{
	Super::LerpToBuffer(PrevIndex, NextIndex, Alpha);

	OnLerpToBuffer(PrevIndex, NextIndex, Alpha);
}

void UTGOR_CustomMovementComponent::StoreToBuffer(int32 Index)
{
	Super::StoreToBuffer(Index);

	OnStoreToBuffer(Index);
}

void UTGOR_CustomMovementComponent::SetBufferSize(int32 Size)
{
	Super::SetBufferSize(Size);

	OnSetBufferSize(Size);
}
