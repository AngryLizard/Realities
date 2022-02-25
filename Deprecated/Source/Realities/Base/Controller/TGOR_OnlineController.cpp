// The Gateway of Realities: Planes of Existence.


#include "TGOR_OnlineController.h"

#include "Realities/Base/TGOR_Singleton.h"
#include "Realities/Base/System/Data/TGOR_UserData.h"
#include "Realities/Base/System/Data/TGOR_ConfigData.h"
#include "Realities/Base/System/Data/TGOR_WorldData.h"
#include "Realities/Base/System/Data/TGOR_DimensionData.h"
#include "Realities/Base/TGOR_GameInstance.h"
#include "Realities/Base/TGOR_GameState.h"
#include "Realities/Base/TGOR_PlayerState.h"
#include "Realities/Actors/Pawns/TGOR_Pawn.h"
#include "Realities/Actors/Pawns/TGOR_Avatar.h"
#include "Realities/Actors/Pawns/TGOR_Spectator.h"
#include "Realities/Components/Player/TGOR_UnlockComponent.h"

#include "Realities/Mod/Dimensions/TGOR_Dimension.h"
#include "Realities/Mod/Channels/TGOR_Channel.h"
#include "Realities/Mod/Commands/TGOR_Command.h"
#include "Realities/Mod/Knowledges/TGOR_Knowledge.h"
#include "Realities/Mod/Knowledges/TGOR_RootKnowledge.h"

#include "Engine/Engine.h"
#include "Engine/NetDriver.h"
#include "Net/UnrealNetwork.h"

#define LOCTEXT_NAMESPACE "OnlineController"

ATGOR_OnlineController::ATGOR_OnlineController(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	OnlineUsersUpdateTime = 5.0f;
	TrackerUpdateTime = 1.0f;

	TimestampSyncTime = 1.0f;
	TimestampAdapt = 0.0f;

	ActiveUserKey = -1;
	MaxCameraSwitchDistance = 10'000.0f;

	PawnCamera = nullptr;
	CameraBlendTime = 1.0f;

	MaxContentLength = 0xFF;
	Singleton = nullptr;

	PlayerNotFound = LOCTEXT("OnlineController_PlayerNotFound", "Player not found.");
	CommandNotFound = LOCTEXT("OnlineController_CommandNotFound", "Command not found.");
	CommandDefaultError = LOCTEXT("OnlineController_CommandDefaultError", "There was an error executing the command.");
	ChannelNotFound = LOCTEXT("OnlineController_ChannelNotFound", "Channel not found.");
	InsufficientPrivileges = LOCTEXT("OnlineController_InsufficientPrivileges", "Insufficient user privileges.");


	UnlockComponent = ObjectInitializer.CreateDefaultSubobject<UTGOR_UnlockComponent>(this, FName(TEXT("UnlockComponent")));
	UnlockComponent->SetNetAddressable();
	UnlockComponent->SetIsReplicated(true);
}

void ATGOR_OnlineController::BeginPlay()
{
	Super::BeginPlay();

	SINGLETON_CHK;
	const FTGOR_Time Timestamp = Singleton->GetGameTimestamp();
	LastOnlineUsersUpdate = Timestamp;
	LastTrackerUpdate = Timestamp;
}


void ATGOR_OnlineController::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (!HasAuthority()) return;
	Logout_Implementation();
}

//FTGOR_Time tst;

void ATGOR_OnlineController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// This tick can happen very early and we don't want Singletoncheck to produce warnings
	UWorld* World = GetWorld();
	if (!IsValid(World->GetGameInstance()) ||
		!IsValid(World->GetGameState())) return;

	SINGLETON_CHK;

	if (HasAuthority())
	{
		// Update tracker
		UpdateTrackerlist();

		// Update online user list
		UpdateUserlist();

		// Handle outstanding possessions
		if (HasPossessionQueue())
		{
			ProgressPossessionQueue();
		}
	}

	// Control local players on client and all on server
	if (IsLocalController() || HasAuthority())
	{
		if (!IsDimensionListUpToDate())
		{
			UpdateDimensionList();
		}
		else if (IsLocalController() && !IsDimensionOwnerUpToDate())
		{
			UpdateDimensionOwner();
		}
	}
}

void ATGOR_OnlineController::PlayerTick(float DeltaTime)
{
	Super::PlayerTick(DeltaTime);

	// This tick specifically can happen very early and we don't want Singletoncheck to produce warnings
	UWorld* World = GetWorld();
	if (!IsValid(World->GetGameInstance()) ||
		!IsValid(World->GetGameState())) return;

	SINGLETON_CHK;
	

	// Test timestamp
	/*
	if (Role == ROLE_AutonomousProxy)
	{
		RPTCK(FString("Adapted: ") + FString::SanitizeFloat(Singleton->GameTimestamp - tst))
	}
	if (Role == ROLE_Authority)
	{
		tst = Singleton->GameTimestamp;
	}
	*/

	// Update timestamp
	Singleton->GameTimestamp += DeltaTime;
	Singleton->GameTimestamp.Normalize();

	if (GetLocalRole() == ROLE_AutonomousProxy)
	{
		TimestampAdapt -= DeltaTime;
		if (TimestampAdapt <= 0.0f)
		{
			SyncTimestampServer(Singleton->GameTimestamp);
			TimestampAdapt = TimestampSyncTime;
		}
	}
}

