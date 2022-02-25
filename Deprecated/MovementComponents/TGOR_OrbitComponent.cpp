// The Gateway of Realities: Planes of Existence.

#include "TGOR_OrbitComponent.h"
#include "GameFramework/Actor.h"
#include "Net/UnrealNetwork.h"

#include "Realities/Utility/TGOR_Math.h"
#include "DrawDebugHelpers.h"
#include "Realities/Base/TGOR_Singleton.h"

UTGOR_OrbitComponent::UTGOR_OrbitComponent()
:	Super(),
	SynchronisesOrbitWithTimestamp(true),
	OrbitThreshold(0.1f),
	Speed(0.02f),
	Plane(FVector::UpVector),
	Center(FVector::ZeroVector),
	CorrectionForce(500.0f)
{
	OrbitThreshold.MaxThreshold = 0.001f;
	OrbitThreshold.UpdateTime = 0.5f;
}

void UTGOR_OrbitComponent::BeginPlay()
{
	Super::BeginPlay();
	
	Major = Base.Location - Center;
	const FVector Minor = Major ^ Plane;

	Radius = Major.Size();
	Axis = UTGOR_Math::Normalize(Major ^ Minor);

	const FVector AngularVelocity = Axis * Speed;
	Base.LinearVelocity = AngularVelocity ^ Major;

	Frame.Faces = FVector::ZeroVector;
}

void UTGOR_OrbitComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	
	if (SynchronisesOrbitWithTimestamp)
	{
		SINGLETON_CHK;
		const FTGOR_Time GameTimestamp = Singleton->GetGameTimestamp();
		const float Time = GameTimestamp.Modf(2 *PI, Speed, 0.0f);

		const FVector TrueLocation = Center + FQuat(Axis, Time) * Major;
		const float LocationDistance = (TrueLocation - Base.Location).SizeSquared();
		if (OrbitThreshold.Update(DeltaTime, LocationDistance))
		{
			Base.Location = TrueLocation;
			Base.LinearVelocity = (Axis * Speed) ^ (TrueLocation - Center);
			if (Base.Component.IsValid())
			{
				Base.Component->SetWorldLocation(Base.Location);
			}
		}
	}
}

void UTGOR_OrbitComponent::ComputePhysics(const FTGOR_MovementTick& Tick, const FTGOR_MovementSpace& Space, bool Replay, FTGOR_MovementOutput& Output)
{
	// Apply acceleration to make planet orbit
	const FVector AngularVelocity = Axis * Speed;
	const FVector CurrentMajor = Base.Location - Center;
	const FVector LinearVelocity = AngularVelocity ^ CurrentMajor;
	const FVector LinearAcceleration = AngularVelocity ^ LinearVelocity;

	const FVector Force = GetMassedLinear(LinearAcceleration);
	Output.Force = Force; //  - Space.ExternalForce
	
	const float Size = CurrentMajor.Size();
	if (Size > SMALL_NUMBER)
	{
		const FVector Normal = CurrentMajor / Size;
		Output.Force += Normal * (Size - Radius) * CorrectionForce;
	}
}
