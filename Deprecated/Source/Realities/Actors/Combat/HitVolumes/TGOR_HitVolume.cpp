
#include "TGOR_HitVolume.h"

#include "Realities/Components/TGOR_SceneComponent.h"
#include "Realities/Components/Combat/TGOR_HitVolumeComponent.h"
#include "Realities/Actors/Dimension/Volumes/TGOR_LevelVolume.h"
#include "Realities/Base/System/Data/TGOR_DimensionData.h"

#include "Realities/Components/Dimension/Interactable/TGOR_InteractableComponent.h"
#include "Realities/Mod/Targets/TGOR_Target.h"

ATGOR_HitVolume::ATGOR_HitVolume(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer),
	MaxDeltaTime(0.02f),
	OwnerInstigator(nullptr),
	HitChannel(ECollisionChannel::ECC_WorldDynamic)
{
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = false;

	RootComponent = ObjectInitializer.CreateDefaultSubobject<UTGOR_SceneComponent>(this, FName(TEXT("RootComponent")));
	SetRootComponent(RootComponent);
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


void ATGOR_HitVolume::HitAround(const FVector& Origin, float Radius, const FTGOR_ForceInstance& Forces)
{
	UWorld* World = GetWorld();

	// Generate Trace params
	TArray<AActor*> Ignore;
	FCollisionQueryParams TraceParams(FName(TEXT("ProjectileTrace")), true, GetOwner());
	TraceParams.bTraceComplex = false;
	TraceParams.bReturnFaceIndex = false;
	TraceParams.bReturnPhysicalMaterial = false;

	// Generate shape info
	const FCollisionShape Shape = FCollisionShape::MakeSphere(Radius);

	// Find interactables
	TArray<FOverlapResult> Overlaps;
	if (World->OverlapMultiByChannel(Overlaps, Origin, FQuat::Identity, HitChannel, Shape, TraceParams))
	{
		for (const FOverlapResult& Overlap : Overlaps)
		{
			UTGOR_InteractableComponent* Interactable = Cast<UTGOR_InteractableComponent>(Overlap.GetComponent());
			if (IsValid(Interactable))
			{
				for (UTGOR_Target* Target : Interactable->Targets)
				{
					FTGOR_AimPoint Point;
					if (Target->OverlapTarget(Interactable, Origin, Radius, Point))
					{
						FTGOR_InfluenceInstance Influence;
						Influence.Target = Point.Target;
						Influence.Forces = Forces * (1.0f - Point.Distance);
						Influence.Instigator = OwnerInstigator;
						Influence.Location = Origin;

						Interactable->Influence(Influence);
					}
				}
			}
		}
	}
}