// The Gateway of Realities: Planes of Existence.


#include "TGOR_WaterTask.h"
#include "DimensionSystem/Components/TGOR_PilotComponent.h"
#include "MovementSystem/Components/TGOR_MovementComponent.h"

#include "Net/UnrealNetwork.h"
#include "Engine/ActorChannel.h"

UTGOR_WaterTask::UTGOR_WaterTask()
	: Super()
{
}

void UTGOR_WaterTask::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

}

void UTGOR_WaterTask::Initialise()
{
	Super::Initialise();
}

bool UTGOR_WaterTask::Invariant(const FTGOR_MovementSpace& Space, const FTGOR_MovementExternal& External) const
{
	return Super::Invariant(Space, External) &&  CanSwimInCurrentSurroundings(External);
}

void UTGOR_WaterTask::Reset(const FTGOR_MovementSpace& Space, const FTGOR_MovementExternal& External)
{
	Super::Reset(Space, External);
}

void UTGOR_WaterTask::QueryInput(FVector& OutInput, FVector& OutView) const
{
	return Super::QueryInput(OutInput, OutView);
}

void UTGOR_WaterTask::Update(const FTGOR_MovementSpace& Space, const FTGOR_MovementExternal& External, const FTGOR_MovementTick& Tick, FTGOR_MovementOutput& Out)
{
	const FTGOR_MovementFrame& Frame = Identifier.Component->GetFrame();
	const FTGOR_MovementBody& Body = RootComponent->GetBody();

	// Compute input dependent forces
	const float SpeedRatio = GetInputForce(Tick, Space, External, Out);

	// Counteract external forces and rotate upside
	Out.Force -= External.Force.GetClampedToMaxSize(WaterControl * Frame.Strength);

	FHitResult Hit;
	FVector RepelForce = FVector::ZeroVector;
	//if (Component->Trace(FVector::ZeroVector, FVector::ZeroVector, 0.0f, 100.0f, Hit))
	if (RootComponent->Overlap(Space.Linear, Space.Angular * FQuat(FVector::RightVector, PI / 2), 75.0f, 200.0f, Hit))
	{
		const float RepelSpring = WallRepelStrength * Frame.Strength;
		RepelForce += Hit.Normal * Hit.PenetrationDepth * RepelSpring;

		RepelForce -= Space.RelativeLinearVelocity * FMath::Sqrt(RepelSpring * Body.Mass);
		const float RepelStrength = RepelForce | Hit.Normal;
		RepelForce = Hit.Normal * FMath::Max(0.0f, RepelStrength);
	}
	
	Out.Force += RepelForce;

	// Correctional torque against external torque
	Out.Torque -= External.Torque;

	// Call on animation
	SwimmingAnimationTick(Space);

	Super::Update(Space, External, Tick, Out);
}


////////////////////////////////////////////////////////////////////////////////////////////////////

bool UTGOR_WaterTask::CanSwimInCurrentSurroundings(const FTGOR_MovementExternal& External) const
{
	return Identifier.Component->GetBouyancyRatio(External.Surroundings) > BouyancyThreshold;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

float UTGOR_WaterTask::GetInputForce(const FTGOR_MovementTick& Tick, const FTGOR_MovementSpace& Space, const FTGOR_MovementExternal& External, FTGOR_MovementOutput& Out) const
{
	// Dampen to prevent oscillation
	Identifier.Component->GetDampingTorque(Tick, Space.RelativeAngularVelocity, AngularDamping, Out);
	return 0.0f;
}