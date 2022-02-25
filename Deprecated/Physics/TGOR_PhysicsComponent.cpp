// The Gateway of Realities: Planes of Existence.


#include "TGOR_PhysicsComponent.h"





UTGOR_PhysicsComponent::UTGOR_PhysicsComponent()
	: Super()
{
	CurrentVelocity = FVector::ZeroVector;
	CurrentSpin = FVector::ZeroVector;

	PendingCoeff = 0.0f;
	PendingFreedom = FVector::ZeroVector;
	PendingHitOffset = FVector::ZeroVector;
	PendingWeight = 0;

	DirtyInertiaTensor = true;
	DirtyTotalMass = true;

	InertiaTensor = FTGOR_InertiaTensor(FVector::OneVector);
	InertiaInverse = FTGOR_InertiaTensor(FVector::OneVector);
}


void UTGOR_PhysicsComponent::BeginningPlay()
{
	// Get Ragdoll spheres
	const TArray<USceneComponent*>& Components = GetAttachChildren();
	for (USceneComponent* Component : Components)
	{
		UClass* Class = Component->GetClass();
		if (Class->ImplementsInterface(UTGOR_PhysicsObjectInterface::StaticClass()))
		{
			TScriptInterface<ITGOR_PhysicsObjectInterface> Interface = Component;
			PhysicsComponents.Add(Interface);
			Interface->SetParent(this);
		}
	}

	CurrentLocation = GetComponentLocation();
	CurrentRotation = GetComponentQuat();
}

FVector UTGOR_PhysicsComponent::ApplyMove(float DeltaTime, const FVector& External, const TArray<AActor*>& Ignore)
{
	// Move with parent on non-restricted axis
	const FVector Restriction = ComputeRestrictionDelta();
	CurrentLocation = Restriction + CurrentLocation;

	// Apply external and internal forces
	ApplyForces(External, DeltaTime);

	// Compute change in location
	const FVector LocationDelta = ComputeLocationDelta(DeltaTime);
		
	// Execute move
	CurrentLocation = LocationDelta + CurrentLocation;
	SetWorldLocation(CurrentLocation);
		
	if (Parent)
	{
		Parent->InvalidateInertiaTensor();
	}

	// Compute change in rotation
	const FQuat RotationDelta = ComputeRotationDelta(DeltaTime);

	// Execute move
	CurrentRotation = RotationDelta * CurrentRotation;
	SetWorldRotation(CurrentRotation);

	if (Parent)
	{
		Parent->InvalidateInertiaTensor();
	}
	
	// Apply to all children
	FVector Correction = FVector(0.0f, 0.0f, 0.0f);
	for (const auto& PhysicsComponent : PhysicsComponents)
	{
		FVector ComponentCorrection = PhysicsComponent->ApplyMove(DeltaTime, External, Ignore);
		
		// Get biggest correction
		Correction.X = (FMath::Abs(ComponentCorrection.X) > FMath::Abs(Correction.X)) ? ComponentCorrection.X : Correction.X;
		Correction.Y = (FMath::Abs(ComponentCorrection.Y) > FMath::Abs(Correction.Y)) ? ComponentCorrection.Y : Correction.Y;
		Correction.Z = (FMath::Abs(ComponentCorrection.Z) > FMath::Abs(Correction.Z)) ? ComponentCorrection.Z : Correction.Z;
	}

	// Resolve any pending impulses
	if (PendingWeight > SMALL_NUMBER)
	{
		const float Coeff = PendingCoeff / PendingWeight;
		const FVector Freedom = PendingFreedom / PendingWeight;
		FVector HitOffset = PendingHitOffset / PendingWeight;
		
		ApplyForce(Coeff, Freedom, HitOffset, 0.0f);
	}
	
	// Reset pending impulses
	PendingCoeff = 0.0f;
	PendingFreedom = FVector::ZeroVector;
	PendingHitOffset = FVector::ZeroVector;
	PendingWeight = 0.0f;
	

	// Recompute inertia tensor if necessary
	ComputeLocalInertiaTensor();
	
	// Compute correction
	FVector RestCorrection = FreedomProject(Correction, FVector::ZeroVector);
	Correction = Correction - RestCorrection;
	
	// Apply correction
	AddWorldOffset(RestCorrection);
	Move(RestCorrection);

	return(Correction);
}

