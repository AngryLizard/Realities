// The Gateway of Realities: Planes of Existence.

#include "Realities.h"
#include "TGOR_FallingMovementComponent.h"

#include "Actors/Pawns/TGOR_MovementCharacter.h"
#include "Components/Combat/TGOR_AimComponent.h"



UTGOR_FallingMovementComponent::UTGOR_FallingMovementComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	MaxSlideAngle = 0.1f;
	BrakingDecelerationFalling = 0.0;

	FallingSphereCollisionEnabled = true;
}

void UTGOR_FallingMovementComponent::BeginPlay()
{
	Super::BeginPlay();

}

void UTGOR_FallingMovementComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UTGOR_FallingMovementComponent::InitialiseMovementmode(EMovementMode CurrentMode, ETGOR_CustomMovementMode CustomMode)
{
	Super::InitialiseMovementmode(CurrentMode, CustomMode);
	
	if (CurrentMode == EMovementMode::MOVE_Falling)
	{
	}
}

bool UTGOR_FallingMovementComponent::CanCrouchInCurrentState() const
{
	return (Super::CanCrouchInCurrentState());
}


float UTGOR_FallingMovementComponent::CalcCustomControlledFallingVelocity(float DeltaTime, const FVector& InputAcceleration)
{
	return(Super::CalcCustomControlledFallingVelocity(DeltaTime, InputAcceleration));
}


float UTGOR_FallingMovementComponent::CalcCustomFreeFallingVelocity(float DeltaTime, const FVector& InputAcceleration)
{
	// Climb if wall found
	if (TryEnterClimb(InputAcceleration))
	{
		return(FLT_MAX);
	}

	// Swim if water was found
	if (DetectWater())
	{
		return(FLT_MAX);
	}

	// Switch downed if necessary
	SwitchDowned();

	// Compute axis
	const float Input = InputAcceleration.Size2D();
	const FVector Location = CreatureCharacter->GetActorLocation();
	const FVector2D Forward = FVector2D(CreatureCharacter->GetActorForwardVector());
	const FVector2D Right = FVector2D(CreatureCharacter->GetActorRightVector());
	const FVector2D Cursor = (FVector2D(CreatureCharacter->AimComponent->AimFocus) - FVector2D(Location)).GetSafeNormal();
	const float Speed = Velocity.Size2D();

	// Check current floor
	FFindFloorResult Result;
	FindFloor(Location, Result, false);
	SurfaceNormal = Result.HitResult.ImpactNormal;

	UpdateFloorAngle();

	// Set collision sphere arguments
	UpdateCollisionSphereParams(FallingSphereCollisionEnabled, FloorAngle);

	// Enable/Disable sliding
	Sliding = Result.bBlockingHit && !Result.bWalkableFloor && SurfaceNormal.Z > MaxSlideAngle;

	// Determine whether sliding or falling
	FVector2D Direction = GetDirection(Input, FVector2D(InputAcceleration), Forward);
	if (Sliding)
	{
		// Use input  //if on flat surface
		//if (Result.HitResult.ImpactNormal.SizeSquared2D() <= SMALL_NUMBER) { }

		// Set angle
		const FVector Axis = CreatureCharacter->GetActorForwardVector();
		const FVector Normal = Result.HitResult.ImpactNormal;

		FloorAngle = FMath::Asin(FVector::DotProduct(Normal, -Axis)) * (180.0f / PI);
	}
	else
	{
		// Rotate in mid air
		FloorAngle = 0.0f;
		if (Input < SMALL_NUMBER)
		{
			Direction = FVector2D(Velocity);
		}
	}

	// Rotate if rotation changed
	float Strength = Direction.Size();
	if (Strength > 0.0f)
	{
		const FVector2D Normal = Direction / Strength;
		const float RightDot = FVector2D::DotProduct(Right, Normal);
		const float ForwardDot = FVector2D::DotProduct(Forward, Normal);
		const float AimDot = FVector2D::DotProduct(Direction, Cursor);
		const float Turn = TurnSlow + (TurnSpeed - TurnSlow) * 0.98f;

		if (CreatureCharacter->IsAggressive && AimDot < -Turnaround)
		{
			SubMovementMode = ETGOR_SubMovementMode::Backwards;
			FaceDirection(-ForwardDot, -RightDot, Turn);
		}
		else
		{
			if (CreatureCharacter->IsAggressive)
			{
				SubMovementMode = ETGOR_SubMovementMode::Move;
			}

			const float CursorForwardDot = FVector2D::DotProduct(Forward, Cursor);
			const float CursorRightDot = FVector2D::DotProduct(Right, Cursor);

			RelativeDirection(ForwardDot, RightDot, CursorForwardDot, CursorRightDot, Turn);
		}
	}

	float Max = GetGroundSpeedMax();

	// Move character
	Velocity = UTGOR_Math::AddLimited(Velocity, InputAcceleration * DeltaTime, Max);

	return(Max);
}

float UTGOR_FallingMovementComponent::GetGroundSpeedMax()
{
	// Move in midair
	switch (SubMovementMode)
	{
	case ETGOR_SubMovementMode::MoveFast:	return(GroundRunSpeed);		break;
	case ETGOR_SubMovementMode::Crouching:	return(GroundCrouchSpeed);	break;
	case ETGOR_SubMovementMode::Move:		return(GroundWalkSpeed);	break;
	case ETGOR_SubMovementMode::Backwards:	return(GroundBackSpeed);	break;
	case ETGOR_SubMovementMode::Downed:		return(GroundCrouchSpeed * DownedSlowdown);	break;
	case ETGOR_SubMovementMode::Dead:		return(FLT_MAX);	break;
	default:								return(GroundWalkSpeed);
	}
}