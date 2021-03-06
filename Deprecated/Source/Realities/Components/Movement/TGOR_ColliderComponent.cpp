// The Gateway of Realities: Planes of Existence.
#include "TGOR_ColliderComponent.h"

#include "Realities/Base/TGOR_Singleton.h"
#include "Realities/Base/System/Data/TGOR_WorldData.h"
#include "Realities/Actors/Dimension/Volumes/TGOR_PhysicsVolume.h"

#include "Components/SkeletalMeshComponent.h"
#include "Components/PrimitiveComponent.h"
#include "Net/UnrealNetwork.h"
#include "DrawDebugHelpers.h"


TAutoConsoleVariable<int32> CVarMovementTraceDebugDrawEnable(TEXT("a.Movement.DebugTraceDraw"), 0, TEXT("Toggle whether movement traces are drawn."));

UTGOR_ColliderComponent::UTGOR_ColliderComponent()
:	Super(),
IgnoreThreshold(0.4f),
IgnoreRadius(2.0f),
Elasticity(0.2f),
Friction(0.1f)
{
}


void UTGOR_ColliderComponent::SetBaseComponent(USceneComponent* Component)
{
	BasePrimitiveComponent = Cast<UPrimitiveComponent>(Component);
	Super::SetBaseComponent(Component);
}

float UTGOR_ColliderComponent::ComputeInertial(const FVector& Point, const FVector& Normal) const
{
	const FTGOR_MovementBody& MovementBody = GetBody();
	if (MovementBody.Mass < SMALL_NUMBER)
	{
		return Super::ComputeInertial(Point, Normal);
	}

	float Inertial = 1.0f / MovementBody.Mass;
	if (CanRotateOnImpact())
	{
		const FTGOR_MovementPosition Position = ComputePosition();
		const FVector Offset = (Point - Position.Linear) / 10'000.0f;

		const FVector Torque = MovementBody.GetUnmassedAngular(Position.Angular, Offset ^ Normal) ^ Offset;
		Inertial += (Torque | Normal);
	}
	return Inertial;
}

bool UTGOR_ColliderComponent::IgnoreTracedComponent(UPrimitiveComponent* Component) const
{
	if (!IsValid(Component)) return true;
	AActor* Other = Component->GetOwner();
	if (!IsValid(Other)) return true;

	// Don't collide with attached children
	AActor* Actor = GetOwner();
	if (Other->IsBasedOnActor(Actor)) return true;

	// Special behaviour if the other is a movement tangible object
	UTGOR_MobilityComponent* MobilityComponent = Other->FindComponentByClass<UTGOR_MobilityComponent>();
	if (IsValid(MobilityComponent))
	{
		// When we're small we want to collide with skeletal mesh
		// When we're bigger we don't want to collide at all
		// When we're about the same size collide with capsule
		const FTGOR_MovementShape& Shape = MobilityComponent->GetBody();
		const float Threshold = IgnoreThreshold * IgnoreThreshold * IgnoreThreshold;

		/*
		if (Shape.Height > SMALL_NUMBER && Shape.Height < 440.0f)
		{
			RPORT(Other->GetName() + " " + FString::SanitizeFloat(Shape.Height) + " vs " + Actor->GetName() + " " + FString::SanitizeFloat(Frame.Height));
		}
		*/

		const FTGOR_MovementBody& MovementBody = GetBody();
		if (Shape.Volume < MovementBody.Volume * Threshold)
		{
			return true;
		}
		else if (Shape.Volume * Threshold >= MovementBody.Volume)
		{
			// When colliding with giant pawns we collide with skeletal meshes only
			// TODO: Some pawns could have static meshes we want to collide with
			if (Other->IsA(APawn::StaticClass()))
			{
				return !Component->IsA(USkeletalMeshComponent::StaticClass());
			}
		}

		// Ignore skeletal meshes if not interacting with giants
		return Component->IsA(USkeletalMeshComponent::StaticClass());
	}
	return false;
}

