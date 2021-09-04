// The Gateway of Realities: Planes of Existence.

#include "TGOR_RigidComponent.h"
#include "Components/PrimitiveComponent.h"
#include "GameFramework/Actor.h"

#include "DimensionSystem/Tasks/TGOR_EuclideanPilotTask.h"
#include "DimensionSystem/Tasks/TGOR_PilotTask.h"
#include "DimensionSystem/Content/TGOR_Pilot.h"

#include "CoreSystem/TGOR_Singleton.h"
#include "DimensionSystem/Data/TGOR_WorldData.h"
#include "DimensionSystem/Volumes/TGOR_PhysicsVolume.h"
#include "RealitiesUtility/Utility/TGOR_Math.h"
#include "DrawDebugHelpers.h"


DECLARE_CYCLE_STAT(TEXT("Simulation"), STAT_Simulation, STATGROUP_RIGID);
DECLARE_CYCLE_STAT(TEXT("Physics"), STAT_Physics, STATGROUP_RIGID);
DECLARE_CYCLE_STAT(TEXT("Move"), STAT_Move, STATGROUP_RIGID);

DECLARE_DWORD_COUNTER_STAT(TEXT("Collision iterations"), STAT_Iteration, STATGROUP_RIGID);
DECLARE_DWORD_COUNTER_STAT(TEXT("Movement pullback count"), STAT_Pullback, STATGROUP_RIGID);
DECLARE_DWORD_COUNTER_STAT(TEXT("Movement translation count"), STAT_Translation, STATGROUP_RIGID);

DECLARE_FLOAT_COUNTER_STAT(TEXT("Movement damping factor"), STAT_DampingFactor, STATGROUP_RIGID);

