// The Gateway of Realities: Planes of Existence.


#include "TGOR_PipelineComponent.h"
#include "GameFramework/Actor.h"

#include "DimensionSystem/Components/TGOR_PilotComponent.h"
#include "SocketSystem/Tasks/TGOR_PipelineSocketTask.h"
#include "SocketSystem/Content/TGOR_PipelineSocket.h"
#include "CoreSystem/TGOR_Singleton.h"
#include "CoreSystem/Storage/TGOR_Package.h"

#include "DrawDebugHelpers.h"
#include "Net/UnrealNetwork.h"

////////////////////////////////////////////////////////////////////////////////////////////////////

UTGOR_PipelineComponent::UTGOR_PipelineComponent()
:	Super(),
	SplineComponent(nullptr),
	SplineRadius(150.0f),
	ClosedEnd(false),
	ClosedStart(false),
	StreamForward(true),
	InitialSpeed(100.0f),
	StreamSpeed(0.0f),
	StreamAcceleration(100.0f),
	StreamDamping(1.0f),
	StreamPhysics(1.0f),
	StreamExternal(FVector::ZeroVector),
	CollisionElasticity(0.5f)
{
	SetIsReplicatedByDefault(true);
}

void UTGOR_PipelineComponent::BeginPlay()
{
	AActor* Actor = GetOwner();
	SplineComponent = Actor->FindComponentByClass<USplineComponent>();
	
	Super::BeginPlay();
}

void UTGOR_PipelineComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	/*
	SINGLETON_CHK;
	UTGOR_WorldData* WorldData = Singleton->GetWorldData();
	ATGOR_PhysicsVolume* MainVolume = WorldData->GetMainVolume();

	if (SplineComponent.IsValid())
	{
		const float TotalDistance = SplineComponent->GetSplineLength();
		for (auto It = Attachments.Parts.CreateIterator(); It; ++It)
		{
			// Make sure not to keep track of already detached mobility
			if (!It->Value.Component->IsAttachedTo(this, It->Value.Socket, It->Key.Index))
			{
				if (GetOwnerRole() == ENetRole::ROLE_Authority)
				{
					It.RemoveCurrent();
				}
			}
			else
			{
				const FVector Direction = SplineComponent->GetDirectionAtDistanceAlongSpline(It->Value.Distance, ESplineCoordinateSpace::World);

				const FTGOR_MovementBody& OurBody = It->Value.Component->GetBody();
				const float Mass = FMath::Max(OurBody.Mass, 0.0001f);

				// Compute damping, clamp to prevent oscillation
				const float DampingDelta = It->Value.Speed * StreamDamping * DeltaTime;
				const float MinSpeed = FMath::Min(-It->Value.Speed, It->Value.Speed);
				const float MaxSpeed = FMath::Max(-It->Value.Speed, It->Value.Speed);
				It->Value.Speed -= FMath::Clamp(DampingDelta, MinSpeed, MaxSpeed);

				// Accelerate towards stream speed
				It->Value.Speed = FMath::FInterpConstantTo(It->Value.Speed, StreamSpeed, DeltaTime, StreamAcceleration);

				// Apply external forces
				It->Value.Speed += (StreamExternal | Direction) / Mass * DeltaTime;

				// Move along spline
				FTGOR_PipelineHitResult HitResult;
				const float Delta = It->Value.Speed * DeltaTime;
				if (Sweep(It->Value.Distance, OurBody.Radius, OurBody.Height/2, Delta, It->Key, HitResult))
				{
					// Move only as fas as we can
					It->Value.Distance += Delta + HitResult.Normal * HitResult.Depth;

					// Check whether we hit another attachment or the pipeline itself
					if (HitResult.Index)
					{
						FTGOR_PipelineAttachment* Ptr = Attachments.Parts.Find(HitResult.Index);
						if (Ptr)
						{
							const FTGOR_MovementBody TheirBody = It->Value.Component->GetBody();
							const float TheirMass = FMath::Max(TheirBody.Mass, 0.0001f);

							// Inelastic collision equation
							const float TotalMass = Mass + TheirMass;
							const float Impulse = Mass * It->Value.Speed + TheirMass * Ptr->Speed;
							It->Value.Speed = (CollisionElasticity * TheirMass * (Ptr->Speed - It->Value.Speed) + Impulse) / TotalMass;
							Ptr->Speed = (CollisionElasticity * Mass * (It->Value.Speed - Ptr->Speed) + Impulse) / TotalMass;
						}
					}
					else
					{
						It->Value.Speed *= -1;
					}
				}
				else
				{
					It->Value.Distance += Delta;
				}

				// Handle edge cases
				if (SplineComponent->IsClosedLoop())
				{
					while (It->Value.Distance > TotalDistance) It->Value.Distance -= TotalDistance;
					while (It->Value.Distance < 0.0f) It->Value.Distance += TotalDistance;
				}
				else if (It->Value.Distance > TotalDistance)
				{
					if (ClosedEnd)
					{
						It->Value.Distance = TotalDistance;
						It->Value.Speed = (It->Value.Speed > 0.0f ? -It->Value.Speed : It->Value.Speed);
						OnHitEnd.Broadcast(It->Value.Component.Get());
					}
					else if (GetOwnerRole() == ENetRole::ROLE_Authority)
					{
						UTGOR_PilotComponent* Attachee = It->Value.Component.Get();
						//Attachee->UnregisterHandle(It->Value.Socket);
						Attachee->DetachFrom(this, It->Value.Socket, It->Key.Index);
						It.RemoveCurrent();
						OnHitEnd.Broadcast(Attachee);
					}
				}
				else if (It->Value.Distance < 0.0f)
				{
					if (ClosedStart)
					{
						It->Value.Distance = 0.0f;
						It->Value.Speed = (It->Value.Speed < 0.0f ? -It->Value.Speed : It->Value.Speed);
						OnHitStart.Broadcast(It->Value.Component.Get());
					}
					else if (GetOwnerRole() == ENetRole::ROLE_Authority)
					{
						UTGOR_PilotComponent* Attachee = It->Value.Component.Get();
						//Attachee->UnregisterHandle(It->Value.Socket);
						Attachee->DetachFrom(this, It->Value.Socket, It->Key.Index);
						It.RemoveCurrent();
						OnHitStart.Broadcast(Attachee);
					}
				}
			}
		}
	}
	*/
}

