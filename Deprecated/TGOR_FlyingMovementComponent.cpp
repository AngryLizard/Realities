// The Gateway of Realities: Planes of Existence.

#include "Realities.h"
#include "TGOR_FlyingMovementComponent.h"

#include "Actors/Pawns/TGOR_MovementCharacter.h"


UTGOR_FlyingMovementComponent::UTGOR_FlyingMovementComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UTGOR_FlyingMovementComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UTGOR_FlyingMovementComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UTGOR_FlyingMovementComponent::InitialiseMovementmode(EMovementMode CurrentMode, ETGOR_CustomMovementMode CustomMode)
{
	Super::InitialiseMovementmode(CurrentMode, CustomMode);

	if (CurrentMode == EMovementMode::MOVE_Flying)
	{
	}
}

bool UTGOR_FlyingMovementComponent::CanCrouchInCurrentState() const
{
	return (Super::CanCrouchInCurrentState());
}


float UTGOR_FlyingMovementComponent::CalcCustomFlyingVelocity(float DeltaTime, const FVector& InputAcceleration)
{
	ForceCrouch();

	// Set collision sphere arguments
	UpdateCollisionSphereParams();

	return(0.0f);
}