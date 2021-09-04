// The Gateway of Realities: Planes of Existence.

#pragma once

#include "RealitiesUGC/Mod/TGOR_ModInstance.h"
#include "RealitiesUtility/Structures/TGOR_Time.h"
#include "RealitiesUtility/Utility/TGOR_Error.h"

#include "Storage/TGOR_PackageMinimal.h"

#include "UObject/Object.h"
#include "TGOR_Singleton.generated.h"

class UTGOR_ContentManager;
class UTGOR_Data;
class UTGOR_Mod;

class UTGOR_ReadBuffer;
class UTGOR_WriteBuffer;

class UTGOR_ModRegistry;



/**
* TGOR_Singleton provides basic functionality to store the current game state (Mods, WorldDatas, ContentManager).
*/
UCLASS()
class CORESYSTEM_API UTGOR_Singleton : public UObject
{
	GENERATED_BODY()

	friend class ATGOR_OnlineController; // TODO: Syncing timestamp... should probably do this in Gamestate
	friend class UTGOR_GameInstance;
	friend class ATGOR_GameState;
	friend class UTGOR_Setup;

public:
	UTGOR_Singleton();

	////////////////////////////////////////////////////////////////////////////////////////////////////
protected:
	
	// Updated via OnlineController
	UPROPERTY()
		FTGOR_Time GameTimestamp;

	////////////////////////////////////////////////////////////////////////////////////////////////////
protected:

	UPROPERTY(BlueprintReadOnly, Category = "!TGOR Content", meta = (AllowPrivateAccess = "true"))
		UTGOR_ContentManager* ContentManager;

	UPROPERTY(BlueprintReadOnly, Category = "!TGOR Config", meta = (AllowPrivateAccess = "true"))
		TArray<UTGOR_Data*> Datas;
public:

	/** Returns content manager */
	UFUNCTION(BlueprintPure, Category = "!TGOR Content", Meta = (Keywords = "C++"))
		UTGOR_ContentManager* GetContentManager() const;

	/** Returns timestamp */
	UFUNCTION(BlueprintPure, Category = "!TGOR System", Meta = (Keywords = "C++"))
		FTGOR_Time GetGameTimestamp() const;

	////////////////////////////////////////////////////////////////////////////////////////////////////

	/** Return current save file version from mods loaded */
	UFUNCTION(BlueprintPure, Category = "!TGOR Content", Meta = (Keywords = "C++"))
		FTGOR_Version GetVersion() const;


	/** Returns config data */
	UFUNCTION(BlueprintPure, Category = "!TGOR System", Meta = (DeterminesOutputType = "Type", Keywords = "C++"))
		UTGOR_Data* GetData(TSubclassOf<UTGOR_Data> Type) const;
	template<typename T> T* GetData() const
	{
		return Cast<T>(GetData(T::StaticClass()));
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////
private:

	//void LoadAssets(UTGOR_Mod* Mod, bool Verbose);
};
