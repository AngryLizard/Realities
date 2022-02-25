// The Gateway of Realities: Planes of Existence.


#include "TGOR_InfluencableComponent.h"
#include "Realities/Components/System/Actions/TGOR_ActionComponent.h"
#include "Realities/Actors/Dimension/TGOR_RegionActor.h"

UTGOR_InfluencableComponent::UTGOR_InfluencableComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

}

void UTGOR_InfluencableComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);


	for (auto Pair = InvincibilityFrames.CreateIterator(); Pair; ++Pair)
	{
		Pair->Value -= DeltaTime;

		if (Pair->Value <= 0.0f)
		{
			Pair.RemoveCurrent();
		}
	}
}

void UTGOR_InfluencableComponent::DirectInfluence(const FTGOR_ElementInstance& State, ATGOR_HitVolume* HitVolume, AActor* Instigator, int32 Identifier, int32 Segment, float Time, const FHitResult& Hit)
{
	if (!InvincibilityFrames.Contains(Identifier) || Identifier < 0)
	{
		// Only add if valid identifier
		if (Identifier >= 0)
		{
			InvincibilityFrames.Add(Identifier, Time);
		}

		// Call influence functions
		if (GetOwner()->HasAuthority())
		{
			DirectInfluenced(State, Instigator);
			OnDirectInfluence.Broadcast(State, Instigator);
		}
		OnInfluenceVisual.Broadcast(Hit.ImpactPoint);
	}
}

void UTGOR_InfluencableComponent::AmbientInfluence()
{
	FTGOR_ElementInstance Ambient;

	// Change state
	if (IsValid(Current))
	{
		ComputeState(Ambient);
	}
	else
	{
		_search.reset();
		ERROR("Region not available", Error)
	}
	
	// Sum all accumulated influences together
	FTGOR_ElementInstance State = Ambient + _posMax + _negMin;
	_posMax = FTGOR_ElementInstance();
	_negMin = FTGOR_ElementInstance();

	AmbientInfluenced(State);
	OnAmbientInfluence.Broadcast(State);
}

void UTGOR_InfluencableComponent::AccumulateAmbient(const FTGOR_ElementInstance& State)
{
	_posMax = _posMax.PosMax(State);
	_negMin = _negMin.NegMin(State);
}

void UTGOR_InfluencableComponent::DirectInfluenced(const FTGOR_ElementInstance& State, AActor* Instigator)
{

}

void UTGOR_InfluencableComponent::AmbientInfluenced(const FTGOR_ElementInstance& State)
{
	DirectInfluenced(State, nullptr);
}


void UTGOR_InfluencableComponent::OnDamageHealthEvent(AActor* DamagedActor, float Points, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser)
{
	AActor* Actor = GetOwner();
	FVector Location = Actor->GetActorLocation();

	UActorComponent* Component = DamageCauser->GetComponentByClass(UTGOR_ActionComponent::StaticClass());
	UTGOR_ActionComponent* Damage = Cast<UTGOR_ActionComponent>(Component);

	FTGOR_ElementInstance State;
	//DirectInfluence(State, Damage, -1, 0.0f, Location);
}


void UTGOR_InfluencableComponent::ComputeState(FTGOR_ElementInstance& State)
{
	AActor* Owner = GetOwner();
	if (Owner == nullptr) return;

	float PrevLerp, NextLerp, MeetLerp;
	_search.getLerps(PrevLerp, NextLerp, MeetLerp);

	const float CLerp = MeetLerp + (PrevLerp + NextLerp) / 3;
	const float OLerp = (NextLerp + PrevLerp) / 3;
	const float PLerp = PrevLerp / 3;
	const float NLerp = NextLerp / 3;

	// Get floor and ceiling values
	const float Floor =		Current->GetFloor()		* CLerp + Opposite->GetFloor()		* OLerp + Prev->GetFloor()		* PLerp + Next->GetFloor()		* NLerp;
	const float Ceiling =	Current->GetCeiling()	* CLerp + Opposite->GetCeiling()	* OLerp + Prev->GetCeiling()	* PLerp + Next->GetCeiling()	* NLerp;
	const float Lerp = FMath::Max(FMath::Min((Owner->GetActorLocation().Z - Floor) / (Ceiling - Floor), 1.0f), 0.0f);

	// Lerp all values
	LerpField(State, &FTGOR_ElementInstance::Temperature, CLerp, OLerp, PLerp, NLerp, Lerp);
}


void UTGOR_InfluencableComponent::LerpField(FTGOR_ElementInstance& State, float FTGOR_ElementInstance::*Field, float CurrentLerp, float OppositeLerp, float PrevLerp, float NextLerp, float Lerp)
{
	// Get top and bottom values
	const float Bottom =	Current->GetBottom().*Field * CurrentLerp	+ Opposite->GetBottom().*Field * OppositeLerp	+ Prev->GetBottom().*Field * PrevLerp	+ Next->GetBottom().*Field * NextLerp;
	const float Top =		Current->GetTop().*Field * CurrentLerp		+ Opposite->GetTop().*Field * OppositeLerp		+ Prev->GetTop().*Field * PrevLerp		+ Next->GetTop().*Field * NextLerp;
	const float Value = Bottom + (Top - Bottom) * Lerp;

	// Enable metamorph next frame if change was big
	State.*Field = Value;
}
