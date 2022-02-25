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
	
	// Apply acceleration
	Base.LinearVelocity += Base.LinearAcceleration * DeltaTime;
	Base.AngularVelocity += Base.AngularAcceleration * DeltaTime;

	if (SynchronisesRotationWithTimestamp)
	{
		SINGLETON_CHK;
		const FTGOR_Time GameTimestamp = Singleton->GetGameTimestamp();

		// TODO: Synchronise properly
		const FVector Rot = Base.AngularVelocity * GameTimestamp.Value();
		const float Angle = Rot.Size();
		if (Angle >= SMALL_NUMBER)
		{
			const FQuat TrueRotation = FQuat(Rot / Angle, Angle);
			const float AngularDistance = TrueRotation.AngularDistance(Base.Rotation);
			if (AngularThreshold.Update(DeltaTime, AngularDistance))
			{
				Base.Rotation = TrueRotation;
			}
		}
	}

	// Rotate base
	const FVector W = Base.AngularVelocity * (DeltaTime / 2);
	const FQuat Rotation = Base.Rotation + FQuat(W.X, W.Y, W.Z, 0.0f) * Base.Rotation;
	Base.Rotation = Rotation.GetNormalized();

	// Translate base
	const FVector Translation = Base.LinearVelocity * DeltaTime;

	if (Base.Component.IsValid())
	{
		// Move component
		FHitResult Hit;
		Base.Component->SetWorldLocation(Base.Location);
		Base.Component->MoveComponent(Translation, Base.Rotation, false, &Hit, MOVECOMP_NoFlags, ETeleportType::None);

		// Update position
		Base.Location = Base.Component->GetComponentLocation();
	}
}

void UTGOR_ConstComponent::SetVelocity(const FVector& LinearVec, const FVector& AngularVec)
{
	Base.LinearVelocity = LinearVec;
	Base.AngularVelocity = AngularVec;
	Base.LinearAcceleration = FVector::ZeroVector;
	Base.AngularAcceleration = FVector::ZeroVector;
}

void UTGOR_ConstComponent::SetAcceleration(const FVector& LinearAcc, const FVector& AngularAcc)
{
	Base.LinearAcceleration = LinearAcc;
	Base.AngularAcceleration = AngularAcc;
}
