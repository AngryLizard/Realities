// The Gateway of Realities: Planes of Existence.

#include "Realities.h"
#include "TGOR_ClimbingMovementComponent.h"

#include "Actors/Pawns/TGOR_MovementCharacter.h"
#include "Components/Combat/Stat/TGOR_StaminaComponent.h"
#include "Components/Combat/Stat/TGOR_HealthComponent.h"


UTGOR_ClimbingMovementComponent::UTGOR_ClimbingMovementComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	LedgeDistanceRatio = 2.0f;
	LedgeHeightRatio = 1.0f;
	LedgeTurnRatio = 0.1f;

	ClimbVerticalSpan = 0.5f;
	ClimbHorizontalSpan = 2.0f;

	ClimbHorizontalSpeed = 100.0f;
	ClimbVerticalSpeed = 150.0f;

	ClimbEdgeMinSpeed = 0.75f;
	ClimbEdgeSlowdown = 0.1f;

	ClimbInverseEdgeMinSpeed = 0.1f;
	ClimbInverseEdgeSlowdown = 2.5f;


	ClimbStaminaThreshold = 10.0f;

	StandupAngle = 65.0f;
	WallSurfacePitch = 0.0f;
}

void UTGOR_ClimbingMovementComponent::BeginPlay()
{
	Super::BeginPlay();

}

void UTGOR_ClimbingMovementComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UTGOR_ClimbingMovementComponent::InitialiseMovementmode(EMovementMode CurrentMode, ETGOR_CustomMovementMode CustomMode)
{
	Super::InitialiseMovementmode(CurrentMode, CustomMode);

	if (IsClimbing())
	{
		SubMovementMode = ETGOR_SubMovementMode::Outside;
	}
}


bool UTGOR_ClimbingMovementComponent::CanCrouchInCurrentState() const
{
	if (IsClimbing())
	{
		return(true);
	}

	return (Super::CanCrouchInCurrentState());
}


bool UTGOR_ClimbingMovementComponent::DoJump(bool bReplayingMoves)
{
	if (IsClimbing())
	{
		if (!bConstrainToPlane || FMath::Abs(PlaneConstraintNormal.Z) != 1.f)
		{
			const float Speed = Velocity.Size();
			if (Speed > Speed * SpeedThreshold)
			{
				CreatureCharacter->JumpKeyHoldTime = CreatureCharacter->GetJumpMaxHoldTime();
				Velocity = Velocity.GetSafeNormal() * 2.0f * JumpZVelocity;
			}
			else
			{
				TurnSpeed = TurnFast;
				TurnAdditive.Yaw = 90.0f;
				Velocity = (SurfaceNormal + FVector(0.0f, 0.0f, 1.0f)).GetSafeNormal() * JumpZVelocity;
			}

			LeaveClimb(MOVE_Falling);
			return (true);
		}
	}
	return(Super::DoJump(bReplayingMoves));
}


bool UTGOR_ClimbingMovementComponent::IsClimbing() const
{
	return(MovementMode == MOVE_Custom && CustomMovementMode == ETGOR_CustomMovementMode::Climb);
}


bool UTGOR_ClimbingMovementComponent::TryEnterClimb(const FVector& InputAcceperation)
{
	WallSurfacePitch = 0.0f;

	if (CreatureCharacter->StaminaComponent->Stat < ClimbStaminaThreshold)
	{
		return(false);
	}

	if (CreatureCharacter->IsAggressive)
	{
		return(false);
	}

	float Radius = CreatureCharacter->GetCapsuleComponent()->GetScaledCapsuleRadius();
	float Width = Radius * ClimbHorizontalSpan;
	FHitResult HitLeft, HitRight;

	if (LineTraceForward(200.0f, FVector(0.0f, +Width, 0.0f), HitRight) &&
		LineTraceForward(200.0f, FVector(0.0f, -Width, 0.0f), HitLeft))
	{
		FVector2D Left = FVector2D(HitLeft.ImpactPoint);
		FVector2D Right = FVector2D(HitRight.ImpactPoint);
		FVector2D Diff = Left - Right;

		FVector2D Normal = FVector2D(Diff.Y, -Diff.X).GetSafeNormal();
		FVector2D Forward = FVector2D(CreatureCharacter->GetActorForwardVector()).GetSafeNormal();
		float ForwardDot = FVector2D::DotProduct(Forward, Normal);

		FVector2D Direction = FVector2D(InputAcceperation).GetSafeNormal();
		float DirectionDot = FVector2D::DotProduct(Direction, Normal);

		// Check if facing the wall, going towards the wall and not going up
		if (ForwardDot < -0.5f && DirectionDot < -0.5f && Velocity.Z < JumpZVelocity / 4)
		{
			ForceMode(EMovementMode::MOVE_Custom, ETGOR_CustomMovementMode::Climb);
			return (true);
		}
	}

	return (false);
}


void UTGOR_ClimbingMovementComponent::LeaveClimb(EMovementMode GroundMode)
{
	if (CanEverSwim() && IsInWater())
	{
		ForceMode(EMovementMode::MOVE_Swimming);
	}
	else
	{
		ForceMode(GroundMode);
	}
}



