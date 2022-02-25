// The Gateway of Realities: Planes of Existence.


#include "TGOR_PhysicsFreeComponent.h"




UTGOR_PhysicsFreeComponent::UTGOR_PhysicsFreeComponent()
	: Super()
{
}

FVector UTGOR_PhysicsFreeComponent::FreedomProject(const FVector& Vector, const FVector& Offset) const
{
	// Allow full movement
	return(Vector);
}

void UTGOR_PhysicsFreeComponent::UpdateStructure()
{
	float Total = 0.0f;
	FVector WeightedLocation = FVector::ZeroVector;
	for (const auto& PhysicsComponent : PhysicsComponents)
	{
		const float Mass = PhysicsComponent->ComputeTotalMass();
		const FVector Offset = PhysicsComponent->GetOffset();
		WeightedLocation += Offset * Mass;
		Total += Mass;
	}

	if (Total > SMALL_NUMBER)
	{
		const FVector Delta = WeightedLocation / Total;

		// Set offsets
		for (const auto& PhysicsComponent : PhysicsComponents)
		{
			PhysicsComponent->MoveOffset(-Delta);
		}

		// Counter move
		const FTransform Transform = GetComponentTransform();
		const FVector WorldDelta = Transform.TransformVector(Delta);
		AddWorldOffset(WorldDelta);
		Move(WorldDelta);
	}
}