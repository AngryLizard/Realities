// The Gateway of Realities: Planes of Existence.


#include "TGOR_FunctionHitVolume.h"

#include "Utility/TGOR_Math.h"
#include "Base/TGOR_Singleton.h"

ATGOR_FunctionHitVolume::ATGOR_FunctionHitVolume(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	SegmentTime = 0.0f;
	SpawnTime = 0.0f;
	Lifetime = 0.0f;

	Head = 0;
}

void ATGOR_FunctionHitVolume::Build(const FTGOR_ElementInstance& State, const FVector& Focus)
{
	if (SpawnTime < SMALL_NUMBER)
	{
		AddEmptySegment();
		CreateHead(Focus, StartTime);
	}
	else
	{
		int32 Num = FMath::FloorToInt(Lifetime / SpawnTime);
		for (int32 i = 0; i < Num; i++)
		{
			AddEmptySegment();
		}
	}

	Super::Build(State, Focus);
}

void ATGOR_FunctionHitVolume::Update(const FTGOR_ElementInstance& State, const FVector& Focus)
{
	Super::Update(State, Focus);
	
	if (SpawnTime > SMALL_NUMBER)
	{
		SINGLETON_CHK;

		const FTGOR_Time Timestamp = Singleton->GetGameTimestamp();

		MoveHead(Focus, Timestamp);
	}
}

bool ATGOR_FunctionHitVolume::VolumeTick(float DeltaSeconds)
{
	SINGLETON_RETCHK(false);

	const FTGOR_Time Timestamp = Singleton->GetGameTimestamp();

	bool AnyActive = false;
	for (FTGOR_FunctionSegment& Segment : Segments)
	{
		if (Segment.Active)
		{
			// Fill gaps if segment is behind
			while (Segment.LastDisplay + SegmentTime < Timestamp)
			{
				Segment.Active = DisplaySegment(Segment, Segment.LastDisplay + SegmentTime);
			}

			// Actually display segment
			Segment.Active = DisplaySegment(Segment, Timestamp);

			// Time segments out
			const FTGOR_Time EndTime = Segment.Time + Lifetime;
			if (Timestamp > EndTime && Segment.Active)
			{
				TimeoutSegment(Segment, Timestamp);

				Segment.Active = false;
				InvalidateVolume(Segment);
			}

			AnyActive = AnyActive || Segment.Active;
		}
	}

	return(AnyActive);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void ATGOR_FunctionHitVolume::Setup(FTGOR_FunctionSegment& Segment)
{
	Segment.Primary = FVector::ZeroVector;
	Segment.Secondary = FVector::ZeroVector;
	Segment.Tertiary = FVector::ZeroVector;
}

FVector ATGOR_FunctionHitVolume::Function(float X, const FTGOR_FunctionSegment& Segment, float Time)
{
	return(FVector(X, 0.0f, 0.0f));
}


////////////////////////////////////////////////////////////////////////////////////////////////////

void ATGOR_FunctionHitVolume::AddEmptySegment()
{
	FTGOR_FunctionSegment Segment;

	Segment.Time = StartTime + -SpawnTime;
	Segment.LastDisplay = Segment.Time;
	Segment.Index = Segments.Num();
	Segment.Active = false;
	Segment.Distance = 0.0f;

	Segments.Add(Segment);
	SpawnVolume(Segment);
}

bool ATGOR_FunctionHitVolume::DisplaySegment(FTGOR_FunctionSegment& Segment, FTGOR_Time Timestamp)
{
	// Move segments according to functions
	const float Time = Timestamp - Segment.Time;
	const FVector From = Function(Time, Segment, Timestamp - StartTime);
	const FVector FromFunction = Segment.Transform.TransformPosition(From);

	const FVector To = Function(Time + SegmentTime, Segment, Timestamp - StartTime);
	const FVector ToFunction = Segment.Transform.TransformPosition(To);

	// Remember last display
	Segment.LastDisplay = Timestamp;
	return(MoveVolume(Segment, FromFunction, ToFunction, Timestamp - StartTime));
}

void ATGOR_FunctionHitVolume::TimeoutSegment(const FTGOR_FunctionSegment& Segment, FTGOR_Time Timestamp)
{
	const float LocTime = Timestamp - Segment.Time;
	const FVector Loc = Function(LocTime, Segment, Timestamp - StartTime);
	const FVector LocFunction = Segment.Transform.TransformPosition(Loc);
	TimeoutVolume(Segment, LocFunction, Timestamp - StartTime);
}

void ATGOR_FunctionHitVolume::MoveHead(const FVector& Focus, FTGOR_Time Timestamp)
{
	const FTGOR_FunctionSegment& HeadSegment = Segments[Head];
	const FTGOR_Time Time = HeadSegment.Time + SpawnTime;
	if (Timestamp > Time && Segments.Num() > 0)
	{
		Head = (Head + 1) % Segments.Num();

		CreateHead(Focus, Timestamp);
		MoveHead(Focus, Timestamp);
	}
}

void ATGOR_FunctionHitVolume::CreateHead(const FVector& Focus, FTGOR_Time Timestamp)
{
	FTGOR_FunctionSegment& HeadSegment = Segments[Head];
	
	if (HeadSegment.Active)
	{
		TimeoutSegment(HeadSegment, Timestamp);
	}

	const FVector Location = GetActorLocation();
	const FVector Diff = Focus - Location;

	HeadSegment.Time = Timestamp;
	HeadSegment.LastDisplay = Timestamp;
	HeadSegment.Active = true;
	HeadSegment.Distance = Diff.Size();

	if (HeadSegment.Distance > SMALL_NUMBER)
	{
		const FVector Normal = GetActorUpVector();
		const FVector Forward = Diff / HeadSegment.Distance;
		const FVector Right = (Normal ^ Forward).GetSafeNormal();
		const FVector Up = Forward ^ Right;

		HeadSegment.Transform = FTransform(Forward, Right, Up, Location);
	}
	else
	{
		HeadSegment.Transform = FTransform(FQuat::Identity, Location, FVector::ZeroVector);
	}

	Setup(HeadSegment);
	ValidateVolume(HeadSegment);
}