bool UTGOR_ClimbingMovementComponent::ComputeSurfaceNormal(float DeltaTime)
{
	float HalfHeight = CreatureCharacter->GetDefaultHalfHeight();
	float Height = HalfHeight * ClimbVerticalSpan;

	float Radius = CreatureCharacter->GetCapsuleComponent()->GetScaledCapsuleRadius();
	float Width = Radius * ClimbHorizontalSpan;

	float Trace = HalfHeight * 2.0f;

	// Trace in front and at four corners around the character to give an averaged
	// surface normal  and an impact position to snap the character to.
	if (!LineTraceForward(Trace, FVector(0.0f, 0.0f, +Height), TopTraceResult)) { return (false); }
	if (!LineTraceForward(Trace, FVector(0.0f, 0.0f, -Height), BottomTraceResult)) { return (false); }
	if (!LineTraceForward(Trace, FVector(0.0f, +Width, 0.0f), RightTraceResult)) { return (false); }
	if (!LineTraceForward(Trace, FVector(0.0f, -Width, 0.0f), LeftTraceResult)) { return (false); }

	// Grab an approximate surface normal between the four trace points.
	WallSurfaceUp = (TopTraceResult.ImpactPoint - BottomTraceResult.ImpactPoint).GetSafeNormal();
	WallSurfaceRight = (RightTraceResult.ImpactPoint - LeftTraceResult.ImpactPoint).GetSafeNormal();
	SurfaceNormal = FVector::CrossProduct(WallSurfaceUp, WallSurfaceRight);
	
	// Get surface pitch
	TurnSpeed = TurnFast;
	TurnAdditive.Yaw = (LeftTraceResult.Distance - RightTraceResult.Distance) * LedgeTurnRatio;
	WallSurfacePitch += (BottomTraceResult.Distance - TopTraceResult.Distance) * LedgeTurnRatio * 100.0f * DeltaTime;
	return (true);
}


bool UTGOR_ClimbingMovementComponent::GrappleWall(float DeltaTime)
{
	// Get wall in front of character
	LineTraceForward(200.0f, FVector(0.0f, 0.0f, 0.0f), CenterTraceResult);
	if (!CenterTraceResult.IsValidBlockingHit())
	{
		return(false);
	}

	// Compute wall surface
	if (!ComputeSurfaceNormal(DeltaTime))
	{
		return(false);
	}

	SetBase(CenterTraceResult.Component.Get(), CenterTraceResult.BoneName);

	// Compute bentness over ledge or corner
	WallEdgeBendness = (TopTraceResult.Distance + BottomTraceResult.Distance)/2 - CenterTraceResult.Distance;
	WallCornerBendness = (LeftTraceResult.Distance + RightTraceResult.Distance) / 2 - CenterTraceResult.Distance;

	// Get pitch differences (dot product between sever normals)
	SurfacePitchDifference = FVector::DotProduct(SurfaceNormal, TopTraceResult.ImpactNormal);
	LedgePitchDifference = FVector::DotProduct(BottomTraceResult.ImpactNormal, TopTraceResult.ImpactNormal);
	CornerPitchDifference = FVector::DotProduct(LeftTraceResult.ImpactNormal, RightTraceResult.ImpactNormal);

	// Get desired distance to wall depending on current capsule size
	float Radius = CharacterOwner->GetCapsuleComponent()->GetScaledCapsuleRadius() * LedgeDistanceRatio;
	float HalfHeight = CharacterOwner->GetCapsuleComponent()->GetScaledCapsuleHalfHeight() * LedgeHeightRatio;
	float Distance = Radius + (HalfHeight - Radius) * FMath::Abs(SurfaceNormal.Z);

	// Change Wall distance to top and bottom trace impact if wall has reflex angle,
	// do either corner or ledge depending on which one is more bent
	FVector Middle = CenterTraceResult.ImpactPoint;
	if (WallEdgeBendness < -SMALL_NUMBER && CornerPitchDifference > WallEdgeBendness)
	{
		Middle = (TopTraceResult.ImpactPoint + BottomTraceResult.ImpactPoint) / 2;
	}
	else if (CornerPitchDifference < -SMALL_NUMBER)
	{
		Middle = (LeftTraceResult.ImpactPoint + RightTraceResult.ImpactPoint) / 2;
	}

	
	// Calculate a velocity needed to move the character to a desired distance from the surface.
	FVector ActorTargetPosition = Middle + SurfaceNormal * Distance;
	Velocity = (ActorTargetPosition - CreatureCharacter->GetActorLocation()) * 10.0f;
	return(true);
}