UTGOR_RigidComponent::UTGOR_RigidComponent()
:	Super(),

	MaxCollisionIterations(3),
	MaxDampingStratification(5.0f),
	SweepsCollision(true),
	StratisfyTimestep(false),
	OrientationSpeed(2.0f),
	
	CollisionWeight(1.0f),

	LinearSpeedClamp(10'000.0f),
	AngularSpeedClamp(50.0f)
{
}

void UTGOR_RigidComponent::InflictPointImpact(const FVector& Point, const FVector& Impulse)
{
	Super::InflictPointImpact(Point, Impulse);

	const FTGOR_MovementDynamic Dynamic = ComputeSpace();
	Impact(Dynamic, Point, Impulse);

	if (CanRotateOnImpact())
	{
		const FVector Offset = (Point - Dynamic.Linear) / 10'000.0f;
		InflictAngularMomentum(Dynamic, Offset ^ Impulse, false);
	}
	InflictLinearMomentum(Dynamic, Impulse, false);
}

void UTGOR_RigidComponent::InflictPointForce(const FVector& Point, const FVector& Force, float DeltaTime)
{
	Super::InflictPointForce(Point, Force, DeltaTime);

	FVector Torque = FVector::ZeroVector;
	if (CanRotateOnImpact())
	{
		const FTGOR_MovementPosition Position = ComputePosition();
		const FVector Offset = (Point - Position.Linear) / 10'000.0f;
		Torque = Offset ^ Force;
	}
	InflictForceTorque(Force, Torque, DeltaTime);
}

float UTGOR_RigidComponent::TickPhysics(float Time)
{
	SCOPE_CYCLE_COUNTER(STAT_Simulation);
	Time = Super::TickPhysics(Time);

	// Recompute move ignore list
	OverlapIgnoreList();

	/*
	if (IsA(UTGOR_MovementComponent::StaticClass()))
	{
		RPORT("--------------------------");
		VPORT(GetOwner());
	}
	*/
	float Energy = 0.0f;
	float Remaining = Time;

	const FTGOR_MovementBody& MovementBody = GetBody();
	FTGOR_MovementSpace Space = ComputeSpace();

	// Simulate pending simulation steps with temporal state
	FTGOR_MovementTick Tick;
	while (PopulateMovementTick(Remaining, Tick))
	{
		// Compute spatial state
		PreComputePhysics(Tick);

		// Compute external forces
		const FTGOR_MovementDamper ExternalForce = MovementBody.ComputeExternalForce(Space, Capture.Surroundings);
		const FTGOR_MovementDamper ExternalTorque = MovementBody.ComputeExternalTorque(Space, Capture.Surroundings);

		FTGOR_MovementExternal External;
		External.Force = ExternalForce.Vector;
		External.Torque = ExternalTorque.Vector;
		External.UpVector = ComputePhysicsUpVector();

		// Consume external impulses
		const FVector InflictedLinearVelocity = MovementBody.GetUnmassedLinear(Inflicted.LinearMomentum);
		const FVector InflictedAngularVelocity = MovementBody.GetUnmassedAngular(Space.Angular, Inflicted.AngularMomentum);

		Space.AddLinearVelocity(InflictedLinearVelocity + Space.LinearAcceleration * Tick.Deltatime);
		Space.AddAngularVelocity(InflictedAngularVelocity + Space.AngularAcceleration * Tick.Deltatime);

		Inflicted.LinearMomentum = FVector::ZeroVector;
		Inflicted.AngularMomentum = FVector::ZeroVector;

		// Initialise movement output
		FTGOR_MovementOutput Out;
		Out.MaxLinearDamping = ExternalForce.Damping;
		Out.MaxAngularDamping = ExternalTorque.Damping;
		Out.Orientation = (Capture.Surroundings.Gravity * MovementBody.Mass) + MovementBody.GetMassedLinear(Space.RelativeLinearAcceleration);
		
		////////////////////////////////////////////////////////////////////////////////////////////
		{
			SCOPE_CYCLE_COUNTER(STAT_Physics);

			// Compute forces and such
			ComputePhysics(Space, External, Tick, Out);

			// Compute damping stresses, slowing down deltatime when over 1.0 to prevent oscillation
			const float MinInertia = MovementBody.Inertia.GetAbsMin();
			if (MovementBody.Mass >= SMALL_NUMBER && MinInertia >= SMALL_NUMBER)
			{
				// Damping is the factor that was applied to velocity, meaning if it gets bigger than 1.0 we oscillate
				const float LinearDampingStress = Out.MaxLinearDamping / MovementBody.Mass;
				const float AngularDampingStress = Out.MaxAngularDamping / MinInertia;
				const float TimeStress = FMath::Max(LinearDampingStress, AngularDampingStress) * Tick.Deltatime;
				if (TimeStress > 1.0f)
				{
					INC_FLOAT_STAT_BY(STAT_DampingFactor, TimeStress);
					Tick.Deltatime /= FMath::Min(TimeStress, MaxDampingStratification);
				}
			}

			External.UpVector = UTGOR_Math::Normalize(FMath::VInterpConstantTo(External.UpVector, -Out.Orientation, Tick.Deltatime, OrientationSpeed));

			// Store upvector in local space
			UTGOR_PilotTask* PilotTask = GetPilotTask();
			if (IsValid(PilotTask))
			{
				const FTGOR_MovementPosition Position = PilotTask->ComputeBase();
				Capture.UpVector = Position.Angular.Inverse() * External.UpVector;
			}
		}

		// Compute used energy for force and torque (Approximated with forward euler E=|F|*dx and E=|T|*da)
		const float LinearEnergy = Out.Force.Size() * Space.RelativeLinearVelocity.Size() * Tick.Deltatime;
		const float AngularEnergy = Out.Torque.Size() * Space.RelativeAngularVelocity.Size() * Tick.Deltatime;
		Energy += LinearEnergy + AngularEnergy;

		Remaining -= Tick.Deltatime;
	}

	PostComputePhysics(Space, Energy, Remaining);
	return Time - Remaining;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void UTGOR_RigidComponent::GetDampingForce(const FTGOR_MovementTick& Tick, const FVector& LinearVelocity, float Damping, FTGOR_MovementOutput& Output) const
{
	const float FinalDamping = FMath::Min(Tick.MaxLinearDamping, Damping);
	Output.MaxLinearDamping = FMath::Max(Output.MaxLinearDamping, FinalDamping);
	Output.Force -= LinearVelocity * FinalDamping;
}

void UTGOR_RigidComponent::GetDampingTorque(const FTGOR_MovementTick& Tick, const FVector& AngularVelocity, float Damping, FTGOR_MovementOutput& Output) const
{
	const float FinalDamping = FMath::Min(Tick.MaxAngularDamping, Damping);
	Output.MaxAngularDamping = FMath::Max(Output.MaxAngularDamping, FinalDamping);
	Output.Torque -= AngularVelocity * FinalDamping;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

bool UTGOR_RigidComponent::PopulateMovementTick(float Time, FTGOR_MovementTick& OutTick) const
{
	// Initial max damping assumes 1024 fps
	const FTGOR_MovementBody& MovementBody = GetBody();
	OutTick.MaxLinearDamping = MovementBody.Mass * 1024.0f;
	OutTick.MaxAngularDamping = MovementBody.Inertia.GetAbsMin() * 1024.0f;

	// Adaptive timestep
	if (StratisfyTimestep)
	{
		OutTick.Deltatime = SimulationTimestep;
		return (Time >= SimulationTimestep);
	}
	else
	{
		// Stratisfy always on very slow frames (other parts of the game will take the hit mkay?)
		// Otherwise some of the simulation will go Nan-bread
		OutTick.Deltatime = FMath::Min(Time, 0.05f);
		return (OutTick.Deltatime >= SMALL_NUMBER);
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void UTGOR_RigidComponent::InflictLinearMomentum(const FTGOR_MovementDynamic& Dynamic, FVector InMomentum, bool CounteractVelocity)
{
	// Make sure velocity doesn't get counted "double" due to replication
	if (CanInflict())
	{
		Inflicted.LinearMomentum += InMomentum;
		if (CounteractVelocity)
		{
			const FTGOR_MovementBody& MovementBody = GetBody();
			const FVector LinearMomentum = MovementBody.GetMassedLinear(Dynamic.LinearVelocity);
			Inflicted.LinearMomentum -= LinearMomentum.ProjectOnTo(InMomentum);
		}
	}
}

void UTGOR_RigidComponent::InflictAngularMomentum(const FTGOR_MovementDynamic& Dynamic, FVector InAngular, bool CounteractAngular)
{
	// Make sure velocity doesn't get counted "double" due to replication
	if (CanInflict())
	{
		Inflicted.AngularMomentum += InAngular;
		if (CounteractAngular)
		{
			const FTGOR_MovementBody& MovementBody = GetBody();
			const FVector AngularMomentum = MovementBody.GetMassedLinear(Dynamic.AngularVelocity);
			Inflicted.AngularMomentum -= AngularMomentum.ProjectOnTo(InAngular);
		}
	}
}

void UTGOR_RigidComponent::InflictForceTorque(const FVector& Force, const FVector& Torque, float DeltaTime)
{
	if (CanInflict())
	{
		Inflicted.LinearMomentum += Force * DeltaTime;
		Inflicted.AngularMomentum += Torque * DeltaTime;
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void UTGOR_RigidComponent::PreComputePhysics(const FTGOR_MovementTick& Tick)
{
}

void UTGOR_RigidComponent::ComputePhysics(FTGOR_MovementSpace& Space, const FTGOR_MovementExternal& External, const FTGOR_MovementTick& Tick, FTGOR_MovementOutput& Output)
{
}

void UTGOR_RigidComponent::PostComputePhysics(const FTGOR_MovementSpace& Space, float Energy, float DeltaTime)
{
	if (Energy > SMALL_NUMBER)
	{
		OnEnergyUsage.Broadcast(Energy);
	}
}

bool UTGOR_RigidComponent::CanInflict() const
{
	return true;
}

void UTGOR_RigidComponent::SimulateSymplectic(FTGOR_MovementSpace& Space, const FTGOR_MovementForce& Force, const FTGOR_MovementExternal& External, float Timestep, bool Sweep)
{
	// Make sure we aren't introducing NaNs
	if (Force.Force.ContainsNaN())
	{
		ERROR("Output force NAN!", Error);
	}

	if (Force.Torque.ContainsNaN())
	{
		ERROR("Output torque NAN!", Error);
	}

	const FTGOR_MovementBody& MovementBody = GetBody();

	// Consume force
	Space.LinearAcceleration = MovementBody.GetUnmassedLinear(Force.Force + External.Force);
	Space.AngularAcceleration = MovementBody.GetUnmassedAngular(Space.Angular, Force.Torque + External.Torque);

	// Update velocities
	Space.AddLinearVelocity(Space.LinearAcceleration * Timestep);
	Space.AddAngularVelocity(Space.AngularAcceleration * Timestep);

	// Make sure velocity is not exploding by clamping it
	Space.SetLinearVelocity(UTGOR_Math::ClampToSize(Space.LinearVelocity, LinearSpeedClamp));
	Space.SetAngularVelocity(UTGOR_Math::ClampToSize(Space.AngularVelocity, AngularSpeedClamp));

	// Simulate move
	SimulateMove(Space, Timestep, Sweep);
}

void UTGOR_RigidComponent::SimulateMove(FTGOR_MovementSpace& Space, float Timestep, bool Sweep)
{
	SCOPE_CYCLE_COUNTER(STAT_Move);

	// Make sure updated component is at the right location

	if (Timestep >= SMALL_NUMBER)
	{
		INC_DWORD_STAT_BY(STAT_Iteration, 1)

		// Compute translation
		const int32 Iterations = SimulateTranslate(Space, Timestep, Sweep, 1.0f, MaxCollisionIterations);
		
		Space.Linear = GetComponentLocation();
		Space.Angular = GetComponentQuat();
	}
	else
	{
		// Update position
		SetWorldLocationAndRotation(Space.Linear, Space.Angular, false, nullptr, ETeleportType::TeleportPhysics);
	}
}

int32 UTGOR_RigidComponent::SimulateTranslate(FTGOR_MovementSpace& Space, float Timestep, bool Sweep, float Ratio, int32 Iteration)
{
	INC_DWORD_STAT_BY(STAT_Translation, 1)

	AActor* const Actor = GetOwner();

	if (Ratio >= SMALL_NUMBER && Iteration >= 0)
	{
		// Rotate base
		const FVector W = Space.RelativeAngularVelocity * (Timestep / 2);
		const FQuat DeltaQuat = FQuat(W.X, W.Y, W.Z, 0.0f);
		Space.Angular = Space.Angular + (DeltaQuat * Ratio) * Space.Angular;
		Space.Angular.Normalize();

		// Initialise hitresult
		FHitResult Hit(NoInit);
		Hit.bBlockingHit = false;
		Hit.Time = 1.f;

		// Move and rotate component (Copied from SafeMove but without recomputing hitresult)

		// Compute translation
		const FVector Translation = Space.RelativeLinearVelocity * Ratio * Timestep;
		//const FCollisionShape& CollisionShape = GetCollisionShape();
		//ProbeSweep(CollisionShape, Space, Translation);

		SimulateSweep(Space, Translation, Hit);
		if(PullBack(Space, Hit, Translation))
		{
			INC_DWORD_STAT_BY(STAT_Pullback, 1)

			// Retry move after having been pulled back
			SimulateSweep(Space, Translation, Hit);
		}

		////SafeMoveUpdatedComponent(Translation, Base.Rotation, SweepsCollision, Hit, ETeleportType::None);
		//const EMoveComponentFlags IncludeBlockingOverlapsWithoutEvents = (MOVECOMP_NeverIgnoreBlockingOverlaps | MOVECOMP_DisableBlockingOverlapDispatch);
		//MoveComponent(Translation, Space.Angular, SweepsCollision && Sweep, &Hit, MOVECOMP_NoFlags, ETeleportType::None);

		if (Collide(Space, Hit))
		{
			// Unrotate base
			const float Rest = FMath::Max(Ratio - Hit.Time, 0.0f);
			Space.Angular = Space.Angular - (DeltaQuat * Rest) * Space.Angular;
			Space.Angular.Normalize();

			// Slide further along surface
			return SimulateTranslate(Space, Timestep, Sweep, Rest, Iteration - 1);
		}
	}
	return Iteration;
}

bool UTGOR_RigidComponent::PullBack(FTGOR_MovementSpace& Space, const FHitResult& OutHit, const FVector& Translation)
{
	// Pullback mostly copied from MovementComponent::ResolvePenetrationImpl
	//Disabled for better handling of moving obstacles
	if (OutHit.bStartPenetrating)
	{
		const FVector Adjustment = GetPenetrationAdjustment(OutHit);
		if (!Adjustment.IsZero())
		{
			FOverlapResult Overlap;
			if (SimulateOverlap(Space, Adjustment - Translation, Overlap))
			{
				// Try sweeping as far as possible...
				FHitResult SweepOutHit(1.f);
				bool bMoved = SimulateSweep(Space, Adjustment, SweepOutHit);

				// Still stuck?
				if (!bMoved && SweepOutHit.bStartPenetrating)
				{
					// Combine two MTD results to get a new direction that gets out of multiple surfaces.
					const FVector SecondMTD = GetPenetrationAdjustment(SweepOutHit);
					const FVector CombinedMTD = Adjustment + SecondMTD;
					if (SecondMTD != Adjustment && !CombinedMTD.IsZero())
					{
						bMoved = SimulateSweep(Space, CombinedMTD, SweepOutHit);
					}
				}

				// Still stuck?
				if (!bMoved)
				{
					// Try moving the proposed adjustment plus the attempted move direction. This can sometimes get out of penetrations with multiple objects
					if (!Translation.IsZero())
					{
						bMoved = SimulateSweep(Space, Adjustment + Translation, SweepOutHit);

						// Finally, try the original move without MTD adjustments, but allowing depenetration along the MTD normal.
						// This was blocked because MOVECOMP_NeverIgnoreBlockingOverlaps was true for the original move to try a better depenetration normal, but we might be running in to other geometry in the attempt.
						// This won't necessarily get us all the way out of penetration, but can in some cases and does make progress in exiting the penetration.
						if (!bMoved && FVector::DotProduct(Translation, Adjustment) > 0.f)
						{
							bMoved = SimulateSweep(Space, Translation, SweepOutHit);
						}
					}
				}

				// Still stuck?
				if (!bMoved)
				{
					// Try moving the proposed adjustment plus the attempted move direction. This can sometimes get out of penetrations with multiple objects
					if (!Translation.IsZero())
					{
						bMoved = SimulateSweep(Space, Adjustment + Translation, SweepOutHit);
					}
				}
				return bMoved;
			}
			else
			{
				// Move without sweeping.
				MoveComponent(Adjustment, Space.Angular, false, nullptr, MOVECOMP_NoFlags, ETeleportType::TeleportPhysics);
				return true;
			}
		}
	}
	return false;
}


FTGOR_MovementParent UTGOR_RigidComponent::FindReparentToActor(AActor* Actor, const FName& Name) const
{
	FTGOR_MovementParent Parent;
	if (IsValid(Actor))
	{
		// Only grab pilot components
		// TODO: Make interface to look for specific bones
		Parent.Mobility = Actor->FindComponentByClass<UTGOR_MobilityComponent>();
		if (IsValid(Parent.Mobility))
		{
			Parent.Index = Parent.Mobility->GetIndexFromName(Name);
			return Parent;
		}
	}

	if (SurroundingVolume.IsValid())
	{
		// Assume level volume if no contact was established
		Parent.Mobility = SurroundingVolume->GetMovement();
		Parent.Index = Parent.Mobility->GetIndexFromName(Name);
	}
	return Parent;
}

FTGOR_MovementParent UTGOR_RigidComponent::FindReparentToComponent(UActorComponent* Component, const FName& Name) const
{
	if (IsValid(Component))
	{
		return FindReparentToActor(Component->GetOwner(), Name);
	}
	else
	{
		return FindReparentToActor(nullptr, Name);
	}
}
