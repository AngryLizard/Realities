// The Gateway of Realities: Planes of Existence.


#include "TGOR_PipelineComponent.h"
#include "GameFramework/Actor.h"

#include "Realities/Components/Movement/TGOR_PilotComponent.h"
#include "Realities/Base/Instances/Dimension/TGOR_DimensionDataInstance.h"
#include "Realities/Base/System/Data/TGOR_WorldData.h"
#include "Realities/Mod/Pipelines/TGOR_Pipeline.h"
#include "Realities/Mod/Targets/Sockets/TGOR_PipelineSocket.h"
#include "Realities/Base/TGOR_Singleton.h"
#include "Realities/Utility/Storage/TGOR_Package.h"
#include "Realities/Actors/Dimension/Volumes/TGOR_PhysicsVolume.h"
#include "Realities/Base/Content/TGOR_ContentManager.h"

#include "DrawDebugHelpers.h"
#include "Net/UnrealNetwork.h"

SERIALISE_INIT_SOURCE_IMPLEMENT(FTGOR_PipelineAttachment)
SERIALISE_INIT_SOURCE_IMPLEMENT(FTGOR_PipelineAttachments)

void FTGOR_PipelineAttachment::Write(FTGOR_GroupWritePackage& Package, UTGOR_Singleton* Context) const
{
	Package.WriteEntry("Component", Component);
	Package.WriteEntry("Distance", Distance);
	Package.WriteEntry("Speed", Speed);
	Package.WriteEntry("Socket", Socket);
}

void FTGOR_PipelineAttachment::Read(FTGOR_GroupReadPackage& Package, UTGOR_Singleton* Context)
{
	Package.ReadEntry("Component", Component);
	Package.ReadEntry("Distance", Distance);
	Package.ReadEntry("Speed", Speed);
	Package.ReadEntry("Socket", Socket);
}

void FTGOR_PipelineAttachment::Send(FTGOR_NetworkWritePackage& Package, UTGOR_Singleton* Context) const
{
	Package.WriteEntry(Component);
	Package.WriteEntry(Distance);
	Package.WriteEntry(Speed);
	Package.WriteEntry(Socket);
}

void FTGOR_PipelineAttachment::Recv(FTGOR_NetworkReadPackage& Package, UTGOR_Singleton* Context)
{
	Package.ReadEntry(Component);
	Package.ReadEntry(Distance);
	Package.ReadEntry(Speed);
	Package.ReadEntry(Socket);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void FTGOR_PipelineAttachments::Write(FTGOR_GroupWritePackage& Package, UTGOR_Singleton* Context) const
{
	Package.WriteEntry("Parts", Parts);
}

void FTGOR_PipelineAttachments::Read(FTGOR_GroupReadPackage& Package, UTGOR_Singleton* Context)
{
	Package.ReadEntry("Parts", Parts);
}

void FTGOR_PipelineAttachments::Send(FTGOR_NetworkWritePackage& Package, UTGOR_Singleton* Context) const
{
	Package.WriteEntry(Parts);
}

void FTGOR_PipelineAttachments::Recv(FTGOR_NetworkReadPackage& Package, UTGOR_Singleton* Context)
{
	Package.ReadEntry(Parts);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

UTGOR_PipelineComponent::UTGOR_PipelineComponent()
:	Super(),
	SplineComponent(nullptr),
	SplineRadius(150.0f),
	ClosedEnd(false),
	ClosedStart(false),
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

	// Get sockets
	SINGLETON_CHK;
	UTGOR_ContentManager* ContentManager = Singleton->GetContentManager();
	for (TSubclassOf<UTGOR_PipelineSocket> SocketClass : SocketClasses)
	{
		Sockets.Append(ContentManager->GetTListFromType(SocketClass));
	}

	Super::BeginPlay();
}

void UTGOR_PipelineComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

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
}

void UTGOR_PipelineComponent::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// Replicate attachment status to everyone
	DOREPLIFETIME_CONDITION(UTGOR_PipelineComponent, Attachments, COND_None);
	/*
	DOREPLIFETIME_CONDITION(UTGOR_PipelineComponent, InitialSpeed, COND_None);
	DOREPLIFETIME_CONDITION(UTGOR_PipelineComponent, StreamSpeed, COND_None);
	DOREPLIFETIME_CONDITION(UTGOR_PipelineComponent, StreamAcceleration, COND_None);
	DOREPLIFETIME_CONDITION(UTGOR_PipelineComponent, StreamDamping, COND_None);
	DOREPLIFETIME_CONDITION(UTGOR_PipelineComponent, StreamPhysics, COND_None);
	DOREPLIFETIME_CONDITION(UTGOR_PipelineComponent, StreamExternal, COND_None);
	*/
}