void UTGOR_ColliderComponent::OverlapIgnoreList()
{
	const FTGOR_MovementBody& MovementBody = GetBody();
	const FTGOR_MovementPosition Position = ComputePosition();

	// Recompute move ignore list
	BasePrimitiveComponent->ClearMoveIgnoreComponents();
	const float ProbeRadius = FMath::Max(MovementBody.Height, MovementBody.Radius);
	ProbeOverlap(ProbeRadius * IgnoreRadius, Position.Linear);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

bool UTGOR_ColliderComponent::ProbeSweep(float Radius, const FVector& Location, const FVector& Translation) const
{
	const FCollisionShape Shape = FCollisionShape::MakeSphere(Radius);
	FTGOR_MovementPosition Position;
	Position.Linear = Location;
	Position.Angular = FQuat::Identity;
	//DrawDebugLine(GetWorld(), Position.Linear, Position.Linear + Translation, FColor::Red, false, -1.0f, 0, 5.0f);
	//DrawDebugSphere(GetWorld(), Position.Linear, Radius, 30, FColor::Red, false, -1.0f, 0, 5.0f);
	//DrawDebugSphere(GetWorld(), Position.Linear + Translation, Radius, 30, FColor::Red, false, -1.0f, 0, 5.0f);

	return ProbeSweep(Shape, Position, Translation);
}

bool UTGOR_ColliderComponent::ProbeSweep(const FCollisionShape& CollisionShape, FTGOR_MovementPosition& Position, const FVector& Translation) const
{
	if (!BasePrimitiveComponent.IsValid()) return false;
	AActor* const Actor = GetOwner();
	UWorld* const World = GetWorld();

	TArray<FHitResult> Hits;
	FCollisionResponseParams ResponseParam;
	FComponentQueryParams Params(SCENE_QUERY_STAT(MoveComponent), Actor);
	BasePrimitiveComponent->InitSweepCollisionParams(Params, ResponseParam);
	Params.TraceTag = TEXT("MoveComponent");

	//const FCollisionShape& CollisionShape = BasePrimitiveComponent->GetCollisionShape();
	const ECollisionChannel TraceChannel = BasePrimitiveComponent->GetCollisionObjectType();

	ResponseParam.CollisionResponse.ReplaceChannels(ECollisionResponse::ECR_Overlap, ECollisionResponse::ECR_Ignore);
	ResponseParam.CollisionResponse.ReplaceChannels(ECollisionResponse::ECR_Block, ECollisionResponse::ECR_Overlap); // Overlap all blocking (will still catch some overlap target)
	World->SweepMultiByChannel(Hits, Position.Linear, Position.Linear + Translation, Position.Angular, TraceChannel, CollisionShape, Params, ResponseParam); //ProbeChannel ECollisionChannel::ECC_Visibility
	if (Hits.Num() > 0)
	{
		CollectSweepResults(Hits, Translation);
		return true;
	}
	return false;
}


bool UTGOR_ColliderComponent::MovementTraceSweep(float Radius, const FVector& Location, const FVector& Translation, FHitResult& Hit) const
{
	UWorld* const World = GetWorld();
	const FCollisionShape Shape = FCollisionShape::MakeSphere(Radius);

	FTGOR_MovementPosition Position;
	Position.Linear = Location;
	Position.Angular = FQuat::Identity;

	return MovementTraceSweep(Shape, Position, Translation, Hit);
}

bool UTGOR_ColliderComponent::MovementTraceSweep(const FCollisionShape& CollisionShape, const FTGOR_MovementPosition& Position, const FVector& Translation, FHitResult& Hit) const
{
	if (!BasePrimitiveComponent.IsValid()) return false;

	Hit.bBlockingHit = false;
	Hit.Time = 1.f;

	AActor* const Actor = GetOwner();
	UWorld* const World = GetWorld();

	FCollisionResponseParams ResponseParam;
	FComponentQueryParams Params(SCENE_QUERY_STAT(MoveComponent), Actor);
	BasePrimitiveComponent->InitSweepCollisionParams(Params, ResponseParam);
	Params.TraceTag = TEXT("MoveComponent");


	if (CVarMovementTraceDebugDrawEnable.GetValueOnAnyThread() == 1)
	{
		const FVector End = Position.Linear + Translation;
		const float Radius = CollisionShape.GetExtent().GetMax();
		DrawDebugSphere(World, Position.Linear, Radius, 1, FColor::Red, false, -1.0f, 0, 1.0f);
		DrawDebugLine(World, Position.Linear, End, FColor::Red, false, -1.0f, 0, 4.0f);
		DrawDebugSphere(World, End, Radius, 1, FColor::Blue, false, -1.0f, 0, 1.0f);
	}

	// If we had a valid blocking hit, store it.
	TArray<FHitResult> Hits;
	const FVector Target = Position.Linear + Translation;
	const ECollisionChannel TraceChannel = BasePrimitiveComponent->GetCollisionObjectType();
	if (World->SweepMultiByChannel(Hits, Position.Linear, Target, Position.Angular, TraceChannel, CollisionShape, Params, ResponseParam))
	{
		const float Radius = CollisionShape.GetExtent().GetMax();
		const bool Blocking = FilterSweepResults(Hits, Translation, Hit);
		if (CVarMovementTraceDebugDrawEnable.GetValueOnAnyThread() == 1)
		{
			DrawDebugSphere(World, Hit.Location, Radius, 1, FColor::Yellow, false, -1.0f, 0, 1.0f);
		}
		return Blocking;
	}
	return false;
}


bool UTGOR_ColliderComponent::SimulateSweep(FTGOR_MovementDynamic& Dynamic, const FVector& Translation, FHitResult& Hit)
{
	if (!BasePrimitiveComponent.IsValid()) return false;

	const float MinMovementDistSq = FMath::Square(4.f * KINDA_SMALL_NUMBER);
	float DeltaSizeSq = Translation.SizeSquared();
	if (DeltaSizeSq <= MinMovementDistSq)
	{
		DeltaSizeSq = 0.f;
	}

	// static things can move before they are registered (e.g. immediately after streaming), but not after.
	if (BasePrimitiveComponent->IsPendingKill())
	{
		Hit.Init();
		return false;
	}

	BasePrimitiveComponent->ConditionalUpdateComponentToWorld();

	Hit.bBlockingHit = false;
	Hit.Time = 1.f;

	bool bFilledHitResult = false;
	bool bIncludesOverlapsAtEnd = false;
	AActor* const Actor = BasePrimitiveComponent->GetOwner();

	// Perform movement collision checking if needed for this actor.
	const bool bCollisionEnabled = BasePrimitiveComponent->IsQueryCollisionEnabled();
	if (bCollisionEnabled && (DeltaSizeSq > 0.f))
	{
		const FCollisionShape& CollisionShape = BasePrimitiveComponent->GetCollisionShape();
		bFilledHitResult = MovementTraceSweep(CollisionShape, Dynamic, Translation, Hit);

		// Update NewLocation based on the hit result
		if (Hit.bBlockingHit)
		{
			const FVector Delta = Hit.Time * Translation;
			if (Delta.SizeSquared() > MinMovementDistSq)
			{
				Dynamic.Linear += Delta;
			}
		}
		else
		{
			Dynamic.Linear += Translation;
		}
	}
	else if (DeltaSizeSq > 0.f)
	{
		// apply move delta even if components has collisions disabled
		Dynamic.Linear += Translation;
	}

	BasePrimitiveComponent->SetWorldLocationAndRotation(Dynamic.Linear, Dynamic.Angular, false, nullptr, ETeleportType::TeleportPhysics);
	if (!bFilledHitResult)
	{
		Hit.Init(Dynamic.Linear, Dynamic.Linear + Translation);
	}

	// Return whether we moved at all.
	return true;
}

bool UTGOR_ColliderComponent::FilterSweepResults(const TArray<FHitResult>& HitResults, const FVector& Translation, FHitResult& Hit) const
{
	AActor* const Actor = GetOwner();
	int32 BlockingHitIndex = INDEX_NONE;
	float BlockingHitNormalDotDelta = BIG_NUMBER;
	for (int32 HitIdx = 0; HitIdx < HitResults.Num(); HitIdx++)
	{
		const FHitResult& TestHit = HitResults[HitIdx];
		if (TestHit.bBlockingHit)
		{
			// If we started penetrating, we may want to ignore it if we are moving out of penetration.
			if ((TestHit.Distance < 0.0f || TestHit.bStartPenetrating))
			{
				if ((TestHit.ImpactNormal | Translation) > 0.0f)
				{
					continue;
				}
			}

			if (TestHit.Time == 0.f)
			{
				// We may have multiple initial hits, and want to choose the one with the normal most opposed to our movement.
				const float NormalDotDelta = (TestHit.ImpactNormal | Translation);
				if (NormalDotDelta < BlockingHitNormalDotDelta)
				{
					BlockingHitNormalDotDelta = NormalDotDelta;
					BlockingHitIndex = HitIdx;
				}
			}
			else if (BlockingHitIndex == INDEX_NONE)
			{
				// First non-overlapping blocking hit should be used, if an overlapping hit was not.
				// This should be the only non-overlapping blocking hit, and last in the results.
				BlockingHitIndex = HitIdx;
				break;
			}
		}
	}

	// Update blocking hit, if there was a valid one.
	if (BlockingHitIndex >= 0)
	{
		Hit = HitResults[BlockingHitIndex];

		Hit.bBlockingHit = true;
		return true;
	}
	return false;
}

void UTGOR_ColliderComponent::CollectSweepResults(const TArray<FHitResult>& HitResults, const FVector& Translation) const
{
	for (const FHitResult& HitResult : HitResults)
	{
		UPrimitiveComponent* Component = HitResult.GetComponent();
		if (IgnoreTracedComponent(Component))
		{
			BasePrimitiveComponent->IgnoreComponentWhenMoving(Component, true);
		}
	}
}

bool UTGOR_ColliderComponent::MovementTraceOverlap(float Radius, const FVector& Location, FOverlapResult& Overlap) const
{
	const FCollisionShape Shape = FCollisionShape::MakeSphere(Radius);
	FTGOR_MovementPosition Position;
	Position.Linear = Location;
	Position.Angular = FQuat::Identity;

	return MovementTraceOverlap(Shape, Position, Overlap);
}

bool UTGOR_ColliderComponent::MovementTraceOverlap(const FCollisionShape& CollisionShape, const FTGOR_MovementPosition& Position, FOverlapResult& Overlap) const
{
	Overlap.bBlockingHit = false;

	AActor* const Actor = GetOwner();
	UWorld* const World = GetWorld();

	FCollisionResponseParams ResponseParam;
	FComponentQueryParams Params(SCENE_QUERY_STAT(MoveComponent), Actor);
	BasePrimitiveComponent->InitSweepCollisionParams(Params, ResponseParam);
	Params.TraceTag = TEXT("MoveComponent");

	TArray<FOverlapResult> Overlaps;
	const ECollisionChannel TraceChannel = BasePrimitiveComponent->GetCollisionObjectType();
	if (World->OverlapMultiByChannel(Overlaps, Position.Linear, Position.Angular, TraceChannel, CollisionShape, Params))
	{
		return FilterOverlapResults(Overlaps, Overlap);
	}
	return false;
}

bool UTGOR_ColliderComponent::ProbeOverlap(float Radius, const FVector& Location) const
{
	const FCollisionShape Shape = FCollisionShape::MakeSphere(Radius);
	FTGOR_MovementPosition Position;
	Position.Linear = Location;
	Position.Angular = FQuat::Identity;
	return ProbeOverlap(Shape, Position);
}

bool UTGOR_ColliderComponent::ProbeOverlap(const FCollisionShape& CollisionShape, FTGOR_MovementPosition& Position) const
{
	if (!BasePrimitiveComponent.IsValid()) return false;
	AActor* const Actor = GetOwner();
	UWorld* const World = GetWorld();

	TArray<FOverlapResult> Overlaps;
	FCollisionResponseParams ResponseParam;
	FComponentQueryParams Params(SCENE_QUERY_STAT(MoveComponent), Actor);
	BasePrimitiveComponent->InitSweepCollisionParams(Params, ResponseParam);
	Params.TraceTag = TEXT("MoveComponent");

	const ECollisionChannel TraceChannel = BasePrimitiveComponent->GetCollisionObjectType();

	ResponseParam.CollisionResponse.ReplaceChannels(ECollisionResponse::ECR_Overlap, ECollisionResponse::ECR_Ignore);
	ResponseParam.CollisionResponse.ReplaceChannels(ECollisionResponse::ECR_Block, ECollisionResponse::ECR_Overlap); // Overlap all blocking (will still catch some overlap target)
	World->OverlapMultiByChannel(Overlaps, Position.Linear, Position.Angular, TraceChannel, CollisionShape, Params, ResponseParam);
	CollectOverlapResults(Overlaps);
	return true;
}

bool UTGOR_ColliderComponent::SimulateOverlap(FTGOR_MovementDynamic& Dynamic, const FVector& Translation, FOverlapResult& Overlap)
{
	const FCollisionShape& CollisionShape = BasePrimitiveComponent->GetCollisionShape(0.1f);

	FTGOR_MovementPosition Position = Dynamic;
	Position.Linear += Translation;

	return MovementTraceOverlap(CollisionShape, Position, Overlap);
}

bool UTGOR_ColliderComponent::FilterOverlapResults(const TArray<FOverlapResult>& OverlapResults, FOverlapResult& Overlap) const
{
	for (const FOverlapResult& OverlapResult : OverlapResults)
	{
		if (!IgnoreTracedComponent(OverlapResult.GetComponent()))
		{
			Overlap = OverlapResult;
			return true;
		}
	}
	return false;
}

void UTGOR_ColliderComponent::CollectOverlapResults(const TArray<FOverlapResult>& OverlapResults) const
{
	for (const FOverlapResult& OverlapResult : OverlapResults)
	{
		UPrimitiveComponent* Component = OverlapResult.GetComponent();
		if (IgnoreTracedComponent(Component))
		{
			BasePrimitiveComponent->IgnoreComponentWhenMoving(Component, true);
		}
	}
}

FVector UTGOR_ColliderComponent::GetPenetrationAdjustment(const FHitResult& Hit) const
{
	const float PullBackDistance = 0.125f;
	const float PenetrationDepth = (Hit.PenetrationDepth > 0.f ? Hit.PenetrationDepth : 0.125f);

	return Hit.Normal * (PenetrationDepth + PullBackDistance);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void UTGOR_ColliderComponent::Impact(const FTGOR_MovementDynamic& Dynamic, const FVector& Point, const FVector& Impact)
{
	OnImpact.Broadcast(Dynamic, Point, Impact);
}

bool UTGOR_ColliderComponent::CanRotateOnImpact() const
{
	return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

FVector UTGOR_ColliderComponent::ComputeCollisionResponse(FTGOR_MovementSpace& Space, const FVector& Point, const FVector& RelativeVelocity, const FVector& Normal, float InvInertial)
{
	const float VerticalVelocity = (Normal | RelativeVelocity);
	const FVector Project = Normal * VerticalVelocity;
	const FVector Impulse = (1 + Elasticity) * Project * InvInertial;

	Impact(Space, Point, Impulse);
	const FTGOR_MovementBody& MovementBody = GetBody();

	// Own collision response directly on the base
	Space.AddLinearVelocity(-MovementBody.GetUnmassedLinear(Impulse));

	if (CanRotateOnImpact())
	{
		const FVector Offset = (Point - Space.Linear) / 10'000.0f;
		Space.AddAngularVelocity(-MovementBody.GetUnmassedAngular(Space.Angular, Offset ^ Impulse));
	}

	return Impulse;
}

FVector UTGOR_ColliderComponent::ComputeFrictionResponse(FTGOR_MovementSpace& Space, const FVector& Point, const FVector& RelativeVelocity, const FVector& Normal, float InvInertial)
{
	// Get slide vector
	const float VerticalVelocity = (Normal | RelativeVelocity);
	const FVector Project = Normal * VerticalVelocity;
	const FVector Slide = RelativeVelocity - Project;
	const float Limit = Slide.Size();
	if (Limit >= SMALL_NUMBER)
	{
		// Compute slide direction
		const FVector Direction = Slide / Limit;

		// Compute friction force (depends on downward force aka project), limit to prevent going backwards
		const float Value = FMath::Min(Project.Size() * Friction, Limit);

		// Set final friction force
		return ComputeCollisionResponse(Space, Point, Direction * Value, Direction, InvInertial);
	}
	return FVector::ZeroVector;
}

bool UTGOR_ColliderComponent::Collide(FTGOR_MovementSpace& Space, const FHitResult& Hit, bool Replay)
{
	AActor* const Actor = GetOwner();
	if (Hit.bBlockingHit)
	{
		// Project onto surface normal TODO: Will not work on rotating objects because Unreal doesn't care about angular velocity
		float Inertial = ComputeInertial(Hit.ImpactPoint, Hit.ImpactNormal);
		FVector WorldVelocity = FVector::ZeroVector;
		UTGOR_MobilityComponent* Other = nullptr;
		if (Hit.Actor.IsValid() && Hit.Actor != Actor)
		{
			Other = Cast<UTGOR_MobilityComponent>(Hit.Actor->GetComponentByClass(UTGOR_MobilityComponent::StaticClass()));
			if (IsValid(Other))
			{
				const FTGOR_MovementSpace MovementSpace = Other->ComputeSpace();
				WorldVelocity = MovementSpace.PointVelocity(Hit.ImpactPoint);
				Inertial += Other->ComputeInertial(Hit.ImpactPoint, Hit.ImpactNormal);
			}
			else
			{
				WorldVelocity = Hit.Component->GetComponentVelocity();
			}
		}

		// Apply collision and friction response (don't apply impulse while replaying)
		if (Inertial >= SMALL_NUMBER && !Replay)
		{
			const float InvInertial = 1.0f / Inertial;
			const FVector RelativeVelocity = (Space.LinearVelocity - WorldVelocity);
			const FVector Impulse = ComputeCollisionResponse(Space, Hit.ImpactPoint, RelativeVelocity, Hit.Normal, InvInertial);
			const FVector Slide = ComputeFrictionResponse(Space, Hit.ImpactPoint, RelativeVelocity, Hit.Normal, InvInertial);

			// Inflict if the collider is a mobility component
			if (IsValid(Other))
			{
				Other->InflictPointImpact(Hit.ImpactPoint, Impulse + Slide, INDEX_NONE);
			}
		}

		return true;
	}
	return false;
}