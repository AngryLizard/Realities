// The Gateway of Realities: Planes of Existence.


#include "TGOR_CustomSpherePhysicsVolume.h"
#include "DimensionSystem/Data/TGOR_DimensionData.h"

#include "DrawDebugHelpers.h"

// Sets default values
ATGOR_CustomSpherePhysicsVolume::ATGOR_CustomSpherePhysicsVolume(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{

}

////////////////////////////////////////////////////////////////////////////////////////////////////


FTGOR_PhysicsProperties ATGOR_CustomSpherePhysicsVolume::ComputeSurroundings(const FVector& Location) const
{
	FTGOR_PhysicsProperties Properties = Super::ComputeSurroundings(Location);

	// Compute in local space
	USphereComponent* Sphere = GetSphere();
	const FTransform Transform = Sphere->GetComponentTransform();
	const FVector Local = Transform.InverseTransformPosition(Location);

	FVector Direction = UpVector;
	ComputeGravityDirection(Local, Direction);

	// Compute depth
	Properties.Depth = ComputeDepth(Local, Direction);
	Properties.Gravity = Transform.TransformVector(Direction * -Gravity);

	// Compute velocities
	ComputeVolumeVelocity(Location, Properties.Velocity);
	ComputeVolumeVorticity(Location, Properties.Vorticity);

	return (Properties);
}