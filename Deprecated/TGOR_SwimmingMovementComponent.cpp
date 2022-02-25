// The Gateway of Realities: Planes of Existence.

#include "Realities.h"
#include "TGOR_SwimmingMovementComponent.h"

#include "Utility/TGOR_ActorUtility.h"
#include "Actors/Pawns/TGOR_MovementCharacter.h"
#include "Components/Combat/TGOR_AimComponent.h"
#include "Components/Combat/Stat/TGOR_HealthComponent.h"




UTGOR_SwimmingMovementComponent::UTGOR_SwimmingMovementComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	Submerged = false;
	BuoyancyDepth = 100.0f;

	SwimmingSwimSpeed = 300.0f;
	SwimmingBackSpeed = 200.0f;
	SwimmingSwimFastSpeed = 600.0f;
	SwimmingDownedDriftSpeed = 800.0f;
	SwimmingDownedDriftGravityFactor = 0.05f;

	WaterSurfaceDist = 0.0f;

	OutofWaterZ = 0.0f;

	MaxSwimSpeed = 600.0f;
	BrakingDecelerationSwimming = 0.0f;
}

void UTGOR_SwimmingMovementComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UTGOR_SwimmingMovementComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UTGOR_SwimmingMovementComponent::InitialiseMovementmode(EMovementMode CurrentMode, ETGOR_CustomMovementMode CustomMode)
{
	Super::InitialiseMovementmode(CurrentMode, CustomMode);

	Submerged = false;
	WaterSurfaceDist = 0.0f;

	if (CurrentMode == EMovementMode::MOVE_Swimming)
	{
		SubMovementMode = ETGOR_SubMovementMode::Outside;
	}
}

bool UTGOR_SwimmingMovementComponent::CanCrouchInCurrentState() const
{
	if(MovementMode == EMovementMode::MOVE_Swimming)
	{
		return(true);
	}

	return (Super::CanCrouchInCurrentState());
}

