// The Gateway of Realities: Planes of Existence.

#include "Realities.h"
#include "TGOR_CollisionSphereComponent.h"

UTGOR_CollisionSphereComponent::UTGOR_CollisionSphereComponent()
{
	LocationCache = FVector::ZeroVector;
}

void UTGOR_CollisionSphereComponent::BeginPlay()
{
	Super::BeginPlay();
}


bool UTGOR_CollisionSphereComponent::Probe(const FVector& Delta, FHitResult& Hit, EMoveComponentFlags MoveFlags, ETeleportType Teleport)
{
	// Reset hitresult
	Hit = FHitResult();

	// Compute source and target position
	const FVector Source = RelativeLocation;
	const FQuat Rotation = GetComponentQuat();

	// Move/Probe component
	if (MoveComponent(Delta, Rotation, true, &Hit, MoveFlags, Teleport))
	{
		RelativeLocation = Source;
		return(Hit.bBlockingHit);
	}
	return(false);
}


FVector UTGOR_CollisionSphereComponent::ProbeCenter()
{
	// Compute target position
	TArray<AActor*> Ignore;
	TArray<FHitResult> Hits;
	const FVector Location = GetComponentLocation();
	const float Radius = GetScaledSphereRadius();
	const FName& Profile = GetCollisionProfileName();

	// Trace for collisions
	if (UKismetSystemLibrary::SphereTraceMultiByProfile(this, Location, Location, Radius, Profile, false, Ignore, EDrawDebugTrace::Type::None, Hits, true))
	{
		float Weight = 0.0f;
		FVector Correction = FVector::ZeroVector;
		for (FHitResult& Hit : Hits)
		{
			if (Hit.bBlockingHit && Hit.bStartPenetrating)
			{
				// Compute actual impact point (Hit.Impact isn't accurate due to penetration)
				const FVector Parent = GetOwner()->GetActorLocation();
				const FVector Local = Location - Parent;

				const float Distance = Hit.PenetrationDepth - Radius;
				const FVector Relative = Hit.Normal * Distance;
				Hit.ImpactPoint = Location + Relative;

				// Intersect with sphere to find point on the sphere
				const FVector Normal = Local.GetSafeNormal();
				const float Bias = (Normal | Relative);
				const float Dir = Bias * Bias - Distance * Distance + Radius * Radius;
				if (Dir < 0.0f)
				{
					Correction = FVector::ZeroVector;
				}
				else
				{
					// Compute final correction vector
					const float Sqr = FMath::Sqrt(Dir);
					const float Sign = FMath::Sign(Sqr - Bias);
					const float Amount = Bias - Sqr * Sign;

					// Always correct away from the parent
					Correction += Amount * Normal;
					Weight += 1.0f;
				}
			}
		}

		// Get average
		if (Weight > SMALL_NUMBER)
		{
			return(Correction / Weight);
		}
		return(Correction);
	}

	return(FVector::ZeroVector);
}