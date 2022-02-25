// The Gateway of Realities: Planes of Existence.


#include "TGOR_SpherePhysicsVolume.h"
#include "Realities/Base/System/Data/TGOR_DimensionData.h"


// Sets default values
ATGOR_SpherePhysicsVolume::ATGOR_SpherePhysicsVolume(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	SphereComponent = ObjectInitializer.CreateDefaultSubobject<USphereComponent>(this, FName(TEXT("SphereComponent")));
	SetRootComponent(SphereComponent);

}

////////////////////////////////////////////////////////////////////////////////////////////////////

bool ATGOR_SpherePhysicsVolume::IsInside(const FVector& Location) const
{
	if (IsValid(SphereComponent))
	{
		// Compute in local space
		const FVector Local = SphereComponent->GetComponentTransform().InverseTransformPosition(Location);

		// Check whether we're inside
		const float Radius = SphereComponent->GetUnscaledSphereRadius();
		const float Dist = Local.SizeSquared();
		return(Dist < Radius * Radius);
	}
	return(false);
}


float ATGOR_SpherePhysicsVolume::ComputeVolume() const
{
	const float Radius = SphereComponent->GetScaledSphereRadius();
	return(Radius * Radius * Radius * PI * (4.0f/3.0f));
}

FBox ATGOR_SpherePhysicsVolume::ComputeBoundingBox() const
{
	return(SphereComponent->Bounds.GetBox());
}

FTGOR_PhysicsProperties ATGOR_SpherePhysicsVolume::ComputeSurroundings(const FVector& Location) const
{
	FTGOR_PhysicsProperties Properties = Super::ComputeSurroundings(Location);

	// Compute in local space
	const FTransform Transform = SphereComponent->GetComponentTransform();
	const FVector Local = Transform.InverseTransformPosition(Location);
	const FVector Direction = Transform.InverseTransformVectorNoScale(FVector::UpVector);

	// Compute depth
	Properties.Depth = ComputeDepth(Local, Direction);

	return (Properties);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

float ATGOR_SpherePhysicsVolume::ComputeDepth(const FVector& Location, const FVector& Direction) const
{
	const float Radius = SphereComponent->GetUnscaledSphereRadius();
	

	// Ray sphere intersection from http://viclw17.github.io/2018/07/16/raytracing-ray-sphere-intersection/
	float A = (Direction | Direction);
	float B = 2.0 * (Location | Direction);
	float C = (Location | Location) - Radius * Radius;
	float D = B * B - 4 * A * C;
	if (D >= 0.0f) 
	{
		return (B + sqrt(D)) / (2.0 * A);
	}

	/*
	// Check whether ray hits sphere
	const float SquaredDist = (Location ^ Direction).SizeSquared();
	const float SquaredRadius = Radius * Radius;
	if (SquaredDist <= SquaredRadius)
	{
		// Compute closest t
		const float MidpointDist = (Location | Direction);
		const float RadiusDifference = SquaredRadius - SquaredDist;

		if (RadiusDifference < SMALL_NUMBER)
		{
			const float Offset = FMath::Sqrt(RadiusDifference);
			const float Depth = MidpointDist - Offset;

			// Use other side of circle in case ray origin is within it
			if (Depth < 0.0f)
			{
				return MidpointDist + Offset;
			}

			// Check whether intersection is within ray limits
			return Depth;
		}
	}
	*/

	return 0.0f;
}