float UTGOR_SwimmingMovementComponent::CalcCustomSwimmingVelocity(float DeltaTime, const FVector& InputAcceleration)
{
	ForceCrouch();

	if (!Submerged)
	{
		// Climb if wall found
		if (TryEnterClimb(InputAcceleration))
		{
			return(FLT_MAX);
		}
	}

	// compute Axis
	const FVector2D Location = FVector2D(CreatureCharacter->GetActorLocation());
	const FVector2D Forward = FVector2D(CreatureCharacter->GetActorForwardVector()).GetSafeNormal();
	const FVector2D Right = FVector2D(CreatureCharacter->GetActorRightVector()).GetSafeNormal();
	const FVector2D Cursor = (FVector2D(CreatureCharacter->AimComponent->AimFocus) - Location).GetSafeNormal();

	// Compute Speed and input strength/direction
	const float Speed = Velocity.Size();
	const float Input = InputAcceleration.Size();
	const float Relative = Input / GetMaxAcceleration();
	const float Horizontal = FVector2D(InputAcceleration.GetSafeNormal()).Size();
	const FVector2D Direction = GetDirection(InputAcceleration.Size2D(), FVector2D(InputAcceleration), Forward);
	float Drift = InputAcceleration.GetSafeNormal().Z;

	// Compute various dot products
	const float AimDot = FVector2D::DotProduct(Direction, Cursor);

	const float CursorForwardDot = FVector2D::DotProduct(Forward, Cursor);
	const float CursorRightDot = FVector2D::DotProduct(Right, Cursor);

	const float ForwardDot = FVector2D::DotProduct(Forward, Direction);
	const float RightDot = FVector2D::DotProduct(Right, Direction);
	
	// Compute underwater behaviour
	WaterSurfaceDist = UpdateSubmerged();

	if (WaterSurfaceDist < 0.0f)
	{
		FFindFloorResult Result;
		FindFloor(GetActorLocation(), Result, false);

		if (Result.bBlockingHit && Result.bWalkableFloor)
		{
			ForceMode(EMovementMode::MOVE_Walking);
			return(FLT_MAX);
		}
	}

	// Set drift value
	if (!Submerged)
	{
		Drift = FMath::Min(Drift, 0.0f);
	}

	// Compute Swimspeeds
	const float SwimSpeed = SwimmingSwimSpeed * SpeedFactor;
	const float BackSpeed = SwimmingBackSpeed * SpeedFactor;
	const float FastSpeed = SwimmingSwimFastSpeed * SpeedFactor;
	const float DownedSpeed = SwimmingSwimSpeed * DownedSlowdown;

	// Set collision sphere arguments
	if (Speed >= SMALL_NUMBER)
	{
		const float Z = Velocity.Z / Speed;
		const float Pitch = FMath::Asin(Z) / PI * 180.0f;
		FloorAngle += (Pitch - FloorAngle) * DeltaTime;
	}
	UpdateCollisionSphereParams(true, FloorAngle);

	// Switch downed if necessary
	SwitchDowned();

	// State machine
	switch (SubMovementMode)
	{
	case ETGOR_SubMovementMode::Outside:
		SwitchSwimmingOutside(Input);
		break;

	case ETGOR_SubMovementMode::Idle:
		SwitchSwimmingIdle(Relative, Input, ForwardDot, AimDot);
		break;

	case ETGOR_SubMovementMode::Move:
		SwitchSwimmingSwimming(Relative, Input, Speed, ForwardDot, Direction, AimDot);
		break;

	case ETGOR_SubMovementMode::Backwards:
		SwitchSwimmingBackwards(Relative, Input, Speed, BackSpeed, Direction, AimDot);
		break;

	case ETGOR_SubMovementMode::MoveFast:
		SwitchSwimmingSwimfast(Relative, Input, ForwardDot, Speed, SwimSpeed, Direction);
		break;

	case ETGOR_SubMovementMode::Downed:
		Drift = 0.0f;
		break;

	case ETGOR_SubMovementMode::Dead:
		Drift = 0.0f;
		break;
	}
	

	// Set direction and forward vectors
	const FVector MoveInput = FVector(Direction * Horizontal, Drift);
	const FVector MoveForward = FVector(Forward * Horizontal, Drift);

	// State machine
	switch (SubMovementMode)
	{
	case ETGOR_SubMovementMode::Outside:
		return(FLT_MAX);

	case ETGOR_SubMovementMode::Idle:
		FaceDirection(ForwardDot, RightDot, TurnSlug);
		Swim(DeltaTime, Input, Speed, SwimSpeed, MoveInput, MoveForward);
		Dive(DeltaTime, Drift, WaterSurfaceDist);
		return(SwimSpeed);

	case ETGOR_SubMovementMode::Move:
		FaceDirection(ForwardDot, RightDot, TurnSlug);
		Swim(DeltaTime, Input, Speed, SwimSpeed, MoveInput, MoveForward);
		Dive(DeltaTime, Drift, WaterSurfaceDist);
		return(SwimSpeed);

	case ETGOR_SubMovementMode::Backwards:
		FaceDirection(-ForwardDot, -RightDot, TurnSlug);
		Swim(DeltaTime, Input, Speed, BackSpeed, MoveInput, MoveForward);
		Dive(DeltaTime, Drift, WaterSurfaceDist);
		return(BackSpeed);

	case ETGOR_SubMovementMode::MoveFast:
		FaceDirection(ForwardDot, RightDot, TurnSlug);
		Swim(DeltaTime, Input, Speed, FastSpeed, MoveInput, MoveForward);
		Dive(DeltaTime, Drift, WaterSurfaceDist);
		return(FastSpeed);

	case ETGOR_SubMovementMode::Downed:
		FaceDirection(ForwardDot, RightDot, TurnSlug);
		Swim(DeltaTime, Input, Speed, DownedSpeed, MoveInput, MoveForward, false);
		if (Submerged) { Velocity.Z -= GetGravityZ() * SwimmingDownedDriftGravityFactor * DeltaTime; }
		else Dive(DeltaTime, 0.0f, WaterSurfaceDist);
		return(SwimmingDownedDriftSpeed);

	case ETGOR_SubMovementMode::Dead:
		if (Submerged) { Velocity.Z -= GetGravityZ() * SwimmingDownedDriftGravityFactor * DeltaTime; }
		else Dive(DeltaTime, 0.0f, WaterSurfaceDist);
		return(SwimmingDownedDriftSpeed);
	}

	return(FLT_MAX);
}


float UTGOR_SwimmingMovementComponent::UpdateSubmerged()
{
	const float WaterLevel = GetSubmerged();
	bool IsUnder = (WaterLevel > BuoyancyDepth / 2);
	bool IsSubmerged = IsInWater() && IsUnder;

	if (Submerged != IsSubmerged)
	{
		Submerged = IsSubmerged;
		CreatureCharacter->MovementUpdate(MovementMode, (ETGOR_CustomMovementMode)CustomMovementMode);
	}
	return(WaterLevel);
}

bool UTGOR_SwimmingMovementComponent::DetectWater()
{
	const float WaterLevel = UpdateSubmerged();

	if (WaterLevel > -0.0f && CanEverSwim() && IsInWater())
	{
		ForceMode(EMovementMode::MOVE_Swimming);
		return(true);
	}
	return(false);
}


void UTGOR_SwimmingMovementComponent::SwitchSwimmingOutside(float Input)
{
	if (Input < SMALL_NUMBER)
	{
		SubMovementMode = ETGOR_SubMovementMode::Idle;
	}
	else if (Input >= SMALL_NUMBER)
	{
		SubMovementMode = ETGOR_SubMovementMode::Move;
	}
}

void UTGOR_SwimmingMovementComponent::SwitchSwimmingIdle(float Relative, float Input, float ForwardDot, float AimDot)
{
	if (Input >= SMALL_NUMBER && ForwardDot > Turnaround)
	{
		if (Relative > 0.6f)
		{
			SubMovementMode = ETGOR_SubMovementMode::MoveFast;
		}
		else
		{
			SubMovementMode = ETGOR_SubMovementMode::Move;
		}
	}
	else if (Input >= SMALL_NUMBER && AimDot < -Turnaround && CreatureCharacter->IsAggressive)
	{
		SubMovementMode = ETGOR_SubMovementMode::Backwards;
	}

}

