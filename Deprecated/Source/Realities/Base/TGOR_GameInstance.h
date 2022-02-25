// The Gateway of Realities: Planes of Existence.

#pragma once

#include "AssetRegistryModule.h"
#include "Engine/World.h"

#include "Realities/Base/Instances/System/TGOR_ModInstance.h"
#include "Realities/Utility/Error/TGOR_Error.h"

#include "Realities/Interfaces/TGOR_SingletonInterface.h"
#include "Engine/GameInstance.h"
#include "TGOR_GameInstance.generated.h"

/**
* TGOR_GameInstance allows access to gamedata and controls spawning/despawning of actors
*/
UCLASS(Blueprintable)
class REALITIES_API UTGOR_GameInstance : public UGameInstance, public ITGOR_SingletonInterface
{
	GENERATED_BODY()

		friend class ATGOR_GameState;
	
public:
	UTGOR_GameInstance(const FObjectInitializer& ObjectInitializer);
	virtual void Init() override;
	virtual void Shutdown() override;
	virtual UTGOR_Singleton* EnsureSingleton(UObject* WorldContextObject) override;

	////////////////////////////////////////////////////////////////////////////////////////////////////

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "TGOR System")
		FTGOR_ModInstance ModSetup;

	UFUNCTION(BlueprintCallable, Category = "TGOR System", Meta = (Keywords = "C++"))
		static UTGOR_Singleton* EnsureSingletonFromWorld(UWorld* World);

	////////////////////////////////////////////////////////////////////////////////////////////////////

	/** Uses custom replication */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TGOR Online")
		bool UseCustomReplication;

	/** Spawns actor only locally (even if repliaction is active) */
	UFUNCTION(BlueprintCallable, Category = "TGOR System", Meta = (Keywords = "C++"))
		static AActor* SpawnActorLocally(TSubclassOf<AActor> Class, AActor* Owner, FVector Location, FRotator Rotation);

	/** Deswpanws an actor */
	UFUNCTION(BlueprintCallable, Category = "TGOR System", Meta = (Keywords = "C++"))
		static void DespawnActor(AActor* Actor);

	/** Switches current profile to another one, resets level if necessary */
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "TGOR System", Meta = (WorldContext = "WorldContextObject", Keywords = "C++"))
		static void SwitchProfile(UObject* WorldContextObject, const FString& Profile);

	/** Switches current profile to null profile, resets level if necessary */
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "TGOR System", Meta = (WorldContext = "WorldContextObject", Keywords = "C++"))
		static void SwitchToNullProfile(UObject* WorldContextObject);

	/** Saves game to file */
	UFUNCTION(BlueprintCallable, Category = "TGOR System", Meta = (WorldContext = "WorldContextObject", Keywords = "C++"))
		static void SaveAll(UObject* WorldContextObject);

	/** Currently active profile */
	UFUNCTION(BlueprintPure, Category = "TGOR System", Meta = (WorldContext = "WorldContextObject", Keywords = "C++"))
		static FString GetCurrentProfile(UObject* WorldContextObject);

	/** Gets save directory path */
	UFUNCTION(BlueprintPure, Category = "TGOR System", Meta = (WorldContext = "WorldContextObject", Keywords = "C++"))
		static FString GetSaveDirectory(UObject* WorldContextObject);

	/** Whether we are currently in the initial profile */
	UFUNCTION(BlueprintPure, Category = "TGOR System", Meta = (WorldContext = "WorldContextObject", Keywords = "C++"))
		static bool IsInNullProfile(UObject* WorldContextObject);
	
	/** Currently active profile */
	UPROPERTY(BlueprintReadOnly, Category = "TGOR System")
		FString CurrentProfile;

	/** Name of initial profile before a profile is selected */
	UPROPERTY(BlueprintReadOnly, Category = "TGOR System")
		FString NullProfile;


	/** Creates content class from class (used so references persist over different versions) */
	UFUNCTION(BlueprintPure, Category = "TGOR System", Meta = (Keywords = "C++"))
		UTGOR_Content* ConstructContent(TSubclassOf<UTGOR_Content> Class);

	////////////////////////////////////////////////////////////////////////////////////////////////////
protected:

	/** Pool of content objects to make construction easier */
	UPROPERTY(BlueprintReadOnly, Category = "TGOR System")
		TMap<TSubclassOf<UTGOR_Content>, UTGOR_Content*> ContentPool;

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
