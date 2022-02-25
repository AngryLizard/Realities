// The Gateway of Realities: Planes of Existence.

#include "Realities.h"
#include "TGOR_GroundMovementComponent.h"

#include "Components/Combat/TGOR_AimComponent.h"
#include "Actors/Pawns/TGOR_MovementCharacter.h"


UTGOR_GroundMovementComponent::UTGOR_GroundMovementComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	GroundWalkSpeed = 650.0f;
	GroundBackSpeed = 500.0f;
	GroundCrouchSpeed = 450.0f;
	GroundRunSpeed = 1000.0f;

	GroundSphereCollisionEnabled = true;

	MaxWalkSpeed = 1200.0f;
	MaxWalkSpeedCrouched = 500.0f;

	RunningWalkAngle = 65.0f;
	WalkingWalkAngle = 40.0f;
	AngleSpeedLerp = 120.0f;
	AngleSlowdown = 0.9f;
		
	BrakingDecelerationWalking = 900.0f;
}

void UTGOR_GroundMovementComponent::BeginPlay()
{
	Super::BeginPlay();

}

void UTGOR_GroundMovementComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UTGOR_GroundMovementComponent::InitialiseMovementmode(EMovementMode CurrentMode, ETGOR_CustomMovementMode CustomMode)
{
	Super::InitialiseMovementmode(CurrentMode, CustomMode);
	
	if (CurrentMode == EMovementMode::MOVE_Walking)
	{
		SubMovementMode = ETGOR_SubMovementMode::Outside;
		CreatureCharacter->UnCrouch();
	}
}

bool UTGOR_GroundMovementComponent::CanCrouchInCurrentState() const
{
	return (Super::CanCrouchInCurrentState());
}


float UTGOR_GroundMovementComponent::CalcCustomWalkingVelocity(float DeltaTime, const FVector& InputAcceleration)
{
	// Swim if water was found
	if (DetectWater())
	{
		return(FLT_MAX);
	}

	// compute Axis
	const FVector2D Location = FVector2D(CreatureCharacter->GetActorLocation());

	const FVector2D Cursor = (FVector2D(CreatureCharacter->AimComponent->AimFocus) - Location).GetSafeNormal();
	const FVector2D Forward = FVector2D(CreatureCharacter->GetActorForwardVector()).GetSafeNormal();
	const FVector2D Right = FVector2D(CreatureCharacter->GetActorRightVector()).GetSafeNormal();

	// Compute Speed and input strength/direction
	const float Speed = Velocity.Size2D();
	const float Input = InputAcceleration.Size2D();
	const float Relative = Input / GetMaxAcceleration();
	const FVector2D Direction = GetDirection(InputAcceleration.Size2D(), FVector2D(InputAcceleration), Forward);
	
	// Compute various dot products
	const float AimDot = FVector2D::DotProduct(Direction, Cursor);
	
	const float CursorForwardDot = FVector2D::DotProduct(Forward, Cursor);
	const float CursorRightDot = FVector2D::DotProduct(Right, Cursor);

	const float ForwardDot = FVector2D::DotProduct(Forward, Direction);
	const float RightDot = FVector2D::DotProduct(Right, Direction);

	SurfaceNormal = CurrentFloor.HitResult.ImpactNormal;
	
	// Reduce friction if moving very fast
	if (Speed > GroundRunSpeed * (1.0f + SpeedThreshold))
	{
		MovementBrakeFactor = 0.1f;
	}
	else
	{
		MovementBrakeFactor = 1.0f;
	}
	
	// Get culminative ratio
	const float Ratio = SpeedFactor * FMath::Min(AnalogInputModifier * 2, 1.0f);

	// Deal with slope
	SetWalkAngle(Speed, GroundRunSpeed);
	const float AngleWalkLimit = GetWalkAngleSpeed(GroundWalkSpeed, Direction);
	const float AngleBackLimit = GetWalkAngleSpeed(GroundBackSpeed, Direction);
	const float AngleRunLimit = GetRunAngleSpeed(DeltaTime, Speed, GroundRunSpeed, GroundWalkSpeed, Direction);

	// Set collision sphere arguments
	UpdateCollisionSphereParams(GroundSphereCollisionEnabled, FloorAngle);

	// Switch downed if necessary
	SwitchDowned();

	// State machine
	switch (SubMovementMode)
	{
	case ETGOR_SubMovementMode::Outside:
		SwitchGroundOutside(Input, Speed);
		break;

	case ETGOR_SubMovementMode::Idle:
		SwitchGroundIdle(Relative, Input, ForwardDot, AimDot);
		break;

	case ETGOR_SubMovementMode::Move:
		SwitchGroundWalking(Relative, Input, Speed, AngleWalkLimit, Direction, AimDot);
		break;

	case ETGOR_SubMovementMode::Backwards:
		SwitchGroundBackwards(Relative, Input, Speed, GroundBackSpeed, Direction, AimDot);
		break;
	
	case ETGOR_SubMovementMode::MoveFast:
		SwitchGroundRunning(Relative, Input, ForwardDot, Speed, AngleWalkLimit, Direction);
		break;
	
	case ETGOR_SubMovementMode::Crouching:
		SwitchGroundCrouching(Input);
		break;

	case ETGOR_SubMovementMode::Downed:
		ForceCrouch();
		break;

	case ETGOR_SubMovementMode::Dead:
		ForceCrouch();
		break;
	}
	
	// State machine
	switch (SubMovementMode)
	{
	case ETGOR_SubMovementMode::Outside:
		MoveDirection(DeltaTime, Input, Forward);
		return(FLT_MAX);

	case ETGOR_SubMovementMode::Idle:
		MoveDirection(DeltaTime, Input, Forward);
		CursorDirection(Input, ForwardDot, RightDot, CursorForwardDot, CursorRightDot);
		return(0.0f);

	case ETGOR_SubMovementMode::Move:
		ArchDirection(DeltaTime, Input, Forward, Direction);
		RelativeDirection(ForwardDot, RightDot, CursorForwardDot, CursorRightDot, TurnMedium);
		return(AngleWalkLimit * Ratio);

	case ETGOR_SubMovementMode::Backwards:
		MoveDirection(DeltaTime, Input, Direction);
		FaceDirection(-ForwardDot, -RightDot, TurnMedium);
		return(AngleBackLimit * Ratio);

	case ETGOR_SubMovementMode::MoveFast:
		RestrictDirection(DeltaTime, Input, Speed, GetMaxSpeed(), Forward);
		FaceDirection(ForwardDot, RightDot, TurnMedium);
		return(AngleRunLimit * Ratio);

	case ETGOR_SubMovementMode::Crouching:
		MoveDirection(DeltaTime, Input, Forward);
		FaceDirection(ForwardDot, RightDot, TurnSlow);
		return(GroundCrouchSpeed * Ratio);

	case ETGOR_SubMovementMode::Downed:
		MoveDirection(DeltaTime, Input, Forward);
		FaceDirection(ForwardDot, RightDot, TurnSlow);
		return(GroundCrouchSpeed * Ratio * DownedSlowdown);

	case ETGOR_SubMovementMode::Dead:
		return(FLT_MAX);
	}

	return(FLT_MAX);
}