void UTGOR_SwimmingMovementComponent::SwitchSwimmingSwimming(float Relative, float Input, float Speed, float Limit, const FVector2D& Direction, float AimDot)
{
	if (Input < SMALL_NUMBER)
	{
		SubMovementMode = ETGOR_SubMovementMode::Idle;
	}
	else if (AimDot < -Turnaround && CreatureCharacter->IsAggressive)
	{
		SubMovementMode = ETGOR_SubMovementMode::Backwards;
	}
	else if (Relative > 0.6f && CanFast(Speed, Direction) && Input >= SMALL_NUMBER && Speed >= Limit * (1.0f - SpeedThreshold))
	{
		SubMovementMode = ETGOR_SubMovementMode::MoveFast;
	}
}

void UTGOR_SwimmingMovementComponent::SwitchSwimmingBackwards(float Relative, float Input, float Speed, float Limit, const FVector2D& Direction, float AimDot)
{
	if (Input < SMALL_NUMBER)
	{
		SubMovementMode = ETGOR_SubMovementMode::Idle;
	}
	else if (AimDot > -Turnaround / 2 && CreatureCharacter->IsAggressive)
	{
		SubMovementMode = ETGOR_SubMovementMode::Move;
	}
	else if (Relative > 0.6f && CanFast(Speed, Direction) && Input >= SMALL_NUMBER && Speed >= Limit * (1.0f - SpeedThreshold))
	{
		SubMovementMode = ETGOR_SubMovementMode::MoveFast;
	}
}

void UTGOR_SwimmingMovementComponent::SwitchSwimmingSwimfast(float Relative, float Input, float ForwardDot, float Speed, float Limit, const FVector2D& Direction)
{
	if (Relative <= 0.6f || !CanFast(Speed, Direction))
	{
		SubMovementMode = ETGOR_SubMovementMode::Move;
	}
	else if (Input < SMALL_NUMBER && Speed < Limit * SpeedThreshold)
	{
		SubMovementMode = ETGOR_SubMovementMode::Idle;
	}
}


float UTGOR_SwimmingMovementComponent::UpwardForce(float WaterLevel)
{
	if (BuoyancyDepth < SMALL_NUMBER)
	{
		return(0.0f);
	}

	// Get general Buoyancy
	const float Gravity = -GetGravityZ();
	const float Range = FMath::Clamp(WaterLevel, -BuoyancyDepth, BuoyancyDepth);
	const float Ratio = FMath::Abs(Range / BuoyancyDepth);
	const float Force = 1.0f - Ratio;
	const float Sign = FMath::Sign(Range);
	const float Damp = FMath::Min(Ratio * 2, 1.0f);

	return(Gravity * Force * Sign * Damp * Buoyancy);
}

void UTGOR_SwimmingMovementComponent::Dive(float DeltaTime, float Drift, float WaterLevel)
{
	float Upward = UpwardForce(WaterLevel);

	if (Upward < 0.0f)
	{
		Velocity.Z = FMath::Min(Velocity.Z, 0.0f);
	}

	if (FMath::Abs(Drift) < SMALL_NUMBER)
	{
		// Generate updrag if allowed
		if (Velocity.Z >= -SwimmingSwimSpeed)
		{
			Velocity.Z += Upward * DeltaTime;
		}
	}
}

void UTGOR_SwimmingMovementComponent::Swim(float DeltaTime, float Input, float Speed, float Limit, const FVector& Direction, const FVector& Forward, bool DriftFriction)
{
	if (FMath::Abs(Input) < SMALL_NUMBER || Speed > Limit)
	{
		// Water drag
		const float Friction = GetPhysicsVolume()->FluidFriction;
		if (DriftFriction)
		{
			Velocity -= Velocity * 10.0f * Friction * DeltaTime;
		}
		else
		{
			Velocity -= FVector(Velocity.X, Velocity.Y, 0.0f) *10.0f * Friction * DeltaTime;
		}
	} 
	else if (Speed < Limit)
	{
		if (CreatureCharacter->IsAggressive)
		{
			Velocity += Direction * Input * DeltaTime;
		}
		else
		{
			Velocity += Forward * Input * DeltaTime;
		}
	}
}


void UTGOR_SwimmingMovementComponent::SwitchDowned()
{
	if (CreatureCharacter->HealthComponent->HealthState == ETGOR_HealthState::Dead)
	{
		SubMovementMode = ETGOR_SubMovementMode::Dead;
	}
	else if (CreatureCharacter->HealthComponent->HealthState == ETGOR_HealthState::Downed)
	{
		SubMovementMode = ETGOR_SubMovementMode::Downed;
	}
	else if(SubMovementMode == ETGOR_SubMovementMode::Downed)
	{
		SubMovementMode = ETGOR_SubMovementMode::Outside;
	}
}
