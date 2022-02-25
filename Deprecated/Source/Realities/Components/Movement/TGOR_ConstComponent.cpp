// The Gateway of Realities: Planes of Existence.


#include "TGOR_ConstComponent.h"
#include "GameFramework/Actor.h"

#include "Realities/Base/TGOR_Singleton.h"
#include "Net/UnrealNetwork.h"


UTGOR_ConstComponent::UTGOR_ConstComponent()
	: Super(),
	SynchronisesRotationWithTimestamp(true),
	AngularThreshold(0.002f)
{
}

void UTGOR_ConstComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	FTGOR_MovementSpace Space = ComputeSpace();

	// Apply acceleration
	Space.LinearVelocity += Space.LinearAcceleration * DeltaTime;
	Space.AngularVelocity += Space.AngularAcceleration * DeltaTime;

	if (SynchronisesRotationWithTimestamp)
	{
		SINGLETON_CHK;
		const FTGOR_Time GameTimestamp = Singleton->GetGameTimestamp();

		// TODO: Synchronise properly
		const FVector Rot = Space.AngularVelocity * GameTimestamp.Value();
		const float Angle = Rot.Size();
		if (Angle >= SMALL_NUMBER)
		{
			const FQuat TrueRotation = FQuat(Rot / Angle, Angle);
			const float AngularDistance = TrueRotation.AngularDistance(Space.Angular);
			if (AngularThreshold.Update(DeltaTime, AngularDistance))
			{
				Space.Angular = TrueRotation;
			}
		}
	}

	// Rotate base
	const FVector W = Space.AngularVelocity * (DeltaTime / 2);
	const FQuat Rotation = Space.Angular + FQuat(W.X, W.Y, W.Z, 0.0f) * Space.Angular;
	Space.Angular = Rotation.GetNormalized();

	// Translate base
	Space.Linear += Space.LinearVelocity * DeltaTime;

	SimulateDynamic(Space);
}

void UTGOR_ConstComponent::SetVelocity(const FVector& LinearVec, const FVector& AngularVec)
{
	FTGOR_MovementSpace Space = ComputeSpace();
	Space.LinearVelocity = LinearVec;
	Space.AngularVelocity = AngularVec;
	Space.LinearAcceleration = FVector::ZeroVector;
	Space.AngularAcceleration = FVector::ZeroVector;
	SimulateDynamic(Space);
}

void UTGOR_ConstComponent::SetAcceleration(const FVector& LinearAcc, const FVector& AngularAcc)
{
	FTGOR_MovementSpace Space = ComputeSpace();
	Space.LinearAcceleration = LinearAcc;
	Space.AngularAcceleration = AngularAcc;
	SimulateDynamic(Space);
}