void UTGOR_PhysicsComponent::SetParent(TScriptInterface<ITGOR_PhysicsObjectInterface> ParentInterface)
{
	Parent = ParentInterface;
}


FTGOR_InertiaTensor UTGOR_PhysicsComponent::ComputeInertiaTensor()
{
	ComputeLocalInertiaTensor();

	// Transform InertiaTensor
	const FTransform& Transform = GetRelativeTransform();
	return(ComputeTransform(Transform, InertiaTensor));
}

void UTGOR_PhysicsComponent::InvalidateInertiaTensor()
{
	// Assume parents are already dirty as well if I am
	if (!DirtyInertiaTensor)
	{
		DirtyInertiaTensor = true;
		if (Parent)
		{
			Parent->InvalidateInertiaTensor();
		}
	}
}

float UTGOR_PhysicsComponent::ComputeTotalMass()
{
	if (DirtyTotalMass)
	{
		TotalMass = 0.0f;
		for (const auto& PhysicsComponent : PhysicsComponents)
		{
			TotalMass += PhysicsComponent->ComputeTotalMass();
		}
	}

	DirtyTotalMass = false;
	return(TotalMass);
}

void UTGOR_PhysicsComponent::InvalidateTotalMass()
{
	if (!DirtyTotalMass)
	{
		DirtyTotalMass = true;
		if (Parent)
		{
			Parent->InvalidateTotalMass();
		}

		InvalidateInertiaTensor();
	}

}


FVector UTGOR_PhysicsComponent::GetOffset() const
{
	return(GetRelativeLocation());
}

void UTGOR_PhysicsComponent::MoveOffset(const FVector& Delta)
{
	SetRelativeLocation( GetRelativeLocation() + Delta);
}

FVector UTGOR_PhysicsComponent::GetLocation() const
{
	return(CurrentLocation);
}

void UTGOR_PhysicsComponent::Move(const FVector& Delta)
{
	CurrentLocation += Delta;

	for (const auto& PhysicsComponent : PhysicsComponents)
	{
		PhysicsComponent->Move(Delta);
	}
}

void UTGOR_PhysicsComponent::ResolveSingle(FVector& PointVelocity, float Coeff, const FVector& HitNormal, const FVector& HitLocation, float Collective)
{
	// Get offset
	const FVector Location = GetLocation();
	const FVector HitOffset = HitLocation - Location;

	// Compute freedom
	FVector Freedom = FreedomProject(HitNormal, HitOffset);
	
	// Resolve via parent if available and retreive point velocity
	if (Parent)
	{
		// Get rest normal to resolve by parent
		const FVector RestNormal = HitNormal - FVector::DotProduct(HitNormal, Freedom) * Freedom;

		// Transform to parent space
		Parent->ResolveSingle(PointVelocity, Coeff, RestNormal, HitLocation, Collective);
	}
	else
	{
		// If root, assume non-moving world
		PointVelocity = FVector(0.0f, 0.0f, 0.0f);
	}

	// Update PointVelocity towards surface (to return to caller)
	PointVelocity += FreedomProject(CurrentVelocity, HitOffset);

	const FVector Angular = FVector::CrossProduct(CurrentSpin, HitOffset);
	PointVelocity += Angular;

	// Apply local impulse if available
	if (Freedom.SizeSquared() > SMALL_NUMBER)
	{
		const float Dot = FVector::DotProduct(PointVelocity, Freedom);
		ApplyForce(Coeff * Dot, Freedom, HitOffset, Collective);
	}
}