USceneComponent* UTGOR_PipelineComponent::GetMovementBasis() const
{
	if (!SplineComponent.IsValid()) return Super::GetMovementBasis();
	return SplineComponent.Get();
}

/*
int32 UTGOR_PipelineComponent::GetIndexFromName(const FName& Name) const
{

}

FName UTGOR_PipelineComponent::GetNameFromIndex(int32 Index) const
{

}
*/

FTGOR_MovementDynamic UTGOR_PipelineComponent::GetIndexTransform(UTGOR_Socket* Socket, int32 Index) const
{
	if (SplineComponent.IsValid())
	{
		if (const FTGOR_PipelineAttachment* Ptr = Attachments.Parts.Find(Index))
		{
			const FQuat AngleRight(FVector::RightVector, (PI / 2));
			const FQuat AngleFlip(FVector::ForwardVector, (Ptr->Socket->Forward ? 0.0f : PI));

			FTGOR_MovementDynamic Dynamic;
			Dynamic.Linear = SplineComponent->GetLocationAtDistanceAlongSpline(Ptr->Distance, ESplineCoordinateSpace::Local);
			Dynamic.Angular = SplineComponent->GetQuaternionAtDistanceAlongSpline(Ptr->Distance, ESplineCoordinateSpace::Local) * AngleRight * AngleFlip;
			Dynamic.Angular.Normalize();

			const FVector Direction = SplineComponent->GetDirectionAtDistanceAlongSpline(Ptr->Distance, ESplineCoordinateSpace::Local);
			Dynamic.LinearVelocity = Direction * Ptr->Speed;
			return Dynamic;
		}
	}

	return FTGOR_MovementDynamic();
}

void UTGOR_PipelineComponent::InflictPointImpact(const FVector& Point, const FVector& Impulse, int32 Index)
{
	ApplyImpulseTo(Impulse * StreamPhysics, Index);
}

void UTGOR_PipelineComponent::InflictPointForce(const FVector& Point, const FVector& Force, float DeltaTime, int32 Index)
{
	ApplyImpulseTo(Force * DeltaTime * StreamPhysics, Index);
}

TSet<UTGOR_Content*> UTGOR_PipelineComponent::GetActiveContent_Implementation() const
{
	TSet<UTGOR_Content*> ContentContext = Super::GetActiveContent_Implementation();
	for (const auto Pair : Attachments.Parts)
	{
		ContentContext.Emplace(Pair.Value.Socket);
	}
	return ContentContext;
}

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

TArray<UTGOR_PilotComponent*> UTGOR_PipelineComponent::GetSocketObjects(TSubclassOf<UTGOR_Socket> Type) const
{
	TArray<UTGOR_PilotComponent*> Objects;
	for (const auto Pair : Attachments.Parts)
	{
		if (Pair.Value.Socket->IsA(Type))
		{
			Objects.Emplace(Pair.Value.Component.Get());
		}
	}
	return Objects;
}

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

	return MaxRadius;
}


float UTGOR_PipelineComponent::GetRadiusAtDistance(float Distance) const
{
	if (!SplineComponent.IsValid()) return 0.0f;
	const FVector Scale = SplineComponent->GetScaleAtDistanceAlongSpline(Distance);
	// TODO: Can we do something smarter then just using Y? Careful with FMath::Min since we interpolate this linearly in sweep collisions
	return Scale.Y * SplineRadius;
}


bool UTGOR_PipelineComponent::IsOccupied(float Distance, float Radius, float Segment) const
{
	const float ML = Distance - Segment;
	const float MR = Distance + Segment;
	if (GetRadiusAtDistance(ML) < Radius || GetRadiusAtDistance(Distance) < Radius || GetRadiusAtDistance(MR) < Radius)
	{
		return false;
	}

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

	return HasHit;
}

FTransform UTGOR_PipelineComponent::GetTransformAt(FTGOR_Serial Index, float Offset) const
{
	const FTGOR_PipelineAttachment* Ptr = Attachments.Parts.Find(Index);
	if (SplineComponent.IsValid() && Ptr)
	{
		const float Total = SplineComponent->GetSplineLength();
		const float Distance = FMath::Clamp(Ptr->Distance + (Ptr->Socket->Forward ? Offset : -Offset), 0.0f, Total);
		return SplineComponent->GetTransformAtDistanceAlongSpline(Distance, ESplineCoordinateSpace::World);
	}
	return GetComponentTransform();
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
	return CanFitToPipeline(Attachee->GetBody(), Socket) && Attachee->CanAttachTo(this);
}

