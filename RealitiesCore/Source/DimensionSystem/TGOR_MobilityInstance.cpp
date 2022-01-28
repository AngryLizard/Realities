// The Gateway of Realities: Planes of Existence.

#include "TGOR_MobilityInstance.h"

#include "RealitiesUtility/Utility/TGOR_Error.h"

#include "CoreSystem/Storage/TGOR_Serialisation.h"
#include "CoreSystem/Storage/TGOR_Package.h"

SERIALISE_INIT_IMPLEMENT(FTGOR_MovementShape)
SERIALISE_INIT_IMPLEMENT(FTGOR_MovementBody)

////////////////////////////////////////////////////////////////////////////////////////////////////

FTGOR_MovementThreshold::FTGOR_MovementThreshold()
:	SpeedMultiplier(1.0f),
	DistanceThreshold(100.0f),
	AngleThreshold(0.1f),
	TimeThreshold(0.5f)
{
}

FTGOR_MovementThreshold::FTGOR_MovementThreshold(const FTGOR_MovementThreshold& Other, float Factor)
{
	SpeedMultiplier = Other.SpeedMultiplier;
	DistanceThreshold = Other.DistanceThreshold * Factor;
	AngleThreshold = Other.AngleThreshold * Factor;
	TimeThreshold = Other.TimeThreshold * Factor;
}

///////////////////////////////////////////

float FTGOR_MovementThreshold::ThresholdDistance(const FVector& A, const FVector& B, float SpeedSquared) const
{
	const float Threshold = DistanceThreshold + SpeedSquared * SpeedMultiplier;
	return FMath::Min((A - B).SizeSquared() / (Threshold * Threshold), 1.0f);
}

float FTGOR_MovementThreshold::ThresholdDistance(float A, float B, float SpeedSquared) const
{
	const float Delta = A - B;
	const float Threshold = DistanceThreshold + SpeedSquared * SpeedMultiplier;
	return FMath::Min((Delta * Delta) / (Threshold * Threshold), 1.0f);
}

float FTGOR_MovementThreshold::ThresholdAngle(const FVector& A, const FVector& B, float SpeedSquared) const
{
	const float Threshold = AngleThreshold * (1.0f + (SpeedSquared / (AngleThreshold * AngleThreshold))) * SpeedMultiplier;
	return FMath::Min((1.0f - (A | B)) / Threshold, 1.0f);
}

float FTGOR_MovementThreshold::ThresholdAngle(const FQuat& A, const FQuat& B, float SpeedSquared) const
{
	const float Threshold = AngleThreshold * (1.0f + (SpeedSquared / (AngleThreshold * AngleThreshold))) * SpeedMultiplier;
	return FMath::Min((1.0f - (A | B)) / Threshold, 1.0f);
}

float FTGOR_MovementThreshold::ThresholdTime(const FTGOR_Time& A, const FTGOR_Time& B) const
{
	const float Delta = A - B;
	return FMath::Min((Delta * Delta) / (TimeThreshold * TimeThreshold), 1.0f);
}


////////////////////////////////////////////////////////////////////////////////////////////////////

FTGOR_MovementForce::FTGOR_MovementForce()
	: Force(FVector::ZeroVector),
	Torque(FVector::ZeroVector)
{
}

void FTGOR_MovementForce::SetLerpForce(const FTGOR_MovementForce& Other, float Alpha)
{
	Force = FMath::Lerp(Force, Other.Force, Alpha);
	Torque = FMath::Lerp(Torque, Other.Torque, Alpha);
}

