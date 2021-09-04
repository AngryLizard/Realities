// The Gateway of Realities: Planes of Existence.


#include "TGOR_BoxPhysicsVolume.h"
#include "DimensionSystem/Data/TGOR_DimensionData.h"


// Sets default values
ATGOR_BoxPhysicsVolume::ATGOR_BoxPhysicsVolume(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	BoxComponent = ObjectInitializer.CreateDefaultSubobject<UBoxComponent>(this, FName(TEXT("BoxComponent")));
	BoxComponent->SetupAttachment(GetRootComponent());
}

////////////////////////////////////////////////////////////////////////////////////////////////////

bool ATGOR_BoxPhysicsVolume::IsInside(const FVector& Location) const
{
	if (IsValid(BoxComponent))
	{
		// Compute in local space
		const FVector Local = BoxComponent->GetComponentTransform().InverseTransformPosition(Location);

		// Check whether we're inside
		const FVector Extend = BoxComponent->GetUnscaledBoxExtent();
		const FVector Abs = Local.GetAbs();
		return(Abs.X < Extend.X && Abs.Y < Extend.Y &&Abs.Z < Extend.Z);
	}
	return(false);
}


float ATGOR_BoxPhysicsVolume::ComputeVolume() const
{
	const FVector Extend = BoxComponent->GetScaledBoxExtent();
	return(Extend.X * Extend.Y + Extend.X * Extend.Z + Extend.Y * Extend.Z);
}

FBox ATGOR_BoxPhysicsVolume::ComputeBoundingBox() const
{
	return(BoxComponent->Bounds.GetBox());
}

FTGOR_PhysicsProperties ATGOR_BoxPhysicsVolume::ComputeSurroundings(const FVector& Location) const
{
	FTGOR_PhysicsProperties Properties = Super::ComputeSurroundings(Location);

	// Compute in local space
	const FTransform Transform = BoxComponent->GetComponentTransform();
	const FVector Local = Transform.InverseTransformPosition(Location);
	const FVector Direction = Transform.InverseTransformVectorNoScale(FVector::UpVector);

	// Compute depth
	Properties.Depth = ComputeDepth(Local, Direction);

	return (Properties);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

float ATGOR_BoxPhysicsVolume::ComputeDepth(const FVector& Location, const FVector& Direction) const
{
	// Project all vertices onto normal (Abs of all products yields maximum of all projections)
	const FVector Extend = BoxComponent->GetUnscaledBoxExtent();
	const float Dist = FMath::Abs(Extend.X * Direction.X) + FMath::Abs(Extend.Y * Direction.Y) + FMath::Abs(Extend.Z * Direction.Z);
	const float Own = Direction | Location;

	return (Dist - Own);
}