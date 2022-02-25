// The Gateway of Realities: Planes of Existence.


#include "TGOR_GroundMovement.h"

#include "Realities/Actors/Pawns/TGOR_Avatar.h"
#include "Realities/Mod/Animations/TGOR_Animation.h"
#include "Realities/Utility/Structs/TGOR_Direction.h"
#include "Realities/Components/Movement/TGOR_MobilityComponent.h"
#include "Realities/Components/Movement/TGOR_MovementComponent.h"
#include "Realities/Components/Creature/TGOR_ModularSkeletalMeshComponent.h"
#include "Realities/Mod/Movements/TGOR_RagdollMovement.h"

UTGOR_GroundMovement::UTGOR_GroundMovement()
	: Super()
{
	LegLengthMultiplier = 1.4f;

	LegDampingMultiplayer = 0.8f;
	LegSpringCoefficient = 0.004f;
	TraceLengthMultiplier = 1.5f;
	TraceRadiusMultiplier = 0.9f;
	TraceUpVector = false;

	StretchMultiplier = 0.2f;
	CrouchMultiplier = 0.4f;
	CrouchSpeedMultiplier = 0.3f;

	BrakeCoefficient = 1200.0f;
	FootFrictionCoefficient = 5.0f;
	AirControl = 75'000.0f;
	FloatControl = 0.0f;
	LeanCoefficient = 200.0f;

	StandupTorque = 2'000.0f;
	AngularDamping = 300.0f;
	SlopeGrip = 0.4f;

	LinearSpeedThreshold = 5'500.0f;
	AngularSpeedThreshold = 40.0f;


	MaximumLegStrength = 800'000.0f;
	LegDropOffMultiplier = 0.05f;

	SpeedThreshold = 64.0f;
	SlopeMultiplier = 0.2f;
	InputMultiplier = 1.0f;

	WallRepelStrength = 25'000.0f;
	RepelRadiusMultiplier = 1.5f;
	RepelHalfHeightMultiplier = 2.5f;

	InsertionClasses.Add(UTGOR_RagdollMovement::StaticClass());
}

void UTGOR_GroundMovement::QueryInputVector(const UTGOR_MovementComponent* Component, FVector& OutInput, FVector& OutView) const
{
	const FVector UpVector = Component->ComputePhysicsUpVector();
	const FQuat InputRotation = Component->GetInputRotation();
	const FVector InputDirection = InputRotation.GetAxisX();
	const float InputStrength = Component->GetInputStrength();

	OutView = InputDirection - UpVector * (InputDirection | UpVector);
	OutView.Normalize();

	const FVector RawInput = Component->GetRawInput();
	const float UpInput = RawInput.Z;

	// Compute normalised input
	const FVector2D Plane = FVector2D(RawInput);
	const float Size = Plane.Size();
	if (Size >= SMALL_NUMBER)
	{
		const FVector2D PlaneInput = Plane / Size;

		// Compute vector base with current capsule up-vector for yaw
		const FVector Right = UpVector ^ InputDirection;
		const FVector Forward = Right ^ UpVector;
		const float Input = FMath::Min(1.0f, InputStrength * Size * InputMultiplier);
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
			OutInput = Normal * Input + UpVector * UpInput;
			return;
		}
	}

	OutInput = UpVector * UpInput;
}

