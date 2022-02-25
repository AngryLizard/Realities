// The Gateway of Realities: Planes of Existence.


#include "TGOR_AimTargetComponent.h"


UTGOR_AimTargetComponent::UTGOR_AimTargetComponent()
	: Super()
{
	TargetFeetOffset = FVector(0.0f, 0.0f, 0.0f);
	TargetHeadOffset = FVector(0.0f, 0.0f, 0.0f);

	CanBeTargeted = true;
}


FVector UTGOR_AimTargetComponent::GetClampedLocation(float Height)
{
	FVector Location = GetOwner()->GetActorLocation();

	const FVector& Feet = TargetFeetOffset;
	const FVector& Head = TargetHeadOffset;

	float Dist = Head.Z - Feet.Z;
	if (Dist < SMALL_NUMBER)
	{
		return (Location + Feet);
	}

	float Lerp = (Height - (Location.Z + Feet.Z)) / Dist;
	return (Location + Feet + (Head - Feet) * FMath::Clamp(Lerp, 0.0f, 1.0f));
}