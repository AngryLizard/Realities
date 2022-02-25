// The Gateway of Realities: Planes of Existence.

#include "Realities.h"
#include "TGOR_SkiingMovementComponent.h"

#include "Actors/Pawns/TGOR_MovementCharacter.h"




UTGOR_SkiingMovementComponent::UTGOR_SkiingMovementComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UTGOR_SkiingMovementComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UTGOR_SkiingMovementComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UTGOR_SkiingMovementComponent::InitialiseMovementmode(EMovementMode CurrentMode, ETGOR_CustomMovementMode CustomMode)
{
	Super::InitialiseMovementmode(CurrentMode, CustomMode);
	
	if (IsSkiing())
	{
		SubMovementMode = ETGOR_SubMovementMode::Outside;
	}
}

bool UTGOR_SkiingMovementComponent::CanCrouchInCurrentState() const
{
	return(Super::CanCrouchInCurrentState());
}

bool UTGOR_SkiingMovementComponent::IsSkiing() const
{
	return(MovementMode == MOVE_Custom && CustomMovementMode == ETGOR_CustomMovementMode::Skiing);
}


float UTGOR_SkiingMovementComponent::CalcCustomSkiingVelocity(float DeltaTime, const FVector& InputAcceleration)
{
	// Set collision sphere arguments
	UpdateCollisionSphereParams();

	return(0.0f);
}