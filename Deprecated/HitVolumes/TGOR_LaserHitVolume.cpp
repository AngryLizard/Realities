// The Gateway of Realities: Planes of Existence.


#include "TGOR_LaserHitVolume.h"

#include "Utility/Error/TGOR_Error.h"
#include "Utility/TGOR_Math.h"
#include "Base/TGOR_Singleton.h"


ATGOR_LaserHitVolume::ATGOR_LaserHitVolume(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	ExpirationTime = 0.0f;
	DelayedLocation = FVector(0.0f, 0.0f, 0.0f);
	MaxLaserVelocity = 0.0f;
	MaxLaserDistance = 5000.0f;
}

void ATGOR_LaserHitVolume::Build(const FTGOR_ElementInstance& State, const FVector& Focus)
{
	DelayedTime = StartTime;
	DelayedLocation = GetActorLocation();
}

void ATGOR_LaserHitVolume::Update(const FTGOR_ElementInstance& State, const FVector& Focus)
{
	SINGLETON_CHK
	
	// Get time passed since last update
	const FTGOR_Time Timestamp = Singleton->GetGameTimestamp();
	const float DeltaSeconds = Timestamp - DelayedTime;

	// Compute laser end point
	const FVector Location = GetActorLocation();
	const FVector Diff = Focus - Location;
	const FVector Normal = Diff.GetSafeNormal();
	FVector Target = Location + Normal * MaxLaserDistance;

	// Limit laser speed
	if (MaxLaserVelocity > SMALL_NUMBER)
	{
		const FVector Delta = Target - DelayedLocation;
		Target = DelayedLocation + Delta.GetClampedToMaxSize(MaxLaserVelocity * DeltaSeconds);
	}

	DelayedLocation = Target;
	DelayedTime = Timestamp;

	RenderVolume(Location, Target, Normal, Timestamp - StartTime);
}

bool ATGOR_LaserHitVolume::VolumeTick(float DeltaSeconds)
{
	Super::VolumeTick(DeltaSeconds);

	SINGLETON_RETCHK(false)

	// Check whether laser has expired
	const FTGOR_Time Timestamp = Singleton->GetGameTimestamp();
	return((Timestamp - DelayedTime) <= ExpirationTime + SMALL_NUMBER);
}

void ATGOR_LaserHitVolume::Terminate(FTGOR_Time TerminationTimestamp)
{

}