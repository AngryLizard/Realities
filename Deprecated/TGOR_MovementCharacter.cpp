///////////////////////////////////////////////////
// The Gateway of Realities: Planes of Existence //
// Copyright 2016-2017                           //
///////////////////////////////////////////////////

#include "Realities.h"
#include "TGOR_MovementCharacter.h"

#include "Components/Movement/TGOR_ReplicatedMovementComponent.h"
#include "Components/Creature/TGOR_SimulationComponent.h"
#include "Components/Combat/Stat/TGOR_StaminaComponent.h"
#include "Components/Combat/Stat/TGOR_HealthComponent.h"

#include "Kismet/KismetMathLibrary.h"
#include "UnrealNetwork.h"

ATGOR_MovementCharacter::ATGOR_MovementCharacter(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass<UTGOR_ReplicatedMovementComponent>(ACharacter::CharacterMovementComponentName))
{
	ReplicatedInput = FVector(0.0f);
	
	SimulationComponent = ObjectInitializer.CreateDefaultSubobject<UTGOR_SimulationComponent>(this, TEXT("SimulationComponent"));
	SimulationComponent->bActiveRegionComponent = true;
	
	bUseControllerRotationYaw = false;
	JumpMaxHoldTime = 0.1f;
	JumpMaxCount = 4;

	InputStrength = 1.0f;
	InputForward = 0.0f;
	InputUpward = 0.0f;
	InputRight = 0.0f;

	_desyncsThisTick = 0;

	InputBase = ETGOR_InputBaseEnumeration::WorldSpace;

	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;
}

void ATGOR_MovementCharacter::PostInitializeComponents()
{
	CreatureMovementComponent = Cast<UTGOR_ReplicatedMovementComponent>(GetMovementComponent());

	Super::PostInitializeComponents();
}

void ATGOR_MovementCharacter::BeginPlay()
{
	Super::BeginPlay();
}

void ATGOR_MovementCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	_desyncsThisTick = 0;

	ApplyMovement();

	if (Role >= ROLE_AutonomousProxy)
	{
		EMovementMode Mode = CreatureMovementComponent->MovementMode;
		ETGOR_CustomMovementMode Custom = (ETGOR_CustomMovementMode)CreatureMovementComponent->CustomMovementMode;
		HealthComponent->EnableStatRegen = CanRegenHealth(Mode, Custom);
		//VitalsComponent->EnableEnergyRegen = CanRegenEnergy(Mode, Custom);
		StaminaComponent->EnableStatRegen = CanRegenStamina(Mode, Custom);

		if (CreatureMovementComponent->SubMovementMode == ETGOR_SubMovementMode::MoveFast)
		{
			if (IsAggressive)
			{
				SetIsAggressive(false);
			}

			SimulationComponent->Exhaust(CreatureMovementComponent->ExhaustStaminaRate * DeltaTime, true);
		}
	}
}


bool ATGOR_MovementCharacter::CanJumpInternal_Implementation() const
{
	if (CreatureMovementComponent->IsSwimming())
	{
		return(false);
	}

	if (HealthComponent->HealthState != ETGOR_HealthState::Normal)
	{
		return(false);
	}

	if (Super::CanJumpInternal_Implementation())
	{
		return(true);
	}
	
	return(CreatureMovementComponent->IsClimbing());
}

void ATGOR_MovementCharacter::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// Replicate to everyone
	DOREPLIFETIME_CONDITION(ATGOR_MovementCharacter, ReplicatedInput, COND_SimulatedOnly);
}


void ATGOR_MovementCharacter::GetInputBase(FVector& Forward, FVector& Right, FVector& Up)
{
	const FRotator Control = GetControlRotation();
	const FRotator Plane = FRotator(0.0f, Control.Yaw, 0.0f);

	switch (InputBase)
	{
	case ETGOR_InputBaseEnumeration::WorldSpace :	
		Forward = UKismetMathLibrary::GetForwardVector(Control);
		Right = UKismetMathLibrary::GetRightVector(Control);
		Up = UKismetMathLibrary::GetUpVector(Control);
		return;

	case ETGOR_InputBaseEnumeration::PlaneSpace :
		Forward = UKismetMathLibrary::GetForwardVector(Plane);
		Right = UKismetMathLibrary::GetRightVector(Plane);
		Up = FVector(0.0f, 0.0f, 1.0f);
		return;

	case ETGOR_InputBaseEnumeration::LocalSpace :
		Forward = GetActorForwardVector();
		Right = GetActorRightVector();
		Up = GetActorUpVector();
		return;

	case ETGOR_InputBaseEnumeration::WallSpace :
		Forward = CreatureMovementComponent->WallSurfaceUp;
		Right = CreatureMovementComponent->WallSurfaceRight;
		Up  = FVector(0.0f, 0.0f, 0.0f);
		return;

	default:
		Forward = FVector(1.0, 0.0f, 0.0f);
		Right = FVector(0.0f, 1.0, 0.0f);
		Up = FVector(0.0f, 0.0f, 1.0f);
	}
}


void ATGOR_MovementCharacter::PlayMontage(UAnimMontage* Montage, float Timer, bool Crouch)
{
	if (!IsValid(CreatureMovementComponent))
	{
		CreatureMovementComponent->PlayMontage(Montage, Timer, Crouch);
	}
}

void ATGOR_MovementCharacter::ApplyMovement()
{
	FVector XAxis, YAxis, ZAxis;
	GetInputBase(XAxis, YAxis, ZAxis);

	FVector Direction = XAxis * InputForward + YAxis * InputRight + FVector(0.0f, 0.0f, 1.0f) * InputUpward;
	float Size = InputForward * InputForward + InputRight * InputRight + InputUpward * InputUpward;

	float Strength = FMath::Min(Size, 1.0f) * InputStrength;
	FVector Normal = Direction.GetSafeNormal() / 2;

	AddMovementInput(Normal, Strength, false);
}

float ATGOR_MovementCharacter::GetCameraHeight(float Ratio)
{
	const APhysicsVolume* PhysVolume = CreatureMovementComponent->GetPhysicsVolume();
	if ((PhysVolume && PhysVolume->bWaterVolume) && !CreatureMovementComponent->Submerged)
	{
		return(Super::GetCameraHeight(0.5f + CreatureMovementComponent->SurfaceLevelRatio * 2));
	}
	else
	{
		return(Super::GetCameraHeight(Ratio));
	}
}

bool ATGOR_MovementCharacter::CanEnterAggressive()
{
	if (!Super::CanEnterAggressive())
	{
		return(false);
	}

	if (!IsValid(CreatureMovementComponent))
	{
		return(false);
	}

	return(!CreatureMovementComponent->IsClimbing());
}

void ATGOR_MovementCharacter::ChangedAggressive(bool Aggressive)
{
}


void ATGOR_MovementCharacter::DetectDesync()
{
	_desyncsThisTick++;

	if (_desyncsThisTick == 16)
	{
		DesyncDetected();
	}
}


void ATGOR_MovementCharacter::DesyncDetected_Implementation()
{
	RPORT("Desynced")
}