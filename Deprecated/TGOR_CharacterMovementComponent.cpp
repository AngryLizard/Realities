// The Gateway of Realities: Planes of Existence. By Salireths.

#include "Realities.h"
#include "TGOR_CharacterMovementComponent.h"

UTGOR_CharacterMovementComponent::UTGOR_CharacterMovementComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	UTurnRate = 2.0f;
	UTurnAngle = 1.8f;
	MinYawControl = 0.3f;

	YawVelocity = 0.0f;
	MaxPitchAngle = 20.0f;
	Pitchfactor = 1.0f;

	bAdaptToGround = true;
	YawDirection = 0.0f;
	LastYaw = 0.0f;
}

void UTGOR_CharacterMovementComponent::InitializeComponent()
{
	Super::InitializeComponent();
}


void UTGOR_CharacterMovementComponent::AdaptToGround()
{
	
	float a = 0.0f;
	if (IsMovingOnGround())
	{
		FVector Forward = GetOwner()->GetActorForwardVector();
		Forward.Z = 0.0f;
		Forward.Normalize();

		FHitResult Hit;
		TArray<AActor*> Ignore;
		FVector Location = GetActorLocation();
		FVector End = Location - FVector(0.0f, 0.0f, 256.0f);
		if (UKismetSystemLibrary::LineTraceSingle(this, Location, End, GroundDetection, false, Ignore, EDrawDebugTrace::Type::None, Hit, true))
		{
			float Rad = FMath::Asin(FVector::DotProduct(Hit.ImpactNormal, Forward));
			a = -Rad / PI * 180.0f;
		}
	}

	NormalPitch = FMath::Clamp(NormalPitch + (a * Pitchfactor - NormalPitch) * 0.1f, -MaxPitchAngle, MaxPitchAngle);
}

void UTGOR_CharacterMovementComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction)
{
	AActor* Actor = GetOwner();

	if (bAdaptToGround) AdaptToGround();

	// Get current direction in radians and compute direction vector
	Angle = FMath::DegreesToRadians(Actor->GetActorRotation().Yaw);

	// Compute Yaw velocity
	YawVelocity = Angle - LastYaw; LastYaw = Angle;
	YawVelocity += (YawVelocity > PI) ? -TWOPI : (YawVelocity < -PI) ? TWOPI : 0;

	// Set Yaw direction (1 if going to the right, -1 if going to the left)
	YawDirection = YawVelocity < 0.0f ? -0.1f : (YawVelocity > 0.0f ? 0.1f : YawDirection);

	// Get orientation
	Orientation = FVector(FMath::Cos(Angle + YawVelocity), FMath::Sin(Angle + YawVelocity), 0.0f);

	// Get Speed/MaxSpeed ratio
	float Max = GetMaxSpeed();
	FVector v = Actor->GetVelocity();
	Ratio = (v.X * v.X + v.Y * v.Y) / (Max * Max); // sqrt(x^2+y^2)/m = (x^2+y^2)/m^2 (faster)
	
	// Default behaviour
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

FVector UTGOR_CharacterMovementComponent::ConstrainInputAcceleration(const FVector& InputAcceleration) const
{
	float Acc = FVector::Dist(FVector(0, 0, 0), InputAcceleration);

	if (Acc == 0.0f)
		return(FVector(0.0f, 0.0f, 0.0f));
	
	// Distance between actual and desired orientation (Also apply default behaviour)
	FVector Direction = Super::ConstrainInputAcceleration(InputAcceleration).GetUnsafeNormal();
	float Dist = FVector::DistSquaredXY(Direction, Orientation);

	// Prevent wiggle
	if (Dist > 0.1f)
	{
		FVector Aim = Orientation;
		// Push character to his last direction
		if (Dist > 3.5f) Aim = FVector(FMath::Cos(Angle + YawDirection), FMath::Sin(Angle + YawDirection), 0.0f);
		// Interpolate between target and actual direction based on speed ratio
		Direction = Direction + (Aim - Direction) * FMath::Min(Ratio * UTurnRate, 0.9f);
	}
	
	// Don't move when slow and wrong rotation
	float Factor = UTurnAngle - Dist * (1.0f - Ratio);

	// Move and also rotate Character (Factor two to prevent full stop on direction change)
	// Max(speed, 0.01) to make sure player turns around even when standing still)
	return(2.0f * Direction * FMath::Max(Acc * Factor, 0.01f));
}

void UTGOR_CharacterMovementComponent::PhysicsRotation(float DeltaTime)
{
	if (!(bOrientRotationToMovement || bUseControllerDesiredRotation)) return;

	const FRotator CurrentRotation = UpdatedComponent->GetComponentRotation(); // Normalized
	FRotator DeltaRot = GetDeltaRotation(DeltaTime) * FMath::Max(MinYawControl, 1.0f - Ratio);
	FRotator DesiredRotation = CurrentRotation;

	DesiredRotation = ComputeOrientToMovementRotation(CurrentRotation, DeltaTime, DeltaRot);

	DesiredRotation.Pitch = 0.f;
	DesiredRotation.Yaw = FRotator::NormalizeAxis(DesiredRotation.Yaw);
	DesiredRotation.Roll = 0.f;

	// Accumulate a desired new rotation.
	const float AngleTolerance = 1e-3f;

	if (!CurrentRotation.Equals(DesiredRotation, AngleTolerance))
	{
		DesiredRotation.Yaw = FMath::FixedTurn(CurrentRotation.Yaw, DesiredRotation.Yaw, DeltaRot.Yaw);
		MoveUpdatedComponent(FVector::ZeroVector, DesiredRotation, true);
	}
}