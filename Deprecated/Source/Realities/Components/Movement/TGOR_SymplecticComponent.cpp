// The Gateway of Realities: Planes of Existence.

#include "TGOR_SymplecticComponent.h"
#include "Components/PrimitiveComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/Actor.h"
#include "Net/UnrealNetwork.h"

#include "Realities/Base/TGOR_Singleton.h"
#include "Realities/Base/System/Data/TGOR_WorldData.h"
#include "Realities/Actors/Dimension/Volumes/TGOR_PhysicsVolume.h"
#include "Realities/Utility/TGOR_Math.h"
#include "Realities/Utility/Structs/TGOR_Direction.h"
#include "DrawDebugHelpers.h"

#include "GameFramework/WorldSettings.h"

DECLARE_CYCLE_STAT(TEXT("Simulation"), STAT_Simulation, STATGROUP_SYMPLECTIC);
DECLARE_CYCLE_STAT(TEXT("Physics"), STAT_Physics, STATGROUP_SYMPLECTIC);
DECLARE_CYCLE_STAT(TEXT("Move"), STAT_Move, STATGROUP_SYMPLECTIC);

DECLARE_DWORD_COUNTER_STAT(TEXT("Collision iterations"), STAT_Iteration, STATGROUP_SYMPLECTIC);
DECLARE_DWORD_COUNTER_STAT(TEXT("Movement pullback count"), STAT_Pullback, STATGROUP_SYMPLECTIC);
DECLARE_DWORD_COUNTER_STAT(TEXT("Movement translation count"), STAT_Translation, STATGROUP_SYMPLECTIC);

DECLARE_FLOAT_COUNTER_STAT(TEXT("Movement damping factor"), STAT_DampingFactor, STATGROUP_SYMPLECTIC);

UTGOR_SymplecticComponent::UTGOR_SymplecticComponent()
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

void UTGOR_SymplecticComponent::InflictPointImpact(const FVector& Point, const FVector& Impulse, int32 Index)
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

void UTGOR_SymplecticComponent::InflictPointForce(const FVector& Point, const FVector& Force, float DeltaTime, int32 Index)
{
	Super::InflictPointForce(Point, Force, Index);

	FVector Torque = FVector::ZeroVector;
	if (CanRotateOnImpact())
	{
		const FTGOR_MovementPosition Position = ComputePosition();
		const FVector Offset = (Point - Position.Linear) / 10'000.0f;
		Torque = Offset ^ Force;
	}
	InflictForceTorque(Force, Torque, DeltaTime);
}