void UTGOR_PipelineComponent::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// Replicate attachment status to everyone
	/*
	DOREPLIFETIME_CONDITION(UTGOR_PipelineComponent, InitialSpeed, COND_None);
	DOREPLIFETIME_CONDITION(UTGOR_PipelineComponent, StreamSpeed, COND_None);
	DOREPLIFETIME_CONDITION(UTGOR_PipelineComponent, StreamAcceleration, COND_None);
	DOREPLIFETIME_CONDITION(UTGOR_PipelineComponent, StreamDamping, COND_None);
	DOREPLIFETIME_CONDITION(UTGOR_PipelineComponent, StreamPhysics, COND_None);
	DOREPLIFETIME_CONDITION(UTGOR_PipelineComponent, StreamExternal, COND_None);
	*/
}

/*
int32 UTGOR_PipelineComponent::GetIndexFromName(const FName& Name) const
{

}

FName UTGOR_PipelineComponent::GetNameFromIndex(int32 Index) const
{

}
*/

/*
void UTGOR_PipelineComponent::InflictPointImpact(const FVector& Point, const FVector& Impulse)
{
	ApplyImpulseTo(Impulse * StreamPhysics, INDEX_NONE);
}

void UTGOR_PipelineComponent::InflictPointForce(const FVector& Point, const FVector& Force, float DeltaTime)
{
	ApplyImpulseTo(Force * DeltaTime * StreamPhysics, INDEX_NONE);
}
*/

/*
bool UTGOR_PipelineComponent::QuerySocketObjects(UTGOR_PilotComponent* Component, FTGOR_MovementVisuals& Visuals) const
{
	const int32 Last = GetLastSplinePoint();
	for (const auto Pair : Attachments.Parts)
	{
		if (Pair.Value.Component == Component)
		{
			const float StartDelta = GetDistanceAt(1);
			const float EndDelta = GetDistanceAt(Last) - GetDistanceAt(Last - 1);
			if (StartDelta >= SMALL_NUMBER && EndDelta >= SMALL_NUMBER)
			{
				const float StartRatio = ClosedStart ? (Pair.Value.Distance / StartDelta) : 1.0f;
				const float EndRatio = ClosedEnd ? ((GetDistanceAt(Last) - Pair.Value.Distance) / EndDelta) : 1.0f;
				Visuals.Blend = 1.0f - FMath::Min3(StartRatio, EndRatio, 1.0f);
				Visuals.Radius = GetRadiusAtDistance(Pair.Value.Distance);
				Visuals.Content = Pair.Value.Socket;
				return true;
			}
			return false;
		}
	}
	return false;
}
*/

