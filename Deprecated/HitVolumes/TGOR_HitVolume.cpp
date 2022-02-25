// The Gateway of Realities: Planes of Existence.


#include "TGOR_HitVolume.h"

#include "Base/TGOR_GameState.h"
#include "Components/Dimension/TGOR_InfluencableComponent.h"

ATGOR_HitVolume::ATGOR_HitVolume(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = true;

	Parent = nullptr;
	Identifier = -1;
}

void ATGOR_HitVolume::Tick(float DeltaSeconds)
{
	const bool Active = VolumeTick(DeltaSeconds);
	if (!Active)
	{
		UTGOR_GameInstance::DespawnActor(this);
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void ATGOR_HitVolume::Build(const FTGOR_ElementInstance& State, const FVector& Focus)
{
	Update(State, Focus);
}

void ATGOR_HitVolume::Update(const FTGOR_ElementInstance& State, const FVector& Focus)
{
	DamageState = State;
}

bool ATGOR_HitVolume::VolumeTick(float DeltaSeconds)
{
	return(true);
}

void ATGOR_HitVolume::Terminate(FTGOR_Time TerminationTimestamp)
{
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void ATGOR_HitVolume::Init(const TArray<AActor*>& CreationIgnore, AActor* CreationParent, FTGOR_Time CreationTimestamp, int32 CreationIdentifier)
{
	Ignore = CreationIgnore;
	Parent = CreationParent;
	StartTime = CreationTimestamp;
	Identifier = CreationIdentifier;
}

FVector ATGOR_HitVolume::GetAverage(const FVector& Default, const TArray<FHitResult>& Hitresults)
{
	int32 Num = Hitresults.Num();
	if (Num == 0) return(Default);
	
	FVector Location = FVector(0.0f, 0.0f, 0.0f);
	for (const FHitResult& Hit : Hitresults)
	{
		Location += Hit.ImpactPoint;
	}
	
	return(Location);
}

bool ATGOR_HitVolume::ApplyMultiDamage(const TArray<FHitResult>& Hitresults, int32 Segment, float Factor)
{
	FTGOR_ElementInstance State = DamageState * Factor;

	// Go through all hitresults
	bool Found = false;
	for (const FHitResult& Hit : Hitresults)
	{
		if (ApplySingleDamage(State, Hit, Segment, Factor))
		{
			Found = true;
		}
	}

	// Return average location
	return(Found);
}

bool ATGOR_HitVolume::ApplySingleDamage(const FTGOR_ElementInstance& State, const FHitResult& Hitresult, int32 Segment, float Factor)
{
	SINGLETON_RETCHK(false)
	const FTGOR_Time Timestamp = Singleton->GetGameTimestamp();
	const float Time = Timestamp - StartTime;

	TWeakObjectPtr<AActor> Actor = Hitresult.Actor;
	const FVector& Source = Hitresult.Location;
	if (Actor.IsValid())
	{
		// See if influencable got found
		UActorComponent* Component = Actor->GetComponentByClass(UTGOR_InfluencableComponent::StaticClass());
		UTGOR_InfluencableComponent* Influencable = Cast<UTGOR_InfluencableComponent>(Component);
		if (IsValid(Influencable))
		{
			// Inflict influence
			Influencable->DirectInfluence(State * Factor, this, Parent, Identifier, Segment, Time, Hitresult);
		}
	}

	return(Actor != this);
}