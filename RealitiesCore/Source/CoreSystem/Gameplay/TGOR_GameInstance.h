// The Gateway of Realities: Planes of Existence.

#pragma once

#include "Engine/World.h"

#include "RealitiesUGC/Mod/TGOR_ModInstance.h"
#include "RealitiesUtility/Utility/TGOR_Error.h"

#include "../Interfaces/TGOR_SingletonInterface.h"
#include "RealitiesUGC/Gameplay/TGOR_ModGameInstance.h"
#include "TGOR_GameInstance.generated.h"

/**
* TGOR_GameInstance allows access to gamedata and controls spawning/despawning of actors
*/
UCLASS(Blueprintable)
class CORESYSTEM_API UTGOR_GameInstance : public UTGOR_ModGameInstance, public ITGOR_SingletonInterface
{
	GENERATED_BODY()

		friend class ATGOR_GameState;
	
public:
	UTGOR_GameInstance(const FObjectInitializer& ObjectInitializer);
	virtual void Shutdown() override;
	virtual UTGOR_Singleton* EnsureSingleton(const UObject* WorldContextObject) const override;
	virtual void HandleGameNetControlMessage(class UNetConnection* Connection, uint8 MessageByte, const FString& MessageStr) override;

	virtual void PreModLoad() override;
	virtual void PostModLoad(ETGOR_ModloadEnumeration Result) override;


	////////////////////////////////////////////////////////////////////////////////////////////////////

	UFUNCTION(BlueprintCallable, Category = "!TGOR System|Internal", Meta = (Keywords = "C++"))
		static UTGOR_Singleton* EnsureSingletonFromWorld(UWorld* World);

	////////////////////////////////////////////////////////////////////////////////////////////////////

	/** Spawns actor only locally (even if repliaction is active) */
	UFUNCTION(BlueprintCallable, Category = "!TGOR System", Meta = (Keywords = "C++"))
		static AActor* SpawnActorLocally(TSubclassOf<AActor> Class, AActor* Owner, FVector Location, FRotator Rotation);

	/** Deswpanws an actor */
	UFUNCTION(BlueprintCallable, Category = "!TGOR System", Meta = (Keywords = "C++"))
		static void DespawnActor(AActor* Actor);

	/** Switches current profile to another one, resets level if necessary */
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "!TGOR System", Meta = (WorldContext = "WorldContextObject", Keywords = "C++"))
		static void SwitchProfile(UObject* WorldContextObject, const FString& Profile);

	/** Switches current profile to null profile, resets level if necessary */
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "!TGOR System", Meta = (WorldContext = "WorldContextObject", Keywords = "C++"))
		static void SwitchToNullProfile(UObject* WorldContextObject);

	/** Saves game to file */
	UFUNCTION(BlueprintCallable, Category = "!TGOR System", Meta = (WorldContext = "WorldContextObject", Keywords = "C++"))
		static void SaveAll(UObject* WorldContextObject);

	/** Currently active profile */
	UFUNCTION(BlueprintPure, Category = "!TGOR System", Meta = (WorldContext = "WorldContextObject", Keywords = "C++"))
		static FString GetCurrentProfile(UObject* WorldContextObject);

	/** Gets save directory path */
	UFUNCTION(BlueprintPure, Category = "!TGOR System", Meta = (WorldContext = "WorldContextObject", Keywords = "C++"))
		static FString GetSaveDirectory(UObject* WorldContextObject);

	/** Whether we are currently in the initial profile */
	UFUNCTION(BlueprintPure, Category = "!TGOR System", Meta = (WorldContext = "WorldContextObject", Keywords = "C++"))
		static bool IsInNullProfile(UObject* WorldContextObject);
	
	/** Currently active profile */
	UPROPERTY(BlueprintReadOnly, Category = "!TGOR System")
		FString CurrentProfile;

	/** Name of initial profile before a profile is selected */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "!TGOR System")
		FString NullProfile;

public:
	////////////////////////////////////////////////////////////////////////////////////////////////////

	/** Spawns an actor with AlwaySpawn and NoFail settings */
	template<typename T>
	static T* SpawnActorForce(TSubclassOf<T> Class, UObject* Owner, FVector Location, FRotator Rotation = FRotator(0.0f, 0.0f, 0.0f))
	{
		FActorSpawnParameters Params;
		Params.bAllowDuringConstructionScript = true;
		Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		Params.bNoFail = true;

		T* t = Owner->GetWorld()->SpawnActor<T>(Class, Location, Rotation, Params);
		if (t == nullptr) ERRET("Spawn invalid", Error, nullptr)

		return(t);
	}
	
private:

	UPROPERTY()
		UTGOR_Singleton* PrivateSingleton;
};
