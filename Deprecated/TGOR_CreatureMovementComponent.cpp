// The Gateway of Realities: Planes of Existence.

#include "Realities.h"
#include "TGOR_CreatureMovementComponent.h"

#include "Utility/TGOR_ActorUtility.h"
#include "Components/Combat/Stat/TGOR_StaminaComponent.h"
#include "Actors/Pawns/TGOR_MovementCharacter.h"


UTGOR_CreatureMovementComponent::UTGOR_CreatureMovementComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	MaxAcceleration = 2048.0;
	BrakingFrictionFactor = 0.5f;
	CrouchedHalfHeight = 95.0f;
	MaxStepHeight = 50.0f;
	
	TurnSlug = 90.0f;
	TurnSlow = 180.0f;
	TurnMedium = 360.0f;
	TurnFast = 720.0f;

	Turnaround = 0.7f;
	SpeedFactor = 1.0f;
	SpeedThreshold = 0.1f;

	JumpZVelocity = 500.0f;
	AirControl = 0.25f;
	AirControlBoostMultiplier = 0.0f;
	AirControlBoostVelocityThreshold = 0.0f;
	FallingLateralFriction = 0.2f;

	SurfaceLevelRatio = 0.4f;

	DownedSlowdown = 0.2f;

	ExhaustStaminaThreshold = 15.0f;
	ExhaustStaminaRate = 10.0f;

	bCanWalkOffLedgesWhenCrouching = true;
	bOrientRotationToMovement = true;

	SubMovementMode = ETGOR_SubMovementMode::Outside;

	FloorAngle = 0.0f;
	SurfaceNormal = FVector(0.0f);
	CreatureCharacter = nullptr;

	CurveCorrection = true;
	NavAgentProps.bCanCrouch = true;
	bUseSeparateBrakingFriction = false;
}

void UTGOR_CreatureMovementComponent::BeginPlay()
{
	Super::BeginPlay();

	CreatureCharacter = Cast<ATGOR_MovementCharacter>(GetOwner());
}

void UTGOR_CreatureMovementComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (CreatureCharacter->GetRemoteRole() > ENetRole::ROLE_SimulatedProxy)
	{
		CreatureCharacter->ReplicatedInput = Acceleration;
	}

	// Draw velocity for debugging
	if (DebugTrace != EDrawDebugTrace::Type::None)
	{
		DrawDebugDirectionalArrow(GetWorld(), GetActorLocation(), GetActorLocation() + Velocity, 10.0f, FColor(0x00FF00), false, -1.0, 0, 5.0f);
	}
}


bool UTGOR_CreatureMovementComponent::CanCrouchInCurrentState() const
{
	return(Super::CanCrouchInCurrentState());
}

bool UTGOR_CreatureMovementComponent::DoJump(bool bReplayingMoves)
{
	return(Super::DoJump(bReplayingMoves));
}

void UTGOR_CreatureMovementComponent::OnMovementModeChanged(EMovementMode PreviousMovementMode, uint8 PreviousCustomMode)
{
	Super::OnMovementModeChanged(PreviousMovementMode, PreviousCustomMode);
	bCrouchMaintainsBaseLocation = true;

	if (!IsValid(CreatureCharacter)) return;

	InitialiseMovementmode(					MovementMode, (ETGOR_CustomMovementMode)CustomMovementMode);
	CreatureCharacter->MovementModeChanged(	MovementMode, (ETGOR_CustomMovementMode)CustomMovementMode);
	CreatureCharacter->MovementUpdate(		MovementMode, (ETGOR_CustomMovementMode)CustomMovementMode);
}

void UTGOR_CreatureMovementComponent::InitialiseMovementmode(EMovementMode CurrentMode, ETGOR_CustomMovementMode CustomMode)
{
}


bool UTGOR_CreatureMovementComponent::CanFast(float Speed, const FVector2D& Direction)
{
	if (GetOwnerRole() <= ROLE_SimulatedProxy)
	{
		return (true);
	}

	if (CreatureCharacter->IsAggressive)
	{
		return(false);
	}

	bool IsOverThreshold = CreatureCharacter->StaminaComponent->Stat > ExhaustStaminaThreshold;
	bool IsOverZero = CreatureCharacter->StaminaComponent->Stat > 0.0f;

	return (IsOverThreshold || (IsOverZero && SubMovementMode == ETGOR_SubMovementMode::MoveFast));
}


