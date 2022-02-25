// The Gateway of Realities: Planes of Existence.


#include "TGOR_GameState.h"
#include "Net/UnrealNetwork.h"

#include "Realities/Base/TGOR_Singleton.h"
#include "Realities/Base/Content/TGOR_ContentManager.h"
#include "Realities/Base/System/Data/TGOR_WorldData.h"
#include "Realities/Base/System/Data/TGOR_DimensionData.h"
#include "Realities/Base/System/Data/TGOR_ConfigData.h"
#include "Realities/Base/System/Data/TGOR_UserData.h"
#include "Realities/Base/TGOR_GameInstance.h"

#include "GameFramework/SpectatorPawn.h"
#include "GameFramework/HUD.h"
#include "Realities/Actors/Pawns/TGOR_Pawn.h"
#include "Realities/Actors/Pawns/TGOR_Spectator.h"
#include "Realities/Base/Controller/TGOR_OnlineController.h"
#include "Realities/Mod/Spawner/Creatures/TGOR_Creature.h"
#include "Realities/Mod/Dimensions/TGOR_Dimension.h"
#include "EngineUtils.h"

#include "Realities/Base/TGOR_WorldSettings.h"
#include "Realities/Base/TGOR_GameInstance.h"

ATGOR_GameState::ATGOR_GameState()
:	IgnorePlayerController(false),
	LoadTimeout(2.0f),
	HasLoaded(false),
	LoadStage(ETGOR_LoadStage::None)
{
	LoadInformation.HasLoaded = false;
}


void ATGOR_GameState::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// Replicate to everyone
	DOREPLIFETIME(ATGOR_GameState, LoadInformation);
	DOREPLIFETIME(ATGOR_GameState, DimensionUpdateState);
}

void ATGOR_GameState::ReloadWorld()
{
	LoadStage = ETGOR_LoadStage::LoadConfig;
}

void ATGOR_GameState::ReloadResources()
{
	LoadStage = ETGOR_LoadStage::LoadMods;
}




float ATGOR_GameState::Progress(ETGOR_BoolEnumeration& Branches)
{
	Branches = ETGOR_BoolEnumeration::IsNot;
	
	// Fallthrough on success, otherwise return progress
	switch (LoadStage)
	{
		case ETGOR_LoadStage::None:														LoadStage = ETGOR_LoadStage::LoadMods;
		case ETGOR_LoadStage::LoadMods:					if (LoadMods())					LoadStage = ETGOR_LoadStage::AwaitPossessed;		else	return(0.0f);
		case ETGOR_LoadStage::AwaitPossessed:			if (AwaitPossessed())			LoadStage = ETGOR_LoadStage::Assemble;				else	return(0.1f);
		case ETGOR_LoadStage::Assemble:					if (Assemble())					LoadStage = ETGOR_LoadStage::LoadConfig;			else	return(0.2f);
		case ETGOR_LoadStage::LoadConfig:				if (LoadConfig())				LoadStage = ETGOR_LoadStage::LoadPersistent;		else	return(0.3f);
		case ETGOR_LoadStage::LoadPersistent:			if (LoadPersistent())			LoadStage = ETGOR_LoadStage::AwaitDimensionLoading;	else	return(0.4f);
		case ETGOR_LoadStage::AwaitDimensionLoading:	if (AwaitDimensionLoading())	LoadStage = ETGOR_LoadStage::AwaitBeginPlay;		else	return(0.5f);
		case ETGOR_LoadStage::AwaitBeginPlay:			if (AwaitBeginPlay())			LoadStage = ETGOR_LoadStage::LoadWorld;				else	return(0.6f);
		case ETGOR_LoadStage::LoadWorld:				if (LoadWorld())				LoadStage = ETGOR_LoadStage::Login;					else	return(0.7f);
		case ETGOR_LoadStage::Login:					if (Login())					LoadStage = ETGOR_LoadStage::PostSpawn;				else	return(0.8f);
		case ETGOR_LoadStage::PostSpawn:				if (PostSpawn())				LoadStage = ETGOR_LoadStage::Finished;				else	return(0.9f);
		default: LoadInformation.HasLoaded = true;										Branches = ETGOR_BoolEnumeration::Is;						return(1.0f);
	}
}

bool ATGOR_GameState::HasFinishedLoading() const
{
	if (LoadStage != ETGOR_LoadStage::Finished) return false;
	if (!Singleton) return false;
	ATGOR_OnlineController* OnlineController = Cast<ATGOR_OnlineController>(LocalController);
	if (IsValid(OnlineController))
	{
		return OnlineController->IsDimensionListUpToDate();
	}
	return false;
}

