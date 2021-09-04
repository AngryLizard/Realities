// The Gateway of Realities: Planes of Existence.


#include "TGOR_PlatformActor.h"

#include "DimensionSystem/Components/TGOR_MobilityComponent.h"
#include "CoreSystem/TGOR_Singleton.h"

ATGOR_PlatformActor::ATGOR_PlatformActor(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = ObjectInitializer.CreateDefaultSubobject<USceneComponent>(this, FName(TEXT("RootComponent")));
	SplineComponent = ObjectInitializer.CreateDefaultSubobject<USplineComponent>(this, FName(TEXT("SplineComponent")));
	MobilityComponent = ObjectInitializer.CreateDefaultSubobject<UTGOR_MobilityComponent>(this, FName(TEXT("MobilityComponent")));
	
	Speed = 1.0f;
	Threshold = 1.0f;
	Offset = 0.0f;

	LocationMode = ETGOR_PlatformLocationMode::MoveNone;
	RotationMode = ETGOR_PlatformRotationMode::MoveNone;
}

void ATGOR_PlatformActor::BeginPlay()
{
	Super::BeginPlay();
}

void ATGOR_PlatformActor::Tick(float DeltaTime)
{
	SINGLETON_CHK
	SetSceneComponent(Singleton->GetGameTimestamp());

	Super::Tick(DeltaTime);
}

void ATGOR_PlatformActor::SetSceneComponent(FTGOR_Time Time)
{
	float Distance = 0.0f;
	const float SplineLength = SplineComponent->GetSplineLength();

	FTGOR_MovementDynamic Dynamic;

	if (LocationMode == ETGOR_PlatformLocationMode::MoveLinearly)
	{
		// Use periodic function that linearly sweeps between both points
		const float Pos = Time.Modf(1.0f, Speed, Offset);
		const float LinAbs = (FMath::Abs(Pos - 0.5f) - 0.25f) * 2.0f;
		const float LinRatio = FMath::Clamp(LinAbs * Threshold, -0.5f, 0.5f) + 0.5f;
		Distance = SplineLength * LinRatio;

		// Set position on spline
		Dynamic.Linear = SplineComponent->GetLocationAtDistanceAlongSpline(Distance, ESplineCoordinateSpace::World);
		Dynamic.LinearVelocity = SplineComponent->GetDirectionAtDistanceAlongSpline(Distance, ESplineCoordinateSpace::World) * SplineLength * Speed * (Pos > 0.5f ? 2.0f : -2.0f);
		Dynamic.LinearAcceleration = FVector::ZeroVector; // TODO
	}
	else if (LocationMode == ETGOR_PlatformLocationMode::MoveSinus)
	{
		// Use periodic wave function that sweeps between both points
		const float Sin = FMath::Sin(Time.Modf(2 * PI, Speed, Offset)) * Threshold;
		const float Cos = FMath::Cos(Time.Modf(2 * PI, Speed, Offset)) * Threshold;
		const float SinRatio = (FMath::Clamp(Sin, -1.0f, 1.0f) + 1.0f) / 2;
		Distance = SplineLength * SinRatio;

		// Set position on spline
		Dynamic.Linear = SplineComponent->GetLocationAtDistanceAlongSpline(Distance, ESplineCoordinateSpace::World);
		Dynamic.LinearVelocity = SplineComponent->GetDirectionAtDistanceAlongSpline(Distance, ESplineCoordinateSpace::World) * Cos * SplineLength / Speed * 2 * PI;
		Dynamic.LinearAcceleration = SplineComponent->GetDirectionAtDistanceAlongSpline(Distance, ESplineCoordinateSpace::World) * -Sin * SplineLength / (Speed * Speed) * 2 * PI;
	}
	else
	{
		Dynamic.Linear = SplineComponent->GetLocationAtDistanceAlongSpline(Distance, ESplineCoordinateSpace::World);
		Dynamic.LinearVelocity = FVector::ZeroVector;
		Dynamic.LinearAcceleration = FVector::ZeroVector;
	}

	FTGOR_MovementPosition Position = MobilityComponent->ComputePosition();
	if (RotationMode == ETGOR_PlatformRotationMode::MoveLinearly)
	{
		// Rotate yaw linearly
		FRotator Rotator = Position.Angular.Rotator();
		Rotator.Yaw = Time.Modf(1.0f, Speed, Offset) * 360.0f;
		Rotator.Pitch = 0.0f;
		Rotator.Roll = 0.0f;

		Dynamic.Angular = SplineComponent->GetRotationAtDistanceAlongSpline(Distance, ESplineCoordinateSpace::World).Quaternion();
		Dynamic.AngularVelocity = FVector::UpVector * Speed / 180.0f * PI;
		Dynamic.AngularAcceleration = FVector::ZeroVector;
	}
	else if (RotationMode == ETGOR_PlatformRotationMode::MoveSpline)
	{
		// Set to rotation to the current spline position
		Dynamic.Angular = SplineComponent->GetRotationAtDistanceAlongSpline(Distance, ESplineCoordinateSpace::World).Quaternion();
		Dynamic.AngularVelocity = FVector::ZeroVector;
		Dynamic.AngularAcceleration = FVector::ZeroVector;
	}
	else
	{
		Dynamic.Angular = Position.Angular;
		Dynamic.AngularVelocity = FVector::ZeroVector;
		Dynamic.AngularAcceleration = FVector::ZeroVector;
	}

	// Move the platform
	//MobilityComponent->Teleport(Dynamic);
}