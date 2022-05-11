// The Gateway of Realities: Planes of Existence.

#include "TGOR_ImpactComponent.h"
#include "CoreSystem/TGOR_Singleton.h"


UTGOR_ImpactComponent::UTGOR_ImpactComponent()
	: Super()
{
}

void UTGOR_ImpactComponent::BeginPlay()
{
	Super::BeginPlay();

	// Invincibility on creation (TODO: This could be handled more gracefully by a more central system)
	SINGLETON_CHK;
	LastImpact = Singleton->GetGameTimestamp();
}

bool UTGOR_ImpactComponent::ApplyImpact(const FVector& Direction, float Strength)
{
	SINGLETON_RETCHK(false);
	const FTGOR_Time CurrentTime = Singleton->GetGameTimestamp();
	if (Strength > KnockbackThreshold && (CurrentTime - LastImpact) > InvincibilityTotalTime)
	{
		LastImpact = CurrentTime;
		OnKnockbackDelegate.Broadcast(Direction, FMath::Clamp(Strength, 0.0f, 1.0f));
		return true;
	}
	return false;
}

bool UTGOR_ImpactComponent::HasInvincibility() const
{
	SINGLETON_RETCHK(false);
	const FTGOR_Time CurrentTime = Singleton->GetGameTimestamp();
	return (CurrentTime - LastImpact) <= InvincibilityTotalTime;
}
