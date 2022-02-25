// The Gateway of Realities: Planes of Existence.

#include "Realities.h"
#include "TGOR_MontageMovementComponent.h"

#include "Actors/Pawns/TGOR_MovementCharacter.h"


UTGOR_MontageMovementComponent::UTGOR_MontageMovementComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	MontageCrouch = false;
	MontageLerp = false;
	MontageSpeed = 30.0f;
	MontageDelta = FVector(0.0f, 0.0f, 0.0f);

	MontageFollowup = EMovementMode::MOVE_Walking;

	MontageFollowupTimer = 0.0f;
	_montageTimer = 0.0f;
}

void UTGOR_MontageMovementComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UTGOR_MontageMovementComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UTGOR_MontageMovementComponent::InitialiseMovementmode(EMovementMode CurrentMode, ETGOR_CustomMovementMode CustomMode)
{
	Super::InitialiseMovementmode(CurrentMode, CustomMode);

	if (IsInMontage())
	{
		SubMovementMode = ETGOR_SubMovementMode::Outside;
	}
}

bool UTGOR_MontageMovementComponent::CanCrouchInCurrentState() const
{
	if(IsInMontage() && MontageCrouch)
	{
		return(true);
	}

	return (Super::CanCrouchInCurrentState());
}


bool UTGOR_MontageMovementComponent::IsInMontage() const
{
	return(MovementMode == MOVE_Custom && CustomMovementMode == ETGOR_CustomMovementMode::Montage);
}


void UTGOR_MontageMovementComponent::CalcCustomMontageVelocity(float DeltaTime)
{
	if (MontageCrouch)
	{
		ForceCrouch();
	}

	Velocity = FVector(0.0f);

	// Set collision sphere arguments
	UpdateCollisionSphereParams(false);

	if (MontageLerp)
	{
		// Move towards target location
		FVector Location = CharacterOwner->GetActorLocation();
		FVector Delta = MontageDelta * MontageSpeed * DeltaTime;
		MontageDelta -= Delta;

		// Must be moved accurately and without collision, velocity is not an option
		CharacterOwner->SetActorLocation(Location + Delta, false, nullptr, ETeleportType::TeleportPhysics);
	}

	// Count timer up
	if (MontageFollowupTimer > 0.0f)
	{
		_montageTimer += DeltaTime;
	}

	// End montage if timer ends
	if (_montageTimer > MontageFollowupTimer)
	{
		ForceMode(MontageFollowup);
		_montageTimer = 0.0f;
	}

	// If no timer is set, end montage after animation is done
	if (!CharacterOwner->IsPlayingRootMotion() && MontageFollowupTimer == 0.0f)
	{
		ForceMode(MontageFollowup);
	}
}

void UTGOR_MontageMovementComponent::PlayMontage(UAnimMontage* Montage, float Timer, bool Crouch)
{
	MontageCrouch = Crouch;
	MontageFollowupTimer = Timer;
	CreatureCharacter->PlayAnimMontage(Montage, 1.0f);
	ForceMode(EMovementMode::MOVE_Custom, ETGOR_CustomMovementMode::Montage);

	MontageLerp = false;
}

void UTGOR_MontageMovementComponent::LerpMontage(const FVector& Delta)
{
	MontageDelta = Delta;
	MontageLerp = true;
}
