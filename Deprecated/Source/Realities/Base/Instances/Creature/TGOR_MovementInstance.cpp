// The Gateway of Realities: Planes of Existence.


#include "TGOR_MovementInstance.h"
#include "Realities/Mod/Targets/Sockets/TGOR_Socket.h"
#include "Realities/Mod/Movements/TGOR_Movement.h"
#include "Realities/Mod/Animations/TGOR_Animation.h"
#include "Realities/Base/TGOR_Singleton.h"
#include "Realities/Base/TGOR_GameInstance.h"
#include "Realities/Components/Movement/TGOR_PilotComponent.h"
#include "Realities/Actors/Dimension/Volumes/TGOR_PhysicsVolume.h"
#include "Realities/Utility/Storage/TGOR_Serialisation.h"
#include "Realities/Utility/Storage/TGOR_Package.h"

#include "Components/PrimitiveComponent.h"
#include "Realities/Base/Content/TGOR_ContentManager.h"
#include "Realities/Utility/TGOR_Math.h"

#include "DrawDebugHelpers.h"

SERIALISE_INIT_SOURCE_IMPLEMENT(FTGOR_MovementBase)
SERIALISE_INIT_IMPLEMENT(FTGOR_MovementShape)
SERIALISE_INIT_IMPLEMENT(FTGOR_MovementBody)
SERIALISE_INIT_IMPLEMENT(FTGOR_MovementFrame)
SERIALISE_INIT_IMPLEMENT(FTGOR_MovementState)
SERIALISE_INIT_SOURCE_IMPLEMENT(FTGOR_MovementContent)
SERIALISE_INIT_IMPLEMENT(FTGOR_MovementCapture)
SERIALISE_INIT_IMPLEMENT(FTGOR_MovementUpdate)
SERIALISE_INIT_SOURCE_IMPLEMENT(FTGOR_MovementContentUpdate)
SERIALISE_INIT_IMPLEMENT(FTGOR_MovementInputUpdate)

////////////////////////////////////////////////////////////////////////////////////////////////////

FTGOR_PositionThreshold::FTGOR_PositionThreshold()
:	Location(100.0f),
	Rotation(PI / 2)
{
}

FTGOR_PositionThreshold::FTGOR_PositionThreshold(float Location, float Rotation)
	: Location(Location),
	Rotation(Rotation)
{

}

FTGOR_PositionThreshold::FTGOR_PositionThreshold(const FTGOR_PositionThreshold& Other, float Multiplier)
: Location(Other.Location * Multiplier),
	Rotation(Other.Rotation * Multiplier)
{
}

////////////////////////////////////////////////////////////////////////////////////////////////////

FTGOR_MovementThreshold::FTGOR_MovementThreshold()
: FTGOR_PositionThreshold(),
	LinearVelocity(600.0f),
	AngularVelocity(3.0f),
	VelocityPositionMultiplier(0.1f),
	TimeThreshold(0.5f)
{
}

FTGOR_MovementThreshold::FTGOR_MovementThreshold(float Location, float Rotation, float LinearVelocity, float AngularVelocity, float VelocityPositionMultiplier, float TimeThreshold)
:	FTGOR_PositionThreshold(Location, Rotation),
	LinearVelocity(LinearVelocity),
	AngularVelocity(AngularVelocity),
	VelocityPositionMultiplier(VelocityPositionMultiplier),
	TimeThreshold(TimeThreshold)
{
}

FTGOR_MovementThreshold::FTGOR_MovementThreshold(const FTGOR_MovementThreshold& Other, float Multiplier)
:	FTGOR_PositionThreshold(Other, Multiplier),
	LinearVelocity(Other.LinearVelocity * Multiplier),
	AngularVelocity(Other.AngularVelocity * Multiplier),
	VelocityPositionMultiplier(Other.VelocityPositionMultiplier),
	TimeThreshold(Other.TimeThreshold * Multiplier)
{
}

