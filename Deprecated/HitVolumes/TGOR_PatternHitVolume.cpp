// The Gateway of Realities: Planes of Existence.


#include "TGOR_PatternHitVolume.h"

#include "Utility/TGOR_Math.h"
#include "Base/TGOR_Singleton.h"

ATGOR_PatternHitVolume::ATGOR_PatternHitVolume(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{
}


void ATGOR_PatternHitVolume::Build(const FTGOR_ElementInstance& State, const FVector& Focus)
{
	NumberOfVolumes = BuildVolumes();

	Super::Build(State, Focus);
}

void ATGOR_PatternHitVolume::Update(const FTGOR_ElementInstance& State, const FVector& Focus)
{
	Super::Update(State, Focus);

	SINGLETON_CHK;

	const FTGOR_Time Timestamp = Singleton->GetGameTimestamp();

	UpdateFocus(Focus, Timestamp);
}

bool ATGOR_PatternHitVolume::VolumeTick(float DeltaSeconds)
{
	Super::VolumeTick(DeltaSeconds);

	SINGLETON_RETCHK(false);

	const float Timestamp = Singleton->GetGameTimestamp() - StartTime;

	// Update volumes in order
	for (int i = 0; i < NumberOfVolumes; i++)
	{
		UpdateVolume(i, Timestamp);
	}

	return(IsRunning(Timestamp));
}


////////////////////////////////////////////////////////////////////////////////////////////////////

void ATGOR_PatternHitVolume::UpdateFocus(const FVector& Focus, FTGOR_Time Time)
{
}

void ATGOR_PatternHitVolume::UpdateVolume(int32 Index, float Time)
{
}

bool ATGOR_PatternHitVolume::IsRunning(float Time)
{
	return(true);
}

int32 ATGOR_PatternHitVolume::BuildVolumes()
{
	return(0);
}