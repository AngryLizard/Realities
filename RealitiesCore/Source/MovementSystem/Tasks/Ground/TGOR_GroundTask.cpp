// The Gateway of Realities: Planes of Existence.


#include "TGOR_GroundTask.h"
#include "AnimationSystem/TGOR_AnimationLibrary.h"
#include "AnimationSystem/Components/TGOR_HandleComponent.h"
#include "AnimationSystem/Components/TGOR_AttachComponent.h"
#include "DimensionSystem/Components/TGOR_PilotComponent.h"
#include "MovementSystem/Components/TGOR_MovementComponent.h"
#include "DimensionSystem/Tasks/TGOR_LinearPilotTask.h"
#include "DimensionSystem/Content/TGOR_Pilot.h"

#include "RealitiesUtility/Utility/TGOR_Math.h"
#include "RealitiesUtility/Structures/TGOR_Direction.h"

#include "Net/UnrealNetwork.h"
#include "Engine/ActorChannel.h"

UTGOR_GroundTask::UTGOR_GroundTask()
	: Super()
{
}

////////////////////////////////////////////////////////////////////////////////////////////////////

bool UTGOR_GroundTask::Initialise()
{
	return Super::Initialise();
}

bool UTGOR_GroundTask::Invariant(const FTGOR_MovementSpace& Space, const FTGOR_MovementExternal& External) const
{
	if (!Super::Invariant(Space, External))
	{
		return false;
	}

	// Check for speed
	return Space.RelativeLinearVelocity.Size() < LinearSpeedThreshold && Space.RelativeAngularVelocity.Size() < AngularSpeedThreshold;
}

void UTGOR_GroundTask::Reset(const FTGOR_MovementSpace& Space, const FTGOR_MovementExternal& External)
{
	Super::Reset(Space, External);
}

void UTGOR_GroundTask::QueryInput(FVector& OutInput, FVector& OutView) const
{
	const FVector UpVector = Identifier.Component->ComputePhysicsUpVector();
	const FQuat InputRotation = Identifier.Component->GetInputRotation();
	const FVector InputDirection = InputRotation.GetAxisX();
	const float InputStrength = Identifier.Component->GetInputStrength();

	OutView = InputDirection - UpVector * (InputDirection | UpVector);
	OutView.Normalize();

	const FVector RawInput = Identifier.Component->GetRawInput();
	const float UpInput = RawInput.Z;
	OutInput = UpVector * UpInput;

	// Compute normalised input
	const FVector2D Plane = FVector2D(RawInput);
	const float Size = Plane.Size();
	if (Size >= SMALL_NUMBER)
	{
		const FVector2D PlaneInput = Plane / Size;
		const float InputSize = Size / UTGOR_Math::ProjectToBox(PlaneInput);

		// Compute vector base with current capsule up-vector for yaw
		const FVector Right = UpVector ^ InputDirection;
		const FVector Forward = Right ^ UpVector;
		const float Input = FMath::Min(1.0f, InputStrength * InputSize);
		const FVector Directive = (Forward * PlaneInput.X + Right * PlaneInput.Y);

		// Normalise input
		float DirectiveSize = Directive.Size();
		if (DirectiveSize >= SMALL_NUMBER)
		{
			// If upside down we want to keep going the opposite direction so we don't turn around
			const FVector ViewUpDirection = InputRotation.GetAxisZ();
			const float UpSign = FMath::Sign(ViewUpDirection | UpVector);

			// Combine up and flat movement
			const FVector Normal = Directive / DirectiveSize * UpSign;
			OutInput += Normal * Input;
		}
	}

}

void UTGOR_GroundTask::Context(const FTGOR_MovementSpace& Space, const FTGOR_MovementExternal& External, const FTGOR_MovementTick& Tick)
{
	Super::Context(Space, External, Tick);
}

