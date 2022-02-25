// The Gateway of Realities: Planes of Existence.

#include "Realities.h"
#include "TGOR_FloatingMovementComponent.h"

UTGOR_FloatingMovementComponent::UTGOR_FloatingMovementComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	MaxFlySpeed = 2000.0f;
	BrakingDecelerationFlying = MaxFlySpeed;
	DefaultWaterMovementMode = EMovementMode::MOVE_Flying;
	DefaultLandMovementMode = EMovementMode::MOVE_Flying;
}

void UTGOR_FloatingMovementComponent::InitializeComponent()
{
	Super::InitializeComponent();
}

void UTGOR_FloatingMovementComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}