void UTGOR_ClimbingMovementComponent::MoveOnWall(float DeltaTime, const FVector& InputAcceleration)
{
	const FVector Input = InputAcceleration.GetSafeNormal();
	const float UpRatio =		FVector::DotProduct(Input, WallSurfaceUp);
	const float RightRatio =	FVector::DotProduct(Input, WallSurfaceRight);

	const float EdgeMinSpeed = WallEdgeBendness < SMALL_NUMBER ? ClimbInverseEdgeMinSpeed : ClimbEdgeMinSpeed;
	const float EdgeSlowdown = WallEdgeBendness < SMALL_NUMBER ? ClimbInverseEdgeSlowdown : ClimbEdgeSlowdown;
	const float EdgeSpeed = FMath::Max(FMath::Pow(LedgePitchDifference, EdgeSlowdown), EdgeMinSpeed);

	const float CornerMinSpeed = WallCornerBendness < SMALL_NUMBER ? ClimbInverseEdgeMinSpeed : ClimbEdgeMinSpeed;
	const float CornerSlowdown = WallCornerBendness < SMALL_NUMBER ? ClimbInverseEdgeSlowdown : ClimbEdgeSlowdown;
	const float CornerSpeed = FMath::Max(FMath::Pow(CornerPitchDifference, CornerSlowdown), CornerMinSpeed);

	const float Ratio = FMath::Min(AnalogInputModifier * 2, 1.0f);
	const float Scale = EdgeSpeed * CornerSpeed * SpeedFactor * Ratio;
	
	// Add velocity based on receieved input.
	Velocity += WallSurfaceRight *	RightRatio *	ClimbHorizontalSpeed *	Scale;
	Velocity += WallSurfaceUp *		UpRatio *		ClimbVerticalSpeed *	Scale;
}


float UTGOR_ClimbingMovementComponent::CalcCustomClimbingVelocity(float DeltaTime, const FVector& InputAcceleration)
{
	ForceCrouch();

	// Slide when going over speed
	float Speed = Velocity.Size();
	if (Speed >= FMath::Max(ClimbHorizontalSpeed, ClimbVerticalSpeed))
	{
		Velocity -= Velocity * DeltaTime;
	}
	else
	{
		Velocity = FVector(0.0f);
	}

	// Leave climbing when swimming
	if (CanEverSwim() && IsInWater())
	{
		const float WaterLevel = GetSubmerged();
		if (WaterLevel > 10.0f)
		{
			// Enter swimming (or falling if not under water)
			LeaveClimb(EMovementMode::MOVE_Falling);
		}
	}

	// Test if character is downed or dead
	if (CreatureCharacter->HealthComponent->HealthState != ETGOR_HealthState::Normal)
	{
		LeaveClimb(EMovementMode::MOVE_Falling);
	}

	// Test for walkable ground below the character and a down input to switch out of climbing mode.
	if (InputAcceleration.Z < -SMALL_NUMBER)
	{
		FFindFloorResult Result;
		FindFloor(CreatureCharacter->GetActorLocation(), Result, false);

		if (Result.bBlockingHit && Result.bWalkableFloor && Result.GetDistanceToFloor() < 5.0f)
		{
			LeaveClimb(EMovementMode::MOVE_Falling);
			return FLT_MAX;
		}
	}
	
	// Move towards wall
	if (!GrappleWall(DeltaTime))
	{
		LeaveClimb(EMovementMode::MOVE_Falling);
		return(FLT_MAX);
	}

	FloorAngle = 90.0f + WallSurfacePitch;

	// Set collision sphere arguments
	UpdateCollisionSphereParams(false, FloorAngle);

	// Stand up if over ledge
	if (WallSurfacePitch < -StandupAngle && SurfacePitchDifference > 0.9f)
	{
		// Move to ground
		FHitResult Hit;
		float Radius = CharacterOwner->GetCapsuleComponent()->GetScaledCapsuleRadius();
		SafeMoveUpdatedComponent(FVector(0.0f, 0.0f, -Radius), CreatureCharacter->GetActorRotation(), true, Hit);
		LeaveClimb(EMovementMode::MOVE_Walking);
		return(FLT_MAX);
	}

	// Fall off if under ledge
	if (WallSurfacePitch > StandupAngle)
	{
		LeaveClimb(EMovementMode::MOVE_Falling);
		return(FLT_MAX);
	}

	// Move character on wall
	MoveOnWall(DeltaTime, InputAcceleration);
	return FLT_MAX;
}


bool UTGOR_ClimbingMovementComponent::LineTraceForward(float Length, const FVector& Offset, FHitResult& Hit)
{
	FVector ActorPosition = CreatureCharacter->GetActorLocation();

	// Create RotationMatrix
	float Yaw = CreatureCharacter->GetActorRotation().Yaw;
	FRotator Rotator = FRotator(WallSurfacePitch, Yaw, 0.0f);
	FRotationMatrix Matrix = FRotationMatrix(Rotator);

	// Get axis
	FVector Forward = Matrix.GetScaledAxis(EAxis::X);
	FVector Right = Matrix.GetScaledAxis(EAxis::Y);
	FVector Up = Matrix.GetScaledAxis(EAxis::Z);

	// Get offset in local space and compute trace position
	FVector Local = Offset.X * Forward + Offset.Y * Right + Offset.Z * Up;
	FVector End = ActorPosition + Forward * Length;
	FVector Start = ActorPosition + Local;

	TArray<AActor*> Ignore;
	return(UKismetSystemLibrary::LineTraceSingle(CreatureCharacter, Start, End, ClimbingTraceType, false, Ignore, DebugTrace, Hit, true));
}
