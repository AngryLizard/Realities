// The Gateway of Realities: Planes of Existence.


#include "TGOR_GameInstance.h"
#include "Engine/World.h"

#include "RealitiesUGC/Mod/TGOR_ContentManager.h"

#include "CoreSystem/Components/TGOR_Component.h"
#include "CoreSystem/TGOR_Singleton.h"
#include "CoreSystem/Gameplay/TGOR_WorldSettings.h"
#include "CoreSystem/Gameplay/TGOR_GameState.h"

#include "Engine/Classes/Kismet/GameplayStatics.h"


UTGOR_GameInstance::UTGOR_GameInstance(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer), NullProfile("Null")
{
	CurrentProfile = NullProfile;
}

void UTGOR_GameInstance::Shutdown()
{
	Super::Shutdown();

}


UTGOR_Singleton* UTGOR_GameInstance::EnsureSingleton(const UObject* WorldContextObject) const
{
	if (PrivateSingleton)
	{
		return PrivateSingleton;
	}

	// Create Singleton if invalid
	// TODO: Need to cast from world and back due to const, maybe there is a better way!
	UWorld* World = GetWorld();
	UTGOR_GameInstance* GameInstance = World->GetGameInstance<UTGOR_GameInstance>();
	GameInstance->PrivateSingleton = NewObject<UTGOR_Singleton>(GameInstance);
	Singleton = GameInstance->PrivateSingleton;

	// Grab registry
	//Singleton->LoadLocalModSetup(ModRegistry);
	return GameInstance->PrivateSingleton;
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

void UTGOR_GameInstance::HandleGameNetControlMessage(class UNetConnection* Connection, uint8 MessageByte, const FString& MessageStr)
{
	Super::HandleGameNetControlMessage(Connection, MessageByte, MessageStr);

	SINGLETON_CHK;

	TArray<FString> ModMessages;
	const int32 ModNum = MessageStr.ParseIntoArray(ModMessages, TEXT("/"), true);

	ModSetup.ActiveMods.Empty();
	for (const FString& ModMessage : ModMessages)
	{
		TArray<FString> ModMembers;
		const int32 MemberNum = ModMessage.ParseIntoArray(ModMembers, TEXT("#"), true);
		if (MemberNum == 2)
		{
			FTGOR_ModEntry Entry;
			Entry.Name = ModMembers[0];
			Entry.Version = FCString::Atoi(*ModMembers[1]);
			ModSetup.ActiveMods.Emplace(Entry);
		}
	}

	if (MessageByte != ModSetup.ActiveMods.Num())
	{
		// TODO: Check mod compatibility here
		// Mod loading failed, let's go back to main menu
		GEngine->ShutdownAllNetDrivers();
		SwitchToNullProfile(this);
	}
}


void UTGOR_GameInstance::PreModLoad()
{
	EnsureSingleton(this);
	Singleton->Datas.Empty();
	PrivateSingleton->ContentManager = ContentManager;
}

void UTGOR_GameInstance::PostModLoad(ETGOR_ModloadEnumeration Result)
{
	if (Result == ETGOR_ModloadEnumeration::Success)
	{
		// TODO: Do proper error handling when failed
	}
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
					//GameState->SaveAll();

					// Reload everything if switching profiles
					Instance->CurrentProfile = Profile;
					UGameplayStatics::OpenLevel(WorldContextObject, FName(*World->GetName()), false);
				}
				else
				{
					// Nullprofile guaranties nothing has been loaded yet so we can just restart world assemblage
					Instance->CurrentProfile = Profile;

					GameState->Setup();
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
				//GameState->SaveAll();
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
			//GameState->SaveAll();
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
