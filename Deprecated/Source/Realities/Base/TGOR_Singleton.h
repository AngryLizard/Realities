// The Gateway of Realities: Planes of Existence.

#pragma once

#include "Realities/Utility/TGOR_CustomEnumerations.h"
#include "Realities/Base/Instances/System/TGOR_ModInstance.h"
#include "Realities/Utility/Datastructures/TGOR_Time.h"
#include "Realities/Utility/Storage/TGOR_PackageMinimal.h"

#include "UObject/Object.h"
#include "TGOR_Singleton.generated.h"

class UTGOR_ContentManager;
class UTGOR_ConfigData;
class UTGOR_WorldData;
class UTGOR_UserData;
class UTGOR_Mod;

class UTGOR_ReadBuffer;
class UTGOR_WriteBuffer;


// TODO: Use UGameInstanceSubsystem

/**
* TGOR_Singleton provides basic functionality to store the current game state (Mods, WorldDatas, ContentManager).
*/
UCLASS()
class REALITIES_API UTGOR_Singleton : public UObject
{
	GENERATED_BODY()

	friend class ATGOR_OnlineController;
	friend class ATGOR_GameState;

public:
	UTGOR_Singleton();

	////////////////////////////////////////////////////////////////////////////////////////////////////
protected:

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "TGOR System")
		FTGOR_ModInstance ModSetup;
	
	// Updated via OnlineController
	UPROPERTY()
		FTGOR_Time GameTimestamp;

	UPROPERTY()
		FTGOR_Version Version;


	////////////////////////////////////////////////////////////////////////////////////////////////////
private:

	UPROPERTY(BlueprintReadOnly, Category = "TGOR Config", meta = (AllowPrivateAccess = "true"))
		UTGOR_ConfigData* ConfigData;

	UPROPERTY(BlueprintReadOnly, Category = "TGOR Online", meta = (AllowPrivateAccess = "true"))
		UTGOR_UserData* UserData;

	UPROPERTY(BlueprintReadOnly, Category = "TGOR Content", meta = (AllowPrivateAccess = "true"))
		UTGOR_ContentManager* ContentManager;

	UPROPERTY(BlueprintReadOnly, Category = "TGOR Dimension", meta = (AllowPrivateAccess = "true"))
		UTGOR_WorldData* WorldData;
public:

	/** Returns config data */
	UFUNCTION(BlueprintPure, Category = "TGOR Config", Meta = (Keywords = "C++"))
		UTGOR_ConfigData* GetConfigData() const;

	/** Returns user data */
	UFUNCTION(BlueprintPure, Category = "TGOR Online", Meta = (Keywords = "C++"))
		UTGOR_UserData* GetUserData() const;

	/** Returns content manager */
	UFUNCTION(BlueprintPure, Category = "TGOR Content", Meta = (Keywords = "C++"))
		UTGOR_ContentManager* GetContentManager() const;

	/** Returns world data */
	UFUNCTION(BlueprintPure, Category = "TGOR Dimension", Meta = (Keywords = "C++"))
		UTGOR_WorldData* GetWorldData() const;

	/** Returns timestamp */
	UFUNCTION(BlueprintPure, Category = "TGOR System", Meta = (Keywords = "C++"))
		FTGOR_Time GetGameTimestamp() const;

	/** Return current version */
	UFUNCTION(BlueprintPure, Category = "TGOR System", Meta = (Keywords = "C++"))
		FTGOR_Version GetVersion() const;

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	static UTGOR_Singleton* Construct(UObject* Outer, const FTGOR_ModInstance& LocalSetup);
	ETGOR_ModloadEnumeration LoadModSetup(UWorld* World, const FTGOR_ModInstance& Setup);
	void LocalModSetup(UWorld* World, bool Verbose = false);

	////////////////////////////////////////////////////////////////////////////////////////////////////
private:

	//void LoadAssets(UTGOR_Mod* Mod, bool Verbose);
};
