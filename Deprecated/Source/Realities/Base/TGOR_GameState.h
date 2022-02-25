// The Gateway of Realities: Planes of Existence.

#pragma once

#include "Realities/Utility/TGOR_CustomEnumerations.h"
#include "Realities/Base/Instances/System/TGOR_ModInstance.h"
#include "Realities/Base/Instances/Dimension/TGOR_DimensionDataInstance.h"
#include "Realities/Utility/Error/TGOR_Error.h"
#include "Realities/Mod/TGOR_Mod.h"


#include "CoreMinimal.h"
#include "Realities/Base/TGOR_Singleton.h"
#include "Realities/Interfaces/TGOR_SingletonInterface.h"
#include "GameFramework/GameState.h"
#include "TGOR_GameState.generated.h"

class UTGOR_HubDimension;
class ATGOR_OnlineController;


UENUM(BlueprintType)
enum class ETGOR_LoadStage : uint8
{
	None,
	LoadMods,
	AwaitPossessed,
	Assemble,
	LoadConfig,
	LoadPersistent,
	AwaitDimensionLoading,
	AwaitBeginPlay,
	LoadWorld,
	Login,
	PostSpawn,
	Finished
};

/**
* 
*/
USTRUCT(BlueprintType)
struct FTGOR_LoadInformation
{
	GENERATED_USTRUCT_BODY()
public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TGOR System")
		bool HasLoaded;

	/** Server's mod setup */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TGOR System")
		FTGOR_ModInstance ModSetup;
};

/**
 * 
 */
UCLASS()
class REALITIES_API ATGOR_GameState : public AGameState, public ITGOR_SingletonInterface
{
	GENERATED_BODY()

		friend class UTGOR_WorldData;

public:
	ATGOR_GameState();

	virtual void GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const override;


	////////////////////////////////////////////////////////////////////////////////////////////////////
protected:

	/** Ignores Controller and controlled Pawn */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "TGOR System")
		bool IgnorePlayerController;

	/** Timeout in seconds for how long server waits for clients to unload */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "TGOR System")
		float LoadTimeout;

	UPROPERTY(BlueprintReadOnly, Category = "TGOR System")
		bool HasLoaded;

	UPROPERTY(BlueprintReadOnly, Category = "TGOR System")
		ETGOR_LoadStage LoadStage;

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Restarts LoadFiles stage */
	UFUNCTION(BlueprintCallable, Category = "TGOR System", Meta = (Keywords = "C++"))
		void ReloadWorld();

	/** Reloads all active mods (server/local only) */
	UFUNCTION(BlueprintCallable, Category = "TGOR System", Meta = (Keywords = "C++"))
		void ReloadResources();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Replicated, Category = "TGOR System")
		FTGOR_LoadInformation LoadInformation;

	UPROPERTY(BlueprintReadOnly, Category = "TGOR System")
		APlayerController* LocalController;

	////////////////////////////////////////////////////////////////////////////////////////////////////
protected:

	/** Server loaded dimensions */
	UPROPERTY(BlueprintReadOnly, Replicated, Category = "TGOR Dimension", Meta = (Keywords = "C++"))
		FTGOR_DimensionUpdateState DimensionUpdateState;

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:
	
	/** On mod load completion */
	UFUNCTION(BlueprintImplementableEvent, Category = "TGOR System", Meta = (Keywords = "C++"))
		void ModsLoadComplete(ETGOR_ModloadEnumeration Response);

	/** Called after the initial dimensions are spawned, for example at the start of the game when the hub level is being loaded. 
	@param True when a initial dimension was loaded, such as when the persistent level is not an initial dimension, otherwise - false.
	*/
	UFUNCTION(BlueprintImplementableEvent, Category = "TGOR Dimension", Meta = (Keywords = "C++"))
		void OnSpawnInitialDimensions(bool SpawnedInitial);

	/** Continues loading and returns value between 0..1 for how much has been loaded */
	UFUNCTION(BlueprintCallable, Category = "TGOR System", Meta = (ExpandEnumAsExecs = "Branches", Keywords = "C++"))
		float Progress(ETGOR_BoolEnumeration& Branches);

	/** Check whether anything loading related is going on for dimensions or content */
	UFUNCTION(BlueprintCallable, Category = "TGOR System", Meta = (Keywords = "C++"))
		bool HasFinishedLoading() const;
	
	////////////////////////////////////////////////////////////////////////////////////////////////////

	/** Gets called when game needs to load server files */
	UFUNCTION(BlueprintImplementableEvent, Category = "TGOR System", Meta = (Keywords = "C++"))
		void LoadServerFiles();

	/** Gets called when game needs to load local files */
	UFUNCTION(BlueprintImplementableEvent, Category = "TGOR System", Meta = (Keywords = "C++"))
		void LoadLocalFiles();

	/** Gets called when game needs to write server files */
	UFUNCTION(BlueprintImplementableEvent, Category = "TGOR System", Meta = (Keywords = "C++"))
		void SaveServerFiles();

	/** Gets called when game needs to write local files */
	UFUNCTION(BlueprintImplementableEvent, Category = "TGOR System", Meta = (Keywords = "C++"))
		void SaveLocalFiles();

	/** Save everything to file */
	UFUNCTION(BlueprintCallable, Category = "TGOR Dimension", Meta = (Keywords = "C++"))
		void SaveAll();

	/** Add dimension to WorldData */
	UFUNCTION(BlueprintCallable, Category = "TGOR Dimension", Meta = (Keywords = "C++"))
		bool AppendDimension(const FName& Identifier, UTGOR_Dimension* Content);

	/** Loads first dimension of given type */
	UFUNCTION(BlueprintCallable, Category = "TGOR Dimension", Meta = (Keywords = "C++"))
		bool LoadDimension(TSubclassOf<UTGOR_Dimension> Current, const FName& Identifier, TSubclassOf<UTGOR_Dimension> Class);

	/** Get first non-abstract Dimension of given type */
	UFUNCTION(BlueprintCallable, Category = "TGOR Dimension", Meta = (Keywords = "C++"))
		UTGOR_Dimension* GetFirstOfType(TSubclassOf<UTGOR_Dimension> Class) const;

private:
	bool LoadMods();
	bool AwaitPossessed();
	bool Assemble();
	bool LoadConfig();
	bool LoadPersistent();
	bool AwaitDimensionLoading();
	bool AwaitBeginPlay();
	bool LoadWorld();
	bool Login();
	bool PostSpawn();

};
