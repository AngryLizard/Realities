// The Gateway of Realities: Planes of Existence.

#include "TGOR_RigidPawnComponent.h"
#include "Components/PrimitiveComponent.h"
#include "GameFramework/Actor.h"

#include "DimensionSystem/Components/TGOR_PilotComponent.h"
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

DECLARE_FLOAT_COUNTER_STAT(TEXT("Movement damping factor"), STAT_DampingFactor, STATGROUP_RIGID);
DECLARE_DWORD_COUNTER_STAT(TEXT("Simulations per tick"), STAT_SimulationCount, STATGROUP_RIGID);

UTGOR_RigidPawnComponent::UTGOR_RigidPawnComponent()
:	Super(),

	MaxDampingStratification(5.0f),
	SweepsCollision(true),
	OrientationSpeed(10.0f),
	SimulationTimestep(0.0f)
{
}

void UTGOR_RigidPawnComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// We always stratify on very slow frames (other parts of the game will take the hit, surely)
	// Otherwise some parts of the simulation might go crazy and go Nan
	TickStratified(DeltaTime, FMath::Min(DeltaTime, 0.05f));

	INC_DWORD_STAT_BY(STAT_SimulationCount, SimulationsPerTick);
	SimulationsPerTick = 0;
}

void UTGOR_RigidPawnComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// Replicate to everyone
	DOREPLIFETIME_CONDITION(UTGOR_RigidPawnComponent, Capture, COND_None);
}

/*
void UTGOR_RigidPawnComponent::InflictPointImpact(const FVector& Point, const FVector& Impulse)
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

void UTGOR_RigidPawnComponent::InflictPointForce(const FVector& Point, const FVector& Force, float DeltaTime)
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
*/

////////////////////////////////////////////////////////////////////////////////////////////////////

FVector UTGOR_RigidPawnComponent::ComputePhysicsUpVector() const
{
	// Stored in local space
	UTGOR_PilotComponent* RootPilot = GetRootPilot();
	if (IsValid(RootPilot))
	{
		const FTGOR_MovementPosition Position = RootPilot->ComputeBase();
		return Position.Angular * Capture.UpVector;
	}
	return Capture.UpVector;
}

AActor* UTGOR_RigidPawnComponent::GetPilotOwner() const
{
	return GetOwner();
}