/*
TArray<UTGOR_PilotComponent*> UTGOR_PipelineComponent::GetSocketObjects(TSubclassOf<UTGOR_Socket> Type) const
{
	TArray<UTGOR_PilotComponent*> Objects;
	for (const TWeakObjectPtr<UTGOR_PipelineSocketTask>& Attachment : Attachments)
	{
		Objects.Emplace(Attachment->PilotComponent);
	}
	return Objects;
}
*/

////////////////////////////////////////////////////////////////////////////////////////////////////

/*
float UTGOR_PipelineComponent::GetDistanceNearestTo(const FVector& Location) const
{
	if (!IsValid(SplineComponent)) return 0.0f;
	const int32 Segments = SplineComponent->GetNumberOfSplineSegments();
	const float InKey = SplineComponent->FindInputKeyClosestToWorldLocation(Location);
	
	// Get segment closest to location
	const float	Ratio = FMath::Frac(InKey);
	const int32 Start = FMath::FloorToInt(InKey);
	const float StartDistance = SplineComponent->GetDistanceAlongSplineAtSplinePoint(Start);
	if (Ratio < SMALL_NUMBER)
	{
		return StartDistance;
	}

	// Get Distance on a segment
	const int32 End = (Start + 1) % Segments;
	const float EndDistance = SplineComponent->GetDistanceAlongSplineAtSplinePoint(End);
	return FMath::Lerp(StartDistance, EndDistance, Ratio);
}
*/

FTGOR_MovementDynamic UTGOR_PipelineComponent::GetPipelineDynamic(float Distance, float Speed, bool Forward) const
{
	if (SplineComponent.IsValid())
	{
		const FQuat AngleRight(FVector::RightVector, (PI / 2));
		const FQuat AngleFlip(FVector::ForwardVector, (Forward ? 0.0f : PI));

		FTGOR_MovementDynamic Dynamic;
		Dynamic.Linear = SplineComponent->GetLocationAtDistanceAlongSpline(Distance, ESplineCoordinateSpace::Local);
		Dynamic.Angular = SplineComponent->GetQuaternionAtDistanceAlongSpline(Distance, ESplineCoordinateSpace::Local) * AngleRight * AngleFlip;
		Dynamic.Angular.Normalize();

		const FVector Direction = SplineComponent->GetDirectionAtDistanceAlongSpline(Distance, ESplineCoordinateSpace::Local);
		Dynamic.LinearVelocity = Direction * Speed;
		return Dynamic;
	}

	return FTGOR_MovementDynamic();
}

float UTGOR_PipelineComponent::GetDistanceAt(int32 SplinePoint) const
{
	if (!SplineComponent.IsValid()) return 0.0f;
	return SplineComponent->GetDistanceAlongSplineAtSplinePoint(SplinePoint);
}

int32 UTGOR_PipelineComponent::GetLastSplinePoint() const
{
	return SplineComponent->GetNumberOfSplineSegments();
}

float UTGOR_PipelineComponent::GetDisplacementAtDistance(float Distance, float Falloff) const
{
	float MaxRadius = 0.0f;
	// TODO:
	/*
	for (const auto Pair : Attachments.Parts)
	{
		const FTGOR_MovementShape& Shape = Pair.Value.Component->GetBody();

		const float ML = Distance - Falloff;
		const float MR = Distance + Falloff;
		const float TL = Pair.Value.Distance - Shape.Height / 2;
		const float TR = Pair.Value.Distance + Shape.Height / 2;

		if ((MR > TL) && (ML < TR))
		{
			// Compute distance to border for falloff
			const float Dist = FMath::Min(FMath::Abs(MR - TL), FMath::Abs(TR - ML));
			if (Falloff >= SMALL_NUMBER)
			{
				const float Radius = FMath::Min(Dist / Falloff, 1.0f) * Shape.Radius;
				MaxRadius = FMath::Max(MaxRadius, Radius);
			}
			else
			{
				MaxRadius = FMath::Max(MaxRadius, Shape.Radius);
			}
		}
	}
	*/

	return MaxRadius;
}


