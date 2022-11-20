// The Gateway of Realities: Planes of Existence.

#include "TGOR_RangeParticipantComponent.h"
#include "RealitiesUtility/Utility/TGOR_Error.h"
#include "RealitiesUtility/Utility/TGOR_Math.h"

#include "DimensionSystem/Content/TGOR_Spawner.h"
#include "DialogueSystem/Content/TGOR_Participant.h"

#include "DrawDebugHelpers.h"
#include "Net/UnrealNetwork.h"

UTGOR_RangeParticipantComponent::UTGOR_RangeParticipantComponent(const FObjectInitializer& ObjectInitializer)
	: Super()
{
}

void UTGOR_RangeParticipantComponent::UpdateContent_Implementation(FTGOR_SpawnerDependencies& Dependencies)
{
	ITGOR_SpawnerInterface::UpdateContent_Implementation(Dependencies);
	Participant = Dependencies.Spawner->GetMFromType<UTGOR_Participant>(SpawnParticipant);
}

TMap<int32, UTGOR_SpawnModule*> UTGOR_RangeParticipantComponent::GetModuleType_Implementation() const
{
	TMap<int32, UTGOR_SpawnModule*> Modules;
	Modules.Emplace(Modules.Num(), Participant);
	return Modules;
}

bool UTGOR_RangeParticipantComponent::IsCompatibleWith(AActor* Actor) const
{
	if (!Participant || !Actor)
	{
		return false;
	}

	return IsLocationInside(Actor->GetActorLocation());
}

UTGOR_Participant* UTGOR_RangeParticipantComponent::GetParticipant() const
{
	return Participant;
}