void UTGOR_PhysicsComponent::ApplySingle(float Coeff, const FVector& HitNormal, const FVector& HitLocation, float Collective)
{
	// Get offset
	const FVector Location = GetLocation();
	const FVector HitOffset = HitLocation - Location;

	// Compute freedom
	FVector Freedom = FreedomProject(HitNormal, HitOffset);

	// Resolve via parent if available and retreive point velocity
	if (Parent)
	{
		// Get rest normal to resolve by parent
		const FVector RestNormal = HitNormal - FVector::DotProduct(HitNormal, Freedom) * Freedom;

		// Transform to parent space
		Parent->ApplySingle(Coeff, RestNormal, HitLocation, Collective);
	}

	// Apply local impulse if available
	if (Freedom.SizeSquared() > SMALL_NUMBER)
	{
		ApplyForce(Coeff, Freedom, HitOffset, Collective);
	}
}

void UTGOR_PhysicsComponent::ApplyForce(float Coeff, const FVector& Freedom, const FVector& HitOffset, float Collective)
{
	if (Collective > SMALL_NUMBER)
	{
		// Store pending forces
		PendingCoeff += Coeff * Collective;
		PendingFreedom += Freedom * Collective;
		PendingHitOffset += HitOffset * Collective;
		PendingWeight += Collective;
	}
	else
	{
		// Compute moment (Transform to local and back for inertiatensor)
		const FTransform& Transform = GetComponentTransform();
		const FVector Cross = FVector::CrossProduct(HitOffset, Freedom);
		const FVector LocalCross = Transform.InverseTransformVector(Cross);
		const FVector LocalInertia = InertiaInverse * LocalCross;
		const FVector Inertia = Transform.TransformVector(LocalInertia);

		// Compute angle
		const FVector Direction = FVector::CrossProduct(Inertia, HitOffset);
		const float Ang = FVector::DotProduct(Direction, Freedom);

		// Compute divisor
		const float Mass = ComputeTotalMass();
		const float Divisor = (1.0f / Mass + Ang);

		// Compute counter velocity
		const float ReactionalImpulse = Coeff / Divisor;
		const FVector ReactionImpulse = Freedom * (ReactionalImpulse / Mass);

		// Apply impulse
		CurrentVelocity = CurrentVelocity - ReactionImpulse;
		CurrentSpin = CurrentSpin - ReactionalImpulse * Inertia;
	}
}



FVector UTGOR_PhysicsComponent::FreedomProject(const FVector& Vector, const FVector& Offset) const
{
	// Zerovector for rigid body
	return(FVector::ZeroVector);
}


FVector UTGOR_PhysicsComponent::ComputeInnerForce()
{
	return(FVector::ZeroVector);
}


FVector UTGOR_PhysicsComponent::ComputeInnerSpin()
{
	return(FVector::ZeroVector);
}


void UTGOR_PhysicsComponent::UpdateStructure()
{
}

void UTGOR_PhysicsComponent::ApplyForces(const FVector& External, float DeltaTime)
{
	// Apply external force
	CurrentVelocity += External * DeltaTime;

	// Compute inner acceleration
	const FVector Force = ComputeInnerForce();
	const FVector Inner = FreedomProject(Force, FVector::ZeroVector);
	const float Acc = Inner.Size();
	if (Acc > SMALL_NUMBER)
	{
		// Change velocity
		const FVector Acceleration = Inner / ComputeTotalMass();
		CurrentVelocity += Acceleration * DeltaTime;

		// Apply reaction force to parent
		if (Parent)
		{
			const FVector Normal = Inner / Acc;
			Parent->ApplySingle(Acc * DeltaTime, Normal, CurrentLocation, false);
		}
	}
}
FVector UTGOR_PhysicsComponent::ComputeRestrictionDelta()
{
	// Move with parent in restricted direction
	const FVector Location = GetComponentLocation();
	const FVector Diff = Location - CurrentLocation;
	const FVector Restriction = Diff - FreedomProject(Diff, FVector::ZeroVector);

	return(Restriction);
}

FVector UTGOR_PhysicsComponent::ComputeLocationDelta(float DeltaTime)
{

	// Move with own velocity in freedom direction
	const FVector Freedom = FreedomProject(CurrentVelocity, FVector::ZeroVector);
	const FVector Force = Freedom - CurrentVelocity; // todo: Implement counter force
	CurrentVelocity = Freedom;

	return(CurrentVelocity * DeltaTime);
}

