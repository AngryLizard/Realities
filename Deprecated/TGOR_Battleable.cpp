///////////////////////////////////////////////////
// The Gateway of Realities: Planes of Existence //
// Copyright 2016-2017                           //
///////////////////////////////////////////////////

#include "Realities.h"
#include "TGOR_Battleable.h"

#include "Kismet/KismetSystemLibrary.h"
#include "Base/System/Data/TGOR_ConfigData.h"
#include "Components/Camera/TGOR_CameraComponent.h"
#include "Components/Combat/Stat/TGOR_HealthComponent.h"
#include "Components/Combat/Stat/TGOR_StaminaComponent.h"
#include "Components/Combat/Stat/TGOR_EnergyComponent.h"
#include "Components/Combat/TGOR_AimComponent.h"
#include "Components/Combat/Equipable/TGOR_UpperComponent.h"
#include "Components/Combat/Equipable/TGOR_LowerComponent.h"

#include "Actors/Pawns/TGOR_Consciousness.h"

#include "UnrealNetwork.h"



ATGOR_Battleable::ATGOR_Battleable(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	IsAggressive = false;

	UpperComponent = ObjectInitializer.CreateDefaultSubobject<UTGOR_UpperComponent>(this, TEXT("UpperComponent"));
	UpperComponent->SetIsReplicated(true);

	LowerComponent = ObjectInitializer.CreateDefaultSubobject<UTGOR_LowerComponent>(this, TEXT("LowerComponent"));
	LowerComponent->SetIsReplicated(true);

	AimComponent = ObjectInitializer.CreateDefaultSubobject<UTGOR_AimComponent>(this, TEXT("AimComponent"));
	AimComponent->SetIsReplicated(true);

	HealthComponent = ObjectInitializer.CreateDefaultSubobject<UTGOR_HealthComponent>(this, TEXT("HealthComponent"));
	HealthComponent->SetIsReplicated(true);

	StaminaComponent = ObjectInitializer.CreateDefaultSubobject<UTGOR_StaminaComponent>(this, TEXT("StaminaComponent"));
	StaminaComponent->SetIsReplicated(true);

	EnergyComponent = ObjectInitializer.CreateDefaultSubobject<UTGOR_EnergyComponent>(this, TEXT("EnergyComponent"));
	EnergyComponent->SetIsReplicated(true);
}

void ATGOR_Battleable::BeginPlay()
{
	Super::BeginPlay();
}

void ATGOR_Battleable::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// Replicate to everyone
	DOREPLIFETIME(ATGOR_Battleable, IsAggressive);
}

void ATGOR_Battleable::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	if (IsAggressive && bIsCrouched)
	{
		SetIsAggressive(false);
	}

	if (!Controller) return;

	// Only change aim when locally controlled
	if (IsLocallyControlled())
	{
		FMinimalViewInfo View;
		Controller->CalcCamera(DeltaTime, View);
		AimComponent->SetFocusToCamera(View.Location, View.Rotation.Vector());
	}
}

bool ATGOR_Battleable::HasCursor_Implementation()
{
	return(Super::HasCursor_Implementation());
}


////////////////////////////////////////////////////////////////////////////////////////////////////


void ATGOR_Battleable::SetIsAggressive(bool Aggressive)
{
	// Check if aggressive is allowed
	if (Aggressive && !CanEnterAggressive())
	{
		return;
	}

	// Initialise combat
	if (Role >= ENetRole::ROLE_AutonomousProxy)
	{
		if (!IsAggressive && Aggressive)
		{
			CombatEntered();
		}
		else if (IsAggressive && !Aggressive)
		{
			CombatLeft();
		}
	}

	// Replicate to server
	if (!HasAuthority())
	{
		ReplicateIsAggressive(Aggressive);
	}

	IsAggressive = Aggressive;
	ChangedAggressive(IsAggressive);
}

void ATGOR_Battleable::ReplicateIsAggressive_Implementation(bool Aggressive)
{
	SetIsAggressive(Aggressive);
}

bool ATGOR_Battleable::ReplicateIsAggressive_Validate(bool Aggressive)
{
	return(true);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void ATGOR_Battleable::SpawnAsConsciousness(TSubclassOf<ATGOR_Consciousness> Class, bool Despawn)
{
	// Get controller
	APlayerController* PlayerController = Cast<APlayerController>(Controller);
	if (!IsValid(PlayerController))
	{
		ERROR("Invalid Controller", Error)
	}

	// Retreive spawn params
	UWorld* World = GetWorld();
	FVector Location = GetActorLocation();
	FRotator Rotation = GetActorRotation();

	// Spawn consciousness
	ATGOR_Consciousness* Consciousness = UTGOR_GameInstance::SpawnActorForce<ATGOR_Consciousness>(Class, World, Location, Rotation);
	if (!IsValid(Consciousness))
	{
		ERROR("Couldn't spawn sonsciousness", Error)
	}

	// Resets user data to prevent respawning as character
	PlayerComponent->ResetUser();

	// Possess the consciousness
	FRotator ControlRotation = PlayerController->GetControlRotation();
	PlayerController->Possess(Consciousness);
	PlayerController->SetControlRotation(ControlRotation);
	
	// Kill myself if enabled
	if (Despawn)
	{
		UTGOR_GameInstance::DespawnActor(this);
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////

bool ATGOR_Battleable::CanEnterAggressive()
{
	return(!bIsCrouched);
}

void ATGOR_Battleable::ChangedAggressive(bool Aggressive)
{

}