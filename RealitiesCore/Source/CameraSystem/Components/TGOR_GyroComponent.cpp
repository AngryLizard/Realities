// The Gateway of Realities: Planes of Existence.


#include "TGOR_GyroComponent.h"

#include "DimensionSystem/Components/TGOR_PilotComponent.h"
#include "PhysicsSystem/Interfaces/TGOR_PilotSimulationInterface.h"
#include "RealitiesUtility/Utility/TGOR_Math.h"
#include "RealitiesUtility/Utility/TGOR_Error.h"

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

	if (PhysicsComponent)
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
		const float AngleSq = Axis.SizeSquared();
		if (AngleSq >= SMALL_NUMBER)
		{
			const float Angle = FMath::Sqrt(AngleSq);
			GyroRotation = (FQuat(Axis / Angle, Angle * Stabilisation * DeltaTime) * GyroRotation).GetNormalized();
		}
	}

	// Prevent shifting rotation when parent turns
	SetWorldRotation(GyroRotation);
}

void UTGOR_GyroComponent::BeginPlay()
{
	Super::BeginPlay();

	AActor* Actor = GetOwner();
	TArray<UActorComponent*> Components = Actor->GetComponentsByInterface(UTGOR_PilotSimulationInterface::StaticClass());
	if (Components.Num() == 0)
	{
		ERROR("No PilotSimulation found", Error);
	}

	PhysicsComponent = TScriptInterface<ITGOR_PilotSimulationInterface>(Components[0]);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