void UTGOR_PipelineComponent::AttachToPipeline(UTGOR_PilotComponent* Attachee, UTGOR_PipelineSocket* Socket, ETGOR_ComputeEnumeration& Branches)
{
	if(GetOwnerRole() != ENetRole::ROLE_Authority) return;

	if (!SplineComponent.IsValid() || !IsValid(Socket)) return;
	Branches = ETGOR_ComputeEnumeration::Failed;

	// Find unique identifier
	int32 Identifier;
	do
	{
		Identifier = FMath::RandRange(0, 0xFFFF);
	} while (Attachments.Parts.Contains(Identifier));

	// Create attachment
	const float Distance = Socket->GetSplineDistance(this);
	FTGOR_PipelineAttachment Attachment;
	Attachment.Distance = Distance;
	Attachment.Component = Attachee;
	Attachment.Speed = 0.0f;
	Attachment.Socket = Socket;

	if (!SplineComponent->IsClosedLoop())
	{
		Attachment.Speed = Socket->Forward ? InitialSpeed : -InitialSpeed;
	}

	// Check validity
	const FTGOR_MovementShape& Shape = Attachee->GetBody();
	if (IsOccupied(Distance, Shape.Radius, Shape.Height / 2)) return;

	// Register on mobility for attachment
	Attachments.Parts.Add(Identifier, Attachment);
	//Attachee->RegisterHandle(this, Socket, this, Identifier);
	Attachee->AttachTo(this, Socket, Identifier);
	Branches = ETGOR_ComputeEnumeration::Success;
}


void UTGOR_PipelineComponent::DetachFromPipeline(UTGOR_PilotComponent* Attachee)
{
	if (GetOwnerRole() != ENetRole::ROLE_Authority) return;

	for (auto It = Attachments.Parts.CreateIterator(); It; ++It)
	{
		// Make sure not to keep track of already detached mobility
		if (It->Value.Component == Attachee)
		{
			//Attachee->UnregisterHandle(It->Value.Socket);
			Attachee->DetachFrom(this, It->Value.Socket, It->Key.Index);
			It.RemoveCurrent();
		}
	}
}

void UTGOR_PipelineComponent::DetachFromPipelineAt(FTGOR_Serial Index)
{
	if (GetOwnerRole() != ENetRole::ROLE_Authority) return;

	const FTGOR_PipelineAttachment* Ptr = Attachments.Parts.Find(Index);
	if (Ptr)
	{
		//Ptr->Component->UnregisterHandle(Ptr->Socket);
		Ptr->Component->DetachFrom(this, Ptr->Socket, Index.Index);
		Attachments.Parts.Remove(Index);
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void UTGOR_PipelineComponent::ApplyImpulse(const FVector& Impulse)
{
	for (const auto& Pair : Attachments.Parts)
	{
		ApplyImpulseTo(Impulse, Pair.Key);
	}
}

void UTGOR_PipelineComponent::ApplyLineImpulse(float Impulse)
{
	for (const auto& Pair : Attachments.Parts)
	{
		ApplyLineImpulseTo(Impulse, Pair.Key);
	}
}
 
void UTGOR_PipelineComponent::ApplyImpulseTo(const FVector& Impulse, FTGOR_Serial Index)
{
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
}

void UTGOR_PipelineComponent::ApplyLineImpulseTo(float Impulse, FTGOR_Serial Index)
{
	FTGOR_PipelineAttachment* Ptr = Attachments.Parts.Find(Index);
	if (Ptr)
	{
		const FTGOR_MovementBody& OurBody = Ptr->Component->GetBody();
		const float Mass = FMath::Max(OurBody.Mass, 0.0001f);
		const float SpeedDelta = Impulse / Mass;
		Ptr->Speed += SpeedDelta;
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////

USplineComponent* UTGOR_PipelineComponent::GetSpline() const
{
	return SplineComponent.Get();
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void UTGOR_PipelineComponent::RepNotifyAttachments()
{
	for (const auto Pair : Attachments.Parts)
	{
		// Force-attach if current attachment doesn't match
		if (!Pair.Value.Component->IsAttachedTo(this, Pair.Value.Socket, Pair.Key.Index))
		{
			// Make sure no other handles with this parent exist
			//Pair.Value.Component->UnregisterHandleWith(this);
			//Pair.Value.Component->RegisterHandle(this, Pair.Value.Socket, this, Pair.Key.Index);
			Pair.Value.Component->AttachTo(this, Pair.Value.Socket, Pair.Key.Index);
		}
	}
}
