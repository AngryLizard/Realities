// The Gateway of Realities: Planes of Existence.

#include "Realities.h"
#include "TGOR_CombatController.h"

#include "Actors/Pawns/TGOR_Battleable.h"
#include "Components/Combat/Stat/TGOR_HealthComponent.h"
#include "Components/Dimension/TGOR_RegionComponent.h"

ATGOR_CombatController::ATGOR_CombatController()
	: Super()
{
	Battleable = nullptr;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

ATGOR_Battleable* ATGOR_CombatController::EnsureBattleable(ETGOR_FetchEnumeration& Branches)
{
	Branches = ETGOR_FetchEnumeration::Empty;
	Battleable = Cast<ATGOR_Battleable>(GetPawn());
	
	if (IsValid(Battleable))
	{ Branches = ETGOR_FetchEnumeration::Found;}
	return(Battleable);
}


UTGOR_RegionComponent* ATGOR_CombatController::FindRegionComponent(ETGOR_FetchEnumeration& Branches)
{
	EnsureBattleable(Branches);
	if (!IsValid(Battleable)) return(nullptr);

	UActorComponent* Component = Battleable->GetComponentByClass(UTGOR_RegionComponent::StaticClass());
	UTGOR_RegionComponent* RegionComponent = Cast<UTGOR_RegionComponent>(Component);

	if (IsValid(RegionComponent))
	{ Branches = ETGOR_FetchEnumeration::Found; }
	return(RegionComponent);
}


////////////////////////////////////////////////////////////////////////////////////////////////////

void ATGOR_CombatController::EnterBattleMode(ETGOR_ComputeEnumeration& Branches)
{
	Branches = ETGOR_ComputeEnumeration::Failed;
	ETGOR_FetchEnumeration State;
	
	EnsureBattleable(State);
	if (!IsValid(Battleable)) return;

	Battleable->SetIsAggressive(true);
	Branches = ETGOR_ComputeEnumeration::Success;
}


void ATGOR_CombatController::LeaveBattleMode(ETGOR_ComputeEnumeration& Branches)
{
	Branches = ETGOR_ComputeEnumeration::Failed;
	ETGOR_FetchEnumeration State;

	EnsureBattleable(State);
	if (!IsValid(Battleable)) return;

	Battleable->SetIsAggressive(false);

	Branches = ETGOR_ComputeEnumeration::Success;
}
