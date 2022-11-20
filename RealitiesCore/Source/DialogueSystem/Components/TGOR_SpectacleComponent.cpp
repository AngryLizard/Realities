// The Gateway of Realities: Planes of Existence.

#include "TGOR_SpectacleComponent.h"
#include "RealitiesUtility/Utility/TGOR_Error.h"
#include "RealitiesUtility/Utility/TGOR_Math.h"

#include "DialogueSystem/Components/TGOR_InstigatorComponent.h"
#include "DialogueSystem/Interfaces/TGOR_ParticipantInterface.h"
#include "DialogueSystem/Content/TGOR_Participant.h"

#include "DrawDebugHelpers.h"
#include "Net/UnrealNetwork.h"

UTGOR_SpectacleComponent::UTGOR_SpectacleComponent()
	: Super()
{
}

void UTGOR_SpectacleComponent::UpdateContent_Implementation(FTGOR_SpawnerDependencies& Dependencies)
{
	Super::UpdateContent_Implementation(Dependencies);

	Dependencies.Process<UTGOR_ParticipantInterface>();

	TArray<UActorComponent*> SpectatorComponents = GetOwner()->GetComponentsByInterface(UTGOR_ParticipantInterface::StaticClass());
	for (UActorComponent* SpectatorComponent : SpectatorComponents)
	{
		ITGOR_ParticipantInterface* ParticipantInterface = Cast<ITGOR_ParticipantInterface>(SpectatorComponent);
		ParticipantCache.Emplace(ParticipantInterface->GetParticipant(), SpectatorComponent);
	}
}

TSubclassOf<UTGOR_Performance> UTGOR_SpectacleComponent::GetPerformanceType() const
{
	return PerformanceType;
}

UTGOR_AnimationComponent* UTGOR_SpectacleComponent::GetAnimationComponent() const
{
	return GetOwner()->FindComponentByClass<UTGOR_AnimationComponent>();
}

////////////////////////////////////////////////////////////////////////////////////////////////////

bool UTGOR_SpectacleComponent::ValidateSpectactleFor(UTGOR_InstigatorComponent* Instigator, FTGOR_SpectacleConfiguration& Configuration) const
{
	return false;
}

void UTGOR_SpectacleComponent::StartSpectacle(UTGOR_InstigatorComponent* Instigator, FTGOR_SpectacleConfiguration& Configuration)
{
}

void UTGOR_SpectacleComponent::EndSpectacle()
{
}

////////////////////////////////////////////////////////////////////////////////////////////////////

UActorComponent* UTGOR_SpectacleComponent::GetParticipantComponentByType(TSubclassOf<UTGOR_Participant> Participant, TSubclassOf<UActorComponent> ComponentClass, ETGOR_FetchEnumeration& Branches)
{
	for (const auto& Pair : ParticipantCache)
	{
		if (Pair.Key->IsA(Participant) && Pair.Value->IsA(ComponentClass))
		{
			Branches = ETGOR_FetchEnumeration::Found;
			return Pair.Value;
		}
	}
	Branches = ETGOR_FetchEnumeration::Empty;
	return nullptr;
}

UActorComponent* UTGOR_SpectacleComponent::GetParticipantComponent(UTGOR_Participant* Participant, TSubclassOf<UActorComponent> ComponentClass, ETGOR_FetchEnumeration& Branches)
{
	UActorComponent* const* Ptr = ParticipantCache.Find(Participant);
	if (Ptr && (*Ptr)->IsA(ComponentClass))
	{
		Branches = ETGOR_FetchEnumeration::Found;
		return (*Ptr);
	}
	Branches = ETGOR_FetchEnumeration::Empty;
	return nullptr;
}


UActorComponent* UTGOR_SpectacleComponent::GetParticipantComponent(UTGOR_Participant* Participant) const
{
	if (UActorComponent* const* Ptr = ParticipantCache.Find(Participant))
	{
		return *Ptr;
	}
	return nullptr;
}