float UTGOR_PipelineComponent::GetRadiusAtDistance(float Distance) const
{
	if (!SplineComponent.IsValid()) return 0.0f;
	const FVector Scale = SplineComponent->GetScaleAtDistanceAlongSpline(Distance);
	// TODO: Can we do something smarter then just using Y? Careful with FMath::Min since we interpolate this linearly in sweep collisions
	return Scale.Y * SplineRadius;
}

bool UTGOR_PipelineComponent::IsClosedLoop() const
{
	if (!SplineComponent.IsValid()) return false;
	return SplineComponent->IsClosedLoop();
}

bool UTGOR_PipelineComponent::IsOccupied(float Distance, float Radius, float Segment) const
{
	const float ML = Distance - Segment;
	const float MR = Distance + Segment;
	if (GetRadiusAtDistance(ML) < Radius || GetRadiusAtDistance(Distance) < Radius || GetRadiusAtDistance(MR) < Radius)
	{
		return false;
	}

	// TODO:
	/*
	for (const auto Pair : Attachments.Parts)
	{
		const FTGOR_MovementShape& Shape = Pair.Value.Component->GetBody();

		const float TL = Pair.Value.Distance - Shape.Height / 2;
		const float TR = Pair.Value.Distance + Shape.Height / 2;

		if ((MR > TL) && (ML < TR))
		{
			return true;
		}
	}
	*/

	return false;
}


bool UTGOR_PipelineComponent::Sweep(float Distance, float Radius, float Segment, float Delta, FTGOR_Serial Ignore, FTGOR_PipelineHitResult& HitResult) const
{
	HitResult.Depth = 0.0f;
	HitResult.Distance = Distance;
	HitResult.Index = INDEX_NONE;
	HitResult.Normal = 0.0f;

	if (!SplineComponent.IsValid()) return false;
	bool HasHit = false;
	
	// Collide with low radius segment
	const float RadiusBefore = GetRadiusAtDistance(Distance);
	const float RadiusAfter = GetRadiusAtDistance(Distance + Delta);
	if (RadiusBefore < Radius)
	{
		// Only collide if we aren't opening up (in case we got stuck)
		if (RadiusAfter <= RadiusBefore)
		{
			HasHit = true;
			// Small offset to make sure floating point precision errors don't allow for penetration
			HitResult.Depth = FMath::Abs(Delta) + SMALL_NUMBER;
			HitResult.Normal = -FMath::Sign(Delta);
		}
	}
	else if (RadiusBefore != RadiusAfter)
	{
		// Check whether we collide along the way
		const float Ratio = (Radius - RadiusAfter) / (RadiusBefore - RadiusAfter);
		if (0.0f <= Ratio && Ratio <= 1.0f)
		{
			HasHit = true;
			HitResult.Depth = FMath::Abs(Delta * (1.0f - Ratio));
			HitResult.Normal = -FMath::Sign(Delta);
		}
	}

	// TODO:
	/*
	// Collide with other attachments
	for (const auto Pair : Attachments.Parts)
	{
		if (Pair.Key != Ignore)
		{
			const FTGOR_MovementShape& Shape = Pair.Value.Component->GetBody();

			const float ML = Distance - Segment;
			const float MR = Distance + Segment;
			const float TL = Pair.Value.Distance - Shape.Height / 2;
			const float TR = Pair.Value.Distance + Shape.Height / 2;

			if (Delta > 0.0f && Distance <= Pair.Value.Distance && MR - TL > HitResult.Depth)
			{
				HitResult.Depth = MR - TL;
				HitResult.Distance = TL;
				HitResult.Index = Pair.Key;
				HitResult.Normal = -1.0f;
				HasHit = true;
			}

			if (Delta < 0.0f && Distance >= Pair.Value.Distance && TR - ML > HitResult.Depth)
			{
				HitResult.Depth = TR - ML;
				HitResult.Distance = TR;
				HitResult.Index = Pair.Key;
				HitResult.Normal = 1.0f;
				HasHit = true;
			}
		}
	}
	*/
	return HasHit;
}


