// The Gateway of Realities: Planes of Existence.

#include "TGOR_DimensionGameState.h"
#include "Net/UnrealNetwork.h"
#include "GameFramework/HUD.h"
#include "EngineUtils.h"

#include "RealitiesUGC/Mod/TGOR_ContentManager.h"

#include "DimensionSystem/Content/TGOR_Dimension.h"
#include "DimensionSystem/Components/TGOR_WorldComponent.h"
#include "DimensionSystem/Data/TGOR_DimensionData.h"
#include "DimensionSystem/Data/TGOR_WorldData.h"



ATGOR_DimensionGameState::ATGOR_DimensionGameState(const FObjectInitializer& ObjectInitializer)
:	Super(ObjectInitializer),
	LoadTimeout(2.0f)
{
	WorldComponent = ObjectInitializer.CreateDefaultSubobject<UTGOR_WorldComponent>(this, FName(TEXT("WorldComponent")));
	WorldComponent->SetNetAddressable();
	WorldComponent->SetIsReplicated(true);
}

void ATGOR_DimensionGameState::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	// Dimensions aren't updated on the hosting player controller so we're doing it here
	if (HasAuthority())
	{
		WorldComponent->UpdateDimensionRequestsFromPlayers();
		if (!WorldComponent->IsDimensionListUpToDate())
		{
			WorldComponent->UpdateDimensionList();
		}
	}
}


void ATGOR_DimensionGameState::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// Replicate to everyone
	DOREPLIFETIME(ATGOR_DimensionGameState, DimensionRequestState);
}


bool ATGOR_DimensionGameState::HasFinishedLoading() const
{
	SINGLETON_RETCHK(false);
	UTGOR_ContentManager* ContentManager = Singleton->GetContentManager();
	if (ContentManager->HasAnySetups()) return false;

	if (HasAuthority())
	{
		return WorldComponent->IsDimensionListUpToDate();
	}


	APlayerController* LocalController = GetLocalPlayerController();
	if (IsValid(LocalController))
	{
		UTGOR_WorldComponent* LocalWorldComponent = LocalController->FindComponentByClass<UTGOR_WorldComponent>();
		if (IsValid(LocalWorldComponent))
		{
			return LocalWorldComponent->IsDimensionListUpToDate();
		}
	}
	return true;
}


////////////////////////////////////////////////////////////////////////////////////////////////////

