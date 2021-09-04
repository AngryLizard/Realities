// The Gateway of Realities: Planes of Existence.


#include "TGOR_NpcController.h"

#include "CoreSystem/TGOR_Singleton.h"
#include "CoreSystem/Gameplay/TGOR_GameInstance.h"
#include "CreatureSystem/Actors/TGOR_Pawn.h"
#include "MovementSystem/Components/TGOR_MovementComponent.h"
#include "AISystem/Content/TGOR_Behaviour.h"

#include "Net/UnrealNetwork.h"
#include "DrawDebugHelpers.h"

#define LOCTEXT_NAMESPACE "NpcController"


bool CheckCloseness(const TArray<FVector>& Points, const FVector& Query, float Distance)
{
	for (const FVector Point : Points)
	{
		if ((Point - Query).SizeSquared() < Distance * Distance)
		{
			return true;
		}
	}
	return false;
}

bool Merge(TArray<TArray<FVector>>& Chunks, const FVector& Query, float Distance)
{
	int32 Merger = -1;

	for (auto Its = Chunks.CreateIterator(); Its; ++Its)
	{
		if (CheckCloseness(*Its, Query, Distance))
		{
			if (Chunks.IsValidIndex(Merger))
			{
				// Merge into previous
				Chunks[Merger].Append(*Its);
				Its.RemoveCurrent();
			}
			else
			{
				// Add point and start merging with other close chunks
				Its->Emplace(Query);
				Merger = Its.GetIndex();
			}
		}
	}
	return Chunks.IsValidIndex(Merger);
}

TArray<FTGOR_Samples> FTGOR_Perception::CreateSamples(const FVector& Project, float Threshold, float Distance) const
{
	TArray<FTGOR_Samples> Samples;

	// Merge close enough points
	TArray<TArray<FVector>> Chunks;
	for (const FTGOR_PerceptionPoint& Point : Points)
	{
		if ((Point.Normal | Project) < Threshold)
		{
			if (!Merge(Chunks, Point.Point, Distance))
			{
				TArray<FVector> Chunk;
				Chunk.Emplace(Point.Point);
				Chunks.Emplace(Chunk);
			}
		}
	}

	// Use average for point
	for (const TArray<FVector>& Chunk : Chunks)
	{
		Samples.Emplace(FTGOR_Samples(Chunk));
	}
	return Samples;
}

FTGOR_Memory::FTGOR_Memory()
{
}

ATGOR_NpcController::ATGOR_NpcController(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer),
	PerceptionUpdateTime(0.5f),
	PerceptionTimeout(10.0f),
	TrackingTrailLength(16),
	Behaviour(nullptr),
	PrevInput(FVector::ZeroVector),
	PrevView(FQuat::Identity),
	NextInput(FVector::ZeroVector),
	NextView(FQuat::Identity)
{
	
}

void ATGOR_NpcController::BeginPlay()
{
	Super::BeginPlay();

	SINGLETON_CHK;
	const FTGOR_Time Timestamp = Singleton->GetGameTimestamp();
	LastPerceptionUpdate = Timestamp;
}


void ATGOR_NpcController::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
}

void ATGOR_NpcController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	SINGLETON_CHK;
	const FTGOR_Time GameTimestamp = Singleton->GetGameTimestamp();
	for (auto PercsIts = Memory.Perceptions.CreateIterator(); PercsIts; ++PercsIts)
	{
		for (auto PointIts = PercsIts->Value.Points.CreateIterator(); PointIts; ++PointIts)
		{
			if ((GameTimestamp - PointIts->Timestamp) > PerceptionTimeout)
			{
				PointIts.RemoveCurrent();
			}
		}

		if (!IsValid(PercsIts->Key) || PercsIts->Value.Points.Num() == 0)
		{
			PercsIts.RemoveCurrent();
		}
	}
	

	APawn* ControlledPawn = GetPawn();
	UTGOR_MovementComponent* Movement = ControlledPawn->FindComponentByClass<UTGOR_MovementComponent>();
	if (IsValid(Movement))
	{
		const float time = GameTimestamp - LastPerceptionUpdate;
		if (time > PerceptionUpdateTime)
		{
			LastPerceptionUpdate = GameTimestamp;

			// Update perception list
			PassivePerception();

			// Check whether we can see the tracked object (does nullcheck also)
			if (CanSeeObject(Tracking.Get()))
			{
				TrackingTrail.Emplace(Tracking->ComputeSpace());
				if (TrackingTrail.Num() > TrackingTrailLength)
				{
					TrackingTrail.RemoveAt(0);
				}
			}

			PrevInput = NextInput;
			PrevView = NextView;
			if (IsValid(Behaviour))
			{
				Behaviour->UpdateBehaviour(this, NextInput, NextView);
			}
			else
			{
				NextInput = FVector::ZeroVector;
				NextView = FQuat::Identity;
			}
			Movement->ForwardInput(PrevInput, PrevView);
		}
		else
		{
			const float Ratio = FMath::Clamp(time / PerceptionUpdateTime, 0.0f, 1.0f);
			const FVector Input = FMath::Lerp(PrevInput, NextInput, Ratio);
			const FQuat View = FMath::Lerp(PrevView, NextView, Ratio);
			Movement->ForwardInput(Input, View);
		}
	}
}

void ATGOR_NpcController::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	/*
	DOREPLIFETIME_CONDITION(ATGOR_NpcController, OnlineUsers, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(ATGOR_NpcController, DimensionRequestList, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(ATGOR_NpcController, HasPendingDeletion, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(ATGOR_NpcController, BodyDisplay, COND_OwnerOnly);
	*/
}

