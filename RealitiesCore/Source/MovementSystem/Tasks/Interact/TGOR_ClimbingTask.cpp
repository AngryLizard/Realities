// The Gateway of Realities: Planes of Existence.


#include "TGOR_ClimbingTask.h"
#include "DimensionSystem/Components/TGOR_PilotComponent.h"
#include "MovementSystem/Components/TGOR_MovementComponent.h"
#include "MovementSystem/Content/TGOR_Movement.h"

#include "RealitiesUtility/Utility/TGOR_Math.h"
#include "RealitiesUtility/Structures/TGOR_Direction.h"

#include "Net/UnrealNetwork.h"
#include "Engine/ActorChannel.h"

FTGOR_ClimbingAttachement::FTGOR_ClimbingAttachement()
	: Local(FVector2D::ZeroVector),
	Location(FVector::ZeroVector)
{
}


UTGOR_ClimbingTask::UTGOR_ClimbingTask()
	: Super()
{
}

void UTGOR_ClimbingTask::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

}

float UTGOR_ClimbingTask::GetMaxSpeed() const
{
	return MaximumSpeed;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

bool UTGOR_ClimbingTask::Initialise()
{
	return Super::Initialise();
}

bool UTGOR_ClimbingTask::Invariant(const FTGOR_MovementSpace& Space, const FTGOR_MovementExternal& External) const
{
	if (!Super::Invariant(Space, External))
	{
		return false;
	}

	const FTGOR_MovementFrame& Frame = Identifier.Component->GetFrame();

	const FVector Forward = Space.Angular.GetAxisX();

	TArray<FHitResult> HitResults;
	TraceAttachementPoint(Space, FVector(0.0f, 0.0f, 0.0f), HitResults);
	TraceAttachementPoint(Space, FVector(0.0f, 0.0f, +1.0f), HitResults);
	TraceAttachementPoint(Space, FVector(0.0f, +1.0f, 0.0f), HitResults);
	TraceAttachementPoint(Space, FVector(0.0f, 0.0f, -1.0f), HitResults);
	TraceAttachementPoint(Space, FVector(0.0f, -1.0f, 0.0f), HitResults);
	for (const FHitResult& HitResult : HitResults)
	{
		if ((HitResults[0].Normal | Forward) < 0.0f)
		{
			return true;
		}
	}
	return false;
}

void UTGOR_ClimbingTask::Reset(const FTGOR_MovementSpace& Space, const FTGOR_MovementExternal& External)
{
	Super::Reset(Space, External);
}

void UTGOR_ClimbingTask::QueryInput(FVector& OutInput, FVector& OutView) const
{
	const FQuat InputRotation = Identifier.Component->GetInputRotation();
	const FVector RawInput = Identifier.Component->GetRawInput();
	const float UpInput = RawInput.Z;

	// Remap input along capsule (up is forward, backwards is up)
	const FVector Remapping = FVector(-RawInput.Z, RawInput.Y, RawInput.X);

	const FTGOR_MovementPosition Position = RootComponent->ComputePosition();

	OutInput = Position.Angular * Remapping;
	OutView = InputRotation.GetAxisX();
}

void UTGOR_ClimbingTask::Update(const FTGOR_MovementSpace& Space, const FTGOR_MovementExternal& External, const FTGOR_MovementTick& Tick, FTGOR_MovementOutput& Out)
{
	const FTGOR_MovementBody& Body = RootComponent->GetBody();
	const FTGOR_MovementFrame& Frame = Identifier.Component->GetFrame();
	const FTGOR_MovementInput& State = Identifier.Component->GetState();

	const FVector Forward = Space.Angular.GetAxisX();

	TArray<FHitResult> HitResults;
	TraceAttachementPoint(Space, FVector(0.0f, +1.0f, +1.0f), HitResults);
	TraceAttachementPoint(Space, FVector(0.0f, -1.0f, +1.0f), HitResults);
	TraceAttachementPoint(Space, FVector(0.0f, +1.0f, -1.0f), HitResults);
	TraceAttachementPoint(Space, FVector(0.0f, -1.0f, -1.0f), HitResults);
	TraceAttachementPoint(Space, FVector(0.0f, 0.0f, 0.0f), HitResults);

	//
	const float GravityAdapt = 750.0f;
	const float SurfaceAdapt = 90'000.0f;
	TArray<FTGOR_ClimbingAttachement> Attachements;
	if (HitResults.Num() == 0)
	{
		// Rotate towards upvector if freefalling
		Out.Torque += (External.UpVector ^ Forward) * GravityAdapt;
	}
	else
	{
		const float DesiredDistance = Body.Radius * DesiredDistanceMultiplier;

		// Compute input params
		const FVector InputProject = State.Input - Forward * (Forward | State.Input);
		const FTGOR_Direction Input = InputProject;

		// Find body bend
		float Bendness = 0.0f;
		for (const FHitResult& HitResult : HitResults)
		{
			const float Dot = HitResult.Normal | Forward;
			Bendness += Dot * Dot;
		}
		Bendness = FMath::Max(Bendness / HitResults.Num(), 0.5f);

		// Apply for all attachements
		for (const FHitResult& HitResult : HitResults)
		{
			FTGOR_ClimbingAttachement Attachement;
			//DrawDebugLine(GetWorld(), HitResult.TraceStart, HitResult.TraceEnd, FColor::Red, false, -1.0f, 0, 5.0f);

			// Flatten actual offset to prevent oscillation
			const FVector ForwardOffset = HitResult.ImpactPoint - Space.Linear;
			const FVector Offset = (ForwardOffset - Forward * (ForwardOffset | Forward)) / 10'000.0f;

			// Compute velocity relative to wall
			const FVector Angular = Offset ^ Space.RelativeAngularVelocity;
			const FVector Linear = Space.RelativeLinearVelocity + Angular;
			const float VerticalSpeed = (Linear | Forward);
			const FVector RelativeVelocity = Linear - VerticalSpeed * Forward;

			// Push away from surface
			const float DistanceRatio = FMath::Clamp(1.0f - HitResult.Distance / DesiredDistance, -1.0f, 1.0f);
			const float PushForce = DistanceRatio * SurfaceAdapt;

			// Compute actual external force per limb
			const FVector ExternalForce = External.Force / HitResults.Num();
			const float ExternalPushForce = (ExternalForce | Forward);

			// Compute push/pull force
			const float Ratio = DistanceRatio * DistanceRatio * DistanceRatio;
			const float FinalMaxPushForce = (1.0f - FMath::Abs(Ratio)) * MaxPushForce * Frame.Strength;
			const float FinalMaxPullForce = (1.0f + Ratio) * MaxPullForce * Frame.Strength;
			const float TotalForce = ExternalPushForce + PushForce + VerticalSpeed * LinearDamping;
			FVector Force = Forward * -FMath::Clamp(TotalForce, -FinalMaxPushForce, FinalMaxPullForce);

			// Push along surface
			const FVector BrakeForce = RelativeVelocity * BrakeCoefficient + (ExternalForce - ExternalPushForce * Forward);
			if (Input)
			{
				// Turn off input force if too fast, counteract braking in movement direction if moving
				const float Speed = RelativeVelocity.Size();
				const float FinalMaximumSpeed = Bendness * Bendness * MaximumSpeed * GetSpeedRatio();
				const float UnderSpeedRatio = FMath::Clamp(1.0f - (Speed - FinalMaximumSpeed) / FinalMaximumSpeed, 0.0f, 1.0f);

				// Counteract braking towards max velocity
				const FVector AntiBrake = Input.Direction * ((BrakeForce | Input.Direction) * UnderSpeedRatio);
				Force += (Input.Direction * MaxDragForce * Frame.Strength + AntiBrake) * UnderSpeedRatio - BrakeForce;
			}
			else
			{
				Force -= BrakeForce;
			}

			Out.Force += Force;

			// Apply torque around forward axis only if there is one attachement point
			const FVector Torque = (Offset ^ Force);
			Out.Torque += Torque;
			if (HitResults.Num() > 1)
			{
				Out.Torque -= Forward * (Forward | Torque);
			}

			// Set base, the results are ordered in a way that prioritises the central trace
			//Out.Parent = Identifier.Component->FindReparentToActor(HitResult.GetActor(), HitResult.BoneName);

			// Find hand location
			const FVector Average = (HitResult.TraceEnd + HitResult.TraceStart) / 2;
			const FVector Local = Space.Angular.UnrotateVector(Average - Space.Linear);
			Attachement.Local = FVector2D(Local.Y, Local.Z);
			Attachement.Location = HitResult.Location;
			Attachements.Emplace(Attachement);
		}
	}

	// Dampen rotation
	Out.Torque -= External.Torque + Space.RelativeAngularVelocity * AngularDamping;

	const FVector UpVector = Space.Angular.GetAxisZ();
	Out.Torque += Forward * ((UpVector ^ External.UpVector) | Forward) * 250.0f;

	ClimbingAnimationTick(Space, Attachements);

	Super::Update(Space, External, Tick, Out);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

FVector UTGOR_ClimbingTask::ComputeInputForce(const FTGOR_MovementSpace& Space) const
{
	const FTGOR_MovementInput& State = Identifier.Component->GetState();
	const FTGOR_MovementFrame& Frame = Identifier.Component->GetFrame();

	const FVector BrakeForce = Space.RelativeLinearVelocity * BrakeCoefficient;

	const float Speed = Space.RelativeLinearVelocity.Size();
	const float FinalMaximumSpeed = MaximumSpeed * GetSpeedRatio();
	if (FinalMaximumSpeed >= SMALL_NUMBER)
	{
		const float SpeedRatio = FMath::Min(Speed / FinalMaximumSpeed, 1.0f);

		// Compute correction vector against current movement direction
		const FTGOR_Direction Input = State.Input;
		if (Input)
		{
			// Turn off input force if too fast, counteract braking in movement direction if moving
			const float UnderSpeedRatio = FMath::Clamp(1.0f - (Speed - FinalMaximumSpeed) / FinalMaximumSpeed, 0.0f, 1.0f);

			// Counteract braking towards max velocity
			const FVector AntiBrake = Input.Direction * (BrakeForce | Input.Direction) * UnderSpeedRatio;
			return((State.Input * MaxDragForce * Frame.Strength + AntiBrake) * UnderSpeedRatio - BrakeForce);
		}
	}
	return(-BrakeForce);
}

bool UTGOR_ClimbingTask::TraceAttachementPoint(const FTGOR_MovementSpace& Space, const FVector& Mask, TArray<FHitResult>& HitResults) const
{
	const FTGOR_MovementBody& Body = RootComponent->GetBody();

	// Compute point offset
	const FVector LocalOffset = FVector(0.0f, Body.Radius * 1.5f, Body.Height * 0.5f) * Mask;
	const FVector GlobalOffset = Space.Angular * LocalOffset;

	const float Bend = 0.3f;
	const float Walldist = -0.7f;

	// Compute trace direction
	const FVector Forward = Space.Angular.GetAxisX();
	const float TraceDistance = Body.Radius * DesiredDistanceMultiplier * 1.5f;
	const FVector TraceBackward = Forward * TraceDistance * Walldist;

	// Trace to find maximum amount of force that can be applied
	FHitResult Hit;
	if (RootComponent->CenteredTrace(Space.Linear + GlobalOffset * Bend - TraceBackward, GlobalOffset * (1.0f - Bend), Forward, TraceDistance * 2, Body.Radius / 2, Hit))
	{
		HitResults.Add(Hit);
		return true;
	}
	return false;
}