bool ATGOR_GameState::LoadMods()
{
	// Wait until world is valid
	UWorld* World = GetWorld();
	if (!IsValid(World))
	{
		return false;
	}

	#if !UE_BUILD_SHIPPING
	// Override profile
	ATGOR_WorldSettings* Settings = Cast<ATGOR_WorldSettings>(World->GetWorldSettings());
	if (IsValid(Settings))
	{
		if (!Settings->ProfileOverride.IsEmpty())
		{
			UTGOR_GameInstance* Instance = Cast<UTGOR_GameInstance>(World->GetGameInstance());
			if (IsValid(Instance))
			{
				Instance->CurrentProfile = Settings->ProfileOverride;
			}
		}
	}
	#endif

	SINGLETON_RETCHK(false);
	ETGOR_ModloadEnumeration ModLoad = ETGOR_ModloadEnumeration::Success;

	// Load locally if server, load received if client
	if (HasAuthority())
	{
		// Already done in Construct: Singleton->LocalModSetup(World);
		LoadInformation.ModSetup = Singleton->ModSetup;
	}
	else
	{
		// Only load if information has been received by server already
		if (!LoadInformation.HasLoaded)
		{
			return false;
		}

		ModLoad = Singleton->LoadModSetup(World, LoadInformation.ModSetup);
	}

	// Notify gamestate
	ModsLoadComplete(ModLoad);

	return ModLoad == ETGOR_ModloadEnumeration::Success;
}


bool ATGOR_GameState::AwaitPossessed()
{
	// Wait until world is valid
	UWorld* World = GetWorld();
	if (!IsValid(World))
	{
		return false;
	}

	// No login is required on dedicated server
	if (IsRunningDedicatedServer())
	{
		return true;
	}

	// wait for player controller
	LocalController = World->GetFirstPlayerController();
	if (!IsValid(LocalController))
	{
		return IgnorePlayerController;
	}

	const bool IsLocalController = LocalController->IsLocalPlayerController();
	if (!IsLocalController)
	{
		return true;
	}

	// Wait until HUD has loaded in
	if (!IsValid(LocalController->GetHUD()))
	{
		return false;
	}
	return true;
}

bool ATGOR_GameState::Assemble()
{
	// Wait until world is valid
	UWorld* World = GetWorld();
	if (!IsValid(World))
	{
		return false;
	}

	SINGLETON_RETCHK(false);

	// Set up database
	Singleton->WorldData->AssembleContentManager(GetWorld());

	return true;
}

bool ATGOR_GameState::LoadConfig()
{
	SINGLETON_RETCHK(false);

	UWorld* World = GetWorld();
	if (IsValid(World))
	{
		// Load local files for client and server
		Singleton->ConfigData->SetFilename("Config");
		Singleton->ConfigData->LoadFromFile();
		LoadLocalFiles();

		if (HasAuthority())
		{
			Singleton->UserData->SetFilename("Users");
			Singleton->UserData->LoadFromFile();
			LoadServerFiles();
		}
	}

	return true;
}

bool ATGOR_GameState::LoadPersistent()
{
	SINGLETON_RETCHK(false);

	// Load persistent level (for in editor play)
	UWorld* World = GetWorld();
	ATGOR_WorldSettings* Settings = Cast<ATGOR_WorldSettings>(World->GetWorldSettings());
	if (IsValid(Settings))
	{
		// Load persistent
		if (*Settings->Dimension)
		{
			UTGOR_Dimension* Dimension = Singleton->ContentManager->GetTFromType<UTGOR_Dimension>(Settings->Dimension);
			if (IsValid(Dimension))
			{
				Singleton->WorldData->AssemblePersistent(Dimension, HasAuthority());
				const FName PersistentIdentifier = Singleton->WorldData->GetPersistentIdentifier();
				AppendDimension(PersistentIdentifier, Dimension);
			}
			else
			{
				ERRET("Persistent dimension not found", Error, false);
			}

			ATGOR_OnlineController* OnlineController = Cast<ATGOR_OnlineController>(LocalController);
			if (IsValid(OnlineController))
			{
				OnlineController->EnsureSingleton(this);
				if (HasAuthority())
				{
					// Server doesn't need approval
					OnlineController->DimensionUpdateList.IsServerApproved = true;
				}
				else
				{
					// Ask server for dimension update
					OnlineController->RequestInitialDimensionUpdate();
				}
			}
		}
	}

	return true;
}

bool ATGOR_GameState::AwaitDimensionLoading()
{
	SINGLETON_RETCHK(false)

	// Wait for server to send dimension update list
	ATGOR_OnlineController* OnlineController = Cast<ATGOR_OnlineController>(LocalController);
	if (IsValid(OnlineController) )
	{
		// Wait until all dimensions are loaded and ready
		if (!OnlineController->IsDimensionListUpToDate())
		{
			OnlineController->UpdateDimensionList();
			return false;
		}
	}
	return true;
}


