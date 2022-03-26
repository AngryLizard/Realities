
#include "TGOR_ProjectileHitVolume.h"
#include "RealitiesUtility/Utility/TGOR_Math.h"

#include "DimensionSystem/Data/TGOR_DimensionData.h"
#include "CoreSystem/TGOR_Singleton.h"
#include "DimensionSystem/Volumes/TGOR_PhysicsVolume.h"

#include "TargetSystem/Components/TGOR_AimComponent.h"
#include "DrawDebugHelpers.h"

FTGOR_ProjectileDataInstance::FTGOR_ProjectileDataInstance()
:	Time(),
	Hits(0),
	Target(FVector::ZeroVector),
	Dynamic(),
	Volume(nullptr),
	Forces()
{
}

ATGOR_ProjectileHitVolume::ATGOR_ProjectileHitVolume(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer),
	ProjectileBody(),
	InitialVelocity(100.0f), 
	Timeout(5.0f), 
	Elasticity(0.5f), 
	HitOnBounce(false), 
	MaxHits(1),
	IgnoreInstigatorThreshold(-1),
	CollisionChannel(ECollisionChannel::ECC_WorldDynamic)
{
	ProjectileBody.SetFromSphere(FVector(0.5f), 1.0f, 1.0f);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

bool ATGOR_ProjectileHitVolume::Update(float DeltaTime)
{
	check(ProjectileBody.Mass > SMALL_NUMBER && "Negative or zero mass for projectiles");
	Super::Update(DeltaTime);

	SINGLETON_RETCHK(false);
	const FTGOR_Time Timestamp = Singleton->GetGameTimestamp();
	
	// Go until timestamp is used up
	while (DeltaTime > 0.0f)
	{
		// Stride if framerate is too big
		const float Delta = FMath::Min(MaxDeltaTime, DeltaTime);
		DeltaTime -= Delta;

		for (auto It = Projectiles.CreateIterator(); It; It++)
		{
			// Update velocity
			const FVector Force = ComputeForce(*It);
			It->Dynamic.LinearVelocity += Force / ProjectileBody.Mass * Delta;

			// Update location
			const FVector OldLocation = It->Dynamic.Linear;
			It->Dynamic.Linear += It->Dynamic.LinearVelocity * Delta;

			// Test for collision
			FHitResult Hit;
			const bool IgnoreInstigator = (IgnoreInstigatorThreshold == -1 || It->Hits < IgnoreInstigatorThreshold);
			if (Trace(OldLocation, It->Dynamic.Linear, IgnoreInstigator, Hit))
			{
				It->Hits++;
				It->Dynamic.Linear = Hit.Location + (Hit.PenetrationDepth + ProjectileBody.Radius) * Hit.Normal;
				if (It->Hits >= MaxHits)
				{
					OnHit(*It);
					It.RemoveCurrent();
					It++;
					break;
				}
				else if (HitOnBounce)
				{
					OnHit(*It);
				}
				It->Dynamic.LinearVelocity = It->Dynamic.LinearVelocity - (1.0f + Elasticity) * (It->Dynamic.LinearVelocity | Hit.Normal) * Hit.Normal;
			}

			// Find new volume
			It->Volume = QueryPhysicsVolume(It->Volume, It->Dynamic.Linear);

			// Display current state
			OnDisplay(*It);

			if (Timestamp - It->Time > Timeout)
			{
				It.RemoveCurrent();
				It++;
			}
		}
	}

	return Projectiles.Num() > 0;
}

void ATGOR_ProjectileHitVolume::Shoot(const FTGOR_AimInstance& Target, const FTGOR_ForceInstance& Forces)
{
	SINGLETON_CHK;

	// Create new projectile
	FTGOR_ProjectileDataInstance Projectile;
	Projectile.Time = Singleton->GetGameTimestamp();
	Projectile.Hits = 0;
	Projectile.Target = UTGOR_AimComponent::ComputeAimLocation(Target, false);

	Projectile.Dynamic.Linear = GetActorLocation();
	Projectile.Dynamic.LinearVelocity = UTGOR_Math::Normalize(Projectile.Target - Projectile.Dynamic.Linear) * InitialVelocity;
	Projectile.Volume = QueryPhysicsVolume(nullptr, Projectile.Dynamic.Linear);
	Projectile.Forces = Forces;

	Projectiles.Emplace(Projectile);
}


FVector ATGOR_ProjectileHitVolume::ComputeForce(const FTGOR_ProjectileDataInstance& Projectile) const
{
	if (IsValid(Projectile.Volume))
	{
		const FTGOR_MovementSpace ParentSpace;
		const FTGOR_MovementSpace Space = ParentSpace.BaseToSpace(Projectile.Dynamic);

		// Compute External forces
		const FTGOR_PhysicsProperties Surroundings = Projectile.Volume->ComputeSurroundings(Projectile.Dynamic.Linear);
		FTGOR_MovementDamper External = ProjectileBody.ComputeExternalForce(Space, Surroundings);
		return External.Vector;
	}

	return FVector::ZeroVector;
}

bool ATGOR_ProjectileHitVolume::Trace(const FVector& Start, const FVector& End, bool IgnoreInstigator, FHitResult& Hit)
{
	UWorld* World = GetWorld();

	// Generate Trace params
	FCollisionQueryParams TraceParams(FName(TEXT("ProjectileTrace")), true, GetOwner());
	TraceParams.bTraceComplex = false;
	TraceParams.bReturnFaceIndex = false;
	TraceParams.bReturnPhysicalMaterial = false;
	if (IgnoreInstigator)
	{
		TraceParams.AddIgnoredActor(OwnerInstigator);
	}

	// Generate shape info and trace for collisions
	Hit = FHitResult(ForceInit);
	const FCollisionShape Shape = FCollisionShape::MakeSphere(ProjectileBody.Radius);
	return World->SweepSingleByChannel(Hit, Start, End, FQuat::Identity, CollisionChannel, Shape, TraceParams);
}
