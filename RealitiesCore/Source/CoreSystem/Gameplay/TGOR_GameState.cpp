// The Gateway of Realities: Planes of Existence.

#include "TGOR_GameState.h"
#include "Net/UnrealNetwork.h"
#include "GameFramework/HUD.h"
#include "EngineUtils.h"

#include "RealitiesUGC/Mod/TGOR_ContentManager.h"

#include "CoreSystem/TGOR_Singleton.h"
#include "CoreSystem/Gameplay/TGOR_GameInstance.h"
#include "CoreSystem/Gameplay/TGOR_WorldSettings.h"


ATGOR_GameState::ATGOR_GameState(const FObjectInitializer& ObjectInitializer)
:	Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = true;
}

void ATGOR_GameState::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	Progress();

	Singleton->GameTimestamp += DeltaSeconds;
	Singleton->GameTimestamp.Normalize();
}


void ATGOR_GameState::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void ATGOR_GameState::Setup()
{
	// Wait until world is valid
	UWorld* World = GetWorld();
	if (!IsValid(World))
	{
		ERROR("Invalid World.", Error);
	}

	// Wait until instance is valid
	UTGOR_GameInstance* Instance = Cast<UTGOR_GameInstance>(World->GetGameInstance());
	if (!IsValid(Instance) || !IsValid(Instance->ModRegistry))
	{
		ERROR("Invalid Game Instance.", Error);
	}

#if !UE_BUILD_SHIPPING
	// Override profile
	ATGOR_WorldSettings* Settings = Cast<ATGOR_WorldSettings>(World->GetWorldSettings());
	if (IsValid(Settings))
	{
		if (!Settings->ProfileOverride.IsEmpty())
		{
			Instance->CurrentProfile = Settings->ProfileOverride;
		}
	}
#endif
}


void ATGOR_GameState::Progress()
{
	// Progress through content manager setups
	SINGLETON_CHK;
	UTGOR_ContentManager* ContentManager = Singleton->GetContentManager();
	if (IsValid(ContentManager))
	{
		if (ContentManager->ProgressSetups(HasAuthority()))
		{
			OnProgress();
		}
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////

APlayerController* ATGOR_GameState::GetLocalPlayerController() const
{
	UWorld* World = GetWorld();
	for (auto It = World->GetPlayerControllerIterator(); It; ++It)
	{
		if (It->Get()->IsLocalController())
		{
			return It->Get();
		}
	}
	return nullptr;
}