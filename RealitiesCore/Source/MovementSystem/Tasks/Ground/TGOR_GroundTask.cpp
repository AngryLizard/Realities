// The Gateway of Realities: Planes of Existence.


#include "TGOR_GroundTask.h"
#include "AnimationSystem/Components/TGOR_HandleComponent.h"
#include "AnimationSystem/Components/TGOR_AttachComponent.h"
#include "DimensionSystem/Components/TGOR_PilotComponent.h"
#include "MovementSystem/Components/TGOR_MovementComponent.h"

#include "RealitiesUtility/Utility/TGOR_Math.h"
#include "RealitiesUtility/Structures/TGOR_Direction.h"

#include "Net/UnrealNetwork.h"
#include "Engine/ActorChannel.h"

UTGOR_GroundTask::UTGOR_GroundTask()
	: Super(),
	LocalUpVector(FVector::UpVector)
{
}

#pragma optimize( "", off )

////////////////////////////////////////////////////////////////////////////////////////////////////

void UTGOR_GroundTask::Initialise()
{
	Super::Initialise();

	GetHandleComponents(Legs, LegTypes, true);
	GetHandleComponents(Arms, ArmTypes, true);

	LocalUpVector = FVector::ZeroVector;
	for (UTGOR_HandleComponent* Leg : Legs)
	{
		LocalUpVector -= Leg->MovementCone->GetAlignmentDirection();
	}
	LocalUpVector.Normalize();
}

bool UTGOR_GroundTask::Invariant(const FTGOR_MovementSpace& Space, const FTGOR_MovementExternal& External) const
{
	if (!Super::Invariant(Space, External))
	{
		return false;
	}

	if (Legs.Num() == 0)
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
			return;
		}
	}

}