FVector2D UTGOR_CreatureMovementComponent::GetDirection(float Input, const FVector2D& InputAcceleration, const FVector2D& Forward)
{
	if (Input < SMALL_NUMBER)
	{
		return(Forward);
	}
	return(InputAcceleration / Input);
}


void UTGOR_CreatureMovementComponent::UpdateCollisionSphereParams(bool Active, float Pitch)
{
	// Set collision sphere arguments
	CollisionSphereCorrectionEnabled = Active;
	CollisionSphereCorrectionPitch = Pitch;
}

void UTGOR_CreatureMovementComponent::UpdateFloorAngle()
{
	FVector Forward = CreatureCharacter->GetActorForwardVector();

	// Add floor angle for animation
	FloorAngle = FMath::Asin(FVector::DotProduct(SurfaceNormal, -Forward)) * (180.0f / PI);
}

float UTGOR_CreatureMovementComponent::GetSubmerged()
{
	const float WaterLevel = UTGOR_ActorUtility::GetWaterLevel(CreatureCharacter);

	FVector Location = CreatureCharacter->GetActorLocation();
	UCapsuleComponent* Capsule = CreatureCharacter->GetCapsuleComponent();
	const float Height = Capsule->GetScaledCapsuleHalfHeight();
	const float CapsuleLevel = Location.Z + Height * SurfaceLevelRatio;

	return (WaterLevel - CapsuleLevel);
}


void UTGOR_CreatureMovementComponent::ForceCrouch()
{
	if (CreatureCharacter->Role > ENetRole::ROLE_SimulatedProxy)
	{
		bWantsToCrouch = true;
		UpdateCharacterStateBeforeMovement(0.0f);
	}
}

void UTGOR_CreatureMovementComponent::ForceUnCrouch()
{
	if (CreatureCharacter->Role > ENetRole::ROLE_SimulatedProxy)
	{
		bWantsToCrouch = false;
		UpdateCharacterStateBeforeMovement(0.0f);
	}
}

void UTGOR_CreatureMovementComponent::ForceMode(EMovementMode Mode, uint8 Custom)
{
	if (CreatureCharacter->Role > ENetRole::ROLE_SimulatedProxy)
	{
		SetMovementMode(Mode, Custom);
	}
}

void UTGOR_CreatureMovementComponent::FaceDirection(float ForwardDot, float RightDot, float Turn)
{
	TurnSpeed = Turn;

	float Angle = FMath::Acos(ForwardDot) / PI * 180.0f;
	float Dir = (RightDot > 0.0f ? 1.0f : -1.0f);

	if (ForwardDot < -0.9f)
	{
		TurnAdditive.Yaw = Angle;
	}
	else
	{
		TurnAdditive.Yaw = Angle * Dir;
	}
}



void UTGOR_CreatureMovementComponent::SimulateMovement(float DeltaTime)
{
	Super::SimulateMovement(DeltaTime);

	const bool bIsSimulatedProxy = (CharacterOwner->Role == ROLE_SimulatedProxy);
	if (bIsSimulatedProxy)
	{
		Acceleration = CreatureCharacter->ReplicatedInput;

		switch (MovementMode)
		{
		case EMovementMode::MOVE_Walking:	CalcCustomWalkingVelocity(DeltaTime, Acceleration); break;
		case EMovementMode::MOVE_Swimming:	CalcCustomSwimmingVelocity(DeltaTime, Acceleration); break;
		case EMovementMode::MOVE_Flying:	CalcCustomFlyingVelocity(DeltaTime, Acceleration); break;
		case EMovementMode::MOVE_Falling:	CalcCustomFreeFallingVelocity(DeltaTime, Acceleration);
											CalcCustomControlledFallingVelocity(DeltaTime, Acceleration); break;
		case EMovementMode::MOVE_Custom:
			switch (CustomMovementMode)
			{
			case ETGOR_CustomMovementMode::Skiing:  CalcCustomSkiingVelocity(DeltaTime, Acceleration); break;
			case ETGOR_CustomMovementMode::Montage:	CalcCustomMontageVelocity(DeltaTime); break;
			case ETGOR_CustomMovementMode::Climb:   CalcCustomClimbingVelocity(DeltaTime, Acceleration); break;
			case ETGOR_CustomMovementMode::Waypoint: break;
			default: break;
			}
			break;
		}

		CalcTurnDelta(DeltaTime);
	}
}

void UTGOR_CreatureMovementComponent::SecureCustomVelocity(float DeltaTime, int32 Iterations)
{
	Super::SecureCustomVelocity(DeltaTime, Iterations);

	CreatureCharacter->DetectDesync();
}