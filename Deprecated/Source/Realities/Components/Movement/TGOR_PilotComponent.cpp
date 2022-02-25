// The Gateway of Realities: Planes of Existence.
#include "TGOR_PilotComponent.h"

#include "Realities/Base/TGOR_Singleton.h"
#include "Realities/Base/System/Data/TGOR_WorldData.h"
#include "Realities/Actors/Dimension/Volumes/TGOR_PhysicsVolume.h"
#include "Realities/Mod/Targets/Sockets/TGOR_NamedSocket.h"
#include "Realities/Utility/Storage/TGOR_Package.h"

#include "Realities/Base/Content/TGOR_ContentManager.h"
#include "Net/UnrealNetwork.h"


UTGOR_PilotComponent::UTGOR_PilotComponent()
:	Super(),
	NetDeltaThreshold(80.0, PI / 2)
{
	BaseComponent = this;
}


void UTGOR_PilotComponent::BeginPlay()
{
	Super::BeginPlay();


	AActor* Actor = GetOwner();
	if (IsValid(Actor))
	{
		SetBaseComponent(Actor->GetRootComponent());
	}
	else
	{
		SetBaseComponent(this);
	}
}


void UTGOR_PilotComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UTGOR_PilotComponent::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}

USceneComponent* UTGOR_PilotComponent::GetBaseComponent() const
{
	return BaseComponent.Get();
}

void UTGOR_PilotComponent::SetBaseComponent(USceneComponent* Component)
{
	BaseComponent = Component;
	ResetToComponent();
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void UTGOR_PilotComponent::SetNetSmooth(FVector Delta, FRotator Rotation)
{
	const FTGOR_MovementPosition Space = ComputeSpace();
	FTGOR_MovementPosition Position;
	NetSmoothDelta.Linear += Space.Linear + Delta;
	NetSmoothDelta.Angular = Space.Angular * Rotation.Quaternion();
}

void UTGOR_PilotComponent::NetSmooth(float Timestep, float Speed)
{
	NetSmoothDelta.Linear = FMath::VInterpTo(NetSmoothDelta.Linear, FVector::ZeroVector, Timestep, Speed);
	NetSmoothDelta.Angular = FMath::QInterpTo(NetSmoothDelta.Angular, FQuat::Identity, Timestep, Speed);
}

void UTGOR_PilotComponent::SetComponentFromSmoothDelta(USceneComponent* Component, const FTransform& InitialWorld)
{
	const FTransform Translate = FTransform(NetSmoothDelta.Linear);
	const FTransform Rotation = FTransform(NetSmoothDelta.Angular);
	Component->SetWorldTransform(Rotation * InitialWorld * Translate);
}

void UTGOR_PilotComponent::MovementAdjust(const FTGOR_MovementBase& Old, const FTGOR_MovementBase& New)
{
	const FTGOR_MovementPosition OldSpace = Old.ComputeSpace();
	const FTGOR_MovementPosition NewSpace = New.ComputeSpace();
	NetSmoothDelta.Linear += OldSpace.Linear - NewSpace.Linear;
	NetSmoothDelta.Angular = (OldSpace.Angular * NewSpace.Angular.Inverse()) * NetSmoothDelta.Angular;

	// Reset if error is too high
	if (NetSmoothDelta.Compare(FTGOR_MovementPosition(), NetDeltaThreshold) > 1.0f)
	{
		NetSmoothDelta = FTGOR_MovementPosition();
	}
}

USceneComponent* UTGOR_PilotComponent::GetMovementBasis() const
{
	return BaseComponent.Get();
}

////////////////////////////////////////////////////////////////////////////////////////////////////

bool UTGOR_PilotComponent::Teleport(const FTGOR_MovementDynamic& Dynamic)
{
	const FTGOR_MovementDynamic OldDynamic = ComputeSpace();
	SimulateDynamic(Dynamic, nullptr, -1);

	SetComponentPosition(Dynamic);
	OnTeleportedMovement.Broadcast(OldDynamic, Dynamic);
	return true;
}