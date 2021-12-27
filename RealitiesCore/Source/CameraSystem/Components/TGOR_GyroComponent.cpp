// The Gateway of Realities: Planes of Existence.


#include "TGOR_GyroComponent.h"

#include "DimensionSystem/Components/TGOR_PilotComponent.h"
#include "PhysicsSystem/Components/TGOR_PhysicsComponent.h"
#include "RealitiesUtility/Utility/TGOR_Math.h"

#include "Engine.h"
#include "Net/UnrealNetwork.h"

UTGOR_GyroComponent::UTGOR_GyroComponent(const FObjectInitializer& ObjectInitializer)
:	Super(ObjectInitializer),
	Stabilisation(10.0f)
{
	PrimaryComponentTick.bCanEverTick = true;
}


void UTGOR_GyroComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (PhysicsComponent.IsValid())
	{
		UTGOR_PilotComponent* Pilot = PhysicsComponent->GetRootPilot();
		if (IsValid(Pilot))
		{
			const FTGOR_MovementDynamic Base = Pilot->ComputeBase();

			const FVector W = Base.AngularVelocity * (DeltaTime / 2);
			const FQuat DeltaQuat = FQuat(W.X, W.Y, W.Z, 0.0f);
			GyroRotation = GyroRotation + DeltaQuat * GyroRotation;
			GyroRotation.Normalize();
		}

		const FVector& UpVector = PhysicsComponent->ComputePhysicsUpVector();
		const FVector Right = GyroRotation.GetAxisX() ^ UpVector;
		const FVector Axis = Right ^ GyroRotation.GetAxisY();
		const float Angle = Axis.Size();
		if (Angle >= SMALL_NUMBER)
		{
			GyroRotation = (FQuat(Axis / Angle, Angle * Stabilisation * DeltaTime) * GyroRotation).GetNormalized();
		}
	}

	// Prevent shifting rotation when parent turns
	SetWorldRotation(GyroRotation);
}

void UTGOR_GyroComponent::BeginPlay()
{
	Super::BeginPlay();

	PhysicsComponent = GetOwnerComponent<UTGOR_PhysicsComponent>();
}

////////////////////////////////////////////////////////////////////////////////////////////////////

