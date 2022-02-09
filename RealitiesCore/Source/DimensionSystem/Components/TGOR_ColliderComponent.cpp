// The Gateway of Realities: Planes of Existence.
#include "TGOR_ColliderComponent.h"

#include "CoreSystem/TGOR_Singleton.h"
#include "DimensionSystem/Data/TGOR_WorldData.h"
#include "DimensionSystem/Volumes/TGOR_PhysicsVolume.h"
#include "DimensionSystem/Components/TGOR_PilotComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/PrimitiveComponent.h"
#include "DrawDebugHelpers.h"

#include "RealitiesUtility/Utility/TGOR_Math.h"

DECLARE_CYCLE_STAT(TEXT("Move"), STAT_Move, STATGROUP_COLLIDER);
DECLARE_DWORD_COUNTER_STAT(TEXT("Collision iterations"), STAT_Iteration, STATGROUP_COLLIDER);
DECLARE_DWORD_COUNTER_STAT(TEXT("Movement pullback count"), STAT_Pullback, STATGROUP_COLLIDER);
DECLARE_DWORD_COUNTER_STAT(TEXT("Movement translation count"), STAT_Translation, STATGROUP_COLLIDER);

TAutoConsoleVariable<int32> CVarArmatureOverlapDebugDrawEnable(TEXT("a.Armature.DebugOverlapDraw"), 0, TEXT("Toggle whether armature overlap are drawn."));
TAutoConsoleVariable<int32> CVarArmatureSurfaceDebugDrawEnable(TEXT("a.Armature.DebugSurfaceDraw"), 0, TEXT("Toggle whether armature surfaces are drawn."));
TAutoConsoleVariable<int32> CVarMovementTraceDebugDrawEnable(TEXT("a.Movement.DebugTraceDraw"), 0, TEXT("Toggle whether movement traces are drawn."));