float UTGOR_RigidPawnComponent::TickPhysics(float Time)
{
	SCOPE_CYCLE_COUNTER(STAT_Simulation);

	float Energy = 0.0f;
	float Remaining = Time;

	UTGOR_PilotComponent* RootPilot = GetRootPilot();
	if (IsValid(RootPilot))
	{
		// Recompute move ignore list
		RootPilot->OverlapIgnoreList();

		const FTGOR_MovementBody& MovementBody = RootPilot->GetBody();
		FTGOR_MovementSpace Space = RootPilot->ComputeSpace();

		// Simulate pending simulation steps with temporal state
		FTGOR_MovementTick Tick;
		while (PopulateMovementTick(Remaining, Tick))
		{
			SimulationsPerTick++;

			// Compute spatial state
			PreComputePhysics(Tick);

			// Compute surrounding physical properties
			if (RootPilot->SurroundingVolume.IsValid())
			{
				// Compute external forces
				FTGOR_MovementOutput Out;
				FTGOR_MovementExternal External;
				ComputeExternal(Space, External, Out);

				// Consume external impulses
				const FVector InflictedLinearVelocity = MovementBody.GetUnmassedLinear(Inflicted.LinearMomentum);
				const FVector InflictedAngularVelocity = MovementBody.GetUnmassedAngular(Space.Angular, Inflicted.AngularMomentum);

				Space.AddLinearVelocity(InflictedLinearVelocity + Space.LinearAcceleration * Tick.DeltaTime);
				Space.AddAngularVelocity(InflictedAngularVelocity + Space.AngularAcceleration * Tick.DeltaTime);

				Inflicted.LinearMomentum = FVector::ZeroVector;
				Inflicted.AngularMomentum = FVector::ZeroVector;
		
				////////////////////////////////////////////////////////////////////////////////////////////
				{
					SCOPE_CYCLE_COUNTER(STAT_Physics);

					// Compute forces and such
					ComputePhysics(Space, External, Tick, Out);

					/*
					// Compute damping stresses, slowing down deltatime when over 1.0 to prevent oscillation
					const float MinInertia = MovementBody.Inertia.GetAbsMin();
					if (MovementBody.Mass >= SMALL_NUMBER && MinInertia >= SMALL_NUMBER)
					{
						// Damping is the factor that was applied to velocity, meaning if it gets bigger than 1.0 we oscillate
						const float LinearDampingStress = Out.MaxLinearDamping / MovementBody.Mass;
						const float AngularDampingStress = Out.MaxAngularDamping / MinInertia;
						const float TimeStress = FMath::Max(LinearDampingStress, AngularDampingStress) * Tick.DeltaTime;
						if (TimeStress > 1.0f)
						{
							INC_FLOAT_STAT_BY(STAT_DampingFactor, TimeStress);
							Tick.DeltaTime /= FMath::Min(TimeStress, MaxDampingStratification);
						}
					}
					*/

					External.UpVector = UTGOR_Math::Normalize(FMath::VInterpConstantTo(External.UpVector, -Out.Orientation, Tick.DeltaTime, OrientationSpeed));

					// Store upvector in local space. Base could have changed during ComputePhysics so we need to recompute it.
					const FTGOR_MovementPosition Position = RootPilot->ComputeBase();
					Capture.UpVector = Position.Angular.Inverse() * External.UpVector;
				}

				// Compute used energy for force and torque (Approximated with forward euler E=|F|*dx and E=|T|*da)
				const float LinearEnergy = Out.Force.Size() * Space.RelativeLinearVelocity.Size() * Tick.DeltaTime;
				const float AngularEnergy = Out.Torque.Size() * Space.RelativeAngularVelocity.Size() * Tick.DeltaTime;
				Energy += LinearEnergy + AngularEnergy;
			}

			Remaining -= Tick.DeltaTime;
		}

		PostComputePhysics(Space, Energy, Remaining);
	}
	return Time - Remaining;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void UTGOR_RigidPawnComponent::RepNotifyCapture(const FTGOR_MovementCapture& Old)
{
}

const FTGOR_MovementCapture& UTGOR_RigidPawnComponent::GetCapture() const
{
	return Capture;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

bool UTGOR_RigidPawnComponent::PopulateMovementTick(float Time, FTGOR_MovementTick& OutTick) const
{
	UTGOR_PilotComponent* RootPilot = GetRootPilot();
	if (IsValid(RootPilot))
	{
		// Initial max damping assumes 1024 fps
		const FTGOR_MovementBody& MovementBody = RootPilot->GetBody();
		OutTick.MaxLinearDamping = MovementBody.Mass * 1024.0f;
		OutTick.MaxAngularDamping = MovementBody.Inertia.GetAbsMin() * 1024.0f;
	}

	// Adaptive timestep
	if (FMath::IsNearlyZero(SimulationTimestep))
	{
		// We always stratify on very slow frames (other parts of the game will take the hit, surely)
		// Otherwise some parts of the simulation might flip their shit and go Nan
		OutTick.DeltaTime = FMath::Min(Time, 0.033f);
		return (OutTick.DeltaTime >= SMALL_NUMBER);
	}
	else
	{
		OutTick.DeltaTime = SimulationTimestep;
		return Time >= SimulationTimestep;
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void UTGOR_RigidPawnComponent::InflictLinearMomentum(const FTGOR_MovementDynamic& Dynamic, FVector InMomentum, bool CounteractVelocity)
{
	// Make sure velocity doesn't get counted "double" due to replication
	UTGOR_PilotComponent* RootPilot = GetRootPilot();
	if (CanInflict() && IsValid(RootPilot))
	{
		Inflicted.LinearMomentum += InMomentum;
		if (CounteractVelocity)
		{
			const FTGOR_MovementBody& MovementBody = RootPilot->GetBody();
			const FVector LinearMomentum = MovementBody.GetMassedLinear(Dynamic.LinearVelocity);
			Inflicted.LinearMomentum -= LinearMomentum.ProjectOnTo(InMomentum);
		}
	}
}

void UTGOR_RigidPawnComponent::InflictAngularMomentum(const FTGOR_MovementDynamic& Dynamic, FVector InAngular, bool CounteractAngular)
{
	// Make sure velocity doesn't get counted "double" due to replication
	UTGOR_PilotComponent* RootPilot = GetRootPilot();
	if (CanInflict() && IsValid(RootPilot))
	{
		Inflicted.AngularMomentum += InAngular;
		if (CounteractAngular)
		{
			const FTGOR_MovementBody& MovementBody = RootPilot->GetBody();
			const FVector AngularMomentum = MovementBody.GetMassedLinear(Dynamic.AngularVelocity);
			Inflicted.AngularMomentum -= AngularMomentum.ProjectOnTo(InAngular);
		}
	}
}

void UTGOR_RigidPawnComponent::InflictForceTorque(const FVector& Force, const FVector& Torque, float DeltaTime)
{
	if (CanInflict())
	{
		Inflicted.LinearMomentum += Force * DeltaTime;
		Inflicted.AngularMomentum += Torque * DeltaTime;
	}
}

void UTGOR_RigidPawnComponent::ComputeExternal(const FTGOR_MovementSpace& Space, FTGOR_MovementExternal& External, FTGOR_MovementOutput& Out)
{
	UTGOR_PilotComponent* RootPilot = GetRootPilot();
	if (IsValid(RootPilot))
	{
		const FTGOR_MovementBody& MovementBody = RootPilot->GetBody();

		External.Surroundings = RootPilot->SurroundingVolume->ComputeAllSurroundings(Space.Linear);

		const FTGOR_MovementDamper ExternalForce = MovementBody.ComputeExternalForce(Space, External.Surroundings);
		External.Force = ExternalForce.Vector;

		const FTGOR_MovementDamper ExternalTorque = MovementBody.ComputeExternalTorque(Space, External.Surroundings);
		External.Torque = ExternalTorque.Vector;

		External.UpVector = ComputePhysicsUpVector();

		// Initialise movement output
		Out.MaxLinearDamping = ExternalForce.Damping;
		Out.MaxAngularDamping = ExternalTorque.Damping;
		Out.Orientation = (External.Surroundings.Gravity * MovementBody.Mass) + MovementBody.GetMassedLinear(Space.RelativeLinearAcceleration);
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void UTGOR_RigidPawnComponent::PreComputePhysics(const FTGOR_MovementTick& Tick)
{
}

void UTGOR_RigidPawnComponent::ComputePhysics(FTGOR_MovementSpace& Space, const FTGOR_MovementExternal& External, const FTGOR_MovementTick& Tick, FTGOR_MovementOutput& Output)
{
}

void UTGOR_RigidPawnComponent::PostComputePhysics(const FTGOR_MovementSpace& Space, float Energy, float DeltaTime)
{
	if (Energy > SMALL_NUMBER)
	{
		//OnEnergyUsage.Broadcast(Energy);
	}
}

bool UTGOR_RigidPawnComponent::CanInflict() const
{
	return true;
}

