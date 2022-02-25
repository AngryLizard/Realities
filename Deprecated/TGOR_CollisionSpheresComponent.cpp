// The Gateway of Realities: Planes of Existence.

#include "Realities.h"
#include "TGOR_CollisionSpheresComponent.h"

#include "TGOR_CollisionSphereComponent.h"


UTGOR_CollisionSpheresComponent::UTGOR_CollisionSpheresComponent()
{
}


void UTGOR_CollisionSpheresComponent::BeginPlay()
{
	Super::BeginPlay();

}


bool UTGOR_CollisionSpheresComponent::Probe(const FVector& Delta, const FQuat& NewRotation, bool bSweep, FHitResult& OutHit, EMoveComponentFlags MoveFlags, ETeleportType Teleport)
{
	if (bSweep)
	{
		// Probe all components
		OutHit = FHitResult(1.0f);
		for (UTGOR_CollisionSphereComponent* Component : SphereComponents)
		{
			FHitResult Hit;
			Component->Probe(Delta, Hit, MoveFlags, Teleport);

			if (Hit.Time < OutHit.Time)
			{
				OutHit = Hit;
			}
		}

	}
	return true;
}

FVector UTGOR_CollisionSpheresComponent::ProbeCenter()
{
	// Probe all components
	float Weight = 0.0f;
	FVector Accumulate = FVector::ZeroVector;
	for (UTGOR_CollisionSphereComponent* Component : SphereComponents)
	{
		FVector Correction = Component->ProbeCenter();
		Accumulate += Correction;

		if (Correction.SizeSquared() > SMALL_NUMBER)
		{
			Weight += 1.0f;
		}
	}

	if (Weight > SMALL_NUMBER)
	{
		return(Accumulate / Weight);
	}
	return Accumulate;
}

void UTGOR_CollisionSpheresComponent::Build()
{
	// Get Ragdoll spheres
	const TArray<USceneComponent*>& Components = GetAttachChildren();
	for (USceneComponent* Component : Components)
	{
		UTGOR_CollisionSphereComponent* SphereComponent = Cast<UTGOR_CollisionSphereComponent>(Component);
		if (IsValid(SphereComponent))
		{
			SphereComponents.Add(SphereComponent);
		}
	}
}