float UTGOR_SymplecticComponent::Simulate(float Time, bool Replay)
{
	SCOPE_CYCLE_COUNTER(STAT_Simulation);

	// Recompute move ignore list
	OverlapIgnoreList();

	// Simulate pending simulation steps with temporal state
	FTGOR_MovementTick Tick;
	while (PopulateMovementTick(Time, Tick))
	{
		const FTGOR_MovementBody& MovementBody = GetBody();
		FTGOR_MovementSpace Space = ComputeSpace();

		// Compute spatial state
		PreComputePhysics(Tick, Replay);

		// Compute external forces
		const FTGOR_MovementDamper ExternalForce = MovementBody.ComputeExternalForce(Space, Capture.Surroundings);
		const FTGOR_MovementDamper ExternalTorque = MovementBody.ComputeExternalTorque(Space, Capture.Surroundings);

		FTGOR_MovementExternal External;
		External.Force = ExternalForce.Vector;
		External.Torque = ExternalTorque.Vector;

		// Compute upvector in world space
		const FTGOR_MovementBase& MovementBase = GetBase();
		const FTGOR_MovementPosition ParentPostion = MovementBase.ComputeParentPosition();
		External.UpVector = ParentPostion.Angular * Capture.UpVector;

		// Initialise movement output
		FTGOR_MovementOutput Out;
		Out.MaxLinearDamping = ExternalForce.Damping;
		Out.MaxAngularDamping = ExternalTorque.Damping;
		Out.Index = MovementBase.Index.Index;
		Out.Base = MovementBase.Parent.Get();
		Out.BaseImpactPoint = Space.Linear;
		Out.Orientation = Capture.Surroundings.Gravity; //External.Force;

		////////////////////////////////////////////////////////////////////////////////////////////
		{
			SCOPE_CYCLE_COUNTER(STAT_Physics);

			// Compute forces and such
			ComputePhysics(Tick, Space, External, Replay, Out);
		}

		// Assume level volume if no base was defined
		if (!Out.Base)
		{
			Out.SetBaseFromVolume(Capture.PhysicsVolume.Get(), "");
		}

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


		////////////////////////////////////////////////////////////////////////////////////////////

		if (!IsAttached())
		{
			// Process new base and apply reaction force to base
			if (IsValid(Out.Base) && !Replay)
			{
				Out.Base->InflictPointForce(Out.BaseImpactPoint, -Out.Force, Tick.Deltatime, Out.Index);
			}

			// Compute final force
			Out.Force += External.Force;
			Out.Torque += External.Torque;

			if (Out.Force.ContainsNaN())
			{
				Out.Force = FVector::ZeroVector;
				RPORT("World force NAN!");
			}
			if (Out.Torque.ContainsNaN())
			{
				Out.Torque = FVector::ZeroVector;
				RPORT("World torque NAN!");
			}

			// Consume force
			Space.LinearAcceleration = MovementBody.GetUnmassedLinear(Out.Force);
			Space.AngularAcceleration = MovementBody.GetUnmassedAngular(Space.Angular, Out.Torque);

			// Update velocities
			const FVector InflictedLinearVelocity = MovementBody.GetUnmassedLinear(Inflicted.LinearMomentum);
			const FVector InflictedAngularVelocity = MovementBody.GetUnmassedAngular(Space.Angular, Inflicted.AngularMomentum);

			Space.AddLinearVelocity(InflictedLinearVelocity + Space.LinearAcceleration * Tick.Deltatime);
			Space.AddAngularVelocity(InflictedAngularVelocity + Space.AngularAcceleration * Tick.Deltatime);

			// Make sure velocity is not exploding by clamping it
			Space.SetLinearVelocity(UTGOR_Math::ClampToSize(Space.LinearVelocity, LinearSpeedClamp));
			Space.SetAngularVelocity(UTGOR_Math::ClampToSize(Space.AngularVelocity, AngularSpeedClamp));

			// Simulate move
			SimulateMove(Space, Tick.Deltatime, Out.CollisionEnabled, Replay);

			// Switch spaces and compute new base if necessary
			SetParent(Out.Base, nullptr, Out.Index);

			// TODO: Performance improvement possible since parent is already computed in SetParent, or just taking already computed parent space from before
			SimulateDynamic(Space);
		}
		else
		{
			// Compute final force
			Out.Force += External.Force;
			Out.Torque += External.Torque;

			// Relay to attachment parent
			if (MovementBase.Parent.IsValid() && !Replay)
			{
				Out.Base->InflictPointForce(Space.Linear, Out.Force, Tick.Deltatime, MovementBase.Index.Index);
			}

			// Don't move but update components
			SimulateMove(Space, 0.0f, Out.CollisionEnabled, true);
		}

		// Store in local space
		const FTGOR_MovementPosition NewParentPostion = MovementBase.ComputeParentPosition();
		const FQuat ParentAngularInv = NewParentPostion.Angular.Inverse();
		Capture.UpVector = ParentAngularInv * External.UpVector;

		Time -= Tick.Deltatime;

		// Reset inflicted velocities
		Inflicted.LinearMomentum = FVector::ZeroVector;
		Inflicted.AngularMomentum = FVector::ZeroVector;


		if (GetOwnerRole() == ENetRole::ROLE_AutonomousProxy)
		{
			//DrawDebugLine(GetWorld(), Space.Linear, Space.Linear + External.UpVector * 200.0f, FColor::Blue, false, -1.0f, 0, 5.0f);
		}
	}

	return Time;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void UTGOR_SymplecticComponent::GetLinearDamping(const FTGOR_MovementTick& Tick, const FVector& LinearVelocity, float Damping, FTGOR_MovementOutput& Output) const
{
	const float FinalDamping = FMath::Min(Tick.MaxLinearDamping, Damping);
	Output.MaxLinearDamping = FMath::Max(Output.MaxLinearDamping, FinalDamping);
	Output.Force -= LinearVelocity * FinalDamping;
}

void UTGOR_SymplecticComponent::GetAngularDamping(const FTGOR_MovementTick& Tick, const FVector& AngularVelocity, float Damping, FTGOR_MovementOutput& Output) const
{
	const float FinalDamping = FMath::Min(Tick.MaxAngularDamping, Damping);
	Output.MaxAngularDamping = FMath::Max(Output.MaxAngularDamping, FinalDamping);
	Output.Torque -= AngularVelocity * FinalDamping;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

bool UTGOR_SymplecticComponent::PopulateMovementTick(float Time, FTGOR_MovementTick& OutTick) const
{
	// Adaptive timestep
	bool Continue = false;
	if (StratisfyTimestep)
	{
		OutTick.Deltatime = SimulationTimestep;
		Continue = (Time >= SimulationTimestep);
	}
	else
	{
		// Stratisfy always on very slow frames (other parts of the game will take the hit mkay?)
		// Otherwise some of the simulation will go Nan-bread
		OutTick.Deltatime = FMath::Min(Time, 0.05f);
		Continue = (OutTick.Deltatime >= SMALL_NUMBER);
	}

	// Max damping is for 1024 fps
	const FTGOR_MovementBody& MovementBody = GetBody();
	OutTick.MaxLinearDamping = MovementBody.Mass * 1024.0f;
	OutTick.MaxAngularDamping = MovementBody.Inertia.GetAbsMin() * 1024.0f;

	return Continue;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void UTGOR_SymplecticComponent::InflictLinearMomentum(const FTGOR_MovementDynamic& Dynamic, FVector InMomentum, bool CounteractVelocity)
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

void UTGOR_SymplecticComponent::InflictAngularMomentum(const FTGOR_MovementDynamic& Dynamic, FVector InAngular, bool CounteractAngular)
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

void UTGOR_SymplecticComponent::InflictForceTorque(const FVector& Force, const FVector& Torque, float DeltaTime)
{
	if (CanInflict())
	{
		Inflicted.LinearMomentum += Force * DeltaTime;
		Inflicted.AngularMomentum += Torque * DeltaTime;
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void UTGOR_SymplecticComponent::PreComputePhysics(const FTGOR_MovementTick& Tick, bool Replay)
{
}

void UTGOR_SymplecticComponent::ComputePhysics(const FTGOR_MovementTick& Tick, const FTGOR_MovementSpace& Space, const FTGOR_MovementExternal& External, bool Replay, FTGOR_MovementOutput& Output)
{
}

bool UTGOR_SymplecticComponent::CanInflict() const
{
	return true;
}


void UTGOR_SymplecticComponent::SimulateMove(FTGOR_MovementSpace& Space, float Timestep, bool Sweep, bool Replay)
{
	SCOPE_CYCLE_COUNTER(STAT_Move);

	// Make sure updated component is at the right location
	USceneComponent* Component = GetBaseComponent();
	if (!IsValid(Component)) return;

	if (Timestep >= SMALL_NUMBER)
	{
		INC_DWORD_STAT_BY(STAT_Iteration, 1)

		// Compute translation
		const int32 Iterations = SimulateTranslate(Space, Timestep, Sweep, 1.0f, MaxCollisionIterations, Replay);
		
		Space.Linear = BasePrimitiveComponent->GetComponentLocation();
		Space.Angular = BasePrimitiveComponent->GetComponentQuat();
	}
	else
	{
		// Update position
		BasePrimitiveComponent->SetWorldLocationAndRotation(Space.Linear, Space.Angular, false, nullptr, ETeleportType::TeleportPhysics);
	}
}

int32 UTGOR_SymplecticComponent::SimulateTranslate(FTGOR_MovementSpace& Space, float Timestep, bool Sweep, float Ratio, int32 Iteration, bool Replay)
{
	INC_DWORD_STAT_BY(STAT_Translation, 1)

	AActor* const Actor = GetOwner();
	USceneComponent* Component = GetBaseComponent();
	if (!IsValid(Component)) return -1;

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
		if (BasePrimitiveComponent.IsValid())
		{
			// Compute translation
			const FVector Translation = Space.RelativeLinearVelocity * Ratio * Timestep;
			//const FCollisionShape& CollisionShape = BasePrimitiveComponent->GetCollisionShape();
			//ProbeSweep(CollisionShape, Space, Translation);

			SimulateSweep(Space, Translation, Hit);
			if(PullBack(Space, Hit, Translation))
			{
				INC_DWORD_STAT_BY(STAT_Pullback, 1)

				// Retry move after having been pulled back
				SimulateSweep(Space, Translation, Hit);
			}
		}

		////SafeMoveUpdatedComponent(Translation, Base.Rotation, SweepsCollision, Hit, ETeleportType::None);
		//const EMoveComponentFlags IncludeBlockingOverlapsWithoutEvents = (MOVECOMP_NeverIgnoreBlockingOverlaps | MOVECOMP_DisableBlockingOverlapDispatch);
		//Component->MoveComponent(Translation, Space.Angular, SweepsCollision && Sweep, &Hit, MOVECOMP_NoFlags, ETeleportType::None);

		if (Collide(Space, Hit, Replay))
		{
			// Unrotate base
			const float Rest = FMath::Max(Ratio - Hit.Time, 0.0f);
			Space.Angular = Space.Angular - (DeltaQuat * Rest) * Space.Angular;
			Space.Angular.Normalize();

			// Slide further along surface
			return SimulateTranslate(Space, Timestep, Sweep, Rest, Iteration - 1, Replay);
		}
	}
	return Iteration;
}

bool UTGOR_SymplecticComponent::PullBack(FTGOR_MovementSpace& Space, const FHitResult& OutHit, const FVector& Translation)
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
				BasePrimitiveComponent->MoveComponent(Adjustment, Space.Angular, false, nullptr, MOVECOMP_NoFlags, ETeleportType::TeleportPhysics);
				return true;
			}
		}
	}
	return false;
}


////////////////////////////////////////////////////////////////////////////////////////////////////

void UTGOR_SymplecticComponent::LoadFromBufferExternal(int32 Index)
{
	Inflicted = InflictedBuffer[Index];
	Super::LoadFromBufferExternal(Index);
}

void UTGOR_SymplecticComponent::LoadFromBufferWhole(int32 Index)
{
	Super::LoadFromBufferWhole(Index);
}

void UTGOR_SymplecticComponent::LerpToBuffer(int32 PrevIndex, int32 NextIndex, float Alpha)
{
	InflictedBuffer[PrevIndex].Lerp(InflictedBuffer[NextIndex], Alpha);
	Super::LerpToBuffer(PrevIndex, NextIndex, Alpha);
}

void UTGOR_SymplecticComponent::StoreToBuffer(int32 Index)
{
	InflictedBuffer[Index] = Inflicted;
	Super::StoreToBuffer(Index);
}

void UTGOR_SymplecticComponent::SetBufferSize(int32 Size)
{
	InflictedBuffer.SetNum(Size);
	Super::SetBufferSize(Size);
}