void UTGOR_PipelineComponent::DrawSplineRadius(float Interval)
{
	if (!SplineComponent.IsValid()) return;

	const float Total = SplineComponent->GetSplineLength();
	const int32 Segments = FMath::FloorToInt(Total / Interval);
	for (int32 Segment = 0; Segment < Segments; Segment++)
	{
		const float Distance = Segment * Interval;
		const float Radius = GetRadiusAtDistance(Distance);
		const FTransform Transform = SplineComponent->GetTransformAtDistanceAlongSpline(Distance, ESplineCoordinateSpace::World, true);
		DrawDebugCircle(GetWorld(), Transform.GetLocation(), Radius, 32, FColor::Red, false, -1.0f, 0, 5.0f, Transform.GetUnitAxis(EAxis::Y), Transform.GetUnitAxis(EAxis::Z), false);
	}

	const float Radius = GetRadiusAtDistance(Total);
	const FTransform Transform = SplineComponent->GetTransformAtDistanceAlongSpline(Total, ESplineCoordinateSpace::World, true);
	DrawDebugCircle(GetWorld(), Transform.GetLocation(), Radius, 32, FColor::Red, false, -1.0f, 0, 5.0f, Transform.GetUnitAxis(EAxis::Y), Transform.GetUnitAxis(EAxis::Z), false);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

FVector UTGOR_PipelineComponent::GetViewLocation(float Distance, float Direction, float Offset) const
{
	if (!SplineComponent.IsValid()) return FVector::ZeroVector;

	const int32 Last = GetLastSplinePoint();
	const float TotalDistance = GetDistanceAt(Last);

	const float Position = Distance - Direction * Offset;
	const float Pivot = FMath::Clamp(Position, 0.0f, TotalDistance);
	const float Delta = Position - Pivot;

	const FVector Normal = SplineComponent->GetDirectionAtDistanceAlongSpline(Pivot, ESplineCoordinateSpace::World);
	const FVector Location = SplineComponent->GetLocationAtDistanceAlongSpline(Pivot, ESplineCoordinateSpace::World);
	return Location + Normal * Delta;
}

float UTGOR_PipelineComponent::GetAccelerationAtDistance(float Distance, float Speed, float Direction) const
{
	if (!SplineComponent.IsValid()) return 0.0f;

	// Compute damping, clamp to prevent oscillation
	const float DampingDelta = Speed * StreamDamping;
	const float MinSpeed = FMath::Min(-Speed, Speed);
	const float MaxSpeed = FMath::Max(-Speed, Speed);
	const float Damping = FMath::Clamp(DampingDelta, MinSpeed, MaxSpeed);

	// Accelerate towards stream speed
	const float TargetSpeed = (StreamForward ? (StreamSpeed * Direction) : StreamSpeed);
	const float Stream = (TargetSpeed - Speed) * StreamAcceleration;

	const FVector Normal = SplineComponent->GetDirectionAtDistanceAlongSpline(Distance, ESplineCoordinateSpace::World);
	const float External = StreamExternal | Normal;

	return External + Stream - Damping;
}

float UTGOR_PipelineComponent::GetDistanceFromEnd(float Distance, float Threshold) const
{
	if (!SplineComponent.IsValid()) return Threshold;
	if (ClosedEnd)
	{
		const float TotalDistance = SplineComponent->GetSplineLength();
		return FMath::Clamp(TotalDistance - Distance, 0.0f, Threshold);
	}
	return Threshold;
}

float UTGOR_PipelineComponent::GetDistanceFromStart(float Distance, float Threshold) const
{
	if (!SplineComponent.IsValid()) return Threshold;
	if (ClosedStart)
	{
		return FMath::Clamp(Distance, 0.0f, Threshold);
	}
	return Threshold;
}

bool UTGOR_PipelineComponent::CollideWithEnd(UTGOR_PilotComponent* Attachee)
{
	OnHitEnd.Broadcast(Attachee);
	return ClosedEnd;
}

bool UTGOR_PipelineComponent::CollideWithStart(UTGOR_PilotComponent* Attachee)
{
	OnHitStart.Broadcast(Attachee);
	return ClosedStart;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

FTGOR_MovementPosition UTGOR_PipelineComponent::GetTransformOfPipeline(UTGOR_PipelineSocket* Socket) const
{
	if (IsValid(Socket) && SplineComponent.IsValid())
	{
		const FQuat AngleRight(FVector::RightVector, (PI / 2));
		const FQuat AngleFlip(FVector::ForwardVector, (Socket->Forward ? 0.0f : PI));

		float Distance = Socket->GetSplineDistance(this);

		FTGOR_MovementPosition Position;
		Position.Linear = SplineComponent->GetLocationAtDistanceAlongSpline(Distance, ESplineCoordinateSpace::World);
		Position.Angular = SplineComponent->GetQuaternionAtDistanceAlongSpline(Distance, ESplineCoordinateSpace::World) * AngleRight * AngleFlip;
		Position.Angular.Normalize();

		return Position;
	}
	return ComputePosition();
}

bool UTGOR_PipelineComponent::CanFitToPipeline(const FTGOR_MovementShape& Shape, UTGOR_PipelineSocket* Socket) const
{
	const float Distance = Socket->GetSplineDistance(this);
	return !IsOccupied(Distance, Shape.Radius, Shape.Height / 2);
}

bool UTGOR_PipelineComponent::CanAttachToPipeline(UTGOR_PilotComponent* Attachee, UTGOR_PipelineSocket* Socket) const
{
	return CanFitToPipeline(Attachee->GetBody(), Socket) && Attachee->CanParent(this);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

bool UTGOR_PipelineComponent::ParentPipelineSocket(UTGOR_PilotComponent* Attachee, UTGOR_PipelineSocket* Socket)
{
	if (IsValid(Attachee))
	{
		UTGOR_PipelineSocketTask* Task = Attachee->GetPOfType<UTGOR_PipelineSocketTask>();
		if (IsValid(Task) && Task->CanParent(this, Socket))
		{
			const FTGOR_MovementSpace Space = Attachee->ComputeSpace();
			Task->Parent(this, Socket);

			// TODO: Possibly make this a function inside task
			Task->State.SocketDirection = Socket->Forward ? 1.0f : -1.0;
			Task->State.SocketDistance = Socket->GetSplineDistance(this);
			Task->State.SocketSpeed = Task->State.SocketDirection * InitialSpeed;

			Attachee->AttachWith(Task->Identifier.Slot);
			return true;
		}
	}
	return false;
}

bool UTGOR_PipelineComponent::CanParentPipelineSocket(UTGOR_PilotComponent* Attachee, UTGOR_PipelineSocket* Socket) const
{
	if (IsValid(Attachee))
	{
		UTGOR_PipelineSocketTask* Task = Attachee->GetPOfType<UTGOR_PipelineSocketTask>();
		return IsValid(Task) && Task->CanParent(this, Socket);
	}
	return false;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void UTGOR_PipelineComponent::RegisterAttachToSocket(UTGOR_PipelineSocketTask* Attachee)
{
	Attachments.AddUnique(Attachee);
}

void UTGOR_PipelineComponent::RegisterDetachFromSocket(UTGOR_PipelineSocketTask* Attachee)
{
	Attachments.Remove(Attachee);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void UTGOR_PipelineComponent::ApplyImpulse(const FVector& Impulse)
{
	// TODO
/**
	for (const auto& Pair : Attachments.Parts)
	{
		ApplyImpulseTo(Impulse, Pair.Key);
	}
	*/
}

void UTGOR_PipelineComponent::ApplyLineImpulse(float Impulse)
{
	// TODO
/**
	for (const auto& Pair : Attachments.Parts)
	{
		ApplyLineImpulseTo(Impulse, Pair.Key);
	}
	*/
}
 
void UTGOR_PipelineComponent::ApplyImpulseTo(const FVector& Impulse, FTGOR_Serial Index)
{
	// TODO
/**
	if (!SplineComponent.IsValid()) return;
	FTGOR_PipelineAttachment* Ptr = Attachments.Parts.Find(Index);
	if (Ptr)
	{
		const FVector Direction = SplineComponent->GetDirectionAtDistanceAlongSpline(Ptr->Distance, ESplineCoordinateSpace::World);
		const FTGOR_MovementBody& OurBody = Ptr->Component->GetBody();
		const float Mass = FMath::Max(OurBody.Mass, 0.0001f);
		const FVector VelocityDelta = Impulse / Mass;
		Ptr->Speed += VelocityDelta | Direction;
	}
	*/
}

void UTGOR_PipelineComponent::ApplyLineImpulseTo(float Impulse, FTGOR_Serial Index)
{
	// TODO
/**
	FTGOR_PipelineAttachment* Ptr = Attachments.Parts.Find(Index);
	if (Ptr)
	{
		const FTGOR_MovementBody& OurBody = Ptr->Component->GetBody();
		const float Mass = FMath::Max(OurBody.Mass, 0.0001f);
		const float SpeedDelta = Impulse / Mass;
		Ptr->Speed += SpeedDelta;
	}
	*/
}

////////////////////////////////////////////////////////////////////////////////////////////////////

USplineComponent* UTGOR_PipelineComponent::GetSpline() const
{
	return SplineComponent.Get();
}

////////////////////////////////////////////////////////////////////////////////////////////////////
