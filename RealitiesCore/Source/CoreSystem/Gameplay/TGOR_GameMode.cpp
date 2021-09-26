// The Gateway of Realities: Planes of Existence.


#include "TGOR_GameMode.h"
#include "TGOR_GameState.h"
#include "Engine/NetConnection.h"
#include "Net/DataChannel.h"

#include "CoreSystem/Gameplay/TGOR_GameSession.h"
#include "CoreSystem/Gameplay/TGOR_GameState.h"
#include "CoreSystem/Gameplay/TGOR_PlayerState.h"

#include "RealitiesUGC/Mod/TGOR_Mod.h"
#include "RealitiesUGC/Mod/TGOR_ContentManager.h"

ATGOR_GameMode::ATGOR_GameMode(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	GameSessionClass = ATGOR_GameSession::StaticClass();
	GameStateClass = ATGOR_GameState::StaticClass();
	PlayerStateClass = ATGOR_PlayerState::StaticClass();

	bStartPlayersAsSpectators = false;
}

void ATGOR_GameMode::GameWelcomePlayer(UNetConnection* Connection, FString& RedirectURL)
{
	Super::GameWelcomePlayer(Connection, RedirectURL);
	
	SINGLETON_CHK;
	UTGOR_ContentManager* ContentManager = Singleton->GetContentManager();

	// Assemble modlist into single string
	const TArray<UTGOR_Mod*>& ActiveMods = ContentManager->GetMods();
	uint8 MessageByte = (uint8)ActiveMods.Num();

	FString MessageStr = "";
	for (const UTGOR_Mod* Mod : ActiveMods)
	{
		MessageStr += Mod->Entry.Name + "#" + FString::FromInt(Mod->Entry.Version) + "/";
	}

	// Send to client so they can assemble their mods before receiving packages
	FNetControlMessage<NMT_GameSpecific>::Send(Connection, MessageByte, MessageStr);

}

void ATGOR_GameMode::PreLogin(const FString& Options, const FString& Address, const FUniqueNetIdRepl& UniqueId, FString& ErrorMessage)
{
	Super::PreLogin(Options, Address, UniqueId, ErrorMessage);

	// TODO: Do authentication here
}