FQuat UTGOR_PhysicsComponent::ComputeRotationDelta(float DeltaTime)
{
	// Compute axis and angle
	const FVector Delta = (CurrentSpin + ComputeInnerSpin()) * DeltaTime;
	const FVector Axis = Delta;//AxisProject(Delta);
	const float Angle = Axis.Size();

	// If at all rotating, Normalize axis
	if (Angle > SMALL_NUMBER)
	{
		const FVector RotAxis = Axis / Angle;
		return(FQuat(RotAxis, Angle));
	}

	// otherwise define zerorotation
	return(FQuat::Identity);
}

FTGOR_InertiaTensor UTGOR_PhysicsComponent::ComputeLocalInertiaTensor()
{
	if (DirtyInertiaTensor)
	{
		// Store current moment
		FVector Moment = InertiaTensor * CurrentSpin;

		// Update structure in case masses change
		UpdateStructure();

		// Reset inertia tensor
		InertiaTensor = FTGOR_InertiaTensor(FVector::ZeroVector);
		for (const auto& PhysicsComponent : PhysicsComponents)
		{
			const FTGOR_InertiaTensor Local = PhysicsComponent->ComputeInertiaTensor();

			// Parallel axis theorem
			const float Mass = PhysicsComponent->ComputeTotalMass();
			const FVector Offset = PhysicsComponent->GetOffset();

			// Add to tensors
			InertiaTensor += Local + ComputeParallelAxisTheorem(Offset, Mass);
		}

		// Inverse
		InertiaInverse = InertiaTensor.Inverse();

		// Change spin to reflect changes
		CurrentSpin = InertiaInverse * Moment;
	}

	DirtyInertiaTensor = false;
	return(InertiaTensor);
}

FTGOR_InertiaTensor UTGOR_PhysicsComponent::ComputeParallelAxisTheorem(const FVector& Offset, float Mass)
{
	const float XY = -Mass * Offset.X * Offset.Y;
	const float XZ = -Mass * Offset.X * Offset.Z;
	const float YZ = -Mass * Offset.Y * Offset.Z;

	const float XX = Offset.X * Offset.X;
	const float YY = Offset.Y * Offset.Y;
	const float ZZ = Offset.Z * Offset.Z;

	const float DX = Mass * (YY + ZZ);
	const float DY = Mass * (XX + ZZ);
	const float DZ = Mass * (XX + YY);

	FTGOR_InertiaTensor Parallel = FTGOR_InertiaTensor(
		FVector(DX, XY, XZ),
		FVector(XY, DY, YZ),
		FVector(XZ, YZ, DZ));

	return(Parallel);
}


FTGOR_InertiaTensor UTGOR_PhysicsComponent::ComputeTransform(const FTransform& Transform, const FTGOR_InertiaTensor& Tensor)
{
	// Be S: Scale and R: Rotation
	// T/S
	const FVector Scale = Transform.GetScale3D();
	const FTGOR_InertiaTensor InvScaled = Tensor.InvScale(Scale);
	
	// (T/S)*R'
	const FVector RowX = Transform.TransformVectorNoScale(InvScaled.X);
	const FVector RowY = Transform.TransformVectorNoScale(InvScaled.Y);
	const FVector RowZ = Transform.TransformVectorNoScale(InvScaled.Z);

	// F * (T/S)*R'
	const FVector ColX = Transform.TransformVector(FVector(RowX.X, RowY.X, RowZ.X));
	const FVector ColY = Transform.TransformVector(FVector(RowX.Y, RowY.Y, RowZ.Y));
	const FVector ColZ = Transform.TransformVector(FVector(RowX.Z, RowY.Z, RowZ.Z));

	// Write in row-format
	return(FTGOR_InertiaTensor(
		FVector(ColX.X, ColY.X, ColZ.X),
		FVector(ColX.Y, ColY.Y, ColZ.Y),
		FVector(ColX.Z, ColY.Z, ColZ.Z)));
}