void UTGOR_GroundTask::Update(FTGOR_MovementSpace& Space, const FTGOR_MovementExternal& External, const FTGOR_MovementTick& Tick, FTGOR_MovementOutput& Output)
{
	const FTGOR_MovementCapture& Capture = Identifier.Component->GetCapture();
	const FTGOR_MovementFrame& Frame = Identifier.Component->GetFrame();

	// Analyze ground
	const FVector Orientation = Space.Angular * LocalUpVector;

	FTGOR_MovementContact Contact;
	UpdateGroundHandles(Space, Orientation, External.UpVector, Contact);
	TraceForGroundContact(Space, Orientation, External.UpVector, Contact);

	// Add movement dependent external forces to total external forces
	FTGOR_MovementRepel Repel;
	//GetRepelForce(Component, Tick, Space, Repel, Out);

	// Compute input force
	const float RawRatio = GetInputForce(Tick, Space, Orientation, External, Contact, Repel, Output);

	const float SpeedRatio = FMath::Clamp(RawRatio, 0.0f, 1.0f);

	// Compute force to make character stand, only take slope into consideration when moving
	const float Stretch = GetStretch(Tick, Space, Orientation, External, Contact);
	//const FVector Normal = FMath::Lerp(External.UpVector, Contact.FrameNormal, SpeedRatio);
	GetStandingForce(Tick, Space, External, Contact.FrameNormal, Contact, Stretch, Output);
	GetStandingTorque(Tick, Space, External, Contact.FrameNormal, SpeedRatio, Output);

	// Compute final desired force, try to counteract external forces
	Output.Force -= Repel.RepelForce + External.Force;
	LimitForces(Space, External, Contact, SpeedRatio, Output);

	// Apply external forces
	Output.Force += Repel.RepelForce;

	if (Output.Torque.ContainsNaN())
	{
		Output.Force = FVector::ZeroVector;
		Output.Torque = FVector::ZeroVector;
		ERROR("Torque singularity", Warning);
	}

	if (Output.Force.ContainsNaN())
	{
		Output.Force = FVector::ZeroVector;
		Output.Torque = FVector::ZeroVector;
		ERROR("Force singularity", Warning);
	}

	Super::Update(Space, External, Tick, Output);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void UTGOR_GroundTask::UpdateGroundHandles(const FTGOR_MovementSpace& Space, const FVector& Orientation, const FVector& UpVector, FTGOR_MovementGround& Ground)
{
	const FTGOR_MovementBody& Body = RootComponent->GetBody();

	float Weight = 0.0f;
	float GroundRatio = 0.0f;
	FVector Normal = FVector::ZeroVector;
	FVector Delta = FVector::ZeroVector;
	for (UTGOR_HandleComponent* Leg : Legs)
	{
		FTGOR_HandleTraceOutput HandleTrace;
		if (Leg->TraceHandle(RootComponent, Space, TraceLengthMultiplier, HandleTrace))
		{
			GroundRatio += -(HandleTrace.Delta | Orientation) / Leg->MovementCone->LimitRadius;
			Normal += HandleTrace.Angular.GetAxisZ();
			Delta += HandleTrace.Delta;
			Weight += 1.0f;
		}
	}

	const FVector Forward = Space.Angular.GetAxisX();
	const FVector Up = (TraceUpVector ? UpVector : Orientation);
	if (Weight < SMALL_NUMBER)
	{
		// Default to plane
		Ground.Slope = 0.0f;
		Ground.SurfaceNormal = Up;
		Ground.GroundRatio = TraceLengthMultiplier;
		Ground.SurfaceOffset = FVector::ZeroVector;
	}
	else
	{
		Ground.Slope = Ground.SurfaceNormal | Up;
		Ground.SurfaceNormal = UTGOR_Math::Normalize(Normal);
		Ground.GroundRatio = GroundRatio / Weight;
		Ground.SurfaceOffset = Delta / Weight;
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////

float UTGOR_GroundTask::GetStretch(const FTGOR_MovementTick& Tick, const FTGOR_MovementSpace& Space, const FVector& Orientation, const FTGOR_MovementExternal& External, const FTGOR_MovementContact& Contact) const
{
	return 1.0f;
}

float UTGOR_GroundTask::GetInputForce(const FTGOR_MovementTick& Tick, const FTGOR_MovementSpace& Space, const FVector& Orientation, const FTGOR_MovementExternal& External, const FTGOR_MovementContact& Contact, const FTGOR_MovementRepel& Repel, FTGOR_MovementOutput& Out) const
{
	const FTGOR_MovementCapture& Capture = Identifier.Component->GetCapture();
	const FTGOR_MovementFrame& Frame = Identifier.Component->GetFrame();

	// Filter sideways input
	Identifier.Component->GetDampingForce(Tick, Contact.FrameVelocity, BrakeCoefficient * Frame.Strength, Out);
	Identifier.Component->GetDampingTorque(Tick, Space.RelativeAngularVelocity, AngularDamping * Frame.Strength, Out);
	return 0.0f;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void UTGOR_GroundTask::TraceForGroundContact(const FTGOR_MovementSpace& Space, const FVector& Orientation, const FVector& UpVector, FTGOR_MovementContact& Contact) const
{
	const FTGOR_MovementInput& State = Identifier.Component->GetState();

	const FVector Angular = Contact.SurfaceOffset ^ Space.RelativeAngularVelocity;
	const FVector Linear = Space.RelativeLinearVelocity + Angular;

	// Compute actual normal from both averaged and sampled
	const FVector FlatVelocity = Linear - UpVector * (UpVector | Linear);
	const float SpeedRatio = FlatVelocity.SizeSquared() / (SpeedThreshold * SpeedThreshold);
	Contact.FrameNormal = UTGOR_Math::Normalize(FMath::Lerp(UpVector, Contact.SurfaceNormal, FMath::Min(SpeedRatio, 1.0f)));

	// Compute Frame forward and right vector
	const FVector Forward = Space.Angular.GetAxisX();
	Contact.FrameRight = UTGOR_Math::Normalize(Contact.FrameNormal ^ Forward);
	Contact.FrameForward = Contact.FrameRight ^ Contact.FrameNormal;

	// Compute input
	Contact.FrameOrtho = UpVector | State.Input;
	const FVector FlatInput = State.Input - UpVector * Contact.FrameOrtho;
	const FVector RightInput = Contact.FrameNormal ^ FlatInput;
	Contact.FrameInput = RightInput ^ Contact.FrameNormal;

	// const FVector Normal = FMath::Lerp(UpVector, Contact.FrameNormal, SpeedRatio);

	// Compute velocity from relative velocity and relative angular velocity
	Contact.VerticalSpeed = (Linear | Contact.FrameNormal);
	Contact.FrameVelocity = Linear - Contact.VerticalSpeed * Contact.FrameNormal;

	// Compute angular velocity along Frame normal
	Contact.FrameAngular = Space.RelativeAngularVelocity.ProjectOnToNormal(Contact.FrameNormal);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

float UTGOR_GroundTask::GetFrictionForce(const FTGOR_MovementOutput& Out, const FTGOR_MovementContact& Contact) const
{
	if (SlopeGrip >= SMALL_NUMBER)
	{
		float Slope = Contact.Slope;

		/*
		if (IsValid(Contact.Component))
		{
			const FWalkableSlopeOverride& Walkable = Contact.Component->GetWalkableSlopeOverride();
			Slope = Walkable.ModifyWalkableFloorZ(Slope);
			if (Slope < 0.0f || Slope > 1.0f)
			{
				return(0.0f);
			}
		}
		*/
		const float SlopeRatio = FMath::Max(0.0f, Slope - (1.0f - SlopeGrip)) * (1.0f / SlopeGrip);
		return(FootFrictionCoefficient * SlopeRatio);
	}
	return 0.0f;
}

void UTGOR_GroundTask::GetRepelForce(const FTGOR_MovementTick& Tick, const FTGOR_MovementSpace& Space, const FVector& Orientation, FTGOR_MovementRepel& Repel, FTGOR_MovementOutput& Out) const
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
		Repel.RepelForce += SpringForce - Orientation * (SpringForce | Orientation);

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

void UTGOR_GroundTask::GetStandingForce(const FTGOR_MovementTick& Tick, const FTGOR_MovementSpace& Space, const FTGOR_MovementExternal& External, const FVector& Normal, const FTGOR_MovementContact& Contact, float Stretch, FTGOR_MovementOutput& Out) const
{
	const FTGOR_MovementFrame& Frame = Identifier.Component->GetFrame();
	const FTGOR_MovementBody& Body = RootComponent->GetBody();

	// Employ spring physics with critical damping (Qualitative with damping multiplier because of clamped downforce inaccuracies)
	const float Spring = LegSpringCoefficient * MaximumLegStrength * Frame.Strength;
	const float DampingCoeff = LegDampingMultiplayer * FMath::Sqrt(4.0f * Spring * Body.Mass);
	
	Identifier.Component->GetDampingForce(Tick, Space.RelativeLinearVelocity.ProjectOnTo(Normal), DampingCoeff, Out);
	Out.Force += Normal * ((Stretch - Contact.GroundRatio) * Spring);

	// Jumping, apply upwards force either when already going up or when staying still enough
	const float UpInput = FMath::Max(0.0f, Contact.FrameOrtho);
	const float Relative = (Normal | Space.RelativeLinearVelocity);
	if ((Contact.GroundRatio < 1.0f && Relative > 10.0f) || Relative > SMALL_NUMBER)
	{
		Out.Force += External.UpVector * UpInput * MaximumLegStrength * Frame.Strength;
	}
}

void UTGOR_GroundTask::GetStandingTorque(const FTGOR_MovementTick& Tick, const FTGOR_MovementSpace& Space, const FTGOR_MovementExternal& External, const FVector& Normal, float SpeedRatio, FTGOR_MovementOutput& Out) const
{
	const FTGOR_MovementCapture& Capture = Identifier.Component->GetCapture();
	const FTGOR_MovementFrame& Frame = Identifier.Component->GetFrame();

	// Dampen to prevent oscillation, correct against external torque
	const FVector Correction = External.Torque;
	const FVector CurrentUp = Space.Angular.GetAxisZ();

	// Rotate to stand upright 
	const FVector Swivel = CurrentUp ^ External.UpVector;
	const FVector SwivelTorque = Swivel * StandupTorque;

	// Lean when rotating and moving fast
	const FVector CurrentRight = Space.Angular.GetAxisY();
	const FVector Lean = Space.RelativeAngularVelocity ^ CurrentRight;
	const FVector LeanTorque = Lean * SpeedRatio * LeanCoefficient;

	Out.Torque += (SwivelTorque - Correction + LeanTorque) * Frame.Strength;
}


void UTGOR_GroundTask::LimitForces(const FTGOR_MovementSpace& Space, const FTGOR_MovementExternal& External, const FTGOR_MovementContact& Contact, float SpeedRatio, FTGOR_MovementOutput& Out) const
{
	const FTGOR_MovementCapture& Capture = Identifier.Component->GetCapture();
	const FTGOR_MovementFrame& Frame = Identifier.Component->GetFrame();

	const FVector FrameNormal = Contact.FrameNormal;

	// Limit force to physical ability
	const float UpForce = FrameNormal | Out.Force;
	const FVector PlaneForce = Out.Force - UpForce * FrameNormal;
	if (LegDropOffMultiplier >= SMALL_NUMBER)
	{
		// Compute max possible force away from plane
		const float DropOff = FMath::Max((Contact.GroundRatio - 1.0f) / LegDropOffMultiplier, 0.0f);
		const float MaxLegForce = MaximumLegStrength * Frame.Strength * FMath::Max(1.0f - DropOff, 0.0f);

		const FVector FinalUpForce = FrameNormal * FMath::Clamp(UpForce, -FloatControl, MaxLegForce + FloatControl);
		const FVector FlatVelocity = Space.RelativeLinearVelocity - (Space.RelativeLinearVelocity | FrameNormal) * FrameNormal;

		// Compute max applicable force in any direction
		const float FrictionForce = GetFrictionForce(Out, Contact);
		const float MaxPlaneForce = AirControl * Frame.Strength + FrictionForce * FMath::Clamp(UpForce, 0.0f, MaxLegForce);
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
		const float MaxTorque = MaxPlaneForce * Contact.SurfaceOffset.Size() / 100.0f * Frame.Strength;
		Out.Torque = UTGOR_Math::ClampToSize(Out.Torque, MaxTorque);
	}
	else
	{
		Out.Force = FVector::ZeroVector;
		Out.Torque = FVector::ZeroVector;
	}
}
#pragma optimize( "", on ) 