void UTGOR_GroundMovementComponent::SetWalkAngle(float Speed, float Limit)
{
	// Automatically set to running angle when crouching
	if (IsCrouching())
	{
		SetWalkableFloorAngle(WalkingWalkAngle);
		return;
	}

	// Three point rule
	const float Diff = Limit - GroundWalkSpeed;
	if (FMath::Abs(Diff) < SMALL_NUMBER) return;

	// Compute ratio between walk and run speed
	const float Ratio = FMath::Max(Speed - GroundWalkSpeed, 0.0f) / Diff;
	
	// Get actual walk angle (rule of three)
	const float WalkAngle = WalkingWalkAngle + (RunningWalkAngle - WalkingWalkAngle) * Ratio;
	
	// Clamp angle
	SetWalkableFloorAngle(FMath::Clamp(WalkAngle, WalkingWalkAngle, RunningWalkAngle));
}


float UTGOR_GroundMovementComponent::GetWalkAngleSpeed(float Limit, const FVector2D& Direction)
{
	UpdateFloorAngle();

	// Compute actual speed in slope
	FVector2D Normal = FVector2D(SurfaceNormal);
	const float Dot = FVector2D::DotProduct(Normal, Direction);
	const float Ratio = 1.0f + FMath::Min(Dot * AngleSlowdown, 0.0f);
	return(Limit * Ratio);
}

float UTGOR_GroundMovementComponent::GetRunAngleSpeed(float DeltaTime, float Speed, float Limit, float MinLimit, const FVector2D& Direction)
{
	const float SlopeRunLimit = GetWalkAngleSpeed(Limit, Direction);
	const float SlopeWalkLimit = GetWalkAngleSpeed(MinLimit, Direction);
	
	if (LastSpeedLimit < SlopeRunLimit)
	{
		return(SlopeRunLimit);
	}

	// Lerp to current speed
	const float lerpedLimit = LastSpeedLimit + FMath::Max(SlopeRunLimit - LastSpeedLimit, -AngleSpeedLerp) * DeltaTime;

	// Only apply when already running
	return(FMath::Max(lerpedLimit, SlopeWalkLimit));
}


void UTGOR_GroundMovementComponent::SwitchGroundOutside(float Input, float Speed)
{
	if (IsCrouching())
	{
		SubMovementMode = ETGOR_SubMovementMode::Crouching;
	}
	else if(Input < SMALL_NUMBER && Speed < SMALL_NUMBER)
	{
		SubMovementMode = ETGOR_SubMovementMode::Idle;
	}
	else if (Speed < GroundRunSpeed)
	{
		SubMovementMode = ETGOR_SubMovementMode::Move;
	}
	else
	{
		SubMovementMode = ETGOR_SubMovementMode::MoveFast;
	}

}

