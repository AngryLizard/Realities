// The Gateway of Realities: Planes of Existence.


#include "TGOR_CustomBoxPhysicsVolume.h"
#include "DimensionSystem/Data/TGOR_DimensionData.h"


// Sets default values
ATGOR_CustomBoxPhysicsVolume::ATGOR_CustomBoxPhysicsVolume(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{

}

////////////////////////////////////////////////////////////////////////////////////////////////////


FTGOR_PhysicsProperties ATGOR_CustomBoxPhysicsVolume::ComputeSurroundings(const FVector& Location) const
{
	FTGOR_PhysicsProperties Properties = Super::ComputeSurroundings(Location);

	// Compute in local space
	UBoxComponent* Box = GetBox();
	const FTransform Transform = Box->GetComponentTransform();
	const FVector Local = Transform.InverseTransformPosition(Location);

	FVector Direction = FVector::UpVector;
	ComputeGravityDirection(Local, Direction);

	// Compute depth
	Properties.Depth = ComputeDepth(Local, Direction);
	Properties.Gravity = Transform.TransformVector(Direction * -Gravity);

	// Compute velocities
	ComputeVolumeVelocity(Location, Properties.Velocity);
	ComputeVolumeVorticity(Location, Properties.Vorticity);

	return (Properties);
}

