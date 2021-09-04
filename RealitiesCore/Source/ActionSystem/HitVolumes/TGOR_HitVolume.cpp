
#include "TGOR_HitVolume.h"

#include "CoreSystem/Gameplay/TGOR_GameInstance.h"

#include "DimensionSystem/Volumes/TGOR_PhysicsVolume.h"
#include "DimensionSystem/Volumes/TGOR_LevelVolume.h"
#include "../Components/TGOR_DamageComponent.h"

ATGOR_HitVolume::ATGOR_HitVolume(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer),
	MaxDeltaTime(0.02f),
	OwnerInstigator(nullptr),
	HitChannel(ECollisionChannel::ECC_WorldDynamic)
{
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = false;

	DamageComponent = ObjectInitializer.CreateDefaultSubobject<UTGOR_DamageComponent>(this, FName(TEXT("DamageComponent")));
	SetRootComponent(DamageComponent);
}


void ATGOR_HitVolume::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Go until timestamp is used up
	while (DeltaTime > 0.0f)
	{
		// Stride if framerate is too big
		const float Delta = FMath::Min(MaxDeltaTime, DeltaTime);
		DeltaTime -= Delta;

		// Update HitVolume
		if (!Update(Delta))
		{
			// Terminate if activity has concluded
			Kill();
			return;
		}
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void ATGOR_HitVolume::AssignOwnerVolume(ATGOR_LevelVolume* Volume)
{
	OwnerVolume = Volume;
}

void ATGOR_HitVolume::AssignOwnerInstigator(AActor* Actor)
{
	OwnerInstigator = Actor;
}

ATGOR_PhysicsVolume* ATGOR_HitVolume::GetPhysicsVolume(ATGOR_PhysicsVolume* Volume, const FVector& Location) const
{
	// Check current volume
	if (IsValid(Volume) && Volume->IsInside(Location))
	{
		return Volume->Query(Location);
	}

	// Check level volume
	if (IsValid(OwnerVolume))
	{
		return OwnerVolume->Query(Location);
	}
	return Volume;
}

bool ATGOR_HitVolume::Update(float DeltaTime)
{
	return false;
}

void ATGOR_HitVolume::Kill()
{
	UTGOR_GameInstance::DespawnActor(this);
}

void ATGOR_HitVolume::Shoot(const FTGOR_AimInstance& Target, const FTGOR_ForceInstance& Forces)
{

}

////////////////////////////////////////////////////////////////////////////////////////////////////