// The Gateway of Realities: Planes of Existence.


#include "TGOR_RagdollTask.h"
#include "DimensionSystem/Components/TGOR_PilotComponent.h"
#include "MovementSystem/Components/TGOR_MovementComponent.h"

#include "RealitiesUtility/Utility/TGOR_Math.h"

#include "Net/UnrealNetwork.h"
#include "Engine/ActorChannel.h"

UTGOR_RagdollTask::UTGOR_RagdollTask()
	: Super()
{
}

void UTGOR_RagdollTask::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

}

bool UTGOR_RagdollTask::Initialise()
{
	return Super::Initialise();
}

bool UTGOR_RagdollTask::Invariant(const FTGOR_MovementSpace& Space, const FTGOR_MovementExternal& External) const
{
	return Super::Invariant(Space, External);
}

void UTGOR_RagdollTask::Reset(const FTGOR_MovementSpace& Space, const FTGOR_MovementExternal& External)
{
	Super::Reset(Space, External);
}

void UTGOR_RagdollTask::QueryInput(FVector& OutInput, FVector& OutView) const
{
	const float InputStrength = Identifier.Component->GetInputStrength();
	const FVector UpVector = Identifier.Component->ComputePhysicsUpVector();
	const FVector RawInput = Identifier.Component->GetRawInput();

	OutInput = UpVector * FMath::Min(InputStrength * RawInput.Size() * 2.0f, 1.0f);
}

void UTGOR_RagdollTask::Update(const FTGOR_MovementSpace& Space, const FTGOR_MovementExternal& External, const FTGOR_MovementTick& Tick, FTGOR_MovementOutput& Out)
{
	const FTGOR_MovementInput& State = Identifier.Component->GetState();
	const FTGOR_MovementFrame& Frame = Identifier.Component->GetFrame();
	const FTGOR_MovementBody& Body = RootComponent->GetBody();

	const float Radius = FVector2D(Body.Radius, Body.Height * 0.5).Size();
	const FVector ExternalBias = -External.UpVector * ExternalBiasMultiplier;
	const FVector Direction = -External.UpVector;//(Space.LinearVelocity + ExternalBias).GetSafeNormal();
	const FVector Translation = Direction * DetectionDistance;

	// Trace surrounding walls
	ImpactTime = 1.0f;
	ImpactLocation = Space.Linear;
	ImpactNormal = -Direction;

	FHitResult Hit;
	if (RootComponent->MovementSphereTraceSweep(Radius, Space.Linear, Translation, Hit))
	{
		ImpactTime = Hit.Time;
		ImpactNormal = Hit.Normal;
		ImpactLocation = Hit.ImpactPoint;
	}

	if (RootComponent->MovementSphereTraceSweep(Radius, Space.Linear, -Translation, Hit) && Hit.Time < ImpactTime)
	{
		ImpactTime = Hit.Time;
		ImpactNormal = Hit.Normal;
		ImpactLocation = Hit.ImpactPoint;
	}

	// Amount of how much we're going towards resting position
	const float Rest = FMath::GetMappedRangeValueClamped(FVector2D(UprightThreshold, 1), FVector2D(0, 1), (1.f - ImpactTime));

	// Dampen forces
	Out.AddDampingForce(Tick, Space.RelativeLinearVelocity, LinearDamping * Rest * Frame.Strength);
	Out.AddDampingTorque(Tick, Space.RelativeAngularVelocity, AngularDamping * Rest * Frame.Strength);

	// Rotate towards said axis
	Out.Torque += ((Space.Angular * LocalUpVector) ^ ImpactNormal) * (UprightTorque * (Frame.Strength * Rest));

	const FVector RelativeVelocity = Space.RelativeLinearVelocity;// +(Space.RelativeAngularVelocity ^ (ImpactLocation - Space.Linear));
	Out.Force += FVector::VectorPlaneProject(RelativeVelocity * (-BreakForce * Rest) - External.Force, ImpactNormal) * (ImpactNormal | External.UpVector);


	Super::Update(Space, External, Tick, Out);
}

float UTGOR_RagdollTask::GetMaxSpeed() const
{
	return MaxSpeed;
}