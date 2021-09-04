// The Gateway of Realities: Planes of Existence.
#pragma once

#include "CoreSystem/Interfaces/TGOR_SingletonInterface.h"

#include "GameFramework/GameMode.h"
#include "TGOR_GameMode.generated.h"

/**
 * 
 */
UCLASS()
class CORESYSTEM_API ATGOR_GameMode : public AGameMode, public ITGOR_SingletonInterface
{
	GENERATED_BODY()
	
public:
	ATGOR_GameMode(const FObjectInitializer& ObjectInitializer);

	void GameWelcomePlayer(UNetConnection* Connection, FString& RedirectURL) override;
	void PreLogin(const FString& Options, const FString& Address, const FUniqueNetIdRepl& UniqueId, FString& ErrorMessage) override;
};