void UTGOR_GroundMovement::Tick(UTGOR_MovementComponent* Component, const FTGOR_MovementTick& Tick, const FTGOR_MovementSpace& Space, const FTGOR_MovementExternal& External, bool Replay, FTGOR_MovementOutput& Out)
{
	const FTGOR_MovementCapture& Capture = Component->GetCapture();
	const FTGOR_MovementContent& Content = Component->GetContent();
	const FTGOR_MovementFrame& Frame = Component->GetFrame();

	// Analyse ground
	FTGOR_MovementContact Contact;
	const float GroundRatio = TraceForGroundContact(Component, Space, External.UpVector, Contact);

	// Set base output
	Out.SetBaseFrom(Contact.Component, Contact.Name);
	Out.BaseImpactPoint = Space.Linear - Contact.FrameNormal * Contact.GroundDistance;
	
	// Add movement dependent external forces to total external forces
	FTGOR_MovementRepel Repel;
	//GetRepelForce(Component, Tick, Space, Repel, Out);

	// Compute input force
	const float RawRatio = GetInputForce(Component, Tick, Space, External, Contact, Repel, GroundRatio, Out);
	const float SpeedRatio = FMath::Clamp(RawRatio, 0.0f, 1.0f);

	// Compute force to make character stand, only take slope into consideration when moving
	const float Stretch = GetStretch(Component, Tick, Space, External, Contact, GroundRatio);
	const FVector Normal = FMath::Lerp(External.UpVector, Contact.FrameNormal, SpeedRatio);
	GetStandingForce(Component, Tick, Space, External, Normal, Contact, Stretch, Out);
	GetStandingTorque(Component, Tick, Space, External, Normal, SpeedRatio, Out);
	// Compute final desired force, try to counteract external forces
	Out.Force -= Repel.RepelForce + External.Force;

	LimitForces(Component, Space, External, Contact, SpeedRatio, Out);

	// Apply external forces
	Out.Force += Repel.RepelForce;

	// Call on animation
	if (IsValid(Content.Animation) && CrouchMultiplier > SMALL_NUMBER)
	{
		GroundAnimationTick(Component, Space, External, GroundRatio, Contact);
	}

	if (Out.Torque.ContainsNaN())
	{
		Out.Force = FVector::ZeroVector;
		Out.Torque = FVector::ZeroVector;
		ERROR("Torque singularity", Warning);
	}
	
	if (Out.Force.ContainsNaN())
	{
		Out.Force = FVector::ZeroVector;
		Out.Torque = FVector::ZeroVector;
		ERROR("Force singularity", Warning);
	}
}

bool UTGOR_GroundMovement::Invariant(const UTGOR_MovementComponent* Component, const FTGOR_MovementTick& Tick, const FTGOR_MovementSpace& Space, const FTGOR_MovementExternal& External) const
{
	if (!Super::Invariant(Component, Tick, Space, External))
	{
		return false;
	}
		
	// Check for speed
	return Space.RelativeLinearVelocity.Size() < LinearSpeedThreshold && Space.RelativeAngularVelocity.Size() < AngularSpeedThreshold;
}

float UTGOR_GroundMovement::GetLegLength(const UTGOR_MovementComponent* Component) const
{
	const FTGOR_MovementBody& Body = Component->GetBody();

	// Grab parameters
	return Body.Height / 2 * LegLengthMultiplier;
}

float UTGOR_GroundMovement::ComputeCrouchSpeedRatio(float GroundRatio) const
{
	// Max velocity depends on current leg length (solved for f(x) = mx + c for f(1)=1 and f(1.0f - Crouch) = SpeedRatio)
	const float CrouchRatio = (1.0f - CrouchSpeedMultiplier) / CrouchMultiplier;
	return FMath::Min((GroundRatio - 1.0f) * CrouchRatio + 1.0f, 1.0f);
}

float UTGOR_GroundMovement::ComputeCrouchRatio(float GroundRatio) const
{
	return FMath::Clamp((GroundRatio - 1.0f + CrouchMultiplier) / CrouchMultiplier, 0.0f, 1.0f);
}

float UTGOR_GroundMovement::ComputeHeightOffset(float GroundRatio, float LegLength) const
{
	return FMath::Max(1.0f - GroundRatio, 0.0f) * LegLength;
}

float UTGOR_GroundMovement::TraceForGroundContact(UTGOR_MovementComponent* Component, const FTGOR_MovementSpace& Space, const FVector& UpVector, FTGOR_MovementContact& Contact) const
{
	const FTGOR_MovementState& State = Component->GetState();

	const float Ratio = TraceForGroundRatio(Component, Space, UpVector, Contact);
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
	const float UpInput = UpVector | State.Input;
	const FVector FlatInput = State.Input - UpVector * UpInput;
	const FVector RightInput = Contact.FrameNormal ^ FlatInput;
	Contact.FrameInput = RightInput ^ Contact.FrameNormal;

	// const FVector Normal = FMath::Lerp(UpVector, Contact.FrameNormal, SpeedRatio);

	// Compute velocity from relative velocity and relative angular velocity
	Contact.VerticalSpeed = (Linear | Contact.FrameNormal);
	Contact.FrameVelocity = Linear - Contact.VerticalSpeed * Contact.FrameNormal;

	// Compute angular velocity along Frame normal
	Contact.FrameAngular = Space.RelativeAngularVelocity.ProjectOnToNormal(Contact.FrameNormal);

	return Ratio;
}