float FTGOR_MovementForce::CompareForce(const FTGOR_MovementForce& Other, const FTGOR_MovementThreshold& Threshold) const
{
	const float ForceDistance = Threshold.ThresholdDistance(Force, Other.Force, 0.0f);
	const float TorqueDistance = Threshold.ThresholdDistance(Torque, Other.Torque, 0.0f);
	return ForceDistance + TorqueDistance;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

FTGOR_MovementExternal::FTGOR_MovementExternal()
	: UpVector(FVector::ZeroVector)
{
}

////////////////////////////////////////////////////////////////////////////////////////////////////

FTGOR_MovementPosition::FTGOR_MovementPosition()
	: Linear(FVector::ZeroVector),
	Angular(FQuat::Identity)
{
}

FTGOR_MovementPosition::FTGOR_MovementPosition(const FVector& Linear, const FQuat& Angular)
	: Linear(Linear),
	Angular(Angular)
{
}

void FTGOR_MovementPosition::SetLerpPosition(const FTGOR_MovementPosition& Other, float Alpha)
{
	Linear = FMath::Lerp(Linear, Other.Linear, Alpha);
	Angular = FMath::Lerp(Angular, Other.Angular, Alpha);
}

float FTGOR_MovementPosition::ComparePosition(const FTGOR_MovementPosition& Other, const FTGOR_MovementThreshold& Threshold) const
{
	const float LinearDistance = Threshold.ThresholdDistance(Linear, Other.Linear, 0.0f);
	const float AngularDistance = Threshold.ThresholdAngle(Angular, Other.Angular, 0.0f);
	return LinearDistance + AngularDistance;
}

FTGOR_MovementPosition FTGOR_MovementPosition::BaseToPosition(const FTGOR_MovementPosition& ParentPosition) const
{
	FTGOR_MovementPosition Position;
	Position.Angular = ParentPosition.Angular * Angular;
	Position.Linear = ParentPosition.Linear + ParentPosition.Angular * Linear;
	return Position;
}

FTGOR_MovementSpace FTGOR_MovementPosition::BaseToSpace(const FTGOR_MovementDynamic& ParentDynamic) const
{
	return FTGOR_MovementDynamic(*this).BaseToSpace(ParentDynamic);
}

void FTGOR_MovementPosition::PositionToBase(const FTGOR_MovementPosition& ParentPosition, const FTGOR_MovementPosition& Position)
{
	const FQuat InvAngular = ParentPosition.Angular.Inverse();
	Angular = InvAngular * Position.Angular;
	Linear = InvAngular * (Position.Linear - ParentPosition.Linear);
}

FVector FTGOR_MovementPosition::TransformPosition(const FVector& Local) const
{
	return Linear + TransformVector(Local);
}

FVector FTGOR_MovementPosition::InverseTransformPosition(const FVector& World) const
{
	return InverseTransformVector(World - Linear);
}

FVector FTGOR_MovementPosition::TransformVector(const FVector& Local) const
{
	return Angular * Local;
}

FVector FTGOR_MovementPosition::InverseTransformVector(const FVector& World) const
{
	return Angular.Inverse() * World;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

FTGOR_MovementDynamic::FTGOR_MovementDynamic()
	: FTGOR_MovementPosition(),
	LinearVelocity(FVector::ZeroVector),
	AngularVelocity(FVector::ZeroVector),
	LinearAcceleration(FVector::ZeroVector),
	AngularAcceleration(FVector::ZeroVector)
{
}

FTGOR_MovementDynamic::FTGOR_MovementDynamic(const FTGOR_MovementPosition& Other)
	: FTGOR_MovementPosition(Other),
	LinearVelocity(FVector::ZeroVector),
	AngularVelocity(FVector::ZeroVector),
	LinearAcceleration(FVector::ZeroVector),
	AngularAcceleration(FVector::ZeroVector)
{
}

void FTGOR_MovementDynamic::Write(FTGOR_GroupWritePackage& Package, UTGOR_Singleton* Context) const
{
	Package.WriteEntry("Linear", Linear);
	Package.WriteEntry("LinearVelocity", LinearVelocity);
	Package.WriteEntry("Angular", Angular);
	Package.WriteEntry("AngularVelocity", AngularVelocity);
}

void FTGOR_MovementDynamic::Read(FTGOR_GroupReadPackage& Package, UTGOR_Singleton* Context)
{
	Package.ReadEntry("Linear", Linear);
	Package.ReadEntry("LinearVelocity", LinearVelocity);
	Package.ReadEntry("Angular", Angular);
	Package.ReadEntry("AngularVelocity", AngularVelocity);
}

void FTGOR_MovementDynamic::Send(FTGOR_NetworkWritePackage& Package, UTGOR_Singleton* Context) const
{
	Package.WriteEntry(Linear);
	Package.WriteEntry(LinearVelocity);
	Package.WriteEntry(Angular);
	Package.WriteEntry(AngularVelocity);
}

void FTGOR_MovementDynamic::Recv(FTGOR_NetworkReadPackage& Package, UTGOR_Singleton* Context)
{
	Package.ReadEntry(Linear);
	Package.ReadEntry(LinearVelocity);
	Package.ReadEntry(Angular);
	Package.ReadEntry(AngularVelocity);
}


void FTGOR_MovementDynamic::SetLerpDynamic(const FTGOR_MovementDynamic& Other, float Alpha)
{
	SetLerpPosition(Other, Alpha);

	LinearVelocity = FMath::Lerp(LinearVelocity, Other.LinearVelocity, Alpha);
	AngularVelocity = FMath::Lerp(AngularVelocity, Other.AngularVelocity, Alpha);
	LinearAcceleration = FMath::Lerp(LinearAcceleration, Other.LinearAcceleration, Alpha);
	AngularAcceleration = FMath::Lerp(AngularAcceleration, Other.AngularAcceleration, Alpha);
}

float FTGOR_MovementDynamic::CompareDynamic(const FTGOR_MovementDynamic& Other, const FTGOR_MovementThreshold& Threshold) const
{
	const float LinearDistance = Threshold.ThresholdDistance(Linear, Other.Linear, LinearVelocity.SizeSquared());
	const float AngularDistance = Threshold.ThresholdAngle(Angular, Other.Angular, AngularVelocity.SizeSquared());
	const float LinearVelocityDistance = Threshold.ThresholdDistance(LinearVelocity, Other.LinearVelocity, LinearAcceleration.SizeSquared());
	const float AngularVelocityDistance = Threshold.ThresholdDistance(AngularVelocity, Other.AngularVelocity, AngularAcceleration.SizeSquared());
	return LinearDistance + AngularDistance + LinearVelocityDistance + AngularVelocityDistance;
}

FVector FTGOR_MovementDynamic::TurningVelocity(const FVector& Point) const
{
	return AngularVelocity ^ (Point - Linear);
}

FVector FTGOR_MovementDynamic::PointVelocity(const FVector& Point) const
{
	return LinearVelocity + TurningVelocity(Point);
}

FTGOR_MovementSpace FTGOR_MovementDynamic::BaseToSpace(const FTGOR_MovementDynamic& ParentDynamic) const
{
	FTGOR_MovementSpace Space;
	const FVector LinearDelta = ParentDynamic.Angular * Linear;
	const FVector RadialVelocity = ParentDynamic.AngularVelocity ^ LinearDelta;

	// Angular
	Space.Angular = ParentDynamic.Angular * Angular;
	Space.RelativeAngularVelocity = ParentDynamic.Angular * AngularVelocity;
	Space.AngularVelocity = ParentDynamic.AngularVelocity + Space.RelativeAngularVelocity;
	Space.RelativeAngularAcceleration = ParentDynamic.AngularAcceleration;
	Space.AngularAcceleration = Space.RelativeAngularAcceleration + ParentDynamic.Angular * AngularAcceleration;

	// Linear
	Space.Linear = ParentDynamic.Linear + LinearDelta;
	Space.RelativeLinearVelocity = ParentDynamic.Angular * LinearVelocity;
	Space.LinearVelocity = ParentDynamic.LinearVelocity + Space.RelativeLinearVelocity + RadialVelocity;
	Space.RelativeLinearAcceleration = ParentDynamic.LinearAcceleration;
	if (LinearDelta.SizeSquared() > SMALL_NUMBER && ParentDynamic.AngularVelocity.SizeSquared() > SMALL_NUMBER)
	{
		const FVector InertiaAcceleration = (LinearDelta / 10'000.0f) ^ Space.RelativeAngularAcceleration;
		const FVector RelativeRadialVelocity = Space.RelativeLinearVelocity - Space.RelativeLinearVelocity.ProjectOnTo(ParentDynamic.AngularVelocity);
		const FVector CentrifugalAcceleration = ParentDynamic.AngularVelocity ^ (RelativeRadialVelocity + RadialVelocity);
		Space.RelativeLinearAcceleration -= InertiaAcceleration + CentrifugalAcceleration;
	}

	Space.LinearAcceleration = Space.RelativeLinearAcceleration + ParentDynamic.Angular * LinearAcceleration;
	return Space;
}

void FTGOR_MovementDynamic::DynamicToBase(const FTGOR_MovementDynamic& ParentDynamic, const FTGOR_MovementDynamic& Dynamic)
{
	const FQuat InvParentAngular = ParentDynamic.Angular.Inverse();
	const FVector LinearDelta = Dynamic.Linear - ParentDynamic.Linear;
	const FVector RadialVelocity = ParentDynamic.AngularVelocity ^ LinearDelta;

	// Angular
	Angular = InvParentAngular * Dynamic.Angular;
	AngularVelocity = InvParentAngular * (Dynamic.AngularVelocity - ParentDynamic.AngularVelocity);
	AngularAcceleration = InvParentAngular * (Dynamic.AngularAcceleration - ParentDynamic.AngularAcceleration);

	// Linear
	Linear = InvParentAngular * LinearDelta;
	FVector RelativeLinearVelocity = Dynamic.LinearVelocity - ParentDynamic.LinearVelocity - RadialVelocity;
	LinearVelocity = InvParentAngular * RelativeLinearVelocity;
	FVector RelativeLinearAcceleration = ParentDynamic.LinearAcceleration;
	if (LinearDelta.SizeSquared() > SMALL_NUMBER && ParentDynamic.AngularVelocity.SizeSquared() > SMALL_NUMBER)
	{
		const FVector InertiaAcceleration = (LinearDelta / 10'000.0f) ^ Dynamic.AngularAcceleration;
		const FVector RelativeRadialVelocity = RelativeLinearVelocity - RelativeLinearVelocity.ProjectOnTo(ParentDynamic.AngularVelocity);//Space.RelativeLinearVelocity -Space.RelativeLinearVelocity.ProjectOnTo(LinearDelta);
		const FVector CentrifugalAcceleration = ParentDynamic.AngularVelocity ^ (RelativeRadialVelocity + RadialVelocity);
		RelativeLinearAcceleration -= InertiaAcceleration + CentrifugalAcceleration;
	}
	LinearAcceleration = InvParentAngular * (Dynamic.LinearAcceleration - RelativeLinearAcceleration);
}

FTGOR_MovementDynamic FTGOR_MovementDynamic::TransformPosition(const FTGOR_MovementPosition& Position) const
{
	FTGOR_MovementDynamic Out;
	Out.Angular = Angular * Position.Angular;
	Out.Linear = Linear + Angular * Position.Linear;

	Out.AngularVelocity = AngularVelocity;
	Out.LinearVelocity = PointVelocity(Out.Linear);

	// TODO: Do this better like in space computation
	Out.AngularAcceleration = AngularAcceleration;
	Out.LinearAcceleration = LinearAcceleration + (AngularVelocity ^ Out.LinearVelocity);

	return Out;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

FTGOR_MovementSpace::FTGOR_MovementSpace()
	: FTGOR_MovementDynamic(),
	RelativeLinearVelocity(FVector::ZeroVector),
	RelativeAngularVelocity(FVector::ZeroVector),
	RelativeLinearAcceleration(FVector::ZeroVector),
	RelativeAngularAcceleration(FVector::ZeroVector)
{
}

FTGOR_MovementSpace::FTGOR_MovementSpace(const FTGOR_MovementDynamic& Other)
	: FTGOR_MovementDynamic(Other),
	RelativeLinearVelocity(FVector::ZeroVector),
	RelativeAngularVelocity(FVector::ZeroVector),
	RelativeLinearAcceleration(FVector::ZeroVector),
	RelativeAngularAcceleration(FVector::ZeroVector)
{
}

void FTGOR_MovementSpace::SetLinearVelocity(const FVector& Velocity)
{
	RelativeLinearVelocity += Velocity - LinearVelocity;
	LinearVelocity = Velocity;
}

void FTGOR_MovementSpace::SetAngularVelocity(const FVector& Velocity)
{
	RelativeAngularVelocity += Velocity - AngularVelocity;
	AngularVelocity = Velocity;
}

void FTGOR_MovementSpace::AddLinearVelocity(const FVector& VelocityDelta)
{
	RelativeLinearVelocity += VelocityDelta;
	LinearVelocity += VelocityDelta;
}

void FTGOR_MovementSpace::AddAngularVelocity(const FVector& VelocityDelta)
{
	RelativeAngularVelocity += VelocityDelta;
	AngularVelocity += VelocityDelta;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

FTGOR_PhysicsProperties::FTGOR_PhysicsProperties()
:	Gravity(0.0f, 0.0f, 0.0f),
	Density(0.0f),
	DragCoeff(0.0f),
	Depth(0.0f),
	Velocity(FVector::ZeroVector),
	Vorticity(FVector::ZeroVector)
{
}

void FTGOR_PhysicsProperties::SetLerpProperties(const FTGOR_PhysicsProperties& Other, float Alpha)
{
	Gravity = FMath::Lerp(Gravity, Other.Gravity, Alpha);
	Density = FMath::Lerp(Density, Other.Density, Alpha);
	DragCoeff = FMath::Lerp(DragCoeff, Other.DragCoeff, Alpha);
	Depth = FMath::Lerp(Depth, Other.Depth, Alpha);
	Velocity = FMath::Lerp(Velocity, Other.Velocity, Alpha);
	Vorticity = FMath::Lerp(Vorticity, Other.Vorticity, Alpha);
}


////////////////////////////////////////////////////////////////////////////////////////////////////

FTGOR_MovementDelta::FTGOR_MovementDelta()
:	LinearMomentum(FVector::ZeroVector),
	AngularMomentum(FVector::ZeroVector)
{
}

void FTGOR_MovementDelta::SetLerpDelta(const FTGOR_MovementDelta& Other, float Alpha)
{
	LinearMomentum = FMath::Lerp(LinearMomentum, Other.LinearMomentum, Alpha);
	AngularMomentum = FMath::Lerp(AngularMomentum, Other.AngularMomentum, Alpha);
}

float FTGOR_MovementDelta::CompareDelta(const FTGOR_MovementDelta& Other, const FTGOR_MovementThreshold& Threshold) const
{
	const float LinearDistance = Threshold.ThresholdDistance(LinearMomentum, Other.LinearMomentum, 0.0f);
	const float AngularDistance = Threshold.ThresholdAngle(AngularMomentum, Other.AngularMomentum, 0.0f);
	return LinearDistance + AngularDistance;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

FTGOR_MovementDamper::FTGOR_MovementDamper()
:	Damping(0.0f),
	Vector(FVector::ZeroVector)
{
}

////////////////////////////////////////////////////////////////////////////////////////////////////


FTGOR_MovementShape::FTGOR_MovementShape()
:	Radius(0.0f),
	Height(0.0f),
	Volume(0.0f)
{
}

void FTGOR_MovementShape::SetLerpShape(const FTGOR_MovementShape& Other, float Alpha)
{
	Radius = FMath::Lerp(Radius, Other.Radius, Alpha);
	Height = FMath::Lerp(Height, Other.Height, Alpha);
	Volume = FMath::Lerp(Volume, Other.Volume, Alpha);
}

void FTGOR_MovementShape::Send(FTGOR_NetworkWritePackage& Package, UTGOR_Singleton* Context) const
{
	Package.WriteEntry(Radius);
	Package.WriteEntry(Height);
	Package.WriteEntry(Volume);
}

void FTGOR_MovementShape::Recv(FTGOR_NetworkReadPackage& Package, UTGOR_Singleton* Context)
{
	Package.ReadEntry(Radius);
	Package.ReadEntry(Height);
	Package.ReadEntry(Volume);
}

////////////////////////////////////////////////////////////////////////////////////////////////////


FTGOR_MovementBody::FTGOR_MovementBody()
:	FTGOR_MovementShape(),
	Mass(1.0f),
	Inertia(FVector::OneVector),
	Faces(FVector::ZeroVector),
	Elasticity(0.5f),
	Friction(0.5f)
{
}

void FTGOR_MovementBody::SetLerpBody(const FTGOR_MovementBody& Other, float Alpha)
{
	FTGOR_MovementShape::SetLerpShape(Other, Alpha);
	Mass = FMath::Lerp(Mass, Other.Mass, Alpha);
	Inertia = FMath::Lerp(Inertia, Other.Inertia, Alpha);
	Faces = FMath::Lerp(Faces, Other.Faces, Alpha);
}

void FTGOR_MovementBody::Send(FTGOR_NetworkWritePackage& Package, UTGOR_Singleton* Context) const
{
	FTGOR_MovementShape::Send(Package, Context);
	Package.WriteEntry(Mass);
	Package.WriteEntry(Inertia);
	Package.WriteEntry(Faces);
	Package.WriteEntry(Elasticity);
	Package.WriteEntry(Friction);
}

void FTGOR_MovementBody::Recv(FTGOR_NetworkReadPackage& Package, UTGOR_Singleton* Context)
{
	FTGOR_MovementShape::Recv(Package, Context);
	Package.ReadEntry(Mass);
	Package.ReadEntry(Inertia);
	Package.ReadEntry(Faces);
	Package.ReadEntry(Elasticity);
	Package.ReadEntry(Friction);
}

void FTGOR_MovementBody::SetFromBox(const FVector& SurfaceSparsity, const FVector& Dimensions, float Weight)
{
	Radius = Dimensions.GetMin() / 2;
	Height = Dimensions.Z;

	// Convert to meters
	const FVector Meters = Dimensions / 100.0f;

	// Compute volume and mass
	Mass = Weight;
	Volume = Meters.X * Meters.Y * Meters.Z;

	// Compute surface area
	const FVector Area = FVector(Meters.Y * Meters.Z, Meters.X * Meters.Z, Meters.X * Meters.Y);
	Faces = Area * SurfaceSparsity;

	// Compute inertia tensor
	Inertia.X = Meters.Y * Meters.Y + Meters.Z * Meters.Z;
	Inertia.Y = Meters.X * Meters.X + Meters.Z * Meters.Z;
	Inertia.Z = Meters.X * Meters.X + Meters.Y * Meters.Y;
	Inertia *= Mass / 12.0f; // See inertia tensor list
}

void FTGOR_MovementBody::SetFromSphere(const FVector& SurfaceSparsity, float BodyRadius, float Weight)
{
	Radius = BodyRadius;
	Height = BodyRadius * 2.0f;

	// Convert to meters
	const float Meters = BodyRadius / 100.0f;
	const float Squared = Meters * Meters;

	// Compute volume and mass
	Mass = Weight;
	Volume = Squared * Meters * PI * 4.0f / 3.0f;

	// Compute surface area
	const FVector Area = FVector(Squared * PI);
	Faces = Area * SurfaceSparsity;

	// Compute inertia tensor
	Inertia = FVector(Squared * Mass * 2.0f / 5.0f);
}

void FTGOR_MovementBody::SetFromCapsule(const FVector& SurfaceSparsity, float BodyRadius, float BodyHalfHeight, float Weight)
{
	Radius = BodyRadius;
	Height = BodyHalfHeight * 2.0f;

	// Convert to meters
	const float RadiusMeters = BodyRadius / 100.0f;
	const float HeightMeters = (BodyHalfHeight - BodyRadius) / 50.0f;
	const float Squared = RadiusMeters * RadiusMeters;

	// Compute surface area
	const FVector Area = FVector(Squared * PI) + FVector(FVector2D(HeightMeters + 2 * RadiusMeters), 0.0f);
	Faces = Area * SurfaceSparsity;

	// Compute inertia tensor
	Inertia = FVector::ZeroVector;
	Mass = Weight;

	// See https://www.gamedev.net/articles/programming/math-and-physics/capsule-inertia-tensor-r3856/
	float rr = PI * Squared;
	float hsM = rr * 2.0f / 3.0f * RadiusMeters;
	float cM = rr * HeightMeters;

	Inertia.Z = Squared * cM * 0.5f;
	Inertia.X = Inertia.Y = Inertia.Z * 0.5f + cM * HeightMeters * HeightMeters / 12.0f;

	float temp0 = hsM * 2.0f * Squared / 5.0f;
	Inertia.Z += temp0 * 2.0f;

	float temp1 = HeightMeters * 0.5f;
	float temp2 = temp0 + hsM * (temp1 * temp1 + 3.0f / 8.0f * HeightMeters * RadiusMeters);

	Inertia.X += temp2 * 2.0f;
	Inertia.Y += temp2 * 2.0f;

	Inertia *= Mass;
	Volume = (cM + hsM * 2.0f);
}

float FTGOR_MovementBody::GetCompressionRatio(const FTGOR_PhysicsProperties& Surroundings) const
{
	const float Compression = 500'000.0f;
	if (Compression >= SMALL_NUMBER)
	{
		return(FMath::Exp(-LNHALF * Surroundings.Density * Surroundings.Depth / 100.0f / Compression));
	}
	return 1.0f;
}

float FTGOR_MovementBody::GetDisplacedMass(const FTGOR_PhysicsProperties& Surroundings) const
{
	if (Radius >= SMALL_NUMBER)
	{
		// Compute bouyancy from mass displacement
		const float Ratio = GetCompressionRatio(Surroundings);
		const float UpForce = Surroundings.Density * Volume * Ratio;
		return(UpForce * FMath::Min(1.0f, Surroundings.Depth / (Radius / 2.0f)));
	}
	return 0.0f;
}

float FTGOR_MovementBody::ComputeDragResponse(const FTGOR_MovementPosition& Position, const FTGOR_PhysicsProperties& Surroundings, const FVector& RelativeVelocity, float Coeff) const
{
	// Ignore zero velocity
	const float Squared = RelativeVelocity.SizeSquared();
	if (Squared < SMALL_NUMBER)
	{
		return 0.0f;
	}
	else
	{
		// Compute drag direction
		const float Speed = FMath::Sqrt(Squared);
		const FVector Direction = RelativeVelocity / Speed;

		// Compute area in movement direction
		const FVector Weights = (Position.Angular.Inverse() * Direction).GetAbs();
		const FVector Normalised = Weights / (Weights.X + Weights.Y + Weights.Z);
		const float Area = Faces | Normalised;

		// Apply drag from movement mode
		return 50.0f * Speed * Surroundings.Density * Coeff * Area;
	}
}

FTGOR_MovementDamper FTGOR_MovementBody::ComputeExternalForce(const FTGOR_MovementSpace& Space, const FTGOR_PhysicsProperties& Surroundings) const
{
	FTGOR_MovementDamper Output;

	//////////////////////////////////////////////////////
	const FVector Gravity = Surroundings.Gravity * Mass;

	//////////////////////////////////////////////////////
	const float DisplacedMass = GetDisplacedMass(Surroundings);
	const FVector Bouyancy = -Surroundings.Gravity * DisplacedMass;

	//////////////////////////////////////////////////////
	const FVector RelativeVelocity = Space.LinearVelocity - Surroundings.Velocity;
	Output.Damping = ComputeDragResponse(Space, Surroundings, RelativeVelocity, Surroundings.DragCoeff);
	const FVector LinearDrag = -RelativeVelocity * Output.Damping;

	//////////////////////////////////////////////////////
	Output.Vector = Gravity + Bouyancy + LinearDrag + GetMassedLinear(Space.RelativeLinearAcceleration);

	if (Output.Vector.ContainsNaN())
	{
		Output.Vector = FVector::ZeroVector;
		RPORT("External Force singularity");
	}

	return Output;
}

FTGOR_MovementDamper FTGOR_MovementBody::ComputeExternalTorque(const FTGOR_MovementSpace& Space, const FTGOR_PhysicsProperties& Surroundings) const
{
	FTGOR_MovementDamper Output;

	//////////////////////////////////////////////////////
	const FVector RelativeVelocity = Space.AngularVelocity - Surroundings.Vorticity;
	Output.Damping = ComputeDragResponse(Space, Surroundings, RelativeVelocity, Surroundings.DragCoeff);
	const FVector AngularDrag = -RelativeVelocity * Output.Damping;

	//////////////////////////////////////////////////////
	Output.Vector = AngularDrag + GetMassedAngular(Space.Angular, Space.RelativeAngularAcceleration);

	if (Output.Vector.ContainsNaN())
	{
		Output.Vector = FVector::ZeroVector;
		RPORT("External Torque singularity");
	}

	return Output;
}

FTGOR_MovementSpace FTGOR_MovementBody::SimulateForce(const FTGOR_MovementSpace& Space, const FTGOR_MovementForce& Force, const FTGOR_MovementForce& External, float DeltaTime) const
{
	FTGOR_MovementSpace Out;

	float LinearSpeedClamp = 10'000.0f;
	float AngularSpeedClamp = 50.0f;

	// Update linear velocities
	Out.LinearAcceleration = GetUnmassedLinear(Force.Force + External.Force);
	const FVector LinearVelocity = Space.LinearVelocity + Space.LinearAcceleration * DeltaTime;
	const FVector RelativeLinearVelocity = Space.RelativeLinearVelocity + Space.LinearAcceleration * DeltaTime;
	Out.LinearVelocity = UTGOR_Math::ClampToSize(LinearVelocity, LinearSpeedClamp);
	Out.RelativeLinearVelocity = RelativeLinearVelocity + Out.LinearVelocity - LinearVelocity;

	// Update angular velocities
	Out.AngularAcceleration = GetUnmassedAngular(Space.Angular, Force.Torque + External.Torque);
	const FVector AngularVelocity = Space.AngularVelocity + Space.AngularAcceleration * DeltaTime;
	const FVector RelativeAngularVelocity = Space.RelativeAngularVelocity + Space.AngularAcceleration * DeltaTime;
	Out.AngularVelocity = UTGOR_Math::ClampToSize(AngularVelocity, AngularSpeedClamp);
	Out.RelativeAngularVelocity = RelativeAngularVelocity + Out.AngularVelocity - AngularVelocity;

	// Copy state values
	Out.Linear = Space.Linear;
	Out.Angular = Space.Angular;
	return Out;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

FVector FTGOR_MovementBody::GetUnmassedLinear(const FVector& Vector) const
{
	if (Mass >= SMALL_NUMBER)
	{
		return Vector / Mass;
	}
	return FVector::ZeroVector;
}

FVector FTGOR_MovementBody::GetUnmassedAngular(const FQuat& Rotation, const FVector& Vector) const
{
	if (Inertia.GetAbsMin() > SMALL_NUMBER)
	{
		return Rotation * ((Rotation.Inverse() * Vector) / Inertia);
	}
	return FVector::ZeroVector;
}

FVector FTGOR_MovementBody::GetMassedLinear(const FVector& Vector) const
{
	return Vector * Mass;
}

FVector FTGOR_MovementBody::GetMassedAngular(const FQuat& Rotation, const FVector& Vector) const
{
	return Rotation * ((Rotation.Inverse() * Vector) * Inertia);
}