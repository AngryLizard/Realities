// The Gateway of Realities: Planes of Existence.
#include "TGOR_SimulationComponent.h"

#include "SimulationSystem/Components/TGOR_StatComponent.h"

#include "Net/UnrealNetwork.h"

UTGOR_SimulationComponent::UTGOR_SimulationComponent()
{
	HealthRegenPerSecond = 20.0f;
	DownedHealthRegenPerSecond = 0.0f;

	EnergyRegenPerSecond = 15.0f;
	StaminaRegenPerSecond = 15.0f;
}


void UTGOR_SimulationComponent::BeginPlay()
{
	Super::BeginPlay();

	AActor* Actor = GetOwner();
	
	Actor->OnTakeAnyDamage.AddDynamic(this, &UTGOR_SimulationComponent::OnDamageHealthEvent);

	// Make sure setup gets loaded after game got loaded automatically
	OnReplicateSetup();
}

void UTGOR_SimulationComponent::RegionTick(float DeltaTime)
{
	Super::RegionTick(DeltaTime);
	
	ServerTick(DeltaTime);

	APawn* Pawn = GetOuterAPawn();
	if (IsValid(Pawn) && !Pawn->IsLocallyControlled())
	{
		AuthorityTick(DeltaTime);
	}
}

void UTGOR_SimulationComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	
	ENetRole Role = GetOwnerRole();
	if (Role == ROLE_AutonomousProxy)
	{
		ClientTick(DeltaTime);
	}

	if (Role >= ROLE_AutonomousProxy)
	{
		APawn* Pawn = GetOuterAPawn();
		if (IsValid(Pawn) && Pawn->IsLocallyControlled())
		{
			OwnerTick(DeltaTime);
		}
	}

	if (Role == ROLE_SimulatedProxy)
	{
		SimulatedTick(DeltaTime);
	}
}


void UTGOR_SimulationComponent::ServerTick(float DeltaTime)
{

}

void UTGOR_SimulationComponent::ClientTick(float DeltaTime)
{

}

void UTGOR_SimulationComponent::AuthorityTick(float DeltaTime)
{
	Regenerate();
}

void UTGOR_SimulationComponent::OwnerTick(float DeltaTime)
{
	Regenerate();
}

void UTGOR_SimulationComponent::SimulatedTick(float DeltaTime)
{

}


void UTGOR_SimulationComponent::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UTGOR_SimulationComponent, CurrentSetup);
}


void UTGOR_SimulationComponent::DirectInfluenced(const FTGOR_ElementInstance& State, AActor* Instigator)
{
	APawn* Pawn = GetOuterAPawn();
	if (IsValid(Pawn))
	{
		//Pawn->HealthComponent->ConsumeStat(1.0f, true);
	}
}

void UTGOR_SimulationComponent::AmbientInfluenced(const FTGOR_ElementInstance& State)
{

}


void UTGOR_SimulationComponent::Exhaust(float Amount, bool Sync)
{
	APawn* Pawn = GetOuterAPawn();
	if (IsValid(Pawn))
	{
		if (Amount < 0.0f)
		{
			//Pawn->StaminaComponent->RestoreStat(Amount, Sync);
		}
		else
		{
			//Pawn->StaminaComponent->ConsumeStat(Amount, Sync);
		}
	}
}

void UTGOR_SimulationComponent::Deplete(float Amount, bool Sync)
{
	APawn* Pawn = GetOuterAPawn();
	if (IsValid(Pawn))
	{
		if (Amount < 0.0f)
		{
			//Pawn->EnergyComponent->RestoreStat(Amount, Sync);
		}
		else
		{
			//Pawn->EnergyComponent->ConsumeStat(Amount, Sync);
		}
	}
}

void UTGOR_SimulationComponent::Damage(float Amount, bool Sync)
{
	APawn* Pawn = GetOuterAPawn();
	if (IsValid(Pawn))
	{
		if (Amount < 0.0f)
		{
			//Pawn->HealthComponent->RestoreStat(Amount, Sync);
		}
		else
		{
			//Pawn->HealthComponent->ConsumeStat(Amount, Sync);
		}
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////

bool UTGOR_SimulationComponent::ApplySetup(const FTGOR_SimulationInstance& Setup)
{
	CurrentSetup = Setup;
	
	return(true);
}


void UTGOR_SimulationComponent::OnReplicateSetup()
{
	// Make sure mods have been loaded
	ApplySetup(CurrentSetup);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void UTGOR_SimulationComponent::Regenerate()
{
	APawn* Pawn = GetOuterAPawn();
	if (IsValid(Pawn))
	{
		// Regenerate depending on current health state
		//if (Pawn->HealthComponent->HealthState == ETGOR_HealthState::Normal)
		{
			//Pawn->HealthComponent->RegenerateStat(HealthRegenPerSecond);
		}
		//else if (Pawn->HealthComponent->HealthState == ETGOR_HealthState::Downed)
		{
			//Pawn->HealthComponent->RegenerateStat(DownedHealthRegenPerSecond);
		}

		// Regenerate eneger and stamina
		//Pawn->EnergyComponent->RegenerateStat(EnergyRegenPerSecond);
		//Pawn->StaminaComponent->RegenerateStat(StaminaRegenPerSecond);
	}
}