float UTGOR_GroundMovement::TraceForGroundRatio(const UTGOR_MovementComponent* Component, const FTGOR_MovementSpace& Space, const FVector& UpVector, FTGOR_MovementGround& Ground) const
{
	const FTGOR_MovementBody& Body = Component->GetBody();

	FHitResult Hit;
	const float LegLength = GetLegLength(Component);
	const float Distance = LegLength * TraceLengthMultiplier;
	const float Radius = Body.Radius * TraceRadiusMultiplier;
	
	const FVector Forward = Space.Angular.GetAxisX();
	const FVector Up = (TraceUpVector ? UpVector : Space.Angular.GetAxisZ());
	if (Component->SampleTrace(Space.Linear, -Up, Forward, Distance, Radius, Hit))
	{
		Ground.SurfaceOffset = Hit.TraceStart - Hit.ImpactPoint;

		// Compute actual normal from both averaged and sampled
		Ground.SurfaceNormal = UTGOR_Math::Normalize(Hit.Normal + Hit.ImpactNormal * SlopeMultiplier);

		// Compute ground distance
		const FVector Offset = Hit.TraceStart - Hit.ImpactPoint;
		Ground.GroundDistance = Offset | Up;

		// Compute slope
		Ground.Slope = Ground.SurfaceNormal | Up;

		// Grab component if actually on ground
		const float Ratio = Ground.GroundDistance / LegLength;
		if (Ratio < 1.0f + StretchMultiplier)
		{
			Ground.Component = Hit.GetComponent();
			Ground.Name = Hit.BoneName;
		}
		return Ratio;
	}

	// Default to plane
	Ground.Slope = 0.0f;
	Ground.SurfaceNormal = Up;
	Ground.GroundDistance = Distance;
	Ground.Component = nullptr;
	Ground.Name = "";
	return TraceLengthMultiplier;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

float UTGOR_GroundMovement::GetStretch(UTGOR_MovementComponent* Component, const FTGOR_MovementTick& Tick, const FTGOR_MovementSpace& Space, const FTGOR_MovementExternal& External, const FTGOR_MovementContact& Contact, float GroundRatio) const
{
	return 0.0f;
}

float UTGOR_GroundMovement::GetInputForce(UTGOR_MovementComponent* Component, const FTGOR_MovementTick& Tick, const FTGOR_MovementSpace& Space, const FTGOR_MovementExternal& External, const FTGOR_MovementContact& Contact, const FTGOR_MovementRepel& Repel, float GroundRatio, FTGOR_MovementOutput& Out) const
{
	const FTGOR_MovementCapture& Capture = Component->GetCapture();
	const FTGOR_MovementFrame& Frame = Component->GetFrame();

	// Filter sideways input
	Component->GetLinearDamping(Tick, Contact.FrameVelocity, BrakeCoefficient * Frame.Strength, Out);
	Component->GetAngularDamping(Tick, Space.RelativeAngularVelocity, AngularDamping * Frame.Strength, Out);
	return 0.0f;
}


float UTGOR_GroundMovement::GetFrictionForce(const FTGOR_MovementOutput& Out, const FTGOR_MovementContact& Contact) const
{
	if (SlopeGrip >= SMALL_NUMBER)
	{
		float Slope = Contact.Slope;

		if (IsValid(Contact.Component))
		{
			const FWalkableSlopeOverride& Walkable = Contact.Component->GetWalkableSlopeOverride();
			Slope = Walkable.ModifyWalkableFloorZ(Slope);
			if (Slope < 0.0f || Slope > 1.0f)
			{
				return(0.0f);
			}
		}
		const float SlopeRatio = FMath::Max(0.0f, Slope - (1.0f - SlopeGrip)) * (1.0f / SlopeGrip);
		return(FootFrictionCoefficient * SlopeRatio);
	}
	return 0.0f;
}

void UTGOR_GroundMovement::GetRepelForce(UTGOR_MovementComponent* Component, const FTGOR_MovementTick& Tick, const FTGOR_MovementSpace& Space, FTGOR_MovementRepel& Repel, FTGOR_MovementOutput& Out) const
{
	const FTGOR_MovementBody& Body = Component->GetBody();
	FHitResult Hit;

	const FVector UpVector = Space.Angular.GetAxisZ();
	const float CapsuleRadius = RepelRadiusMultiplier * Body.Radius;
	const float CapsuleHalfHeight = RepelHalfHeightMultiplier * Body.Radius;

	//if (Component->Trace(FVector::ZeroVector, FVector::ZeroVector, 0.0f, 100.0f, Hit))
	if (Component->Overlap(Space.Linear, Space.Angular * FQuat(FVector::RightVector, PI / 2), CapsuleRadius, CapsuleHalfHeight, Hit))
	{
		// Compute spring force
		Repel.RepelNormal = Hit.Normal;
		const float RepelSpring = WallRepelStrength;
		const FVector SpringForce = Hit.Normal * Hit.PenetrationDepth * RepelSpring;
		Repel.RepelForce += SpringForce - UpVector * (SpringForce | UpVector);

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
}

void UTGOR_GroundMovement::GetStandingForce(UTGOR_MovementComponent* Component, const FTGOR_MovementTick& Tick, const FTGOR_MovementSpace& Space, const FTGOR_MovementExternal& External, const FVector& Normal, const FTGOR_MovementContact& Contact, float Stretch, FTGOR_MovementOutput& Out) const
{
	const FTGOR_MovementFrame& Frame = Component->GetFrame();
	const FTGOR_MovementBody& Body = Component->GetBody();

	// Compute desired ground clearance
	const float LegLength = GetLegLength(Component);
	const float DesiredHeight = LegLength * (1.0f + FMath::Min(Stretch, StretchMultiplier) * CrouchMultiplier);
	const float TurnOffset = (Body.Height / 2 - Body.Radius) * (1.0f - FMath::Abs(Space.Angular.GetAxisZ() | Contact.FrameNormal));
	const float Clearance = DesiredHeight - Contact.GroundDistance -TurnOffset;	

	// Employ spring physics with critical damping (Qualitative with damping multiplier because of clamped downforce inaccuracies)
	const float Spring = LegSpringCoefficient * MaximumLegStrength * Frame.Strength;
	const float DampingCoeff = LegDampingMultiplayer * FMath::Sqrt(4.0f * Spring * Body.Mass);

	Component->GetLinearDamping(Tick, Space.RelativeLinearVelocity.ProjectOnTo(Normal), DampingCoeff, Out);
	Out.Force += Normal * (Clearance * Spring);
}

void UTGOR_GroundMovement::GetStandingTorque(UTGOR_MovementComponent* Component, const FTGOR_MovementTick& Tick, const FTGOR_MovementSpace& Space, const FTGOR_MovementExternal& External, const FVector& Normal, float SpeedRatio, FTGOR_MovementOutput& Out) const
{
	const FTGOR_MovementCapture& Capture = Component->GetCapture();
	const FTGOR_MovementFrame& Frame = Component->GetFrame();

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


void UTGOR_GroundMovement::LimitForces(UTGOR_MovementComponent* Component, const FTGOR_MovementSpace& Space, const FTGOR_MovementExternal& External, const FTGOR_MovementContact& Contact, float SpeedRatio, FTGOR_MovementOutput& Out) const
{
	const FTGOR_MovementFrame& Frame = Component->GetFrame();
	const FTGOR_MovementCapture& Capture = Component->GetCapture();

	const FVector FrameNormal = Contact.FrameNormal;

	// Limit force to physical ability
	const float UpForce = FrameNormal | Out.Force;
	const FVector PlaneForce = Out.Force - UpForce * FrameNormal;

	const float LegLength = GetLegLength(Component);
	if (LegDropOffMultiplier >= SMALL_NUMBER && LegLength >= SMALL_NUMBER)
	{
		// Compute max possible force away from plane
		const float DropOff = FMath::Max(((Contact.GroundDistance - LegLength) / LegLength) / LegDropOffMultiplier, 0.0f);
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
		const float MaxTorque = MaxPlaneForce * LegLength / 100.0f * Frame.Strength;
		Out.Torque = UTGOR_Math::ClampToSize(Out.Torque, MaxTorque);
	}
	else
	{
		Out.Force = FVector::ZeroVector;
		Out.Torque = FVector::ZeroVector;
	}
}