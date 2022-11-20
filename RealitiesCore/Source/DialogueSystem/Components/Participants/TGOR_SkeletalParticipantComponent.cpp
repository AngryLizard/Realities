// The Gateway of Realities: Planes of Existence.

#include "TGOR_SkeletalParticipantComponent.h"
#include "RealitiesUtility/Utility/TGOR_Error.h"
#include "RealitiesUtility/Utility/TGOR_Math.h"

#include "DimensionSystem/Content/TGOR_Spawner.h"
#include "DialogueSystem/Content/TGOR_Participant.h"

#include "DrawDebugHelpers.h"
#include "Net/UnrealNetwork.h"

UTGOR_SkeletalParticipantComponent::UTGOR_SkeletalParticipantComponent()
	: Super()
{
	bHiddenInGame = true;
}

void UTGOR_SkeletalParticipantComponent::BeginPlay()
{
	Super::BeginPlay();

	UWorld* World = GetWorld();
	check(World);

	// We want this skeleton to still be visible while playing in sequencer but not in-game
	if (!World->IsPlayInPreview())
	{
		SetHiddenInGame(true);
	}
}

void UTGOR_SkeletalParticipantComponent::UpdateContent_Implementation(FTGOR_SpawnerDependencies& Dependencies)
{
	ITGOR_SpawnerInterface::UpdateContent_Implementation(Dependencies);
	Participant = Dependencies.Spawner->GetMFromType<UTGOR_Participant>(SpawnParticipant);
}

TMap<int32, UTGOR_SpawnModule*> UTGOR_SkeletalParticipantComponent::GetModuleType_Implementation() const
{
	TMap<int32, UTGOR_SpawnModule*> Modules;

	Modules.Emplace(Modules.Num(), Participant);
	return Modules;
}

bool UTGOR_SkeletalParticipantComponent::IsCompatibleWith(AActor* Actor) const
{
	if (!Participant)
	{
		return false;
	}

	// Check whether skeleton matches requirement
	USkeletalMeshComponent* Root = Actor->FindComponentByClass<USkeletalMeshComponent>();
	if (Root)
	{
		USkinnedAsset* OwnSkinnedAsset = GetSkinnedAsset();
		USkinnedAsset* OtherSkinnedAsset = Root->GetSkinnedAsset();
		if (OtherSkinnedAsset && OwnSkinnedAsset && OtherSkinnedAsset->GetSkeleton()->IsCompatible(OwnSkinnedAsset->GetSkeleton()))
		{
			return true;
		}
	}
	return false;
}

UTGOR_Participant* UTGOR_SkeletalParticipantComponent::GetParticipant() const
{
	return Participant;
}