void ATGOR_OnlineController::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(ATGOR_OnlineController, OnlineUsers, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(ATGOR_OnlineController, DimensionUpdateList, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(ATGOR_OnlineController, HasPendingDeletion, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(ATGOR_OnlineController, BodyDisplay, COND_OwnerOnly);
}

void ATGOR_OnlineController::PawnLeavingGame()
{
	ATGOR_Pawn* C = Cast<ATGOR_Pawn>(GetPawn());
	if (IsValid(C) && !C->DespawnsOnLogout())
	{
		UnPossess();
		SetPawn(nullptr);
	}
	else
	{
		Super::PawnLeavingGame();
	}
}

void ATGOR_OnlineController::SetPawn(APawn* InPawn)
{
	// Switch camera
	if (IsValid(InPawn))
	{
		UTGOR_CameraComponent* OldCamera = PawnCamera;
		PawnCamera = Cast<UTGOR_CameraComponent>(InPawn->GetComponentByClass(UTGOR_CameraComponent::StaticClass()));
		if (IsValid(OldCamera) && IsValid(PawnCamera) && OldCamera != PawnCamera)
		{
			const FVector Source = OldCamera->GetComponentLocation();
			const FVector Target = PawnCamera->GetComponentLocation();
			const bool Teleport = (Source - Target).SizeSquared() > MaxCameraSwitchDistance * MaxCameraSwitchDistance;

			OldCamera->CopyToCamera(PawnCamera, CameraBlendTime, Teleport);
		}
	}

	Super::SetPawn(InPawn);
}

////////////////////////////////////////////////////////////////////////////////////////////////////


void ATGOR_OnlineController::GetNetOccupation(ETGOR_NetOccupation& Branches)
{
	Branches = ETGOR_NetOccupation::Invalid;

	if (IsLocalPlayerController())
	{
		if (HasAuthority())
		{
			Branches = ETGOR_NetOccupation::Host;
		}
		else
		{
			Branches = ETGOR_NetOccupation::Client;
		}
	}
	else
	{
		if (HasAuthority())
		{
			Branches = ETGOR_NetOccupation::Client;
		}
		else
		{
			Branches = ETGOR_NetOccupation::Remote;
		}
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////

bool ATGOR_OnlineController::OnLoginSetup_Implementation(int32 Key, FTGOR_UserProperties& User, bool IsNewUser)
{
	return true;
}

bool ATGOR_OnlineController::OnRegistrationSetup_Implementation(FTGOR_UserProperties& User)
{
	User.Serial = CreateUniqueSerial();
	return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

int32 ATGOR_OnlineController::CreateUniqueSerial() const
{
	// Try random serial number until an unused one has been found.
	const int32 Serial = rand() % 1000;

	// Try again if serial already exists
	if (HasSerial(Serial))
	{
		// TODO: Theoretically can go forever if the registered player count exceeds 1000
		return CreateUniqueSerial();
	}
	return Serial;
}

bool ATGOR_OnlineController::HasSerial(int32 Serial) const
{
	if (!IsValid(Singleton)) return(false);
	UTGOR_UserData* UserData = Singleton->GetUserData();
	return(!UserData->ForEachUser([Serial](FTGOR_UserInstance& Instance) -> bool
	{
		return Instance.Properties.Serial != Serial;
	}));
}

bool ATGOR_OnlineController::IsOnline() const
{
	if (!IsValid(Singleton)) return(false);
	UTGOR_UserData* UserData = Singleton->GetUserData();

	if (UserData->HasUser(ActiveUserKey))
	{
		const FTGOR_UserInstance& User = UserData->GetUser(ActiveUserKey);
		return User.Controller == this;
	}
	return false;
}

ETGOR_RenameResponse ATGOR_OnlineController::RenameUser(const FString& NewName)
{
	SINGLETON_RETCHK(ETGOR_RenameResponse::Invalid)
	UTGOR_UserData* UserData = Singleton->GetUserData();

	if (UserData->HasUser(ActiveUserKey))
	{
		FTGOR_UserInstance& User = UserData->GetUser(ActiveUserKey);
		if (NewName.Len() > 50)
		{
			return ETGOR_RenameResponse::TooLong;
		}
		User.Properties.Name = NewName;
		OnUserUpdate.Broadcast();
	}
	return ETGOR_RenameResponse::Success;
}

void ATGOR_OnlineController::PossessSpectator()
{
	SINGLETON_CHK;

	// Spawn new spectator if not already spectating
	APawn* Previous = GetPawn();
	if (!IsValid(Previous) || !Previous->IsA(ATGOR_Spectator::StaticClass()))
	{
		// Get persistent dimension to spawn spectator into
		ETGOR_FetchEnumeration State;
		UTGOR_WorldData* WorldData = Singleton->GetWorldData();
		const FName PersistentIdentifier = WorldData->GetPersistentIdentifier();
		UTGOR_DimensionData* Persistent = WorldData->GetDimension(PersistentIdentifier, State);
		if (State == ETGOR_FetchEnumeration::Found)
		{
			// Create new spectator and go into spectator state
			AActor* PlayerStart = StartSpot.Get();
			AGameModeBase* GameModeBase = GetWorld()->GetAuthGameMode();
			if (!PlayerStart) PlayerStart = GameModeBase->FindPlayerStart(this);
			ATGOR_Spectator* Spectator = Cast<ATGOR_Spectator>(GameModeBase->SpawnDefaultPawnFor(this, PlayerStart));
			if (IsValid(Spectator))
			{
				Persistent->RegisterDimensionObject(Spectator->GetIdentity());
				Possess(Spectator);
				UnlockUpdate(Spectator);
				SetDimensionOwner(PersistentIdentifier);
				UpdateBodyDisplay();

			}
		}
	}
}

void ATGOR_OnlineController::PossessUserBody()
{
	UpdateBodyDisplay();

	SINGLETON_CHK;
	UTGOR_UserData* UserData = Singleton->GetUserData();
	if (UserData->HasUser(ActiveUserKey))
	{
		const FTGOR_UserInstance& User = UserData->GetUser(ActiveUserKey);

		// Possess user
		if (BodyDisplay.Num() > 0)
		{
			if (User.CurrentBody == -1)
			{
				RequestBodySwitch(BodyDisplay[0].Identifier);
			}
			else if (User.Bodies.Contains(User.CurrentBody))
			{
				RequestBodySwitch(User.CurrentBody);
			}
			else
			{
				PossessSpectator();
				OnPossessionResponse(ETGOR_PossessionResponse::NotFound);
			}
		}
		else
		{
			PossessSpectator();
			OnPossessionResponse(ETGOR_PossessionResponse::Invalid);
		}
	}
}

void ATGOR_OnlineController::LoginAccept_Implementation(int32 Key, ETGOR_LoginResponse Response)
{
	SINGLETON_CHK;

	UTGOR_ConfigData* ConfigData = Singleton->GetConfigData();

	// Get user map data
	FTGOR_UserMapData& UserData = ConfigData->GetUserData();

	// Set user key
	ActiveUserKey = Key;
	UserData.UserKey = Key;
	OnLoginResponse(Response);
	OnUserUpdate.Broadcast();
}


void ATGOR_OnlineController::Authentication_Implementation(int32 Key, const FTGOR_Buffer& Message)
{
	SINGLETON_CHK;
	UTGOR_UserData* UserData = Singleton->GetUserData();

	// Get user
	if (UserData->HasUser(Key))
	{
		FTGOR_UserInstance& User = UserData->GetUser(Key);

		// Make sure there is an ongoing login
		if (!User.Password.HasData())
		{
			RPORT("There is no onging login");
			LoginAccept(-1, ETGOR_LoginResponse::Invalid);
			return;
		}

		// Make sure user isn't already online
		if (User.Controller.IsValid())
		{
			RPORT("User logging in twice at the same time");
			LoginAccept(-1, ETGOR_LoginResponse::Invalid);
			return;
		}

		// Decrypt password
		FTGOR_Buffer Password = UTGOR_UserData::Decrypt(Message, User.PrivateKey);

		// Check if password matches 
		if (User.Password == Password)
		{
			// Invalidate password
			User.Password.Clear();

			if (OnLoginSetup(Key, User.Properties, false))
			{
				ActiveUserKey = Key;
				User.Controller = this;
				UnlockComponent->ResetKnowledgeTree(User.Trackers);
				UpdateUserlist(true);
				PossessUserBody();

				// Notify client
				LoginAccept(Key, ETGOR_LoginResponse::Success);
			}
			else
			{
				RPORT("User login rejected");
			}
			
			return;
		}
	}

	RPORT("User doesn't exist");
	LoginAccept(-1, ETGOR_LoginResponse::Invalid);
}

bool ATGOR_OnlineController::Authentication_Validate(int32 Key, const FTGOR_Buffer& Message)
{
	return(true);
}

void ATGOR_OnlineController::RequestAuthentication_Implementation(const FTGOR_Buffer& Message, const FTGOR_Buffer& PublicKey)
{
	SINGLETON_CHK;
	UTGOR_ConfigData* ConfigData = Singleton->GetConfigData();

	// Get user map data
	FTGOR_UserMapData& UserMapData = ConfigData->GetUserData();

	// Relais password back to server with own encryption to prevent 3rd party from deducting password from answer
	const FTGOR_Buffer& Password = UTGOR_UserData::Decrypt(Message, UserMapData.PrivateKey);
	const FTGOR_Buffer& Answer = UTGOR_UserData::Encrypt(Password, PublicKey);
	Authentication(UserMapData.UserKey, Answer);
}

void ATGOR_OnlineController::Logout_Implementation()
{
	if (ActiveUserKey != -1)
	{
		// Get user
		UTGOR_UserData* UserData = Singleton->GetUserData();
		if (UserData->HasUser(ActiveUserKey))
		{
			FTGOR_UserInstance& User = UserData->GetUser(ActiveUserKey);
			User.Controller = nullptr;
			RPORT(FString("User disconnected: ") + User.Properties.Name)
		}

		PossessSpectator();
		OnUserUpdate.Broadcast();
	}
}

bool ATGOR_OnlineController::Logout_Validate()
{
	return true;
}

void ATGOR_OnlineController::Login_Implementation(int32 Key, const FTGOR_Buffer& PublicKey)
{
	SINGLETON_CHK
	UTGOR_UserData* UserData = Singleton->GetUserData();

	// Create on registration
	if (Key == -1)
	{
		// Logout player first in case we are already logged in
		Logout_Implementation();

		// Store user under new key
		FTGOR_UserInstance User = FTGOR_UserInstance();

		if (OnRegistrationSetup(User.Properties))
		{
			User.PublicKey = PublicKey;
			User.PrivateKey = UTGOR_UserData::CreatePrivateKey();
			User.Controller = this;
			ActiveUserKey = UserData->StoreUser(User);

			// Send accepted message
			if (ActiveUserKey != -1)
			{
				if (OnLoginSetup(ActiveUserKey, User.Properties, true))
				{
					UnlockComponent->ResetKnowledgeTree(User.Trackers);
					UpdateUserlist(true);
					PossessUserBody();
					LoginAccept(ActiveUserKey, ETGOR_LoginResponse::Created);
				}
				else
				{
					RPORT("Login setup failed")
					LoginAccept(-1, ETGOR_LoginResponse::Invalid);
				}
				return;
			}
		}

		RPORT("Register setup failed")
		LoginAccept(-1, ETGOR_LoginResponse::Invalid);
		return;
	}

	// Get user
	if (UserData->HasUser(Key))
	{
		FTGOR_UserInstance& User = UserData->GetUser(Key);
	
		// No need to login if already logged in
		if (Key == ActiveUserKey && User.Controller.IsValid())
		{
			return;
		}

		// Logout player first in case we are already logged in
		Logout_Implementation();

		// Check if public keys match
		if (PublicKey != User.PublicKey)
		{
			RPORT("Public keys don't match")
			LoginAccept(-1, ETGOR_LoginResponse::Invalid);
			return;
		}

		// Check if user is already online
		if (User.Controller.IsValid())
		{
			RPORT("User is already online")
			LoginAccept(-1, ETGOR_LoginResponse::Invalid);
			return;
		}

		// Send request to client
		User.Password = UTGOR_UserData::CreatePassword();
		const FTGOR_Buffer& Message = UTGOR_UserData::Encrypt(User.Password, PublicKey);
		const FTGOR_Buffer& Remote = UTGOR_UserData::CreatePublicKey(User.PrivateKey);
		RequestAuthentication(Message, Remote);
		return;
	}

	RPORT("User doesn't exist")
	LoginAccept(-1, ETGOR_LoginResponse::Invalid);
}

bool ATGOR_OnlineController::Login_Validate(int32 Key, const FTGOR_Buffer& PublicKey)
{
	return(true);
}

void ATGOR_OnlineController::RequestLogin()
{
	SINGLETON_CHK;
	UnlockComponent->ClearKnowledgeTree();

	UTGOR_ConfigData* ConfigData = Singleton->GetConfigData();
	
	FTGOR_UserMapData& UserMapData = ConfigData->GetUserData();
	FTGOR_Buffer PublicKey = UTGOR_UserData::CreatePublicKey(UserMapData.PrivateKey);
	Login(UserMapData.UserKey, PublicKey);
}



////////////////////////////////////////////////////////////////////////////////////////////////////

int32 ATGOR_OnlineController::GetMaxContentLength() const
{ 
	return MaxContentLength;
}

const TArray<FTGOR_UserInfo>& ATGOR_OnlineController::GetOnlineUsers() const
{ 
	return OnlineUsers; 
}

void ATGOR_OnlineController::UpdateUserlist(bool Force)
{
	SINGLETON_CHK;
	UTGOR_UserData* UserData = Singleton->GetUserData();
	const FTGOR_Time GameTimestamp = Singleton->GetGameTimestamp();

	if (Force || GameTimestamp > LastOnlineUsersUpdate + OnlineUsersUpdateTime)
	{
		// Reset online user list
		OnlineUsers.Empty();

		// Find playercontroller
		UWorld* World = GetWorld();
		auto Iterator = World->GetPlayerControllerIterator();
		for (; Iterator; Iterator++)
		{
			ATGOR_OnlineController* Controller = Cast<ATGOR_OnlineController>(*Iterator);
			if (IsValid(Controller))
			{
				// Get controllable characters
				ATGOR_Pawn* C = Cast<ATGOR_Pawn>(Controller->GetPawn());
				if (IsValid(C))
				{
					// Check if names match
					if (UserData->HasUser(Controller->ActiveUserKey))
					{
						const FTGOR_UserInstance& Recipient = UserData->GetUser(Controller->ActiveUserKey);

						// Build info
						FTGOR_UserInfo Info;
						Info.Properties = Recipient.Properties;
						Info.Ping = Controller->PlayerState->ExactPing;
						Info.Location = C->GetActorLocation();

						OnlineUsers.Add(Info);
					}
				}
			}
		}
	}
}



void ATGOR_OnlineController::SendChatMessageToPlayer(int32 UserSerial, const FString& MessageText)
{
	FTGOR_ChatMessageInstance Message;
	Message.Message = MessageText;
	Message.UserSerial = UserSerial;
	Message.Domain = ETGOR_ChatDomainEnumeration::Private;
	SendChatMessage(Message);
}

void ATGOR_OnlineController::SendChatMessageToSystem(UTGOR_Command* Command, const FString& MessageText)
{
	FTGOR_ChatMessageInstance Message;
	Message.Message = MessageText;
	Message.Command = Command;
	Message.Domain = ETGOR_ChatDomainEnumeration::System;
	SendChatMessage(Message);
}

void ATGOR_OnlineController::SendChatMessageToChannel(UTGOR_Channel* Channel, const FString& MessageText)
{
	FTGOR_ChatMessageInstance Message;
	Message.Message = MessageText;
	Message.Channel = Channel;
	Message.Domain = ETGOR_ChatDomainEnumeration::Public;
	SendChatMessage(Message);
}


void ATGOR_OnlineController::SendChatMessage_Implementation(FTGOR_ChatMessageInstance Message)
{
	SINGLETON_CHK;
	UTGOR_UserData* UserData = Singleton->GetUserData();

	// Make sure client doesn't send too big of a message
	Message.Message = Message.Message.Left(MaxContentLength);

	// Get user
	if (UserData->HasUser(ActiveUserKey))
	{
		const FTGOR_UserInstance& User = UserData->GetUser(ActiveUserKey);
		Message.Sender = User.Properties.Name;
		if (Message.Domain == ETGOR_ChatDomainEnumeration::Private)
		{
			ProcessChatMessageToPlayer(User, Message);
		}
		else if (Message.Domain == ETGOR_ChatDomainEnumeration::System)
		{
			ProcessChatMessageToSystem(User, Message);
		}
		else if (Message.Domain == ETGOR_ChatDomainEnumeration::Public)
		{
			ProcessChatMessageToChannel(User, Message);
		}
	}
	else
	{
		ERROR("Received chat message from unknown user", Error)
	}
}

bool ATGOR_OnlineController::SendChatMessage_Validate(FTGOR_ChatMessageInstance Message)
{
	return(true);
}


void ATGOR_OnlineController::ProcessChatMessageToPlayer(const FTGOR_UserInstance& User, FTGOR_ChatMessageInstance Message)
{
	SINGLETON_CHK;
	UTGOR_UserData* UserData = Singleton->GetUserData();

	// Find playercontroller with specified user name
	UWorld* World = GetWorld();
	auto Iterator = World->GetPlayerControllerIterator();
	for (;Iterator; Iterator++)
	{
		ATGOR_OnlineController* Controller = Cast<ATGOR_OnlineController>(*Iterator);
		if (IsValid(Controller))
		{
			// Check if names match
			if (UserData->HasUser(Controller->ActiveUserKey))
			{
				const FTGOR_UserInstance& Recipient = UserData->GetUser(Controller->ActiveUserKey);
				if (Recipient.Properties.Serial == Message.UserSerial)
				{
					Message.Domain = ETGOR_ChatDomainEnumeration::Private;
					Message.UserSerial = User.Properties.Serial;
					Controller->ReceiveChatMessage(Message);
					return;
				}
			}
		}
	}

	SendErrorMessage(PlayerNotFound);
}

void ATGOR_OnlineController::ProcessChatMessageToSystem(const FTGOR_UserInstance& User, FTGOR_ChatMessageInstance Message)
{
	// Get currently controlled
	ATGOR_Pawn* C = Cast<ATGOR_Pawn>(GetPawn());
	if (IsValid(C))
	{
		// Fetch command
		if (IsValid(Message.Command))
		{
			// Check privileges
			if (Message.Command->CanExecuteCommand(User))
			{
				// Call command
				FString Error = CommandDefaultError.ToString();
				if (!Message.Command->Call(Error, C, this, User, Message.Message))
				{
					SendErrorMessage(FText::FromString(Error));
				}
				return;
			}

			SendErrorMessage(InsufficientPrivileges);
			return;
		}
	}

	SendErrorMessage(CommandNotFound);
}

void ATGOR_OnlineController::ProcessChatMessageToChannel(const FTGOR_UserInstance& User, FTGOR_ChatMessageInstance Message)
{
	// Get currently controlled
	ATGOR_Pawn* C = Cast<ATGOR_Pawn>(GetPawn());
	if (IsValid(C))
	{
		// Fetch channel
		if (IsValid(Message.Channel))
		{
			// Get players from channel
			TArray<int32> Players = Message.Channel->GetPlayersInChannel(C, this, User);

			// Find playercontroller with specified key
			UWorld* World = GetWorld();
			auto Iterator = World->GetPlayerControllerIterator();
			for (; Iterator; Iterator++)
			{
				ATGOR_OnlineController* Controller = Cast<ATGOR_OnlineController>(*Iterator);
				if (IsValid(Controller) && Players.Contains(Controller->ActiveUserKey))
				{
					// Send if part of the channel
					Message.Domain = ETGOR_ChatDomainEnumeration::Public;
					Controller->ReceiveChatMessage(Message);
				}
			}

			return;
		}
	}
	SendErrorMessage(ChannelNotFound);
}

void ATGOR_OnlineController::SendErrorMessage(const FText& MessageText)
{
	SINGLETON_CHK;
	UTGOR_UserData* UserData = Singleton->GetUserData();

	// Get user
	if (UserData->HasUser(ActiveUserKey))
	{
		const FTGOR_UserInstance& User = UserData->GetUser(ActiveUserKey);
	
		FTGOR_ChatMessageInstance ErrorMessage;
		ErrorMessage.Sender = "System";
		ErrorMessage.Message = MessageText.ToString();
		ErrorMessage.UserSerial = User.Properties.Serial;
		ErrorMessage.Domain = ETGOR_ChatDomainEnumeration::System;
		ReceiveChatMessage(ErrorMessage);
	}
}

void ATGOR_OnlineController::SendErrorMessageLocally(FString Message)
{
	FTGOR_ChatMessageInstance ErrorMessage;
	ErrorMessage.Sender = "System";
	ErrorMessage.Message = Message;
	ErrorMessage.UserSerial = 0;
	ErrorMessage.Domain = ETGOR_ChatDomainEnumeration::System;
	OnProcessChatMessage(ErrorMessage);
}


void ATGOR_OnlineController::ReceiveChatMessage_Implementation(FTGOR_ChatMessageInstance Message)
{
	OnProcessChatMessage(Message);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void ATGOR_OnlineController::UpdateTrackerlist(bool Force)
{
	SINGLETON_CHK;
	UTGOR_UserData* UserData = Singleton->GetUserData();
	const FTGOR_Time GameTimestamp = Singleton->GetGameTimestamp();

	if (Force || GameTimestamp > LastTrackerUpdate + TrackerUpdateTime)
	{
		// Update trackers
		if (UserData->HasUser(ActiveUserKey))
		{
			UserData->UpdateTrackers(ActiveUserKey, GameTimestamp - LastTrackerUpdate);

			// Check all pending knowledges
			const FTGOR_UserInstance& User = UserData->GetUser(ActiveUserKey);
			if (UnlockComponent->CheckPendingKnowledges(User.Trackers))
			{
				UnlockUpdate(GetPawn());
			}
			UnlockComponent->UpdatePinboard(User.Trackers);
		}

		// Update timestamp
		LastTrackerUpdate = GameTimestamp;
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void ATGOR_OnlineController::SyncTimestampClient_Implementation(FTGOR_Time Timestamp, FTGOR_Time Reference)
{
	SINGLETON_CHK;
	
	DuplexTime = Singleton->GameTimestamp - Reference;
	const float TimeDiff = Timestamp - Reference;

	// If time difference is more than two times ping away from the server timestamp, adapt directly
	if (TimeDiff > DuplexTime)
	{
		const FTGOR_Time TargetTime = Timestamp + DuplexTime / 2;
		Singleton->GameTimestamp = TargetTime;
	}
	else
	{
		const FTGOR_Time TargetTime = Timestamp + DuplexTime / 2;
		const float Adapt = TargetTime - Singleton->GameTimestamp;
		TimestampAdapt -= FMath::Abs(Adapt) * 10.0f;
		Singleton->GameTimestamp += Adapt;
	}
}


void ATGOR_OnlineController::SyncTimestampServer_Implementation(FTGOR_Time Timestamp)
{
	SINGLETON_CHK;

	const FTGOR_Time GameTimestamp = Singleton->GetGameTimestamp();
	DuplexTime = (GameTimestamp - Timestamp) * 2;
	SyncTimestampClient(GameTimestamp, Timestamp);
}

bool ATGOR_OnlineController::SyncTimestampServer_Validate(FTGOR_Time Timestamp)
{
	return(true);
}


void ATGOR_OnlineController::RepNotifyDimension(const FTGOR_DimensionUpdateList& Old)
{
	UpdateDimensionList();
	UpdateDimensionOwner();
}

////////////////////////////////////////////////////////////////////////////////////////////////////

UTGOR_DimensionData* ATGOR_OnlineController::CheckPlayerDimension(TSubclassOf<UTGOR_Dimension> Type, ETGOR_FetchEnumeration& Branches)
{
	Branches = ETGOR_FetchEnumeration::Empty;
	SINGLETON_RETCHK(nullptr);

	UTGOR_UserData* UserData = Singleton->GetUserData();
	UTGOR_WorldData* WorldData = Singleton->GetWorldData();
	if (UserData->HasUser(ActiveUserKey))
	{
		const FTGOR_UserInstance& User = UserData->GetUser(ActiveUserKey);
		for (const auto& Pair : User.Dimensions)
		{
			if (Pair.Key->IsA(Type))
			{
				if (WorldData->HasFinishedLoading(Pair.Value))
				{
					return WorldData->GetDimension(Pair.Value, Branches);
				}
			}
		}
	}
	return nullptr;
}

FName ATGOR_OnlineController::EnsurePlayerDimension(UTGOR_Dimension* Dimension, ETGOR_FetchEnumeration& Branches)
{
	Branches = ETGOR_FetchEnumeration::Empty;
	SINGLETON_RETCHK(FName());
	if (!IsValid(Dimension))
	{
		ERRET("Invalid player dimension provided", Error, FName());
	}

	UTGOR_UserData* UserData = Singleton->GetUserData();
	UTGOR_WorldData* WorldData = Singleton->GetWorldData();
	if (UserData->HasUser(ActiveUserKey))
	{
		FTGOR_UserInstance& User = UserData->GetUser(ActiveUserKey);
		const FName* Ptr = User.Dimensions.Find(Dimension);
		if (Ptr)
		{
			Branches = ETGOR_FetchEnumeration::Found;
			return *Ptr;
		}

		const FName Identifier = WorldData->GetUniqueIdentifier("User");
		if (WorldData->AddIdentifier(Dimension, Identifier))
		{
			User.Dimensions.Add(Dimension, Identifier);
			Branches = ETGOR_FetchEnumeration::Found;
			return Identifier;
		}
	}
	return FName();
}

void ATGOR_OnlineController::AssignPlayerDimension(const FName& Identifier)
{
	SINGLETON_CHK;
	UTGOR_UserData* UserData = Singleton->GetUserData();
	UTGOR_WorldData* WorldData = Singleton->GetWorldData();
	if (UserData->HasUser(ActiveUserKey) && WorldData->HasIdentifier(Identifier))
	{
		UTGOR_Dimension* Dimension = WorldData->GetDimensionContent(Identifier);
		FTGOR_UserInstance& User = UserData->GetUser(ActiveUserKey);
		User.Dimensions.Add(Dimension, Identifier);
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////

bool ATGOR_OnlineController::HasDimensionUpdate(const FName& Identifier) const
{
	return DimensionUpdateList.Dimensions.Contains(Identifier);
}

void ATGOR_OnlineController::AddExistingDimensionUpdate(const FName& Identifier)
{
	SINGLETON_CHK;
	UTGOR_WorldData* WorldData = Singleton->GetWorldData();
	UTGOR_Dimension* Dimension = WorldData->GetDimensionContent(Identifier);
	if (IsValid(Dimension))
	{
		AddDimensionUpdate(Identifier, Dimension);
	}
}

void ATGOR_OnlineController::AddDimensionUpdate(const FName& Identifier, UTGOR_Dimension* Dimension)
{
	SINGLETON_CHK;

	FTGOR_DimensionUpdateEntry Entry;
	Entry.Content = Dimension;
	Entry.Timestamp = Singleton->GetGameTimestamp();
	DimensionUpdateList.Dimensions.Add(Identifier, Entry);
}

void ATGOR_OnlineController::RemoveDimensionUpdate(const FName& Identifier)
{
	SINGLETON_CHK;

	int32 Count = DimensionUpdateList.Dimensions.Remove(Identifier);
	if (Count > 0)
	{
		PendingDeletions.Add(Identifier, Singleton->GetGameTimestamp());
		HasPendingDeletion = true;
	}
}


void ATGOR_OnlineController::SetDimensionOwner(const FName& Identifier)
{
	DimensionUpdateList.Owner = Identifier;
}

void ATGOR_OnlineController::CopyDimensionUpdateListFromHost()
{
	ATGOR_OnlineController* HostController = GetWorld()->GetFirstPlayerController<ATGOR_OnlineController>();
	if (IsValid(HostController) && HostController != this) // No need to copy myself
	{
		DimensionUpdateList.Dimensions = HostController->DimensionUpdateList.Dimensions;
	}

}

void ATGOR_OnlineController::UpdateDimensionList()
{
	if (!DimensionUpdateList.IsServerApproved)
	{
		return;
	}

	SINGLETON_CHK;
	UWorld* World = GetWorld();
	UTGOR_WorldData* WorldData = Singleton->GetWorldData();
	const bool IsServer = HasAuthority();
	bool IsRealityUnloading = false;

	// Unload any loaded dimensions that are not in the update list
	TArray<FName> Keys;
	WorldData->GetLoadedDimensions(Keys);
	for (const FName& Key : Keys)
	{
		// Ignore already unloaded and persistent levels
		if (WorldData->HasBeenSpawned(Key) && Key != WorldData->GetPersistentIdentifier())
		{
			// Check whether dimension is in our update list
			bool HasDimension = false;
			for (const auto& Pair : DimensionUpdateList.Dimensions)
			{
				// Make sure loaded dimensions with the wrong key are unloaded
				if (Pair.Key == Key)
				{
					HasDimension = true;
				}
			}

			// TODO: Server needs to also check all clients before unloading a dimension.
			// The way it is done here is very inefficient.
			if (IsServer)
			{
				for (auto It = World->GetPlayerControllerIterator(); It; ++It)
				{
					ATGOR_OnlineController* Controller = Cast<ATGOR_OnlineController>(*It);
					if (IsValid(Controller))
					{
						for (const auto& Pair : Controller->DimensionUpdateList.Dimensions)
						{
							// Make sure loaded dimensions with the wrong key are unloaded
							if (Pair.Key == Key)
							{
								HasDimension = true;
							}
						}
					}
				}
			}

			if (!HasDimension)
			{
				if (WorldData->ProgressUnloadDimension(Key, IsServer))
				{
					// Let server know we have unloaded so host can continue unloading
					NotifyDimensionUnload(Key);
				}
				else if(WorldData->IsReality(Key))
				{
					IsRealityUnloading = true;
				}
			}
		}
	}

	// Load new dimensions
	for (auto& Pair : DimensionUpdateList.Dimensions)
	{
		// Don't load a new reality while another reality is currently unloading
		if (!Pair.Value.Content->Reality || !IsRealityUnloading)
		{
			// Make sure dimension is dialed, also returns false if another dimension is currently unloading on the given identifier
			if (WorldData->SetIdentifier(Pair.Value.Content, Pair.Key))
			{
				// Progress if necessary
				if (WorldData->ProgressLoadDimension(Pair.Key, IsServer))
				{
					// Set location and content in case the values get changed by progressing and another controller wants to copy
					Pair.Value.Content = WorldData->GetDimensionContent(Pair.Key);
					Pair.Value.Timestamp = Singleton->GetGameTimestamp();
				}
			}
		}
	}
}

bool ATGOR_OnlineController::IsDimensionListUpToDate() const
{
	if (!IsValid(Singleton)) ERRET(SINGLETON_ERR, Error, false);
	if (!DimensionUpdateList.IsServerApproved) return false;
	UTGOR_WorldData* WorldData = Singleton->GetWorldData();

	if (HasPendingDeletion) return false;

	// TODO: Ignores dimensions about to be deleted
	for (const auto& Pair : DimensionUpdateList.Dimensions)
	{
		if (!WorldData->HasFinishedLoading(Pair.Key))
		{
			return false;
		}
	}
	return true;
}

void ATGOR_OnlineController::UpdateDimensionOwner()
{
	SINGLETON_CHK;
	UTGOR_WorldData* WorldData = Singleton->GetWorldData();
	WorldData->SwitchActiveDimension(DimensionUpdateList.Owner);	
}

bool ATGOR_OnlineController::IsDimensionOwnerUpToDate() const
{
	if (!IsValid(Singleton)) ERRET(SINGLETON_ERR, Error, false);
	UTGOR_WorldData* WorldData = Singleton->GetWorldData();
	return (WorldData->GetActiveIdentifier() == DimensionUpdateList.Owner);
}


void ATGOR_OnlineController::RequestInitialDimensionUpdate_Implementation()
{
	// TODO: For now just copies current setup from host
	CopyDimensionUpdateListFromHost();
	DimensionUpdateList.IsServerApproved = true;
}

bool ATGOR_OnlineController::RequestInitialDimensionUpdate_Validate()
{
	return(true);
}

void ATGOR_OnlineController::NotifyDimensionUnload_Implementation(FName Identifier)
{
	PendingDeletions.Remove(Identifier);
	if (PendingDeletions.Num() == 0)
	{
		HasPendingDeletion = false;
	}
}

bool ATGOR_OnlineController::NotifyDimensionUnload_Validate(FName Identifier)
{
	return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

bool ATGOR_OnlineController::StartListenServer(FString Address, int32 Port)
{
	UWorld* World = GetWorld();
	FURL Url(nullptr, *World->GetLocalURL(), TRAVEL_Absolute);
	Url.Port = Port;
	return World->Listen(Url);
}

bool ATGOR_OnlineController::StopListenServer()
{
	UWorld* World = GetWorld();
	GEngine->ShutdownWorldNetDriver(World);
	return(true);
}

bool ATGOR_OnlineController::StartConnectingToServer(FString Address, int32 Port)
{
	UWorld* World = GetWorld();
	if (Port > 0)
	{
		Address = Address + ":" + FString::FromInt(Port);
	}
	GEngine->SetClientTravel(World, *Address, TRAVEL_Absolute);
	return(true);
}

////////////////////////////////////////////////////////////////////////////////////////////////////


ATGOR_HUD* ATGOR_PlayerController::GetCachedHud() const
{ 
	return Hud; 
}


////////////////////////////////////////////////////////////////////////////////////////////////////

const char* prefixes[] = {
	"Sir_",
	"Mrs_",
	"Lady_",
	"Lord_",
	"M'",
	"Herr_",
	"Frau_",
	"Senior_",
	"Junior_",
	"Alpha_",
	"Junior_",
	"Master_",
	"Grand_",
	"San_",
	"The_",
	"Old_",
	"Dr_",
	"Doc_",
	"Prof_",
	"Count_",
	"Darth_",
	"Major_",
	"Sergeant_",
	"Judge_",
	"GrossBuerger_",
	"Dear_",
	"Don_",
	"D'",
	"Comrade_",
	"Kaiser_",
	"Abbot_",
	"Agent_",
	"Sister_",
	"Mother_",
	"Father_",
	"Pope_",
	"Pastor_",
	"Servant_",
	"",
	"",
	"",
	"",
	"",
	"",
	""
};

const char* petnames[] = { 
	"Sin",
	"Toro",
	"Atom",
	"Strong",
	"Albino",
	"Ollie",
	"Seinfeld",
	"Mushroom",
	"Vixen",
	"Lady",
	"Newman",
	"Skylar",
	"Burger",
	"Shrooms",
	"Squint",
	"Luigi",
	"Aqua",
	"Lars",
	"Derp",
	"Rudolph",
	"Moose",
	"Magnet",
	"Smokey",
	"Nash",
	"Bambi",
	"Daisy",
	"Reno",
	"Flap",
	"Prince",
	"Derg",
	"Barker",
	"Cynthia",
	"Doc",
	"Prec",
	"Emperor",
	"Drunk",
	"Mumble",
	"Bubble",
	"Spike",
	"Torch",
	"Sour",
	"Aura",
	"Skippy",
	"Trident",
	"Arctic",
	"Moose",
	"Babs",
	"Cash",
	"Biggie",
	"Highly",
	"Sputnik",
	"Aaliyah",
	"Rich",
	"Claw",
	"Elton",
	"Dawn",
	"Stan",
	"Hans",
	"Boop",
	"Snoot",
	"Randal",
	"Snek",
	"Sam",
	"Talon",
	"Jacques",
	"Linux",
	"Rugged",
	"Heidi",
	"Mimi",
	"Natasha",
	"Dick",
	"Footsy",
	"Elmer",
	"Ariel",
	"Birb",
	"Claudia",
	"Mozzarella",
	"Riptide",
	"Cumalot",
	"Ruger",
	"Burb",
	"Turok",
	"Baloo",
	"Paws",
	"Monster",
	"Jordan",
	"Manson",
	"Jerky",
	"Clive",
	"Herman",
	"Digby",
	"Tail",
	"Kelsey",
	"Borb",
	"Diablo",
	"Hagrid",
	"Butt",
	"Becky",
	"Slayer",
	"Comet",
	"Smash",
	"Bertha",
	"Houdini",
	"Snoopy",
	"Bell",
	"Stitches",
	"Crimson",
	"Jango",
	"Jive",
	"Harry",
	"Meta",
	"Simon",
	"Alpine",
	"Tummy",
	"Ming",
	"Maniac",
	"Dasher",
	"Maya",
	"Boi",
	"Gurl",
	""
};


FString ATGOR_OnlineController::GenerateUsername()
{
	FMath::RandInit(time(NULL));

	const int preLength = sizeof(prefixes) / sizeof(char*);
	const int preIndex = FMath::Rand() % preLength;
	
	FString prefix = FString(prefixes[preIndex]);
	
	const int namLength = sizeof(petnames) / sizeof(char*);
	const int firstIndex = FMath::Rand() % namLength;
	const int secndIndex = FMath::Rand() % namLength;
	
	FString first = FString(petnames[firstIndex]);
	FString second = FString(petnames[secndIndex]);

	return(prefix + first + second.ToLower());
}

int32 ATGOR_OnlineController::GetActiveUserKey()
{
	return(ActiveUserKey);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void ATGOR_OnlineController::SetPossessionQueue(const FTGOR_DimensionIdentifier& Identifier)
{
	PossessionQueue = Identifier;

	// Immediately progress in case it's already valid
	ProgressPossessionQueue();
}


void ATGOR_OnlineController::ClearPossessionQueue()
{
	PossessionQueue = FTGOR_DimensionIdentifier();
}

bool ATGOR_OnlineController::HasPossessionQueue() const
{
	return PossessionQueue.DimensionIdentifier != "";
}

bool ATGOR_OnlineController::ProgressPossessionQueue()
{
	SINGLETON_RETCHK(false);
	UTGOR_WorldData* WorldData = Singleton->GetWorldData();
	UTGOR_UserData* UserData = Singleton->GetUserData();

	// Ignore invalid identifiers
	if (!WorldData->HasIdentifier(PossessionQueue.DimensionIdentifier))
	{
		ClearPossessionQueue();
		OnPossessionResponse(ETGOR_PossessionResponse::Invalid);
	}

	if (WorldData->HasFinishedLoading(PossessionQueue.DimensionIdentifier))
	{
		ETGOR_FetchEnumeration State;
		UTGOR_IdentityComponent* Identity = WorldData->GetIdentity(PossessionQueue, State);

		// Clear queue whether or not this step fails
		ClearPossessionQueue();
		if (State == ETGOR_FetchEnumeration::Found)
		{
			// Execute possesstion if pawn exists and isn't already possessed
			ATGOR_Pawn* OtherPawn = Cast<ATGOR_Pawn>(Identity->GetOwner());
			if (IsValid(OtherPawn))
			{
				if (OtherPawn->GetController() == nullptr)
				{
					ATGOR_Pawn* OwnPawn = Cast<ATGOR_Pawn>(GetPawn());
					if (IsValid(OwnPawn))
					{
						Possess(OtherPawn);
						UnlockUpdate(OtherPawn);
						SetDimensionOwner(OtherPawn->GetDimensionIdentifier(State));
						OnPossessionResponse(ETGOR_PossessionResponse::Success);

						// Update other controllers since a body can be owned by multiple
						return UserData->ForEachController([](ATGOR_OnlineController* OnlineController) { OnlineController->UpdateBodyDisplay(); return true; });
					}
					else
					{
						// Technically this is an error case already
						APawn* Own = GetPawn();
						Possess(Own);
						UnlockUpdate(Own);
					}
				}
				else
				{
					OnPossessionResponse(ETGOR_PossessionResponse::Collision);
					return false;
				}
			}
		}
		OnPossessionResponse(ETGOR_PossessionResponse::Invalid);
		return false;
	}
	else if(WorldData->HasFinishedUnLoading(PossessionQueue.DimensionIdentifier) && !HasDimensionUpdate(PossessionQueue.DimensionIdentifier))
	{
		// Only try loading unloaded dimensions if they are properly unloaded and not already queued
		AddExistingDimensionUpdate(PossessionQueue.DimensionIdentifier);
	}

	OnPossessionResponse(ETGOR_PossessionResponse::Waiting);
	return false;
}


void ATGOR_OnlineController::UpdateBodyDisplay()
{
	BodyDisplay.Reset();

	SINGLETON_CHK;
	UTGOR_UserData* UserData = Singleton->GetUserData();
	UTGOR_WorldData* WorldData = Singleton->GetWorldData();
	if (UserData->HasUser(ActiveUserKey))
	{
		const FTGOR_UserInstance& User = UserData->GetUser(ActiveUserKey);
		for (const auto& Pair : User.Bodies)
		{
			// Only display bodies we can actually possess and that aren't already possessed
			if (!Pair.Value.Avatar.IsValid() || (Pair.Value.Avatar->GetController() == nullptr && Pair.Value.Avatar != GetPawn()))
			{
				FTGOR_UserBodyDisplay Display;
				Display.Creature = Pair.Value.Creature;
				Display.Dimension = WorldData->GetDimensionContent(Pair.Value.Identifier.DimensionIdentifier);
				Display.Identifier = Pair.Key;
				BodyDisplay.Emplace(Display);
			}
		}
	}
}

void ATGOR_OnlineController::RequestBodySwitch_Implementation(int32 Identifier)
{
	if (Identifier == -1) return;

	SINGLETON_CHK;
	UTGOR_UserData* UserData = Singleton->GetUserData();

	// Get user with current key
	if (UserData->HasUser(ActiveUserKey))
	{
		FTGOR_UserInstance& User = UserData->GetUser(ActiveUserKey);
		const FTGOR_UserBody* Ptr = User.Bodies.Find(Identifier);
		if (Ptr)
		{
			// Possess body
			SetPossessionQueue(Ptr->Identifier);
			User.CurrentBody = Identifier;
			OnUserUpdate.Broadcast();
			return;
		}
	}

	OnPossessionResponse(ETGOR_PossessionResponse::NotFound);
}

bool ATGOR_OnlineController::RequestBodySwitch_Validate(int32 Identifier)
{
	return true;
}


////////////////////////////////////////////////////////////////////////////////////////////////////


void ATGOR_OnlineController::UnlockUpdate(AActor* Actor)
{
	if (IsValid(Actor))
	{
		TScriptInterface<ITGOR_UnlockInterface> ActorObject = Actor;
		if (ActorObject)
		{
			ActorObject->Execute_UpdateUnlocks(Actor, UnlockComponent);
		}

		const TSet<UActorComponent*>& Components = Actor->GetComponents();
		for (UActorComponent* Component : Components)
		{
			TScriptInterface<ITGOR_UnlockInterface> ComponentObject = Component;
			if (ComponentObject)
			{
				ComponentObject->Execute_UpdateUnlocks(Component, UnlockComponent);
			}
		}
	}
}

void ATGOR_OnlineController::RequestPin_Implementation(FTGOR_KnowledgePinRequest Request)
{
	SINGLETON_CHK;
	UTGOR_UserData* UserData = Singleton->GetUserData();

	// Get user with current key
	if (UserData->HasUser(ActiveUserKey))
	{
		const FTGOR_UserInstance& User = UserData->GetUser(ActiveUserKey);
		UnlockComponent->RequestPin(User.Trackers, Request);
	}
}

bool ATGOR_OnlineController::RequestPin_Validate(FTGOR_KnowledgePinRequest Request)
{
	return true;
}

#undef LOCTEXT_NAMESPACE