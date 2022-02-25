///////////////////////////////////////////////////
// The Gateway of Realities: Planes of Existence //
// Copyright 2016-2017                           //
///////////////////////////////////////////////////


#include "TGOR_HealthComponent.h"
#include "Net/UnrealNetwork.h"

UTGOR_HealthComponent::UTGOR_HealthComponent()
: Super()
{
	Immortal = false;
	EnableDownedState = false;

	HealthState = ETGOR_HealthState::Normal;
}

void UTGOR_HealthComponent::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UTGOR_HealthComponent, HealthState);
}

bool UTGOR_HealthComponent::CanRegen() const
{
	return(Super::CanRegen() && HealthState != ETGOR_HealthState::Dead);
}

void UTGOR_HealthComponent::OnStateReplicated(ETGOR_HealthState PreviousState)
{
	if (HealthState == ETGOR_HealthState::Downed && PreviousState != ETGOR_HealthState::Downed)
	{
		OnDowned.Broadcast();
	}
	else if (HealthState == ETGOR_HealthState::Normal && PreviousState == ETGOR_HealthState::Downed)
	{
		OnRecoverFromDowned.Broadcast();
	}
	else if (HealthState == ETGOR_HealthState::Dead && (PreviousState == ETGOR_HealthState::Downed || PreviousState == ETGOR_HealthState::Normal))
	{
		OnDeath.Broadcast();
	}
}

void UTGOR_HealthComponent::ChangeStat(float Amount)
{
	// Only deduct health if not dead yet
	if (HealthState == ETGOR_HealthState::Dead)
	{
		State.Stat = 0.0f;
		return;
	}

	Super::ChangeStat(Amount);

	if (GetOwner()->HasAuthority())
	{
		// Change state in case health reached 0
		if (State.Stat <= SMALL_NUMBER)
		{
			State.Stat = 0.0f;

			// Place the actor into downed state if enabled and not currently downed, otherwise they are now dead.
			if (HealthState == ETGOR_HealthState::Normal && EnableDownedState)
			{
				HealthState = ETGOR_HealthState::Downed;
				OnDowned.Broadcast();
				State.Stat = StatMax / 2.0f;
			}
			else
			{
				HealthState = ETGOR_HealthState::Dead;
				OnDeath.Broadcast();
			}
		}
		else if (HealthState == ETGOR_HealthState::Downed && State.Stat >= StatMax - SMALL_NUMBER)
		{
			// Change state in case health reached max
			HealthState = ETGOR_HealthState::Normal;
			OnRecoverFromDowned.Broadcast();
		}
	}
}
