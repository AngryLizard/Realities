///////////////////////////////////////////////////
// The Gateway of Realities: Planes of Existence //
// Copyright 2016-2017                           //
///////////////////////////////////////////////////

#include "Realities.h"
#include "TGOR_CreatureCharacter.h"

#include "Base/TGOR_Singleton.h"
#include "Components/System/TGOR_InteractableComponent.h"
#include "Components/Creature/TGOR_CharacterMeshComponent.h"

#include "Utility/Error/TGOR_Error.h"


ATGOR_CreatureCharacter::ATGOR_CreatureCharacter(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass<UTGOR_CharacterMeshComponent>(ACharacter::MeshComponentName))
{
	bUseControllerRotationYaw = true;
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;

	// Actor Components

	SelfInteractable = ObjectInitializer.CreateDefaultSubobject<UTGOR_InteractableComponent>(this, TEXT("SelfInteractable"));
	SelfInteractable->SetSphereRadius(0.0f);
	SelfInteractable->BlockInteract = true;
	
	// Scene Components
	auto Capsule = GetCapsuleComponent();

	CharacterMeshComponent = Cast<UTGOR_CharacterMeshComponent>(GetMesh());
	if (IsValid(CharacterMeshComponent))
	{
		CharacterMeshComponent->SetIsReplicated(true);
	}
	else
	{
		ERROR("CharacterMesh not loaded correctly", Error)
	}

	//Set variables
	CrouchedEyeHeight = 32.0;
}

void ATGOR_CreatureCharacter::BeginPlay()
{
	Super::BeginPlay();
}

void ATGOR_CreatureCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}

void ATGOR_CreatureCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ATGOR_CreatureCharacter::Starting_Implementation()
{
}

////////////////////////////////////////////////////////////////////////////////////////////////////

