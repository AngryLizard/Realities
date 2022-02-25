// The Gateway of Realities: Planes of Existence.


#include "TGOR_PhysicsSphereComponent.h"
#include "DrawDebugHelpers.h"
#include "Realities/Utility/TGOR_Math.h"


UTGOR_PhysicsSphereComponent::UTGOR_PhysicsSphereComponent()
	: Super()
{
	Mass = 1.0f;
	Bouncyness = 0.3f;

	Friction = 0.1f;
	Drag = 0.00006f;

	CurrentLocation = FVector(0.0f, 0.0f, 0.0f);
}

void UTGOR_PhysicsSphereComponent::BeginPlay()
{
	Super::BeginPlay();

	CurrentLocation = GetComponentLocation();
}


FVector UTGOR_PhysicsSphereComponent::ApplyMove(float DeltaTime, const FVector& External, const TArray<AActor*>& Ignore)
{
	// Compute target position
	FHitResult Hit;
	FVector Location = GetComponentLocation();
	const float Radius = GetScaledSphereRadius();
	const FName& Profile = GetCollisionProfileName();
	const FVector Direction = Location - CurrentLocation;
	
	// Trace for collisions
	UKismetSystemLibrary::SphereTraceSingleByProfile(this, CurrentLocation, Location, Radius, Profile, false, Ignore, DebugType, Hit, true);
	if (Hit.bBlockingHit && FVector::DotProduct(Hit.Normal, Direction) < SMALL_NUMBER)
	{
		// Resolve penetration
		if (Hit.bStartPenetrating)
		{
			Hit.Location += Hit.Normal * Hit.PenetrationDepth;
			Hit.ImpactPoint = Hit.Location - Hit.Normal * (Radius - Hit.PenetrationDepth);
		}
		
		// Set locations
		const FVector Correction = Hit.Location - Location;

		// Slide vector along surface for friction
		FVector Project = FVector::VectorPlaneProject(Direction, Hit.Normal);
		FVector Slide = UTGOR_Math::Normalize(Project);

		if (DebugType > EDrawDebugTrace::None)
		{
			DrawDebugPoint(GetWorld(), Hit.ImpactPoint, 12.0f, FColor::Red, true, 20.0f);
			DrawDebugLine(GetWorld(), Hit.ImpactPoint, Hit.ImpactPoint + Hit.Normal * 100.0f, FColor::Red, true, 20.0f);
		}

		// Resolve impact and friction
		FVector PointVelocity = FVector::ZeroVector;
		ResolveSingle(PointVelocity, (1.0f + Bouncyness), Hit.Normal, Hit.ImpactPoint, Correction.Size());
		if (Friction > SMALL_NUMBER)
		{
			ResolveSingle(PointVelocity, Friction, Slide, Hit.ImpactPoint, 0.0f);
		}

		float MaxVelocity = PointVelocity.SizeSquared();

		// No drag on impact
		return(Correction);
	}

	// Update location
	CurrentLocation = GetComponentLocation();
	FVector PointVelocity = FVector::ZeroVector;

	// Compute velocity
	const float Speed = Direction.Size();
	if (Speed > SMALL_NUMBER && Drag > SMALL_NUMBER)
	{
		// Compute drag direction
		const FVector Normal = -Direction / Speed;

		// Compute drag
		const float Density = 1.2f;
		const float QuarterArea = PI * Radius * Radius;
		const float Coeff = Drag * Speed * QuarterArea * Density / 10'000.0f;

		// Apply drag
		ResolveSingle(PointVelocity, Coeff * DeltaTime, Normal, CurrentLocation, 0.0f);
	}

	return(FVector::ZeroVector);
}

void UTGOR_PhysicsSphereComponent::SetParent(TScriptInterface<ITGOR_PhysicsObjectInterface> ParentInterface)
{
	Parent = ParentInterface;
}

FTGOR_InertiaTensor UTGOR_PhysicsSphereComponent::ComputeInertiaTensor()
{
	const float Radius = GetScaledSphereRadius();

	const float RR = Radius * Radius;
	const float RM = 2.0f / 3.0f * Mass * RR;

	const FVector Diag = FVector(RM, RM, RM);

	// Spheres don't care about rotation, no transform needed
	FTGOR_InertiaTensor InertiaTensor = FTGOR_InertiaTensor(Diag);
	return(InertiaTensor);
}

float UTGOR_PhysicsSphereComponent::ComputeTotalMass()
{
	return(Mass);
}

FVector UTGOR_PhysicsSphereComponent::GetOffset() const
{
	return(GetRelativeLocation());
}

void UTGOR_PhysicsSphereComponent::MoveOffset(const FVector& Delta)
{
	SetRelativeLocation(GetRelativeLocation() + Delta);
}

FVector UTGOR_PhysicsSphereComponent::GetLocation() const
{
	return(CurrentLocation);
}

void UTGOR_PhysicsSphereComponent::Move(const FVector& Delta)
{
	CurrentLocation += Delta;
}

void UTGOR_PhysicsSphereComponent::ResolveSingle(FVector& PointVelocity, float Coeff, const FVector& HitNormal, const FVector& HitLocation, float Collective)
{
	if (Parent)
	{
		Parent->ResolveSingle(PointVelocity, Coeff, HitNormal, HitLocation, Collective);
	}
}

void UTGOR_PhysicsSphereComponent::ApplySingle(float Coeff, const FVector& HitNormal, const FVector& HitLocation, float Collective)
{
	if (Parent)
	{
		Parent->ApplySingle(Coeff, HitNormal, HitLocation, Collective);
	}
}