void UTGOR_GroundMovementComponent::SwitchGroundIdle(float Relative, float Input, float Dot, float Aim)
{
	if (bWantsToCrouch)
	{
		SubMovementMode = ETGOR_SubMovementMode::Crouching;
	}
	else if (Input >= SMALL_NUMBER && (Dot > Turnaround || CreatureCharacter->IsAggressive))
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
	else if (Input >= SMALL_NUMBER && Aim < -Turnaround && CreatureCharacter->IsAggressive)
	{
		SubMovementMode = ETGOR_SubMovementMode::Backwards;
	}
}

void UTGOR_GroundMovementComponent::SwitchGroundWalking(float Relative, float Input, float Speed, float Limit, const FVector2D& Direction, float Dot)
{
	if (IsCrouching())
	{
		SubMovementMode = ETGOR_SubMovementMode::Crouching;
	}
	else if(Input < SMALL_NUMBER && (!CreatureCharacter->IsAggressive || (Speed <= Speed * SpeedThreshold)))
	{
		SubMovementMode = ETGOR_SubMovementMode::Idle;
	}
	else if (Dot < -Turnaround && CreatureCharacter->IsAggressive)
	{
		SubMovementMode = ETGOR_SubMovementMode::Backwards;
	}
	else if (Relative > 0.6f && CanFast(Speed, Direction) && Input >= SMALL_NUMBER && Speed >= Limit * (1.0f - SpeedThreshold))
	{
		SubMovementMode = ETGOR_SubMovementMode::MoveFast;
	}
}


void UTGOR_GroundMovementComponent::SwitchGroundBackwards(float Relative, float Input, float Speed, float Limit, const FVector2D& Direction, float Dot)
{
	if (IsCrouching())
	{
		SubMovementMode = ETGOR_SubMovementMode::Crouching;
	}
	else if (Input < SMALL_NUMBER)
	{
		SubMovementMode = ETGOR_SubMovementMode::Idle;
	}
	else if (Dot > -Turnaround / 2 || !CreatureCharacter->IsAggressive)
	{
		SubMovementMode = ETGOR_SubMovementMode::Move;
	}
	else if (Relative > 0.6f && CanFast(Speed, Direction) && Input >= SMALL_NUMBER && Speed >= Limit * (1.0f - SpeedThreshold))
	{
		SubMovementMode = ETGOR_SubMovementMode::MoveFast;
	}
}

void UTGOR_GroundMovementComponent::SwitchGroundRunning(float Relative, float Input, float ForwardDot, float Speed, float Limit, const FVector2D& Direction)
{
	if (IsCrouching())
	{
		SubMovementMode = ETGOR_SubMovementMode::Crouching;
	}
	else if (Relative <= 0.6f || !CanFast(Speed, Direction))
	{
		SubMovementMode = ETGOR_SubMovementMode::Move;
	}
	else if (Input < SMALL_NUMBER && Speed < Limit * (1.0f - SpeedThreshold))
	{
		SubMovementMode = ETGOR_SubMovementMode::Idle;
	}
}

void UTGOR_GroundMovementComponent::SwitchGroundCrouching(float Input)
{
	if (!IsCrouching() && Input < SMALL_NUMBER)
	{
		SubMovementMode = ETGOR_SubMovementMode::Idle;
	}
	else if (!IsCrouching() && Input >= SMALL_NUMBER)
	{
		SubMovementMode = ETGOR_SubMovementMode::Move;
	}
}


void UTGOR_GroundMovementComponent::GridDirection(float ForwardDot, float RightDot, float Turn)
{
	if (ForwardDot < Turnaround)
	{
		FaceDirection(ForwardDot, RightDot, Turn);
	}
}

void UTGOR_GroundMovementComponent::CursorDirection(float Input, float ForwardDot, float RightDot, float CursorForwardDot, float CursorRightDot)
{
	if (CreatureCharacter->IsAggressive && Input < SMALL_NUMBER)
	{
		GridDirection(CursorForwardDot, CursorRightDot, TurnSlow);
	}
	else if(Input >= SMALL_NUMBER)
	{
		FaceDirection(ForwardDot, RightDot, TurnMedium);
	}
}

void UTGOR_GroundMovementComponent::RelativeDirection(float ForwardDot, float RightDot, float CursorForwardDot, float CursorRightDot, float Turn)
{
	if (!CreatureCharacter->IsAggressive || ForwardDot > -Turnaround)
	{
		FaceDirection(ForwardDot, RightDot, Turn);
	}
	else
	{
		FaceDirection(CursorForwardDot, CursorRightDot, Turn);
	}
}


void UTGOR_GroundMovementComponent::ArchDirection(float DeltaTime, float Input, const FVector2D& Forward, const FVector2D& Direction)
{
	if (CreatureCharacter->IsAggressive)
	{
		MoveDirection(DeltaTime, Input, Direction);
	}
	else
	{
		MoveDirection(DeltaTime, Input, Forward);
	}
}


void UTGOR_GroundMovementComponent::RestrictDirection(float DeltaTime, float Input, float Speed, float Limit, const FVector2D& Forward)
{
	if (Speed < Limit * (1.0f + SpeedThreshold))
	{
		MoveDirection(DeltaTime, Input, Forward);
	}
}