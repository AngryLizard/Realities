// The Gateway of Realities: Planes of Existence.


#include "TGOR_GameInstance.h"
#include "Realities/Mod/TGOR_CoreMod.h"

#include "Realities/Components/TGOR_Component.h"
#include "Realities/Base/Content/TGOR_Content.h"
#include "Realities/Base/Content/TGOR_ContentManager.h"
#include "Realities/Base/TGOR_Singleton.h"
#include "Engine/World.h"
#include "Realities/Base/TGOR_WorldSettings.h"
#include "Realities/Base/TGOR_GameState.h"

#include "BasicReplicationGraph.h"
#include "Engine/ReplicationDriver.h"
#include "TGOR_ReplicationGraph.h"
#include "Engine/Classes/Kismet/GameplayStatics.h"


UTGOR_GameInstance::UTGOR_GameInstance(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer), UseCustomReplication(true), NullProfile("Null")
{
	CurrentProfile = NullProfile;
}

void UTGOR_GameInstance::Init()
{
	Super::Init();

	if (UseCustomReplication)
	{
		UReplicationDriver::CreateReplicationDriverDelegate().BindLambda([](UNetDriver* ForNetDriver, const FURL& URL, UWorld* World) -> UReplicationDriver*
		{
			return NewObject<UTGOR_ReplicationGraph>(GetTransientPackage());
		});
	}
}

void UTGOR_GameInstance::Shutdown()
{
	Super::Shutdown();

}


UTGOR_Singleton* UTGOR_GameInstance::EnsureSingleton(UObject* WorldContextObject)
{
	if (PrivateSingleton)
	{
		return PrivateSingleton;
	}

	// Create Singleton if invalid
	PrivateSingleton = UTGOR_Singleton::Construct(WorldContextObject, ModSetup);
	Singleton = PrivateSingleton;

	UWorld* World = WorldContextObject->GetWorld();
	Singleton->LocalModSetup(World);
	return PrivateSingleton;
}

UTGOR_Singleton* UTGOR_GameInstance::EnsureSingletonFromWorld(UWorld* World)
{
	if (!IsValid(World))
	{
		ERRET("World invalid", Error, nullptr)
	}

	// Get Gameinstance
	UTGOR_GameInstance* GameInstance = World->GetGameInstance<UTGOR_GameInstance>();
	if (!IsValid(GameInstance))
	{
		ERRET("GameInstance invalid", Error, nullptr)
	}

	// Ensure singleton
	return(GameInstance->EnsureSingleton(World));
}


AActor* UTGOR_GameInstance::SpawnActorLocally(TSubclassOf<AActor> Class, AActor* Owner, FVector Location, FRotator Rotation)
{
	if (*Class == nullptr) ERRET("Class invalid", Error, nullptr)
	if (!IsValid(Owner)) ERRET("Owner invalid", Error, nullptr)

	UWorld* World = Owner->GetWorld();
	ULevel* Level = World->GetCurrentLevel();
	AActor* Actor = NewObject<AActor>(Level, *Class);

	Level->Actors.Add(Actor);
	Level->ActorsForGC.Add(Actor);
	FTransform Transform(Rotation, Location);
	Actor->PostSpawnInitialize(Transform, Owner, nullptr, false, true, false);

	return(Actor);
}


void UTGOR_GameInstance::DespawnActor(AActor* Actor)
{
	Actor->GetWorld()->DestroyActor(Actor);
}

void UTGOR_GameInstance::SwitchProfile(UObject* WorldContextObject, const FString& Profile)
{
	UWorld* World = WorldContextObject->GetWorld();
	UTGOR_GameInstance* Instance = Cast<UTGOR_GameInstance>(World->GetGameInstance());
	if (IsValid(Instance))
	{
		ATGOR_GameState* GameState = Cast<ATGOR_GameState>(World->GetGameState());
		if (IsValid(GameState))
		{
			if (Instance->CurrentProfile != Profile)
			{
				if (Instance->CurrentProfile != Instance->NullProfile)
				{
					// Save before switch
					GameState->SaveAll();

					// Reload everything if switching profiles
					Instance->CurrentProfile = Profile;
					UGameplayStatics::OpenLevel(WorldContextObject, FName(*World->GetName()), false);
				}
				else
				{
					// Nullprofile guaranties nothing has been loaded yet so we can just restart world assemblage
					Instance->CurrentProfile = Profile;

					GameState->ReloadResources();
				}
			}
		}
	}
	else
	{
		ERROR("Cannot find GameInstance, profile wasn't changed.", Error);
	}
}