bool ATGOR_GameState::AwaitBeginPlay()
{
	// Call BeginPlay if necessary
	UWorld* World = GetWorld();
	if (IsValid(World))
	{
		if (!World->bBegunPlay)
		{
			for (FActorIterator It(World); It; ++It)
			{
				It->DispatchBeginPlay(true);
			}
			World->bBegunPlay = true;
		}
	}
	World->SetShouldTick(true);
	return true;
}

bool ATGOR_GameState::LoadWorld()
{
	SINGLETON_RETCHK(false)

	UWorld* World = GetWorld();
	if (!IsValid(World))
	{
		return false;
	}

	if (!IsValid(World->GetGameState()))
	{
		return false;
	}

	if (HasAuthority())
	{
		// Load server only files
		Singleton->WorldData->LoadWorldFromFile();
	}
	return true;
}

bool ATGOR_GameState::Login()
{
	SINGLETON_RETCHK(false);

	// No login is required on dedicated server or when in menu mode
	if (IsRunningDedicatedServer())
	{
		return true;
	}

	// Only tries to login with online controllers but doesn't require it
	ATGOR_OnlineController* OnlineController = Cast<ATGOR_OnlineController>(LocalController);
	if (!IsValid(OnlineController))
	{
		return true;
	}

	UWorld* World = GetWorld();
	ATGOR_WorldSettings* Settings = Cast<ATGOR_WorldSettings>(World->GetWorldSettings());
	if (!IsValid(Settings))
	{
		return false;
	}

	if (Settings->TestEntry)
	{
		// When testing we can assume login to happen immediately
		OnlineController->RequestLogin();
	}
	else
	{
		// No login is required in null profile and not currently testing
		if (UTGOR_GameInstance::IsInNullProfile(this))
		{
			return true;
		}

		// Login if an account exists, create new account if not
		OnlineController->RequestLogin();
	}

	return true;
}

bool ATGOR_GameState::PostSpawn()
{
	UWorld* World = GetWorld();
	ATGOR_WorldSettings* Settings = Cast<ATGOR_WorldSettings>(World->GetWorldSettings());
	if (!IsValid(Settings))
	{
		return false;
	}

	if (Settings->TestEntry)
	{
		// If there was no save, spawn default pawn immediately
		APawn* Pawn = LocalController->GetPawn();
		ATGOR_Spectator* Spectator = Cast<ATGOR_Spectator>(Pawn);
		if (IsValid(Spectator))
		{
			FTGOR_CreatureSetupInstance Setup;
			Setup.Creature = Singleton->ContentManager->GetTFromType<UTGOR_Creature>(Settings->TestCreature);
			Spectator->RequestNewBody(Setup);
			return true;
		}
		return IsValid(Pawn);
	}
	return true;
}


void ATGOR_GameState::SaveAll()
{
	SINGLETON_CHK;
	
	UWorld* World = GetWorld();
	if (IsValid(World))
	{
		if (HasAuthority())
		{
			SaveServerFiles();
			Singleton->WorldData->SaveWorldToFile();

			Singleton->UserData->SetFilename("Users");
			Singleton->UserData->SaveToFile();
		}

		SaveLocalFiles();
		Singleton->ConfigData->SetFilename("Config");
		Singleton->ConfigData->SaveToFile();
	}

	EOUT("File saved!");
}

bool ATGOR_GameState::AppendDimension(const FName& Identifier, UTGOR_Dimension* Content)
{
	// Set dimensions to load
	ATGOR_OnlineController* OnlineController = Cast<ATGOR_OnlineController>(LocalController);
	if (IsValid(OnlineController))
	{
		OnlineController->AddDimensionUpdate(Identifier, Content);
		return true;
	}
	return false;
}

bool ATGOR_GameState::LoadDimension(TSubclassOf<UTGOR_Dimension> Current, const FName& Identifier, TSubclassOf<UTGOR_Dimension> Class)
{
	// Set dimensions to load
	if (*Class && !Current->IsChildOf(Class))
	{
		// Get first Hub dimension
		UTGOR_Dimension* Dimension = GetFirstOfType(Class);
		if (IsValid(Dimension))
		{
			return(AppendDimension(Identifier, Dimension));
		}
		else
		{
			ERRET(FString("No dimension found for ") + Class->GetName(), Error, false)
		}
	}
	return false;
}

UTGOR_Dimension* ATGOR_GameState::GetFirstOfType(TSubclassOf<UTGOR_Dimension> Class) const
{
	TArray<UTGOR_Dimension*> Dimensions = Singleton->ContentManager->GetTListFromType<UTGOR_Dimension>(Class);
	if (Dimensions.Num() > 0)
	{
		return Dimensions[0];
	}
	return nullptr;
}