////////////////////////////////////////////////////////////////////////////////////////////////////

bool ATGOR_NpcController::CanSeeObject(UTGOR_MobilityComponent* Mobility) const
{
	// TODO: Implement dis
	return IsValid(Mobility);
}

bool ATGOR_NpcController::SamplePerception(UTGOR_ColliderComponent* Collider, FVector2D SpreadX, FVector2D SpreadY, float Radius, float Distance)
{
	if (IsValid(Collider))
	{
		// Compute perception ray
		const FTGOR_MovementPosition Position = Collider->ComputePosition();
		UTGOR_MobilityComponent* ParentBase = Collider->GetParent();

		// Initialise perception
		SINGLETON_RETCHK(false);

		if (IsValid(ParentBase))
		{
			const float SX = Random.FRandRange(SpreadX.X, SpreadX.Y);
			const float SY = Random.FRandRange(SpreadY.X, SpreadY.Y);
			const FVector Direction = Position.Angular * FVector(1.0f, SX, SY);
			const FVector Delta = Direction * Distance;

			// Sample world
			FHitResult Hit;
			if (Collider->MovementSphereTraceSweep(Radius, Position.Linear, Delta, Hit))
			{
				if (Hit.Actor.IsValid())
				{
					UTGOR_MobilityComponent* Base = Hit.Actor->FindComponentByClass<UTGOR_MobilityComponent>();
					if (IsValid(Base))
					{
						ParentBase = Base;
					}
				}

				// Register hit in movement base
				const FTGOR_MovementPosition BasePosition = ParentBase->ComputePosition();

				FTGOR_PerceptionPoint PerceptionPoint;
				PerceptionPoint.Timestamp = Singleton->GetGameTimestamp();
				PerceptionPoint.Normal = BasePosition.InverseTransformVector(Hit.Normal);
				PerceptionPoint.Point = BasePosition.InverseTransformPosition(Hit.ImpactPoint);

				// Add perception
				FTGOR_Perception& Perception = Memory.Perceptions.FindOrAdd(ParentBase, FTGOR_Perception());
				Perception.Points.Emplace(PerceptionPoint);
				return true;
			}

		}
	}
	return false;
}

void ATGOR_NpcController::SetTrackingObject(UTGOR_MobilityComponent* Mobility)
{
	TrackingTrail.Empty();
	Tracking = Mobility;
}


void ATGOR_NpcController::GetPerceivedObstacles(const FVector& Project, float Threshold, float Distance, TArray<FTGOR_MovementObstacle>& Obstacles) const
{
	Obstacles.Empty();
	for (const auto& Pair : Memory.Perceptions)
	{
		if (IsValid(Pair.Key))
		{
			const FTGOR_MovementDynamic Dynamic = Pair.Key->ComputeSpace();

			// Use average for point
			const TArray<FTGOR_Samples> Samples = Pair.Value.CreateSamples(Project, Threshold, Distance);
			for (const FTGOR_Samples& Sample : Samples)
			{
				FTGOR_MovementObstacle Obstacle;
				FTGOR_MovementPosition Position;
				Sample.Pca(32, Position.Angular, Obstacle.Extent, Position.Linear);
				Obstacle.Extent = Obstacle.Extent.ComponentMax(FVector::OneVector);
				Obstacle.Dynamic = Dynamic.TransformPosition(Position);
				
				//FVector(Sample.RadiusAlong(Mean, FVector::ForwardVector), Sample.RadiusAlong(Mean, FVector::RightVector), Sample.RadiusAlong(Mean, FVector::UpVector));
				Obstacles.Emplace(Obstacle);
			}
		}
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void ATGOR_NpcController::DebugDrawPerceptions(FVector Project, float Threshold, float Distance, float Duration, FLinearColor Color)
{
	SINGLETON_CHK;
	const FTGOR_Time GameTimestamp = Singleton->GetGameTimestamp();

	for (const auto PercsPair : Memory.Perceptions)
	{
		if (IsValid(PercsPair.Key))
		{
			const FTGOR_MovementPosition Position = PercsPair.Key->ComputePosition();

			for (const FTGOR_PerceptionPoint& Point : PercsPair.Value.Points)
			{
				const FVector Location = Position.TransformPosition(Point.Point);
				const FVector Normal = Position.TransformVector(Point.Normal);

				FColor Blend = FMath::Lerp(Color, FLinearColor::Black, (GameTimestamp - Point.Timestamp) / PerceptionTimeout).ToFColor(false);
				DrawDebugSolidPlane(GetWorld(), FPlane(Location, Normal), Location, FVector2D(10.0f, 10.0f), Blend, false, Duration, 0);
			}
		}
	}

	TArray<FTGOR_MovementObstacle> Obstacles;
	GetPerceivedObstacles(Project, Threshold, Distance, Obstacles);
	for (const FTGOR_MovementObstacle& Obstacle : Obstacles)
	{
		DrawDebugBox(GetWorld(), Obstacle.Dynamic.Linear, Obstacle.Extent, Obstacle.Dynamic.Angular, Color.ToFColor(false), false, Duration, 0, 10.0f);
		//DrawDebugBox(GetWorld(), Obstacle.Dynamic.Linear, Obstacle.Extent + Distance, Obstacle.Dynamic.Angular, (Color * 0.5).ToFColor(false), false, Duration, 0, 2.0f);
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////

#undef LOCTEXT_NAMESPACE