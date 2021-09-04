// The Gateway of Realities: Planes of Existence.

#pragma once

#include "CoreMinimal.h"

#include "RealitiesUGC/Mod/TGOR_ModInstance.h"
#include "RealitiesUGC/Mod/TGOR_ModSetup.h"

#include "../TGOR_Singleton.h"
#include "../Interfaces/TGOR_SingletonInterface.h"
#include "GameFramework/GameState.h"
#include "TGOR_GameState.generated.h"

class UTGOR_HubDimension;
class ATGOR_OnlineController;
class UTGOR_Creature;

/**
* 
*/
USTRUCT(BlueprintType)
struct FTGOR_LoadInformation
{
	GENERATED_USTRUCT_BODY()
public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "!TGOR System")
		bool HasLoaded;

	/** Server's mod setup */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "!TGOR System")
		FTGOR_ModInstance ModSetup;
};

/**
 * 
 */
UCLASS()
class CORESYSTEM_API ATGOR_GameState : public AGameState, public ITGOR_SingletonInterface
{
	GENERATED_BODY()

public:
	ATGOR_GameState(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual void Tick(float DeltaSeconds) override;
	virtual void GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const override;

	//////////////////////////////////////////// IMPLEMENTABLES ////////////////////////////////////////

	/** On setup change */
	UFUNCTION(BlueprintImplementableEvent, Category = "!TGOR System", Meta = (Keywords = "C++"))
		void OnProgress();

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Loads the whole game (can be called manually and will reload the whole game) */
	UFUNCTION(BlueprintCallable, Category = "!TGOR System|Internal", Meta = (Keywords = "C++"))
		void Setup();

	/** Continues loading and calls the necessary callbacks */
	UFUNCTION(BlueprintCallable, Category = "!TGOR System|Internal", Meta = (Keywords = "C++"))
		void Progress();

	////////////////////////////////////////////////////////////////////////////////////////////////////

	/** Get local player controller (we assume there is only one)*/
	UFUNCTION(BlueprintPure, Category = "!TGOR System|Internal", Meta = (Keywords = "C++"))
		APlayerController* GetLocalPlayerController() const;

};
