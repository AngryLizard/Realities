// The Gateway of Realities: Planes of Existence.


#include "TGOR_ConstComponent.h"

#include "DimensionSystem/Components/TGOR_PilotComponent.h"
#include "DimensionSystem/Tasks/TGOR_EuclideanPilotTask.h"
#include "CoreSystem/TGOR_Singleton.h"


UTGOR_ConstComponent::UTGOR_ConstComponent()
	: Super(),
	SynchronisesRotationWithTimestamp(true),
	AngularThreshold(0.002f)
{
}

/*
float UTGOR_ConstComponent::Simulate(float Time)
{
	FTGOR_MovementSpace Space = ComputeSpace();

	// Apply acceleration
	Space.LinearVelocity += Space.LinearAcceleration * Time;
	Space.AngularVelocity += Space.AngularAcceleration * Time;

	if (SynchronisesRotationWithTimestamp)
	{
		SINGLETON_RETCHK(Time);
		const FTGOR_Time GameTimestamp = Singleton->GetGameTimestamp();

		// TODO: Synchronise properly
		const FVector Rot = Space.AngularVelocity * GameTimestamp.Value();
		const float Angle = Rot.Size();
		if (Angle >= SMALL_NUMBER)
		{
			const FQuat TrueRotation = FQuat(Rot / Angle, Angle);
			const float AngularDistance = TrueRotation.AngularDistance(Space.Angular);
			if (AngularThreshold.Update(Time, AngularDistance))
			{
				Space.Angular = TrueRotation;
			}
		}
	}

	// Rotate base
	const FVector W = Space.AngularVelocity * (Time / 2);
	const FQuat Rotation = Space.Angular + FQuat(W.X, W.Y, W.Z, 0.0f) * Space.Angular;
	Space.Angular = Rotation.GetNormalized();

	// Translate base
	Space.Linear += Space.LinearVelocity * Time;

	UTGOR_EuclideanPilotTask* PilotTask = GetPOfType<UTGOR_EuclideanPilotTask>();
	if (IsValid(PilotTask))
	{
		PilotTask->SimulateDynamic(Space);
	}

	return Super::Simulate(Time);
}

void UTGOR_ConstComponent::SetVelocity(const FVector& LinearVec, const FVector& AngularVec)
{
	FTGOR_MovementSpace Space = ComputeSpace();
	Space.LinearVelocity = LinearVec;
	Space.AngularVelocity = AngularVec;
	Space.LinearAcceleration = FVector::ZeroVector;
	Space.AngularAcceleration = FVector::ZeroVector;

	UTGOR_EuclideanPilotTask* PilotTask = GetPOfType<UTGOR_EuclideanPilotTask>();
	if (IsValid(PilotTask))
	{
		PilotTask->SimulateDynamic(Space);
	}
}

void UTGOR_ConstComponent::SetAcceleration(const FVector& LinearAcc, const FVector& AngularAcc)
{
	FTGOR_MovementSpace Space = ComputeSpace();
	Space.LinearAcceleration = LinearAcc;
	Space.AngularAcceleration = AngularAcc;

	UTGOR_EuclideanPilotTask* PilotTask = GetPOfType<UTGOR_EuclideanPilotTask>();
	if (IsValid(PilotTask))
	{
		PilotTask->SimulateDynamic(Space);
	}
}

*/