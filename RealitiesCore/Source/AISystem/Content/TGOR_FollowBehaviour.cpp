// The Gateway of Realities: Planes of Existence.
#include "TGOR_FollowBehaviour.h"

#include "PhysicsSystem/Components/TGOR_ColliderComponent.h"

UTGOR_FollowBehaviour::UTGOR_FollowBehaviour()
: Super()
{
}

FVector UTGOR_FollowBehaviour::PredictLocation(const FTGOR_MovementDynamic& Dynamic, float Time) const
{
	// TODO: Would be nice to include acceleration, but things get intensive (multiple solutions for shortest distance)
	return Dynamic.Linear + Dynamic.LinearVelocity * Time;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

FVector UTGOR_FollowBehaviour::ComputeFollowGradient(const FTGOR_MovementDynamic& Dynamic, const FTGOR_MovementDynamic& Other, float Distance) const
{
	const float Time = FMath::Max(ComputeClosestTime(Dynamic, Other), 0.0f);
	const FVector Target = PredictLocation(Other, Time);


	const FVector Delta = Target - Dynamic.Linear;
	if (Distance < SMALL_NUMBER)
	{
		return Delta;
	}
	else
	{
		const FVector Velocity = Dynamic.LinearVelocity;
		const float Speed = Velocity.Size();
		if (Speed < SMALL_NUMBER)
		{
			// Directly move towards target center
			const float Ratio = FMath::Max(Delta.SizeSquared() / (Distance * Distance) - 1.0f, 0.0f);
			return Delta * Ratio;
		}
		else
		{
			const FVector Direction = Velocity / Speed;

			// Project onto velocity
			const float Length = Direction | Delta;
			const FVector Project = Delta - (Direction * Length);

			const float Lateral = Project.Size();
			if (Lateral < SMALL_NUMBER)
			{
				// Directly move towards target center, linear decay
				const float Ratio = FMath::Max(Length / Distance - 1.0f, 0.0f); // Get strength relative to distance to circle
				return Direction * Ratio;
			}
			else
			{
				const FVector Sideways = Project / Lateral;
				const FVector Forward = Direction * FMath::Sign(Length);

				// Move towards circle around target center
				const float Sphere = FMath::Sqrt(FMath::Max(Distance * Distance - Lateral * Lateral, 0.0f)); // Distance offset along circle
				const FVector Move = Forward * FMath::Max((FMath::Abs(Length) - Sphere), 0.0f); // Get strength relative to distance to circle
				const FVector Offset = Sideways * FMath::Max(Lateral - Distance, 0.0f); // Clamp lateral length
				return (Move + Offset);
			}
		}

	}
}

FVector UTGOR_FollowBehaviour::ComputeCollisionGradient(const FTGOR_MovementDynamic& Dynamic, UTGOR_ColliderComponent* Collision, const FTGOR_MovementDynamic& Other, float Distance) const
{
	if (IsValid(Collision))
	{
		FHitResult Hit;
		if (Collision->MovementTraceSweep(Dynamic, Other.Linear - Dynamic.Linear, Hit))
		{
			return Hit.Normal * (1.0f - FMath::Clamp((Hit.Normal | (Dynamic.Linear - Hit.Location)) / Distance, 0.0f, 1.0f));
		}
	}
	return FVector::ZeroVector;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

float UTGOR_FollowBehaviour::ComputeClosestTime(const FTGOR_MovementDynamic& Dynamic, const FTGOR_MovementDynamic& Other) const
{
	const FVector v = Dynamic.LinearVelocity - Other.LinearVelocity;
	const FVector s = Dynamic.Linear - Other.Linear;

	const float vv = v.SizeSquared();
	if (vv < SMALL_NUMBER)
	{
		return 0.0f;
	}

	const FVector dv = v / vv;
	return -(s | dv);
}

float UTGOR_FollowBehaviour::ComputeClosestError(const FTGOR_MovementDynamic& Dynamic, const FTGOR_MovementObstacle& Other) const
{
	const FVector v = (Dynamic.LinearVelocity - Other.Dynamic.LinearVelocity) * 0.001f;
	const FVector s = (Dynamic.Linear - Other.Dynamic.Linear) * 0.001f;

	const float vv = v.SizeSquared();
	if (vv < KINDA_SMALL_NUMBER)
	{
		// FD will fail here since the error doesn't depend on v
		const float ss = s.SizeSquared();
		return 0.5f / ss;
	}
	else
	{
		const FVector dv = v / vv;

		const float ds = s | dv;
		const FVector e = s - v * ds;

		const float ee = e.SizeSquared();
		return 0.5f / ee;
	}
}

FVector UTGOR_FollowBehaviour::ComputeClosestGradient(const FTGOR_MovementDynamic& Dynamic, const FTGOR_MovementObstacle& Other) const
{
	const FVector v = (Dynamic.LinearVelocity - Other.Dynamic.LinearVelocity) * 0.001f;
	const FVector s = (Dynamic.Linear - Other.Dynamic.Linear) * 0.001f;

	const float vv = v.SizeSquared();
	if (vv < KINDA_SMALL_NUMBER)
	{
		const float ss = s.SizeSquared();
		return s / (-ss * ss);
	}
	else
	{
		const FVector dv = v / vv;

		const float ds = s | dv;
		const FVector e = s - v * ds;

		const float de = dv | e;
		const FVector t = -(s * de - (v * (de * 2) + e) * ds) * 0.001f;

		const float ee = e.SizeSquared();
		return Other.Dynamic.Angular * ((Other.Dynamic.Angular.Inverse() * (t / (-ee * ee))) * Other.Extent);
	}
}


FVector UTGOR_FollowBehaviour::ComputeClosestGradientFd(const FTGOR_MovementDynamic& Dynamic, const FTGOR_MovementObstacle& Other) const
{
	float e = ComputeClosestError(Dynamic, Other);

	const float h = 0.001f;
	FVector g = FVector::ZeroVector;
	for (int i = 0; i < 3; i++)
	{
		FTGOR_MovementDynamic dx = Dynamic;
		dx.LinearVelocity[i] += h;

		float deTowards = 0.0f;
		float deAway = 0.0f;
		const float de = ComputeClosestError(dx, Other);
		g[i] += (e - de) / h;
	}
	return g;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

FVector UTGOR_FollowBehaviour::ComputeColliderAvoidanceGradient(const FTGOR_MovementDynamic& Dynamic, const TArray<FTGOR_MovementObstacle>& Colliders, float MaxTime) const
{
	FVector Gradient = FVector::ZeroVector;
	for (const FTGOR_MovementObstacle& Collider : Colliders)
	{
		const float Time = ComputeClosestTime(Dynamic, Collider.Dynamic);
		if (KINDA_SMALL_NUMBER < Time && Time < MaxTime)
		{
			Gradient += ComputeClosestGradient(Dynamic, Collider);
		}
	}
	return Gradient;
}