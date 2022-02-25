// The Gateway of Realities: Planes of Existence.


#include "TGOR_GuidedComponent.h"
#include "GameFramework/Actor.h"

#include "Realities/Base/TGOR_Singleton.h"
#include "Net/UnrealNetwork.h"


UTGOR_GuidedComponent::UTGOR_GuidedComponent()
	: Super(),
	Timestamp()
{
}

void UTGOR_GuidedComponent::BeginPlay()
{
	Super::BeginPlay();

	SINGLETON_CHK;
	Timestamp = Singleton->GetGameTimestamp();
}

void UTGOR_GuidedComponent::SetVelocity(const FVector& LinearVec, const FVector& AngularVec, const FVector& LinearAcc, const FVector& AngularAcc)
{
	Base.LinearVelocity = LinearVec;
	Base.AngularVelocity = AngularVec;
	Base.LinearAcceleration = LinearAcc;
	Base.AngularAcceleration = AngularAcc;
}

void UTGOR_GuidedComponent::SimulateVelocity(const FVector& LinearVec, const FVector& AngularVec)
{
	SINGLETON_CHK;

	// Compute time delta to sample forces
	const FTGOR_Time GameTimestamp = Singleton->GetGameTimestamp();
	const float DeltaTime = GameTimestamp - Timestamp;
	if (DeltaTime > SMALL_NUMBER)
	{
		Timestamp = GameTimestamp;
		Base.LinearAcceleration = (LinearVec - Base.LinearVelocity) / DeltaTime;
		Base.AngularAcceleration = (AngularVec - Base.AngularVelocity) / DeltaTime;
	}

	// Set velocities
	Base.LinearVelocity = LinearVec;
	Base.AngularVelocity = AngularVec;
}
