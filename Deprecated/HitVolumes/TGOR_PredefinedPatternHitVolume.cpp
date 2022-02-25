// The Gateway of Realities: Planes of Existence.


#include "TGOR_PredefinedPatternHitVolume.h"

#include "Base/TGOR_Singleton.h"

ATGOR_PredefinedPatternHitVolume::ATGOR_PredefinedPatternHitVolume(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PatternContainer = ObjectInitializer.CreateDefaultSubobject<USceneComponent>(this, FName(TEXT("PatternContainer")));
	SetRootComponent(PatternContainer);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void ATGOR_PredefinedPatternHitVolume::UpdateVolume(int32 Index, float Time)
{
	ETGOR_FetchEnumeration State;
	UTGOR_HitVolumeComponent* Volume = GetVolumeAt(Index, State);

	if (IsValid(Volume))
	{
		UpdatePredefinedVolume(Index, Volume, Time);
	}
}

bool ATGOR_PredefinedPatternHitVolume::IsRunning(float Time)
{
	return Time < LifeTime;
}


int32 ATGOR_PredefinedPatternHitVolume::BuildVolumes()
{
	const TArray<USceneComponent*>& Volumes = PatternContainer->GetAttachChildren();

	for (USceneComponent* Volume : Volumes)
	{
		UTGOR_HitVolumeComponent* PatternVolume = Cast<UTGOR_HitVolumeComponent>(Volume);
		if (IsValid(PatternVolume))
		{
			const int32 Index = PatternVolumes.Num();
			PatternVolumes.Add(PatternVolume);
			BuildPredefinedVolume(Index, PatternVolume);

			const float VolumeEndTime = PatternVolume->GetVolumeEnd();
			if (VolumeEndTime > LifeTime)
			{
				LifeTime = VolumeEndTime;
			}
		}
	}

	return(PatternVolumes.Num());
}



UTGOR_HitVolumeComponent* ATGOR_PredefinedPatternHitVolume::GetVolumeAt(int32 Index, ETGOR_FetchEnumeration& Branches)
{
	Branches = ETGOR_FetchEnumeration::Empty;

	// Make sure index is valid
	int32 Num = FMath::Min(NumberOfVolumes, PatternVolumes.Num());
	if (0 <= Index && Index < Num)
	{
		Branches = ETGOR_FetchEnumeration::Found;
		return(PatternVolumes[Index]);
	}

	return(nullptr);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

FVector ATGOR_PredefinedPatternHitVolume::GetParabolaTo(const FVector& Location, float DownAcceleration, float Duration)
{
	if (Duration < SMALL_NUMBER)
	{
		return(FVector(0.0f, 0.0f, 0.0f));
	}

	// Compute parabola parameters
	FVector Diff = Location - GetActorLocation();
	float Dist = Diff.Size2D();
	float Height = Diff.Z;

	// Compute required velocity (h = v*t - 1/2*g*t^2  =>  v = h/t + 1/2*g*t)
	float VelY = Height / Duration + 0.5f * DownAcceleration * Duration;
	float VelX = Dist / Duration;

	// Get direction on plane
	FVector2D Direction = FVector2D(Diff) / Dist;

	// Build velocity vector
	return(FVector(Direction * VelX, VelY));
}

void ATGOR_PredefinedPatternHitVolume::ApplyDefault(int32 Index, UTGOR_HitVolumeComponent* Volume, ETGOR_BoolEnumeration& Branches)
{
	Branches = ETGOR_BoolEnumeration::IsNot;

	SINGLETON_CHK

	const float Time = Singleton->GetGameTimestamp() - StartTime;

	// Make sure volume is valid
	if (!IsValid(Volume))
	{
		return;
	}

	// Get setting
	const bool Active = (Volume->GetVolumeStart() <= Time) && (Time <= Volume->GetVolumeEnd());
	Volume->SetVisibility(Active, true);

	if (Active)
	{		
		if (!Volume->IsVolumeActive())
		{
			TerminatePredefinedVolume(Index, Volume, Time);
			Volume->SetVolumeActive(true);
		}

		Branches = ETGOR_BoolEnumeration::Is;
	}
	else
	{
		Volume->SetVolumeActive(false);
	}
}