FTGOR_PositionThreshold FTGOR_MovementThreshold::operator()(float Linear, float Angular) const
{
	FTGOR_PositionThreshold LocationThreshold;
	const float LinearSpeedRatio = 1.0f + (Linear / LinearVelocity * VelocityPositionMultiplier);
	const float AngularSpeedRatio = 1.0f + (Angular / AngularVelocity * VelocityPositionMultiplier);
	LocationThreshold.Location = Location * LinearSpeedRatio;
	LocationThreshold.Rotation = Rotation * LinearSpeedRatio;
	return LocationThreshold;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

FTGOR_MovementForce::FTGOR_MovementForce()
	: Force(FVector::ZeroVector),
	Torque(FVector::ZeroVector)
{
}

FTGOR_MovementForce FTGOR_MovementForce::Lerp(const FTGOR_MovementForce& Other, float Alpha) const
{
	FTGOR_MovementForce Out;
	Out.Force = FMath::Lerp(Force, Other.Force, Alpha);
	Out.Torque = FMath::Lerp(Torque, Other.Torque, Alpha);
	return Out;
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

FTGOR_MovementPosition FTGOR_MovementPosition::Lerp(const FTGOR_MovementPosition& Other, float Alpha) const
{
	FTGOR_MovementPosition Out;
	Out.Linear = FMath::Lerp(Linear, Other.Linear, Alpha);
	Out.Angular = FMath::Lerp(Angular, Other.Angular, Alpha);
	return Out;
}

float FTGOR_MovementPosition::Compare(const FTGOR_MovementPosition& Position, const FTGOR_PositionThreshold& Threshold) const
{
	const float Distance = (Linear - Position.Linear).SizeSquared() / (Threshold.Location * Threshold.Location);
	const float Angle = Angular.AngularDistance(Position.Angular) / Threshold.Rotation;
	return Distance + Angle;
}

FTGOR_MovementPosition FTGOR_MovementPosition::BaseToPosition(const FTGOR_MovementPosition& ParentSpace) const
{
	FTGOR_MovementPosition Position;
	Position.Angular = ParentSpace.Angular * Angular;
	Position.Linear = ParentSpace.Linear + ParentSpace.Angular * Linear;
	return Position;
}

void FTGOR_MovementPosition::PositionToBase(const FTGOR_MovementPosition& ParentPosition, const FTGOR_MovementPosition& Position)
{
	const FQuat InvAngular = ParentPosition.Angular.Inverse();
	Angular = InvAngular * Position.Angular;
	Linear = InvAngular * (Position.Linear - ParentPosition.Linear);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

FTGOR_MovementDynamic::FTGOR_MovementDynamic()
:	FTGOR_MovementPosition(),
	LinearVelocity(FVector::ZeroVector),
	AngularVelocity(FVector::ZeroVector),
	LinearAcceleration(FVector::ZeroVector),
	AngularAcceleration(FVector::ZeroVector)
{
}

FTGOR_MovementDynamic FTGOR_MovementDynamic::Lerp(const FTGOR_MovementDynamic& Other, float Alpha) const
{
	FTGOR_MovementDynamic Out;
	Out.FTGOR_MovementPosition::Lerp(Other, Alpha);

	Out.LinearVelocity = FMath::Lerp(LinearVelocity, Other.LinearVelocity, Alpha);
	Out.AngularVelocity = FMath::Lerp(AngularVelocity, Other.AngularVelocity, Alpha);
	Out.LinearAcceleration = FMath::Lerp(LinearAcceleration, Other.LinearAcceleration, Alpha);
	Out.AngularAcceleration = FMath::Lerp(AngularAcceleration, Other.AngularAcceleration, Alpha);
	return Out;
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
	if (LinearDelta.SizeSquared() > SMALL_NUMBER&& ParentDynamic.AngularVelocity.SizeSquared() > SMALL_NUMBER)
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
	if (LinearDelta.SizeSquared() > SMALL_NUMBER&& ParentDynamic.AngularVelocity.SizeSquared() > SMALL_NUMBER)
	{
		const FVector InertiaAcceleration = (LinearDelta / 10'000.0f) ^ Dynamic.AngularAcceleration;
		const FVector RelativeRadialVelocity = RelativeLinearVelocity - RelativeLinearVelocity.ProjectOnTo(ParentDynamic.AngularVelocity);//Space.RelativeLinearVelocity -Space.RelativeLinearVelocity.ProjectOnTo(LinearDelta);
		const FVector CentrifugalAcceleration = ParentDynamic.AngularVelocity ^ (RelativeRadialVelocity + RadialVelocity);
		RelativeLinearAcceleration -= InertiaAcceleration + CentrifugalAcceleration;
	}
	LinearAcceleration = InvParentAngular * (Dynamic.LinearAcceleration - RelativeLinearAcceleration);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

FTGOR_MovementSpace::FTGOR_MovementSpace()
:	FTGOR_MovementDynamic(),
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

FTGOR_MovementBase::FTGOR_MovementBase()
: FTGOR_MovementBase(nullptr)
{
}

FTGOR_MovementBase::FTGOR_MovementBase(UTGOR_MobilityComponent* Component)
: FTGOR_MovementDynamic(),
	Index(),
	Socket(nullptr),
	Parent(nullptr),
	Component(Component)
{
}

FTGOR_MovementBase FTGOR_MovementBase::Lerp(const FTGOR_MovementBase& Other, float Alpha) const
{
	// If component/parent changed we don't lerp at all
	if (Component != Other.Component || Parent != Other.Parent)
	{
		return Alpha < 0.5f ? (*this) : Other;
	}

	// Lerp dynamic part
	FTGOR_MovementBase Out = *this;
	Out.FTGOR_MovementDynamic::operator=(FTGOR_MovementDynamic::Lerp(Other, Alpha));
	return Out;
}

float FTGOR_MovementBase::Compare(const FTGOR_MovementBase& Base, const FTGOR_MovementThreshold& Threshold) const
{
	if (Parent.Get() != Base.Parent.Get())
	{
		return 1.0f;
	}

	if (Index != Base.Index)
	{
		return 1.0f;
	}

	if (Socket != Base.Socket)
	{
		return 1.0f;
	}

	const float LinearSpeed = Base.LinearVelocity.Size();
	const float AngularSpeed = Base.AngularVelocity.Size();
	const float Cost = FTGOR_MovementPosition::Compare(Base, Threshold(LinearSpeed, AngularSpeed));
	if (Cost >= 1.0f)
	{
		return 1.0f;
	}

	const float Distance = (LinearVelocity - Base.LinearVelocity).SizeSquared() / (Threshold.LinearVelocity * Threshold.LinearVelocity);
	const float Angle = (AngularVelocity - Base.AngularVelocity).SizeSquared() / (Threshold.AngularVelocity * Threshold.AngularVelocity);
	return Cost + Distance + Angle;
}

bool FTGOR_MovementBase::HasCycle() const
{
	return Parent.IsValid() && Parent->HasParent(Component.Get());
}

void FTGOR_MovementBase::Adjust(const FTGOR_MovementBase& Base)
{
	Index = Base.Index;
	Socket = Base.Socket;
	Parent = Base.Parent;
	Linear = Base.Linear;
	Angular = Base.Angular;
	LinearVelocity = Base.LinearVelocity;
	AngularVelocity = Base.AngularVelocity;
}


FTGOR_MovementPosition FTGOR_MovementBase::ComputePosition() const
{
	return BaseToPosition(ComputeParentPosition());
}

FTGOR_MovementPosition FTGOR_MovementBase::ComputeParentPosition() const
{
	if (Parent.IsValid())
	{
		const FTGOR_MovementPosition Position = Parent->ComputePosition();
		if (Index || IsValid(Socket))
		{
			const FTGOR_MovementPosition Local = Parent->GetIndexTransform(Socket, Index.Index);
			return Local.BaseToPosition(Position);
		}
		return Position;
	}
	return FTGOR_MovementSpace();
}

////////////////////////////////////////////////////////////////////////////////////////////////////

FTGOR_MovementSpace FTGOR_MovementBase::ComputeSpace() const
{
	return BaseToSpace(ComputeParentSpace());
}

FTGOR_MovementSpace FTGOR_MovementBase::ComputeParentSpace() const
{
	if (Parent.IsValid())
	{
		FTGOR_MovementSpace Space = Parent->ComputeSpace();
		if (Index || IsValid(Socket))
		{
			const FTGOR_MovementDynamic Local = Parent->GetIndexTransform(Socket, Index.Index);
			return Local.BaseToSpace(Space);
		}

		return Space;
	}
	return FTGOR_MovementSpace();
}



void FTGOR_MovementBase::Send(FTGOR_NetworkWritePackage& Package, UTGOR_Singleton* Context) const
{
	Package.WriteEntry(Index);
	Package.WriteEntry(Socket);
	Package.WriteEntry(Parent);
	Package.WriteEntry(Linear);
	Package.WriteEntry(LinearVelocity);
	Package.WriteEntry(Angular);
	Package.WriteEntry(AngularVelocity);
}

void FTGOR_MovementBase::Recv(FTGOR_NetworkReadPackage& Package, UTGOR_Singleton* Context)
{
	Package.ReadEntry(Index);
	Package.ReadEntry(Socket);
	Package.ReadEntry(Parent);
	Package.ReadEntry(Linear);
	Package.ReadEntry(LinearVelocity);
	Package.ReadEntry(Angular);
	Package.ReadEntry(AngularVelocity);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

FTGOR_MovementRecord::FTGOR_MovementRecord()
{
}

FTGOR_MovementRecord::FTGOR_MovementRecord(const FTGOR_MovementBase& Base)
	: FTGOR_MovementRecord()
{
	SetRoot(Base);
}

FTGOR_MovementRecord FTGOR_MovementRecord::Lerp(const FTGOR_MovementRecord& Other, float Alpha) const
{
	FTGOR_MovementRecord Out;
	Out.Root = Root.Lerp(Other.Root, Alpha);
	Out.Parent = Parent.Lerp(Other.Parent, Alpha);
	return Out;
}

void FTGOR_MovementRecord::Simulate() const
{
	if (Root.Parent.IsValid())
	{
		Root.Parent->SimulateDynamic(Parent, Root.Socket, Root.Index);
	}
}

const FTGOR_MovementBase& FTGOR_MovementRecord::GetRoot() const
{
	return Root;
}

void FTGOR_MovementRecord::SetRoot(const FTGOR_MovementBase& Base)
{
	Root = Base;
	if (Root.Parent.IsValid())
	{
		Parent = Root.ComputeParentSpace();
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////

FTGOR_MovementTick::FTGOR_MovementTick()
	: Deltatime(0.0f),
	MaxLinearDamping(0.0f),
	MaxAngularDamping(0.0f)
{

}

////////////////////////////////////////////////////////////////////////////////////////////////////

FTGOR_MovementDelta::FTGOR_MovementDelta()
:	LinearMomentum(FVector::ZeroVector),
	AngularMomentum(FVector::ZeroVector)
{
}

FTGOR_MovementDelta FTGOR_MovementDelta::Lerp(const FTGOR_MovementDelta& Other, float Alpha) const
{
	FTGOR_MovementDelta Out;
	Out.LinearMomentum = FMath::Lerp(LinearMomentum, Other.LinearMomentum, Alpha);
	Out.AngularMomentum = FMath::Lerp(AngularMomentum, Other.AngularMomentum, Alpha);
	return Out;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

FTGOR_MovementDamper::FTGOR_MovementDamper()
	: Damping(0.0f),
Vector(FVector::ZeroVector)
{
}

FTGOR_MovementOutput::FTGOR_MovementOutput()
	: FTGOR_MovementForce(),
	Base(nullptr),
	Index(INDEX_NONE),
	BaseImpactPoint(FVector::ZeroVector),
	Orientation(FVector::UpVector),
	CollisionEnabled(true),
	MaxLinearDamping(0.0f),
	MaxAngularDamping(0.0f)
{
}

void FTGOR_MovementOutput::SetBaseFrom(AActor* Owner, const FName& Name)
{
	Index = INDEX_NONE;
	Base = nullptr;
	if (IsValid(Owner))
	{
		// Only grab pilot components
		// TODO: Make interface to look for specific bones
		UActorComponent* MobilityComponent = Owner->GetComponentByClass(UTGOR_MobilityComponent::StaticClass());
		if (IsValid(MobilityComponent))
		{
			Base = Cast<UTGOR_MobilityComponent>(MobilityComponent);
			Index = Base->GetIndexFromName(Name);
		}
	}
}

void FTGOR_MovementOutput::SetBaseFrom(UActorComponent* Component, const FName& Name)
{
	Index = INDEX_NONE;
	Base = nullptr;
	if (IsValid(Component))
	{
		SetBaseFrom(Component->GetOwner(), Name);
	}
}

void FTGOR_MovementOutput::SetBaseFromVolume(ATGOR_PhysicsVolume* Volume, const FName& Name)
{
	Index = INDEX_NONE;
	Base = nullptr;
	if (IsValid(Volume))
	{
		Base = Volume->GetOwning();
		Index = Base->GetIndexFromName(Name);
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////

FTGOR_MovementGround::FTGOR_MovementGround()
:	SurfaceNormal(FVector::UpVector),
	SurfaceOffset(FVector::ZeroVector),
	Slope(0.0f),
	GroundDistance(0.0f),
	Component(nullptr),
	Name("")
{
}

////////////////////////////////////////////////////////////////////////////////////////////////////

FTGOR_MovementContact::FTGOR_MovementContact()
:	FrameNormal(FVector::UpVector), 
	FrameRight(FVector::RightVector),
	FrameForward(FVector::ForwardVector),
	FrameInput(FVector::ZeroVector),
	FrameVelocity(FVector::ZeroVector),
	FrameAngular(FVector::ZeroVector),
	VerticalSpeed(0.0f)
{
}


FTGOR_VisualContact::FTGOR_VisualContact()
	: Location(FVector::ZeroVector),
	Normal(FVector::UpVector),
	Blocking(false)
{
}

////////////////////////////////////////////////////////////////////////////////////////////////////

FTGOR_MovementRepel::FTGOR_MovementRepel()
:	RepelNormal(FVector::ZeroVector), 
	RepelForce(FVector::ZeroVector), 
	RepelHit(false)
{
}

FVector FTGOR_MovementRepel::Project(const FVector& Direction, float Threshold) const
{
	if (RepelHit)
	{
		const float Dot = Direction | RepelNormal;
		if (Dot < 0.0f)
		{
			const FVector Vector = (Direction - Dot * RepelNormal);
			const float Size = Vector.Size();
			if (Size >= Threshold)
			{
				return(Vector / Size);
			}
		}
	}
	return(Direction);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

FTGOR_PhysicsProperties::FTGOR_PhysicsProperties()
	: Gravity(0.0f, 0.0f, 0.0f),
	Density(0.0f),
	DragCoeff(0.0f),
	Depth(0.0f),
	Velocity(FVector::ZeroVector),
	Vorticity(FVector::ZeroVector)
{
}

FTGOR_PhysicsProperties FTGOR_PhysicsProperties::Lerp(const FTGOR_PhysicsProperties& Other, float Alpha) const
{
	FTGOR_PhysicsProperties Out;

	Out.Gravity = FMath::Lerp(Gravity, Other.Gravity, Alpha);
	Out.Density = FMath::Lerp(Density, Other.Density, Alpha);
	Out.DragCoeff = FMath::Lerp(DragCoeff, Other.DragCoeff, Alpha);
	Out.Depth = FMath::Lerp(Depth, Other.Depth, Alpha);
	Out.Velocity = FMath::Lerp(Velocity, Other.Velocity, Alpha);
	Out.Vorticity = FMath::Lerp(Vorticity, Other.Vorticity, Alpha);
	return Out;
}

////////////////////////////////////////////////////////////////////////////////////////////////////


FTGOR_MovementShape::FTGOR_MovementShape()
: Radius(0.0f),
Height(0.0f),
Volume(0.0f)
{
}

FTGOR_MovementShape FTGOR_MovementShape::Lerp(const FTGOR_MovementShape& Other, float Alpha) const
{
	FTGOR_MovementShape Out;

	Out.Radius = FMath::Lerp(Radius, Other.Radius, Alpha);
	Out.Height = FMath::Lerp(Height, Other.Height, Alpha);
	Out.Volume = FMath::Lerp(Volume, Other.Volume, Alpha);
	
	return Out;
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
: FTGOR_MovementShape(),
Mass(1.0f),
Inertia(FVector::OneVector),
Faces(FVector::ZeroVector)
{
}

FTGOR_MovementBody FTGOR_MovementBody::Lerp(const FTGOR_MovementBody& Other, float Alpha) const
{
	FTGOR_MovementBody Out;

	Out.FTGOR_MovementShape::operator=(FTGOR_MovementShape::Lerp(Other, Alpha));
	Out.Mass = FMath::Lerp(Mass, Other.Mass, Alpha);
	Out.Inertia = FMath::Lerp(Inertia, Other.Inertia, Alpha);
	Out.Faces = FMath::Lerp(Faces, Other.Faces, Alpha);
	return Out;
}

void FTGOR_MovementBody::Send(FTGOR_NetworkWritePackage& Package, UTGOR_Singleton* Context) const
{
	FTGOR_MovementShape::Send(Package, Context);
	Package.WriteEntry(Mass);
	Package.WriteEntry(Inertia);
	Package.WriteEntry(Faces);
}

void FTGOR_MovementBody::Recv(FTGOR_NetworkReadPackage& Package, UTGOR_Singleton* Context)
{
	FTGOR_MovementShape::Recv(Package, Context);
	Package.ReadEntry(Mass);
	Package.ReadEntry(Inertia);
	Package.ReadEntry(Faces);
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
	Height = (BodyHalfHeight + BodyRadius) * 2.0f;

	// Convert to meters
	const float RadiusMeters = BodyRadius / 100.0f;
	const float HeightMeters = BodyHalfHeight / 50.0f;
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
		const float Ratio = GetCompressionRatio( Surroundings);
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

////////////////////////////////////////////////////////////////////////////////////////////////////

FTGOR_MovementFrame::FTGOR_MovementFrame()
:	Agility(1.0f),
	Strength(1.0f)
{
}

FTGOR_MovementFrame FTGOR_MovementFrame::Lerp(const FTGOR_MovementFrame& Other, float Alpha) const
{
	FTGOR_MovementFrame Out;
	Out.Agility = FMath::Lerp(Agility, Other.Agility, Alpha);
	Out.Strength = FMath::Lerp(Strength, Other.Strength, Alpha);
	return Out;
}

void FTGOR_MovementFrame::Send(FTGOR_NetworkWritePackage& Package, UTGOR_Singleton* Context) const
{
	Package.WriteEntry(Agility);
	Package.WriteEntry(Strength);
}

void FTGOR_MovementFrame::Recv(FTGOR_NetworkReadPackage& Package, UTGOR_Singleton* Context)
{
	Package.ReadEntry(Agility);
	Package.ReadEntry(Strength);
}

/*
bool FTGOR_MovementFrame::NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess)
{
	bOutSuccess = true;
	uint8 Mask;
	
	Ar << Mass;
	Ar << Radius;
	Ar << Height;
	Ar << Volume;

	SERIALISE_MASK_VECT(Faces);
	SERIALISE_MASK_VECT(Inertia);

	Ar << Autonomy;

	return bOutSuccess;
}
*/

////////////////////////////////////////////////////////////////////////////////////////////////////

FTGOR_MovementState::FTGOR_MovementState()
:	Input(FVector::ZeroVector),
	View(FVector::ZeroVector)
{
}

FTGOR_MovementState FTGOR_MovementState::Lerp(const FTGOR_MovementState& Other, float Alpha) const
{
	FTGOR_MovementState Out;

	/*
	// Pseudo-polar lerp is more accurate to input behaviour (?)
	const float InputSize = Input.Size();
	const float OtherSize = Other.Input.Size();
	const float OutSize = FMath::Lerp(InputSize, OtherSize, Alpha);
	const FVector InputDirection = UTGOR_Math::Normalize(FMath::Lerp(Input, Other.Input, Alpha));
	Out.Input = InputDirection * OutSize;
	*/

	Out.Input = FMath::Lerp(Input, Other.Input, Alpha);
	Out.View = UTGOR_Math::Normalize(FMath::Lerp(View, Other.View, Alpha));
	return Out;
}

float FTGOR_MovementState::Compare(const FTGOR_MovementState& State, double InputThres) const
{
	return ((Input - State.Input).SizeSquared() / (InputThres*InputThres));
}

void FTGOR_MovementState::Send(FTGOR_NetworkWritePackage& Package, UTGOR_Singleton* Context) const
{
	Package.WriteEntry(Input);
	Package.WriteEntry(View);
}

void FTGOR_MovementState::Recv(FTGOR_NetworkReadPackage& Package, UTGOR_Singleton* Context)
{
	Package.ReadEntry(Input);
	Package.ReadEntry(View);
}

/*
bool FTGOR_MovementState::NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess)
{
	bOutSuccess = true;
	uint8 Mask;

	SERIALISE_MASK_NORM(Input);
	SERIALISE_MASK_NORM(View);

	return bOutSuccess;
}
*/

////////////////////////////////////////////////////////////////////////////////////////////////////

FTGOR_MovementContent::FTGOR_MovementContent()
:	Movement(nullptr),
	Animation(nullptr),
	HasOverride(false)
{
}

FTGOR_MovementContent FTGOR_MovementContent::Lerp(const FTGOR_MovementContent& Other, float Alpha) const
{
	FTGOR_MovementContent Out;

	Out.Movement = Alpha < 0.5f ? Movement : Other.Movement;
	Out.Animation = Alpha < 0.5f ? Animation : Other.Animation;
	Out.HasOverride = Alpha < 0.5f ? HasOverride : Other.HasOverride;
	return Out;
}

float FTGOR_MovementContent::Compare(const FTGOR_MovementContent& State) const
{
	if (Movement != State.Movement)
	{
		return 1.0f;
	}

	if (Animation != State.Animation)
	{
		return 1.0f;
	}
	return 0.0f;
}

void FTGOR_MovementContent::Send(FTGOR_NetworkWritePackage& Package, UTGOR_Singleton* Context) const
{
	Package.WriteEntry(Movement);
}

void FTGOR_MovementContent::Recv(FTGOR_NetworkReadPackage& Package, UTGOR_Singleton* Context)
{
	Package.ReadEntry(Movement);
}

/*
bool FTGOR_MovementContent::NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess)
{
	bOutSuccess = true;
	uint8 Mask;
	SERIALISE_INIT_SOURCE;
	
	SERIALISE_MASK_SOURCE(Movement, Movement);
	// Not necessary to replicate
	//SERIALISE_MASK_SOURCE(Animation, Animation);

	return bOutSuccess;
}
*/

////////////////////////////////////////////////////////////////////////////////////////////////////

FTGOR_MovementCapture::FTGOR_MovementCapture()
:	PhysicsVolume(nullptr),
	UpVector(FVector::UpVector)
{
}

FTGOR_MovementCapture FTGOR_MovementCapture::Lerp(const FTGOR_MovementCapture& Other, float Alpha) const
{
	FTGOR_MovementCapture Out;
	Surroundings.Lerp(Other.Surroundings, Alpha);

	Out.PhysicsVolume = Alpha < 0.5f ? PhysicsVolume : Other.PhysicsVolume;
	Out.UpVector = UTGOR_Math::Normalize(FMath::Lerp(UpVector, Other.UpVector, Alpha));
	return Out;
}

float FTGOR_MovementCapture::Compare(const FTGOR_MovementCapture& State, float DotThres) const
{
	return (1.0 - (State.UpVector | UpVector)) / DotThres;
}

void FTGOR_MovementCapture::Send(FTGOR_NetworkWritePackage& Package, UTGOR_Singleton* Context) const
{
	Package.WriteEntry(UpVector);
}

void FTGOR_MovementCapture::Recv(FTGOR_NetworkReadPackage& Package, UTGOR_Singleton* Context)
{
	Package.ReadEntry(UpVector);
}

/*
bool FTGOR_MovementCapture::NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess)
{
	bOutSuccess = true;
	uint8 Mask;

	SERIALISE_MASK_NORM(Orientation);
	SERIALISE_MASK_NORM(UpVector);

	return bOutSuccess;
}
*/

////////////////////////////////////////////////////////////////////////////////////////////////////

FTGOR_MovementUpdate::FTGOR_MovementUpdate()
:	Timestamp(), Base(), UpVector(FVector::UpVector)
{
}

void FTGOR_MovementUpdate::Send(FTGOR_NetworkWritePackage& Package, UTGOR_Singleton* Context) const
{
	Package.WriteEntry(Timestamp);
	Package.WriteEntry(Base);
	Package.WriteEntry(UpVector);
}

void FTGOR_MovementUpdate::Recv(FTGOR_NetworkReadPackage& Package, UTGOR_Singleton* Context)
{
	Package.ReadEntry(Timestamp);
	Package.ReadEntry(Base);
	Package.ReadEntry(UpVector);
}

/*
bool FTGOR_MovementUpdate::NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess)
{
	bOutSuccess = true;

	Timestamp.NetSerialize(Ar, Map, bOutSuccess);
	Base.NetSerialize(Ar, Map, bOutSuccess);

	return bOutSuccess;
}
*/

////////////////////////////////////////////////////////////////////////////////////////////////////

FTGOR_MovementContentUpdate::FTGOR_MovementContentUpdate()
	: FTGOR_MovementUpdate(), Movement(nullptr)
{

}

void FTGOR_MovementContentUpdate::Send(FTGOR_NetworkWritePackage& Package, UTGOR_Singleton* Context) const
{
	FTGOR_MovementUpdate::Send(Package, Context);
	Package.WriteEntry(Movement);
}

void FTGOR_MovementContentUpdate::Recv(FTGOR_NetworkReadPackage& Package, UTGOR_Singleton* Context)
{
	FTGOR_MovementUpdate::Recv(Package, Context);
	Package.ReadEntry(Movement);
}

/*
bool FTGOR_MovementContentUpdate::NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess)
{
	uint8 Mask;
	SERIALISE_INIT_SOURCE;

	FTGOR_MovementUpdate::NetSerialize(Ar, Map, bOutSuccess);
	SERIALISE_MASK_SOURCE(Movement, Movement);

	return bOutSuccess;
}
*/

////////////////////////////////////////////////////////////////////////////////////////////////////

FTGOR_MovementInputUpdate::FTGOR_MovementInputUpdate()
	: FTGOR_MovementContentUpdate(), State()
{
}

FTGOR_MovementInputUpdate::FTGOR_MovementInputUpdate(const FTGOR_MovementUpdate& Other)
	: FTGOR_MovementContentUpdate(), State()
{
	FTGOR_MovementUpdate::operator=(Other);
}

void FTGOR_MovementInputUpdate::Send(FTGOR_NetworkWritePackage& Package, UTGOR_Singleton* Context) const
{
	FTGOR_MovementUpdate::Send(Package, Context);
	Package.WriteEntry(State);
}

void FTGOR_MovementInputUpdate::Recv(FTGOR_NetworkReadPackage& Package, UTGOR_Singleton* Context)
{
	FTGOR_MovementUpdate::Recv(Package, Context);
	Package.ReadEntry(State);
}

/*
bool FTGOR_MovementInputUpdate::NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess)
{
	FTGOR_MovementUpdate::NetSerialize(Ar, Map, bOutSuccess);
	State.NetSerialize(Ar, Map, bOutSuccess);

	return bOutSuccess;
}
*/

////////////////////////////////////////////////////////////////////////////////////////////////////

FTGOR_MovementVisuals::FTGOR_MovementVisuals()
	: Blend(0.0f), Radius(0.0f), Content(nullptr)
{
}