void UTGOR_GroundTask::Update(const FTGOR_MovementSpace& Space, const FTGOR_MovementExternal& External, const FTGOR_MovementTick& Tick, FTGOR_MovementOutput& Output)
{
	const FTGOR_MovementFrame& Frame = Identifier.Component->GetFrame();

	// Analyze ground
	MovementOrientation = Space.Angular * LocalUpVector;
	UpdateGroundHandles(Space, External.UpVector);
	TraceForGroundContact(Space, External.UpVector);

	// Add movement dependent external forces to total external forces
	FTGOR_MovementRepel Repel;
	//GetRepelForce(Component, Tick, Space, Repel, Out);

	MaxGroundRatio = 1.0f; //FMath::Clamp(2.0f - MovementContact.MaxGroundRatio, 0.0f, 1.0f);
	MinGroundRatio = 0.0f; // FMath::Clamp(1.0f - MovementContact.MinGroundRatio, 0.0f, MaxGroundRatio);

	// Compute input force
	const float RawRatio = GetInputForce(Tick, Space, External, Repel, Output);
	const float TotalSpeedRatio = FMath::Clamp(RawRatio * MovementContact.FrameVelocity.SizeSquared() / FMath::Square(GetMaxSpeed()), 0.0f, 1.0f);
	const float InputSpeedRatio = FMath::Clamp(RawRatio, 0.0f, 1.0f);

	// Compute force to make character stand, only take slope into consideration when moving
	const float Stretch = GetStretch(Tick, Space, External);
	GetStandingForce(Tick, Space, External, MovementContact.FrameNormal, Stretch, Output);

	const FVector SlopeForward = FVector::VectorPlaneProject(External.UpVector, MovementContact.FrameNormal).GetSafeNormal();
	const float SlopeIntensity = FMath::Square(MovementContact.FrameForward | SlopeForward);
	const FVector Normal = FMath::Lerp(External.UpVector, MovementContact.FrameNormal, TotalSpeedRatio * SlopeIntensity).GetSafeNormal();
	GetStandingTorque(Tick, Space, External, Normal, TotalSpeedRatio, Output);

	// Compute force applied from the ground to achieve input torque
	Output.Force += MovementContact.SurfaceOffset ^ Output.Torque;

	// Compute final desired force, try to counteract external forces
	Output.Force -= Repel.RepelForce + External.Force;
	LimitForces(Space, External, InputSpeedRatio, Output);

	// Apply external forces
	Output.Force += Repel.RepelForce;

	Super::Update(Space, External, Tick, Output);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void UTGOR_GroundTask::GetGroundContact(const FTGOR_MovementSpace& Space, FVector& SurfaceLocation, FVector& SurfaceNormal) const
{
	SurfaceNormal = MovementContact.SurfaceNormal;
	SurfaceLocation = Space.Linear + MovementContact.SurfaceOffset;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void UTGOR_GroundTask::UpdateGroundHandles(const FTGOR_MovementSpace& Space, const FVector& UpVector)
{
	const FTGOR_MovementBody& Body = RootComponent->GetBody();
	const FVector Up = (TraceUpVector ? UpVector : MovementOrientation);

	float Weight = 0.0f;
	float GroundRatio = 0.0f;
	float MinRatio = FLT_MAX;
	float MaxRatio = 0.0f;
	FVector Normal = FVector::ZeroVector;
	FVector Delta = FVector::ZeroVector;
	for (UTGOR_HandleComponent* Foot : Primitives)
	{
		FTGOR_ConeTraceOutput HandleTrace;
		const float Radius = Foot->GetScaledCapsuleRadius();
		if (Foot->MovementCone->TraceSpread(RootComponent.Get(), Space, Radius, TraceLengthMultiplier, HandleTrace))
		{
			const FTGOR_MovementDynamic Dynamic = HandleTrace.GetDynamicFromTrace(Space);

			UTGOR_LinearPilotTask* Task = Foot->GetCurrentPOfType<UTGOR_LinearPilotTask>();
			if (IsValid(Task))
			{
				// Reparent
				Task->Parent(HandleTrace.Parent.Mobility, HandleTrace.Parent.Index);
				Task->SimulateDynamic(Dynamic);
			}

			const float Ratio = -(HandleTrace.Delta | MovementOrientation) / Foot->MovementCone->LimitRadius;
			MinRatio = FMath::Min(MinRatio, Ratio);
			MaxRatio = FMath::Max(MaxRatio, Ratio);
			GroundRatio += Ratio;

			Normal += Dynamic.Angular.GetAxisZ();
			Delta += Dynamic.Linear - Space.Linear;
			Weight += 1.0f;
		}
		else
		{
			GroundRatio += TraceLengthMultiplier * 0.1f;
			Normal += Up * 0.1f;
			Delta -= Up * (Foot->MovementCone->LimitRadius * 0.1f);
			Weight += 0.1f;
		}
	}

	if (Weight < SMALL_NUMBER)
	{
		// Default to plane
		MovementContact.SurfaceNormal = Up;
		MovementContact.Slope = 0.0f;
		MovementContact.GroundRatio = TraceLengthMultiplier;
		MovementContact.SurfaceOffset = FVector::ZeroVector;
		MinRatio = MaxRatio = 1.0f;
	}
	else
	{
		MovementContact.SurfaceNormal = UTGOR_Math::Normalize(Normal);
		MovementContact.Slope = MovementContact.SurfaceNormal | Up;
		MovementContact.GroundRatio = GroundRatio / Weight;
		MovementContact.SurfaceOffset = Delta / Weight;
	}

	MovementContact.MinGroundRatio = MinRatio;
	MovementContact.MaxGroundRatio = MaxRatio;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

float UTGOR_GroundTask::GetStretch(const FTGOR_MovementTick& Tick, const FTGOR_MovementSpace& Space, const FTGOR_MovementExternal& External) const
{
	return 1.0f;
}

float UTGOR_GroundTask::GetInputForce(const FTGOR_MovementTick& Tick, const FTGOR_MovementSpace& Space, const FTGOR_MovementExternal& External, const FTGOR_MovementRepel& Repel, FTGOR_MovementOutput& Out) const
{
	const FTGOR_MovementFrame& Frame = Identifier.Component->GetFrame();

	// Filter sideways input
	Out.AddDampingForce(Tick, MovementContact.FrameVelocity, BrakeCoefficient * Frame.Strength);
	Out.AddDampingTorque(Tick, Space.RelativeAngularVelocity, AngularDamping * Frame.Strength);
	return 0.0f;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void UTGOR_GroundTask::TraceForGroundContact(const FTGOR_MovementSpace& Space, const FVector& UpVector)
{
	const FTGOR_MovementInput& State = Identifier.Component->GetState();

	const FVector Angular = Space.RelativeAngularVelocity ^ MovementContact.SurfaceOffset;
	const FVector Linear = Space.RelativeLinearVelocity + Angular;

	// Compute actual normal from both averaged and sampled
	const FVector FlatVelocity = Linear - UpVector * (UpVector | Linear);
	const float SpeedRatio = FlatVelocity.SizeSquared() / (SpeedThreshold * SpeedThreshold);
	MovementContact.FrameNormal = UTGOR_Math::Normalize(FMath::Lerp(UpVector, MovementContact.SurfaceNormal, FMath::Min(SpeedRatio, 1.0f)));

	// Compute Frame forward and right vector
	const FVector Forward = Space.Angular.GetAxisX();
	MovementContact.FrameRight = UTGOR_Math::Normalize(MovementContact.FrameNormal ^ Forward);
	MovementContact.FrameForward = MovementContact.FrameRight ^ MovementContact.FrameNormal;

	// Compute input
	MovementContact.FrameOrtho = UpVector | State.Input;
	const FVector FlatInput = State.Input - UpVector * MovementContact.FrameOrtho;
	const FVector RightInput = MovementContact.FrameNormal ^ FlatInput;
	MovementContact.FrameInput = RightInput ^ MovementContact.FrameNormal;

	// const FVector Normal = FMath::Lerp(UpVector, MovementContact.FrameNormal, SpeedRatio);

	// Compute velocity from relative velocity and relative angular velocity
	MovementContact.VerticalSpeed = (Linear | MovementContact.FrameNormal);
	MovementContact.FrameVelocity = Linear - MovementContact.VerticalSpeed * MovementContact.FrameNormal;

	// Compute angular velocity along Frame normal
	MovementContact.FrameAngular = Space.RelativeAngularVelocity.ProjectOnToNormal(MovementContact.FrameNormal);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

float UTGOR_GroundTask::GetFrictionForce(const FTGOR_MovementOutput& Out) const
{
	if (SlopeGrip >= SMALL_NUMBER)
	{
		float Slope = MovementContact.Slope;

		/*
		if (IsValid(MovementContact.Component))
		{
			const FWalkableSlopeOverride& Walkable = MovementContact.Component->GetWalkableSlopeOverride();
			Slope = Walkable.ModifyWalkableFloorZ(Slope);
			if (Slope < 0.0f || Slope > 1.0f)
			{
				return(0.0f);
			}
		}
		*/
		const float SlopeRatio = FMath::Max(0.0f, Slope - (1.0f - SlopeGrip)) * (1.0f / SlopeGrip);
		return FootFrictionCoefficient * SlopeRatio;
	}
	return 0.0f;
}

void UTGOR_GroundTask::GetRepelForce(const FTGOR_MovementTick& Tick, const FTGOR_MovementSpace& Space, FTGOR_MovementRepel& Repel, FTGOR_MovementOutput& Out) const
{
	const FTGOR_MovementBody& Body = RootComponent->GetBody();

	/*
	const float CapsuleRadius = RepelRadiusMultiplier * Body.Radius;
	const float CapsuleHalfHeight = RepelHalfHeightMultiplier * Body.Radius;

	FHitResult Hit;
	//if (Component->Trace(FVector::ZeroVector, FVector::ZeroVector, 0.0f, 100.0f, Hit))
	if (Identifier.Component->Overlap(Space.Linear, Space.Angular * FQuat(FVector::RightVector, PI / 2), CapsuleRadius, CapsuleHalfHeight, Hit))
	{
		// Compute spring force
		Repel.RepelNormal = Hit.Normal;
		const float RepelSpring = WallRepelStrength;
		const FVector SpringForce = Hit.Normal * Hit.PenetrationDepth * RepelSpring;
		Repel.RepelForce += SpringForce - MovementOrientation * (SpringForce | MovementOrientation);

		// Compute spring damping
		// TODO: Can oscillate
		Repel.RepelForce -= Space.RelativeLinearVelocity * FMath::Sqrt(RepelSpring * Body.Mass);

		// Project onto surface
		const float RepelStrength = Repel.RepelForce | Hit.Normal;
		Repel.RepelForce = Hit.Normal * FMath::Max(0.0f, RepelStrength);
		Repel.RepelHit = true;

		if (Hit.Actor.IsValid())
		{
			UActorComponent* ActorComponent = Hit.Actor->GetComponentByClass(UTGOR_MobilityComponent::StaticClass());
			if (IsValid(ActorComponent))
			{
				UTGOR_MobilityComponent* Mobility = Cast<UTGOR_MobilityComponent>(ActorComponent);
				Mobility->InflictPointForce(Hit.ImpactPoint, -Repel.RepelForce, INDEX_NONE);
			}
		}
	}
	*/
}

void UTGOR_GroundTask::GetStandingForce(const FTGOR_MovementTick& Tick, const FTGOR_MovementSpace& Space, const FTGOR_MovementExternal& External, const FVector& Normal, float Stretch, FTGOR_MovementOutput& Out) const
{
	const FTGOR_MovementFrame& Frame = Identifier.Component->GetFrame();
	const FTGOR_MovementBody& Body = RootComponent->GetBody();

	// Employ spring physics with critical damping (Qualitative with damping multiplier because of clamped downforce inaccuracies)
	const float Spring = LegSpringCoefficient * MaximumLegStrength * Frame.Strength;
	const float DampingCoeff = LegDampingMultiplayer * FMath::Sqrt(4.0f * Spring * Body.Mass);
	
	Out.AddDampingForce(Tick, Space.RelativeLinearVelocity.ProjectOnTo(Normal), DampingCoeff);

	// Make sure we don't over- or understretch any leg
	const float TargetRatio = MinGroundRatio + (MaxGroundRatio - MinGroundRatio) * Stretch;
	const float SpringForce = (TargetRatio - MovementContact.GroundRatio) * Spring;
	Out.Force += Normal * SpringForce;

	// Jumping, apply upwards force either when already going up or when staying still enough
	const float UpInput = FMath::Max(0.0f, MovementContact.FrameOrtho);
	const float Relative = (Normal | Space.RelativeLinearVelocity);
	if ((MovementContact.GroundRatio < MaxGroundRatio && Relative > 10.0f) || Relative > SMALL_NUMBER)
	{
		const float JumpForce = UpInput * MaximumLegStrength * Frame.Strength;
		Out.Force += External.UpVector * JumpForce * JumpCoefficient;
	}
}

void UTGOR_GroundTask::GetStandingTorque(const FTGOR_MovementTick& Tick, const FTGOR_MovementSpace& Space, const FTGOR_MovementExternal& External, const FVector& Normal, float SpeedRatio, FTGOR_MovementOutput& Out) const
{
	const FTGOR_MovementFrame& Frame = Identifier.Component->GetFrame();

	// Dampen to prevent oscillation, correct against external torque
	const FVector Correction = External.Torque;
	const FVector CurrentUp = Space.Angular.GetAxisZ();

	// Rotate to stand upright 
	const FVector Swivel = CurrentUp ^ Normal;
	const FVector SwivelTorque = Swivel * StandupTorque;

	// Lean when rotating and moving fast
	const FVector CurrentRight = Space.Angular.GetAxisY();
	const FVector Lean = Space.RelativeAngularVelocity ^ CurrentRight;
	const FVector LeanTorque = Lean * SpeedRatio * LeanCoefficient;

	Out.Torque += (SwivelTorque - Correction + LeanTorque) * Frame.Strength;
}

void UTGOR_GroundTask::LimitForces(const FTGOR_MovementSpace& Space, const FTGOR_MovementExternal& External, float SpeedRatio, FTGOR_MovementOutput& Out) const
{
	const FTGOR_MovementFrame& Frame = Identifier.Component->GetFrame();

	const FVector FrameNormal = MovementContact.FrameNormal;

	// Limit force to physical ability
	const float UpForce = FrameNormal | Out.Force;
	const FVector PlaneForce = Out.Force - UpForce * FrameNormal;
	if (LegDropOffMultiplier >= SMALL_NUMBER)
	{
		// Compute max possible force away from plane
		const float DropOff = FMath::Max((MovementContact.GroundRatio - 1.0f) / LegDropOffMultiplier, 0.0f);
		const float MaxLegForce = MaximumLegStrength * Frame.Strength * FMath::Max(1.0f - DropOff, 0.0f);

		const FVector FinalUpForce = FrameNormal * FMath::Clamp(UpForce, -FloatControl, MaxLegForce + FloatControl);
		const FVector FlatVelocity = Space.RelativeLinearVelocity - (Space.RelativeLinearVelocity | FrameNormal) * FrameNormal;
		//const FVector FlatVelocity = MovementContact.FrameVelocity - (MovementContact.FrameVelocity | FrameNormal) * FrameNormal;

		// Compute max applicable force in any direction
		const float FrictionForce = GetFrictionForce(Out);
		const float FrictionExternalForce = UpForce; // (External.Force | FrameNormal)
		const float MaxPlaneForce = AirControl * Frame.Strength + FrictionForce * FMath::Clamp(FrictionExternalForce, 0.0f, MaxLegForce);
		FVector DesiredPlaneForce = UTGOR_Math::ClampToSize(PlaneForce, MaxPlaneForce);

		// Split planeforce into forward movement and sideways direction
		const FTGOR_Direction Velocity = FTGOR_Direction(FlatVelocity);
		if (Velocity && SpeedRatio >= SMALL_NUMBER)
		{
			// Limit friction in velocity direction
			const float ForwardForce = FMath::Max(DesiredPlaneForce | Velocity.Direction, 0.0f);
			DesiredPlaneForce -= ForwardForce * SpeedRatio * Velocity.Direction;
		}

		// Apply planeforce
		Out.Force = DesiredPlaneForce + FinalUpForce;

		// Max torque, since (torque = offset x force)
		const float MaxTorque = MaxPlaneForce * MovementContact.SurfaceOffset.Size() / 100.0f * Frame.Strength;
		Out.Torque = UTGOR_Math::ClampToSize(Out.Torque, MaxTorque);
	}
	else
	{
		Out.Force = FVector::ZeroVector;
		Out.Torque = FVector::ZeroVector;
	}
}