void UTGOR_GameInstance::SwitchToNullProfile(UObject* WorldContextObject)
{
	UWorld* World = WorldContextObject->GetWorld();
	UTGOR_GameInstance* Instance = Cast<UTGOR_GameInstance>(World->GetGameInstance());
	if (IsValid(Instance))
	{
		if (Instance->CurrentProfile != Instance->NullProfile)
		{
			ATGOR_GameState* GameState = Cast<ATGOR_GameState>(World->GetGameState());
			if (IsValid(GameState))
			{
				// Save before switch
				GameState->SaveAll();
			}

			// Reload everything if switching profiles
			Instance->CurrentProfile = Instance->NullProfile;
			UGameplayStatics::OpenLevel(WorldContextObject, FName(*World->GetName()), false);
		}
	}
	else
	{
		ERROR("Cannot find GameInstance, profile wasn't changed.", Error);
	}
}


void UTGOR_GameInstance::SaveAll(UObject* WorldContextObject)
{
	UWorld* World = WorldContextObject->GetWorld();
	UTGOR_GameInstance* Instance = Cast<UTGOR_GameInstance>(World->GetGameInstance());
	if (IsValid(Instance))
	{
		ATGOR_GameState* GameState = Cast<ATGOR_GameState>(World->GetGameState());
		if (IsValid(GameState))
		{
			GameState->SaveAll();
		}
	}
}

FString UTGOR_GameInstance::GetCurrentProfile(UObject* WorldContextObject)
{
	UWorld* World = WorldContextObject->GetWorld();
	UTGOR_GameInstance* Instance = Cast<UTGOR_GameInstance>(World->GetGameInstance());
	if (IsValid(Instance))
	{
		return Instance->CurrentProfile;
	}
	else
	{
		ERRET("Cannot find GameInstance, path couldn't be evaluated.", Error, "");
	}
}

FString UTGOR_GameInstance::GetSaveDirectory(UObject* WorldContextObject)
{
	UWorld* World = WorldContextObject->GetWorld();
	UTGOR_GameInstance* Instance = Cast<UTGOR_GameInstance>(World->GetGameInstance());
	if (IsValid(Instance))
	{
		ATGOR_WorldSettings* Settings = Cast<ATGOR_WorldSettings>(World->GetWorldSettings());
		if (IsValid(Settings))
		{
			const FString GameDir = FPaths::ProjectSavedDir();
			return GameDir / Settings->SaveDirectory / Instance->CurrentProfile;
		}
		else
		{
			ERRET("Cannot find WorldSettings, path couldn't be evaluated.", Error, "");
		}
	}
	else
	{
		ERRET("Cannot find GameInstance, path couldn't be evaluated.", Error, "");
	}
}

bool UTGOR_GameInstance::IsInNullProfile(UObject* WorldContextObject)
{
	UWorld* World = WorldContextObject->GetWorld();
	UTGOR_GameInstance* Instance = Cast<UTGOR_GameInstance>(World->GetGameInstance());
	if (IsValid(Instance))
	{
		return (Instance->CurrentProfile == Instance->NullProfile);
	}
	return false;
}


UTGOR_Content* UTGOR_GameInstance::ConstructContent(TSubclassOf<UTGOR_Content> Class)
{
	UTGOR_Content*& Content = ContentPool.FindOrAdd(Class);
	if (IsValid(Content))
	{
		Content->Reset();
	}
	else if(*Class)
	{
		Content = NewObject<UTGOR_Content>(this, Class);
	}
	else
	{
		Content = nullptr;
	}
	return Content;
}