UTGOR_ColliderComponent::UTGOR_ColliderComponent()
:	Super(),
IgnoreThreshold(0.4f),
IgnoreRadius(2.0f),
MaxCollisionIterations(3)
{
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
	UTGOR_PilotComponent* PilotComponent = UTGOR_PilotComponent::FindOwningPilot(Component);
	if (IsValid(PilotComponent))
	{
		if (PilotComponent->HasParent(this))
		{
			return true;
		}

		// When we're small we want to collide with skeletal mesh
		// When we're bigger we don't want to collide at all
		// When we're about the same size collide with capsule
		const FTGOR_MovementShape& Shape = PilotComponent->GetBody();
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
	ClearMoveIgnoreComponents();
	const float ProbeRadius = FMath::Max(MovementBody.Height, MovementBody.Radius);
	ProbeOverlap(ProbeRadius * IgnoreRadius, Position.Linear);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

bool UTGOR_ColliderComponent::ProbeSweep(float Radius, const FVector& Location, const FVector& Translation)
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

bool UTGOR_ColliderComponent::ProbeSweep(const FCollisionShape& CollisionShape, FTGOR_MovementPosition& Position, const FVector& Translation)
{
	AActor* const Actor = GetOwner();
	UWorld* const World = GetWorld();

	TArray<FHitResult> Hits;
	FCollisionResponseParams ResponseParam;
	FComponentQueryParams Params(SCENE_QUERY_STAT(MoveComponent), Actor);
	InitSweepCollisionParams(Params, ResponseParam);
	Params.TraceTag = TEXT("MoveComponent");

	//const FCollisionShape& CollisionShape = GetCollisionShape();
	const ECollisionChannel TraceChannel = GetCollisionObjectType();

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

bool UTGOR_ColliderComponent::MovementSphereTraceSweep(float Radius, const FVector& Location, const FVector& Translation, FHitResult& Hit) const
{
	const FCollisionShape Shape = FCollisionShape::MakeSphere(Radius);

	FTGOR_MovementPosition Position;
	Position.Linear = Location;
	Position.Angular = FQuat::Identity;

	return MovementTraceSweep(Shape, Position, Translation, Hit);
}

bool UTGOR_ColliderComponent::MovementTraceSweep(const FTGOR_MovementPosition& Position, const FVector& Translation, FHitResult& Hit) const
{
	const FCollisionShape& CollisionShape = GetCollisionShape();
	return MovementTraceSweep(CollisionShape, Position, Translation, Hit);
}

bool UTGOR_ColliderComponent::MovementTraceSweep(const FCollisionShape& CollisionShape, const FTGOR_MovementPosition& Position, const FVector& Translation, FHitResult& Hit) const
{
	Hit.bBlockingHit = false;
	Hit.Time = 1.f;

	AActor* const Actor = GetOwner();
	UWorld* const World = GetWorld();

	FCollisionResponseParams ResponseParam;
	FComponentQueryParams Params(SCENE_QUERY_STAT(MoveComponent), Actor);
	InitSweepCollisionParams(Params, ResponseParam);
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
	const ECollisionChannel TraceChannel = GetCollisionObjectType();
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
	const float MinMovementDistSq = FMath::Square(4.f * KINDA_SMALL_NUMBER);
	float DeltaSizeSq = Translation.SizeSquared();
	if (DeltaSizeSq <= MinMovementDistSq)
	{
		DeltaSizeSq = 0.f;
	}

	// static things can move before they are registered (e.g. immediately after streaming), but not after.
	if (IsPendingKill())
	{
		Hit.Init();
		return false;
	}

	ConditionalUpdateComponentToWorld();

	Hit.bBlockingHit = false;
	Hit.Time = 1.f;

	bool bFilledHitResult = false;
	bool bIncludesOverlapsAtEnd = false;
	AActor* const Actor = GetOwner();

	// Perform movement collision checking if needed for this actor.
	const bool bCollisionEnabled = IsQueryCollisionEnabled();
	if (bCollisionEnabled && (DeltaSizeSq > 0.f))
	{
		const FCollisionShape& CollisionShape = GetCollisionShape();
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

	SetWorldLocationAndRotation(Dynamic.Linear, Dynamic.Angular, false, nullptr, ETeleportType::TeleportPhysics);
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

void UTGOR_ColliderComponent::CollectSweepResults(const TArray<FHitResult>& HitResults, const FVector& Translation)
{
	for (const FHitResult& HitResult : HitResults)
	{
		UPrimitiveComponent* Component = HitResult.GetComponent();
		if (IgnoreTracedComponent(Component))
		{
			IgnoreComponentWhenMoving(Component, true);
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
	InitSweepCollisionParams(Params, ResponseParam);
	Params.TraceTag = TEXT("MoveComponent");

	TArray<FOverlapResult> Overlaps;
	const ECollisionChannel TraceChannel = GetCollisionObjectType();
	if (World->OverlapMultiByChannel(Overlaps, Position.Linear, Position.Angular, TraceChannel, CollisionShape, Params))
	{
		return FilterOverlapResults(Overlaps, Overlap);
	}
	return false;
}

bool UTGOR_ColliderComponent::ProbeOverlap(float Radius, const FVector& Location)
{
	const FCollisionShape Shape = FCollisionShape::MakeSphere(Radius);
	FTGOR_MovementPosition Position;
	Position.Linear = Location;
	Position.Angular = FQuat::Identity;
	return ProbeOverlap(Shape, Position);
}

bool UTGOR_ColliderComponent::ProbeOverlap(const FCollisionShape& CollisionShape, const FTGOR_MovementPosition& Position)
{
	AActor* const Actor = GetOwner();
	UWorld* const World = GetWorld();

	TArray<FOverlapResult> Overlaps;
	FCollisionResponseParams ResponseParam;
	FComponentQueryParams Params(SCENE_QUERY_STAT(MoveComponent), Actor);
	InitSweepCollisionParams(Params, ResponseParam);
	Params.TraceTag = TEXT("MoveComponent");

	const ECollisionChannel TraceChannel = GetCollisionObjectType();

	ResponseParam.CollisionResponse.ReplaceChannels(ECollisionResponse::ECR_Overlap, ECollisionResponse::ECR_Ignore);
	ResponseParam.CollisionResponse.ReplaceChannels(ECollisionResponse::ECR_Block, ECollisionResponse::ECR_Overlap); // Overlap all blocking (will still catch some overlap target)
	World->OverlapMultiByChannel(Overlaps, Position.Linear, Position.Angular, TraceChannel, CollisionShape, Params, ResponseParam);
	CollectOverlapResults(Overlaps);
	return true;
}

bool UTGOR_ColliderComponent::SimulateOverlap(FTGOR_MovementDynamic& Dynamic, const FVector& Translation, FOverlapResult& Overlap)
{
	const FCollisionShape& CollisionShape = GetCollisionShape(0.1f);

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

void UTGOR_ColliderComponent::CollectOverlapResults(const TArray<FOverlapResult>& OverlapResults)
{
	for (const FOverlapResult& OverlapResult : OverlapResults)
	{
		UPrimitiveComponent* Component = OverlapResult.GetComponent();
		if (IgnoreTracedComponent(Component))
		{
			IgnoreComponentWhenMoving(Component, true);
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
	return false;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void UTGOR_ColliderComponent::InflictPointImpact(const FVector& Point, const FVector& Impulse)
{
}

void UTGOR_ColliderComponent::InflictPointForce(const FVector& Point, const FVector& Force, float DeltaTime)
{
}

float UTGOR_ColliderComponent::ComputeInertial(const FVector& Point, const FVector& Normal) const
{
	const FTGOR_MovementBody& MovementBody = GetBody();
	if (MovementBody.Mass < SMALL_NUMBER)
	{
		return 0.0f;
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

////////////////////////////////////////////////////////////////////////////////////////////////////

FVector UTGOR_ColliderComponent::ComputeCollisionResponse(FTGOR_MovementSpace& Space, const FVector& Point, const FVector& RelativeVelocity, const FVector& Normal, float InvInertial)
{
	const float VerticalVelocity = (Normal | RelativeVelocity);
	const FVector Project = Normal * VerticalVelocity;
	const FVector Impulse = (1 + Body.Elasticity.Value) * Project * InvInertial;

	Impact(Space, Point, Impulse);

	// Own collision response directly on the base
	Space.AddLinearVelocity(-Body.GetUnmassedLinear(Impulse));

	if (CanRotateOnImpact())
	{
		const FVector Offset = (Point - Space.Linear) / 10'000.0f;
		Space.AddAngularVelocity(-Body.GetUnmassedAngular(Space.Angular, Offset ^ Impulse));
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
		const float Value = FMath::Min(Project.Size() * Body.Friction.Value, Limit);

		// Set final friction force
		return ComputeCollisionResponse(Space, Point, Direction * Value, Direction, InvInertial);
	}
	return FVector::ZeroVector;
}

bool UTGOR_ColliderComponent::Collide(FTGOR_MovementSpace& Space, const FHitResult& Hit)
{
	AActor* const Actor = GetOwner();
	if (Hit.bBlockingHit)
	{
		// Project onto surface normal TODO: Will not work on rotating objects because Unreal doesn't care about angular velocity
		float Inertial = ComputeInertial(Hit.ImpactPoint, Hit.ImpactNormal);
		FVector WorldVelocity = FVector::ZeroVector;
		UTGOR_ColliderComponent* Other = nullptr;
		if (Hit.Component.IsValid() && Hit.Component->GetOwner() != Actor)
		{
			Other = Cast<UTGOR_ColliderComponent>(Hit.Component->GetOwner()->GetComponentByClass(UTGOR_ColliderComponent::StaticClass()));
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
		if (Inertial >= SMALL_NUMBER)
		{
			const float InvInertial = 1.0f / Inertial;
			const FVector RelativeVelocity = (Space.LinearVelocity - WorldVelocity);
			const FVector Impulse = ComputeCollisionResponse(Space, Hit.ImpactPoint, RelativeVelocity, Hit.Normal, InvInertial);
			const FVector Slide = ComputeFrictionResponse(Space, Hit.ImpactPoint, RelativeVelocity, Hit.Normal, InvInertial);

			// Inflict if the collider is a mobility component
			if (IsValid(Other))
			{
				Other->InflictPointImpact(Hit.ImpactPoint, Impulse + Slide);
			}
		}

		return true;
	}
	return false;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void UTGOR_ColliderComponent::SimulateMove(FTGOR_MovementSpace& Space, const FTGOR_MovementPosition& Offset, float Timestep, bool Sweep)
{
	SCOPE_CYCLE_COUNTER(STAT_Move);

	// Make sure updated component is at the right location

	if (Timestep >= SMALL_NUMBER && Sweep)
	{
		INC_DWORD_STAT_BY(STAT_Iteration, 1);

		// Compute translation
		const int32 Iterations = SimulateTranslate(Space, Offset, Timestep, Sweep, 1.0f, MaxCollisionIterations);

		Space.Linear = GetComponentLocation();
		Space.Angular = GetComponentQuat();
	}
	else
	{
		Space.Linear = Offset.Linear + Space.Linear;
		Space.Angular = Offset.Angular * Space.Angular;

		// Update position
		SetWorldLocationAndRotation(Space.Linear, Space.Angular, false, nullptr, ETeleportType::TeleportPhysics);
	}
}

int32 UTGOR_ColliderComponent::SimulateTranslate(FTGOR_MovementSpace& Space, const FTGOR_MovementPosition& Offset, float Timestep, bool Sweep, float Ratio, int32 Iteration)
{
	INC_DWORD_STAT_BY(STAT_Translation, 1);

	AActor* const Actor = GetOwner();

	if (Ratio >= SMALL_NUMBER && Iteration >= 0)
	{
		const FQuat OffsetRotation = FQuat::Slerp(FQuat::Identity, Offset.Angular, Ratio);

		// Rotate base
		const FVector W = Space.RelativeAngularVelocity * (Timestep / 2);
		const FQuat DeltaQuat = FQuat(W.X, W.Y, W.Z, 0.0f);
		Space.Angular = OffsetRotation * (Space.Angular + (DeltaQuat * Ratio) * Space.Angular);
		Space.Angular.Normalize();

		// Initialise hitresult
		FHitResult Hit(NoInit);
		Hit.bBlockingHit = false;
		Hit.Time = 1.f;

		// Move and rotate component (Copied from SafeMove but without recomputing hitresult)

		// Compute translation
		const FVector Translation = (Space.RelativeLinearVelocity * Timestep + Offset.Linear) * Ratio;
		//const FCollisionShape& CollisionShape = GetCollisionShape();
		//ProbeSweep(CollisionShape, Space, Translation);

		SimulateSweep(Space, Translation, Hit);
		if (PullBack(Space, Hit, Translation))
		{
			INC_DWORD_STAT_BY(STAT_Pullback, 1);

			// Retry move after having been pulled back
			SimulateSweep(Space, Translation, Hit);
		}

		////SafeMoveUpdatedComponent(Translation, Base.Rotation, SweepsCollision, Hit, ETeleportType::None);
		//const EMoveComponentFlags IncludeBlockingOverlapsWithoutEvents = (MOVECOMP_NeverIgnoreBlockingOverlaps | MOVECOMP_DisableBlockingOverlapDispatch);
		//MoveComponent(Translation, Space.Angular, SweepsCollision && Sweep, &Hit, MOVECOMP_NoFlags, ETeleportType::None);

		if (Collide(Space, Hit))
		{
			// Unrotate base
			const float Rest = FMath::Max(Ratio - Hit.Time, 0.0f);
			Space.Angular = Space.Angular - (DeltaQuat * Rest) * Space.Angular;
			Space.Angular.Normalize();

			// Slide further along surface
			return SimulateTranslate(Space, Offset, Timestep, Sweep, Rest, Iteration - 1);
		}
	}
	return Iteration;
}

bool UTGOR_ColliderComponent::PullBack(FTGOR_MovementSpace& Space, const FHitResult& OutHit, const FVector& Translation)
{
	// Pullback mostly copied from MovementComponent::ResolvePenetrationImpl
	//Disabled for better handling of moving obstacles
	if (OutHit.bStartPenetrating)
	{
		const FVector Adjustment = GetPenetrationAdjustment(OutHit);
		if (!Adjustment.IsZero())
		{
			FOverlapResult Overlap;
			if (SimulateOverlap(Space, Adjustment - Translation, Overlap))
			{
				// Try sweeping as far as possible...
				FHitResult SweepOutHit(1.f);
				bool bMoved = SimulateSweep(Space, Adjustment, SweepOutHit);

				// Still stuck?
				if (!bMoved && SweepOutHit.bStartPenetrating)
				{
					// Combine two MTD results to get a new direction that gets out of multiple surfaces.
					const FVector SecondMTD = GetPenetrationAdjustment(SweepOutHit);
					const FVector CombinedMTD = Adjustment + SecondMTD;
					if (SecondMTD != Adjustment && !CombinedMTD.IsZero())
					{
						bMoved = SimulateSweep(Space, CombinedMTD, SweepOutHit);
					}
				}

				// Still stuck?
				if (!bMoved)
				{
					// Try moving the proposed adjustment plus the attempted move direction. This can sometimes get out of penetrations with multiple objects
					if (!Translation.IsZero())
					{
						bMoved = SimulateSweep(Space, Adjustment + Translation, SweepOutHit);

						// Finally, try the original move without MTD adjustments, but allowing depenetration along the MTD normal.
						// This was blocked because MOVECOMP_NeverIgnoreBlockingOverlaps was true for the original move to try a better depenetration normal, but we might be running in to other geometry in the attempt.
						// This won't necessarily get us all the way out of penetration, but can in some cases and does make progress in exiting the penetration.
						if (!bMoved && FVector::DotProduct(Translation, Adjustment) > 0.f)
						{
							bMoved = SimulateSweep(Space, Translation, SweepOutHit);
						}
					}
				}

				// Still stuck?
				if (!bMoved)
				{
					// Try moving the proposed adjustment plus the attempted move direction. This can sometimes get out of penetrations with multiple objects
					if (!Translation.IsZero())
					{
						bMoved = SimulateSweep(Space, Adjustment + Translation, SweepOutHit);
					}
				}
				return bMoved;
			}
			else
			{
				// Move without sweeping.
				MoveComponent(Adjustment, Space.Angular, false, nullptr, MOVECOMP_NoFlags, ETeleportType::TeleportPhysics);
				return true;
			}
		}
	}
	return false;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

bool UTGOR_ColliderComponent::CenteredTrace(const FVector& Location, const FVector& Offset, const FVector& Direction, float Distance, float Radius, FHitResult& HitOut) const
{
	UWorld* World = GetWorld();
	const FVector Start = Location + Offset;

	if (MovementSphereTraceSweep(Radius, Start, Direction * Distance, HitOut))
	{
		return true;
	}
	return false;
}

bool UTGOR_ColliderComponent::SampleTrace(const FVector& Location, const FVector& Direction, const FVector& Principal, float Distance, float Radius, FHitResult& HitOut) const
{
	if (CenteredTrace(Location, FVector::ZeroVector, Direction, Distance, Radius, HitOut))
	{
		UWorld* World = GetWorld();
		check(World && "World invalid");

		// Compute orthonormal base
		FVector Orth = Principal ^ Direction;
		if (Orth.SizeSquared() < KINDA_SMALL_NUMBER)
		{
			// Find world aligned vector to orthogonalise
			if (1.0f - Orth.X * Orth.X < KINDA_SMALL_NUMBER)
			{
				Orth = FVector::RightVector;
			}
			else
			{
				Orth = FVector::ForwardVector;
			}
		}
		const float TraceRadius = Radius / 4;
		const FVector Forward = UTGOR_Math::Normalize(Orth ^ Direction) * (Radius - TraceRadius);
		const FVector Right = Forward ^ Direction;

		bool HitAll = true;
		FHitResult HitForward, HitBackward, HitRight, HitLeft;
		HitAll = CenteredTrace(Location, Forward, Direction, Distance, TraceRadius, HitForward) && HitAll;
		HitAll = CenteredTrace(Location, -Forward, Direction, Distance, TraceRadius, HitBackward) && HitAll;
		HitAll = CenteredTrace(Location, Right, Direction, Distance, TraceRadius, HitRight) && HitAll;
		HitAll = CenteredTrace(Location, -Right, Direction, Distance, TraceRadius, HitLeft) && HitAll;

		if (HitAll)
		{
			// Compute normal from samples
			const FVector Scaled = (HitForward.Location - HitBackward.Location) ^ (HitRight.Location - HitLeft.Location);
			HitOut.ImpactNormal = UTGOR_Math::Normalize(Scaled);

			// Lerp between directions to account for ledges
			const float ForwardMatch = HitOut.ImpactNormal | HitForward.ImpactNormal;
			const float BackwardMatch = HitOut.ImpactNormal | HitBackward.ImpactNormal;
			const float RightMatch = HitOut.ImpactNormal | HitRight.ImpactNormal;
			const float LeftMatch = HitOut.ImpactNormal | HitLeft.ImpactNormal;
			const FVector Accumulate = ForwardMatch * HitForward.ImpactNormal + BackwardMatch * HitBackward.ImpactNormal + RightMatch * HitRight.ImpactNormal + LeftMatch * HitLeft.ImpactNormal;
			const float Sum = ForwardMatch + BackwardMatch + RightMatch + LeftMatch;
			if (Sum >= SMALL_NUMBER)
			{
				HitOut.Normal = UTGOR_Math::Normalize(Accumulate / Sum);
			}
			else
			{
				HitOut.Normal = HitOut.ImpactNormal;
			}

			// Compute average location
			HitOut.ImpactPoint = (HitForward.Location + HitBackward.Location + HitRight.Location + HitLeft.Location) / 4;

			// Project onto direction vector
			const FVector Diff = Location - HitOut.ImpactPoint;
			HitOut.Location = Location + Diff.ProjectOnToNormal(Direction);

			// Debug draw
			if (CVarArmatureSurfaceDebugDrawEnable.GetValueOnAnyThread() == 1)
			{
				DrawDebugPoint(World, HitForward.ImpactPoint, 4.0f, FColor::Blue, false, -1.0f, 0);
				DrawDebugPoint(World, HitBackward.ImpactPoint, 4.0f, FColor::Blue, false, -1.0f, 0);
				DrawDebugLine(World, HitForward.ImpactPoint, HitBackward.ImpactPoint, FColor::Green, false, -1.0f, 0, 2.0f);
				DrawDebugPoint(World, HitRight.ImpactPoint, 4.0f, FColor::Blue, false, -1.0f, 0);
				DrawDebugPoint(World, HitLeft.ImpactPoint, 4.0f, FColor::Blue, false, -1.0f, 0);
				DrawDebugLine(World, HitLeft.ImpactPoint, HitRight.ImpactPoint, FColor::Green, false, -1.0f, 0, 2.0f);
				DrawDebugLine(World, HitOut.ImpactPoint, HitOut.ImpactPoint + HitOut.Normal * 100.0f, FColor::Red, false, -1.0f, 0, 4.0f);
			}
		}

		return HitOut.bBlockingHit;
	}
	return false;
}

bool UTGOR_ColliderComponent::Overlap(const FVector& Location, const FQuat& Rotation, float Radius, float HalfHeight, FHitResult& HitOut) const
{
	UWorld* World = GetWorld();
	check(World);

	const float DoubleThreshold = 10.0f;

	TArray<AActor*> Ignore;
	FCollisionQueryParams TraceParams(FName(TEXT("MovementModeTrace")), true, GetOwner());
	TraceParams.bTraceComplex = false;
	TraceParams.bReturnFaceIndex = false;
	TraceParams.bReturnPhysicalMaterial = false;
	TraceParams.bIgnoreTouches = true;

	//Re-initialize hit info
	TArray<FHitResult> Hits;
	const ECollisionChannel CollisionChannel = GetCollisionObjectType();
	const FCollisionShape Shape = FCollisionShape::MakeCapsule(Radius, HalfHeight);

	if (CVarArmatureOverlapDebugDrawEnable.GetValueOnAnyThread() == 1)
	{
		DrawDebugCapsule(World, Location, HalfHeight, Radius, Rotation, FColor::Blue, false, -1.0f, 0, 3.0f);
	}

	if (World->SweepMultiByChannel(Hits, Location, Location + FVector(0.001f, 0.0f, 0.0f), Rotation, CollisionChannel, Shape, TraceParams))
	{
		// Always assume first as default
		if (FilterSweepResults(Hits, FVector::ZeroVector, HitOut))
		{
			// Compute hit plane
			const FPlane Plane = FPlane(HitOut.ImpactPoint, HitOut.Normal);

			// Sum up all points in front of the hit plane
			int32 Count = 0;
			FVector Acc = FVector::ZeroVector;
			for (const FHitResult& Hit : Hits)
			{
				if (Plane.PlaneDot(Hit.ImpactPoint) >= 0.0f)
				{
					if (CVarArmatureOverlapDebugDrawEnable.GetValueOnAnyThread() == 1)
					{
						DrawDebugPoint(World, Hit.ImpactPoint, 10.0f, FColor::Red, false, -1.0f, 0);
					}
					Acc += Hit.Normal * Hit.PenetrationDepth;
					Count += 1;
				}
			}

			// Compute average normal and depth
			if (Count > 0)
			{
				const FVector Average = Acc / Count;
				const float AverageDepth = Average.Size();
				if (AverageDepth >= SMALL_NUMBER)
				{
					HitOut.Normal = Average / AverageDepth;
				}
			}
		}
		else if (CVarArmatureOverlapDebugDrawEnable.GetValueOnAnyThread() == 1)
		{
			DrawDebugPoint(World, HitOut.ImpactPoint, 10.0f, FColor::Red, false, -1.0f, 0);
			DrawDebugLine(World, HitOut.ImpactPoint, HitOut.ImpactPoint + HitOut.Normal * 1000.0f, FColor::Yellow, false, -1.0f, 0, 6.0f);
		}

		return true;
	}
	HitOut = FHitResult(ForceInit);
	return false;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void UTGOR_ColliderComponent::VisualTrace(USceneComponent* VisualComponent, const FVector& Offset, const FVector& Direction, float Distance, float Radius, FTGOR_VisualContact& Contact) const
{
	if (IsValid(VisualComponent))
	{
		FHitResult HitOut;
		const FTransform Transform = GetComponentTransform();
		const FTransform VisualTransform = VisualComponent->GetComponentTransform();

		const FVector Location = Transform.TransformPosition(Offset);
		const FVector Normal = UTGOR_Math::Normalize(Transform.TransformVectorNoScale(Direction));
		const FVector Translation = Normal * Distance;
		if (MovementSphereTraceSweep(Radius, Location, Translation, HitOut))
		{
			Contact.Location = VisualTransform.InverseTransformPosition(HitOut.ImpactPoint);
			Contact.Normal = VisualTransform.InverseTransformVectorNoScale(HitOut.ImpactNormal);
			Contact.Blocking = HitOut.bBlockingHit;
		}
		else
		{
			Contact.Location = VisualTransform.InverseTransformPosition(Location + Translation);
			Contact.Normal = VisualTransform.InverseTransformVectorNoScale(Normal);
			Contact.Blocking = false;
		}
	}
}
