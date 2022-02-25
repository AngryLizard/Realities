// The Gateway of Realities: Planes of Existence.

#pragma once

#include "Realities/Utility/Error/TGOR_Error.h"
#include "Realities/Base/Instances/Player/TGOR_UserInstance.h"
#include "Realities/Base/Instances/Player/TGOR_ChatMessageInstance.h"
#include "Realities/Base/Instances/Dimension/TGOR_DimensionDataInstance.h"
#include "Realities/Base/Instances/Creature/TGOR_CreatureInstance.h"
#include "Realities/Base/Instances/Creature/TGOR_KnowledgeInstance.h"
#include "Realities/Utility/Storage/TGOR_Buffer.h"

#include "Realities/Interfaces/TGOR_SingletonInterface.h"
#include "GameFramework/PlayerController.h"
#include "TGOR_OnlineController.generated.h"

////////////////////////////////////////////// DECL //////////////////////////////////////////////////////

class UTGOR_Channel;
class UTGOR_Command;
class UTGOR_Knowledge;
class UTGOR_CameraComponent;
class UTGOR_UnlockComponent;

////////////////////////////////////////////// ENUM //////////////////////////////////////////////////////

UENUM(BlueprintType)
enum class ETGOR_RenameResponse : uint8
{
	Success,
	TooLong,
	Invalid
};

UENUM(BlueprintType)
enum class ETGOR_LoginResponse : uint8
{
	Success,
	Created,
	Invalid
};

UENUM(BlueprintType)
enum class ETGOR_PossessionResponse : uint8
{
	Success, // Succeeded
	Waiting, // Is waiting for dimension to load
	Collision, // Is already possessed
	NotFound, // Pawn doesn't exist
	Invalid // Dimension doesn't exist
};

////////////////////////////////////////////// STRUCT //////////////////////////////////////////////////////

////////////////////////////////////////////// DELEGATES //////////////////////////////////////////////////////


DECLARE_DYNAMIC_MULTICAST_DELEGATE(FUserUpdateDelegate);


/**
* TGOR_OnlineController allows a playercontroller to connect to a server with a useraccount
*/
UCLASS()
class REALITIES_API ATGOR_OnlineController : public APlayerController, public ITGOR_SingletonInterface
{
	GENERATED_BODY()

	friend class UTGOR_WorldData;
	friend class ATGOR_GameState;

public:

	////////////////////////////////////////////////////////////////////////////////////////////////////

	ATGOR_OnlineController(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void Tick(float DeltaTime) override;
	virtual void PlayerTick(float DeltaTime) override;
	virtual void GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const override;
	virtual void PawnLeavingGame() override;
	virtual void SetPawn(APawn* InPawn) override;

	//////////////////////////////////////////// IMPLEMENTABLES ////////////////////////////////////////
	
	/** On body changed failed response (Called on server only) */
	UFUNCTION(BlueprintImplementableEvent, Category = "TGOR Online", Meta = (Keywords = "C++"))
		void OnPossessionResponse(ETGOR_PossessionResponse Response);

	/** On login response (Called on client only) */
	UFUNCTION(BlueprintImplementableEvent, Category = "TGOR Online", Meta = (Keywords = "C++"))
		void OnLoginResponse(ETGOR_LoginResponse Response);

	/** Setup for user on server (argument "User" can be changed!), IsNewUser is true on new registrations, In that case, registrationSetup is called before this. */
	UFUNCTION(BlueprintNativeEvent, Category = "TGOR Online", Meta = (Keywords = "C++"))
		bool OnLoginSetup(int32 Key, UPARAM(ref) FTGOR_UserProperties& User, bool IsNewUser);
		virtual bool OnLoginSetup_Implementation(int32 Key, UPARAM(ref) FTGOR_UserProperties& User, bool IsNewUser);

	/** Setup for newly created user on server (argument "User" can be changed!) */
	UFUNCTION(BlueprintNativeEvent, Category = "TGOR Online", Meta = (Keywords = "C++"))
		bool OnRegistrationSetup(UPARAM(ref) FTGOR_UserProperties& User);
		virtual bool OnRegistrationSetup_Implementation(UPARAM(ref) FTGOR_UserProperties& User);

	/** Process received message from server */
	UFUNCTION(BlueprintImplementableEvent, Category = "TGOR Chat", Meta = (Keywords = "C++"))
		void OnProcessChatMessage(const FTGOR_ChatMessageInstance& Message);

	////////////////////////////////////////////////////////////////////////////////////////////////////

	/** Notifies client about new unlocks */
	UPROPERTY(BlueprintAssignable, Category = "TGOR Online")
		FUserUpdateDelegate OnUserUpdate;


	////////////////////////////////////////////////// COMPONENTS //////////////////////////////////////
private:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Character, meta = (AllowPrivateAccess = "true"))
		UTGOR_UnlockComponent* UnlockComponent;

public:

	FORCEINLINE UTGOR_UnlockComponent* GetUnlock() const { return UnlockComponent; }

	////////////////////////////////////////////////////////////////////////////////////////////////////
protected:

	/** Get the Role of this controller */
	UFUNCTION(BlueprintCallable, Category = "TGOR Online", Meta = (ExpandEnumAsExecs = "Branches", Keywords = "C++"))
		void GetNetOccupation(ETGOR_NetOccupation& Branches);
	
	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Create unique serial number between 0 and 999. */
	UFUNCTION(BlueprintPure, Category = "TGOR Online", Meta = (Keywords = "C++"))
		int32 CreateUniqueSerial() const;

	/** Checks whether a serial key already exists. */
	UFUNCTION(BlueprintPure, Category = "TGOR Online", Meta = (Keywords = "C++"))
		bool HasSerial(int32 Serial) const;

	/** Checks whether this user is online. */
	UFUNCTION(BlueprintPure, BlueprintAuthorityOnly, Category = "TGOR Online", Meta = (Keywords = "C++"))
		bool IsOnline() const;

	/** Renames this user. */
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "TGOR Online", Meta = (Keywords = "C++"))
		ETGOR_RenameResponse RenameUser(const FString& NewName);

	/** Requests possession of spectator */
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "TGOR Online", Meta = (Keywords = "C++"))
		void PossessSpectator();

	/** Requests possession of last used (or first available) body associated with this user */
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "TGOR Online", Meta = (Keywords = "C++"))
		void PossessUserBody();

	/** Server accepted login. */
	UFUNCTION(BlueprintCallable, Reliable, Client, Category = "TGOR Online", Meta = (Keywords = "C++"))
		void LoginAccept(int32 Key, ETGOR_LoginResponse Response);
		virtual void LoginAccept_Implementation(int32 Key, ETGOR_LoginResponse Response);

	/** Authenticate with server */
	UFUNCTION(BlueprintCallable, Reliable, Server, WithValidation, Category = "TGOR Online", Meta = (Keywords = "C++"))
		void Authentication(int32 Key, const FTGOR_Buffer& Message);
		virtual void Authentication_Implementation(int32 Key, const FTGOR_Buffer& Message);
		virtual bool Authentication_Validate(int32 Key, const FTGOR_Buffer& Message);

	/** Server requests authentification. */
	UFUNCTION(BlueprintCallable, Reliable, Client, Category = "TGOR Online", Meta = (Keywords = "C++"))
		void RequestAuthentication(const FTGOR_Buffer& Message, const FTGOR_Buffer& PublicKey);
		virtual void RequestAuthentication_Implementation(const FTGOR_Buffer& Message, const FTGOR_Buffer& PublicKey);

	/** Logout from server */
	UFUNCTION(BlueprintCallable, Reliable, Server, WithValidation, Category = "TGOR Online", Meta = (Keywords = "C++"))
		void Logout();
		virtual void Logout_Implementation();
		virtual bool Logout_Validate();

	/** Authenticate with server */
	UFUNCTION(BlueprintCallable, Reliable, Server, WithValidation, Category = "TGOR Online", Meta = (Keywords = "C++"))
		void Login(int32 Key, const FTGOR_Buffer& PublicKey);
		virtual void Login_Implementation(int32 Key, const FTGOR_Buffer& PublicKey);
		virtual bool Login_Validate(int32 Key, const FTGOR_Buffer& PublicKey);

	/** Tries to login client */
	UFUNCTION(BlueprintCallable, Category = "TGOR Online", Meta = (Keywords = "C++"))
		void RequestLogin();

	////////////////////////////////////////////////////////////////////////////////////////////////////
protected:

	/** Max allowed content length */
	UPROPERTY(EditDefaultsOnly, Category = "TGOR Chat", Meta = (Keywords = "C++"))
		int32 MaxContentLength;

	/** Users currently online */
	UPROPERTY(Replicated)
		TArray<FTGOR_UserInfo> OnlineUsers;

	/** Last list update (Update at most once per second) */
	UPROPERTY(BlueprintReadOnly, Category = "TGOR Chat", Meta = (Keywords = "C++"))
		FTGOR_Time LastOnlineUsersUpdate;

	/** Least time in between list updates */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TGOR Chat", Meta = (Keywords = "C++"))
		float OnlineUsersUpdateTime;

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Get currently active dimension identifier */
	UFUNCTION(BlueprintPure, Category = "TGOR Chat", Meta = (Keywords = "C++"))
		int32 GetMaxContentLength() const;

	/** Get currently active dimension identifier */
	UFUNCTION(BlueprintPure, Category = "TGOR Chat", Meta = (Keywords = "C++"))
		const TArray<FTGOR_UserInfo>& GetOnlineUsers() const;


	/** Update client with currently online players */
	UFUNCTION(BlueprintCallable, Category = "TGOR Chat", Meta = (Keywords = "C++"))
		void UpdateUserlist(bool Force = false);


	/** Sends a message to a player */
	UFUNCTION(BlueprintCallable, Category = "TGOR Chat", Meta = (Keywords = "C++"))
		void SendChatMessageToPlayer(int32 UserSerial, const FString& MessageText);

	/** Sends a message to the system (commands) */
	UFUNCTION(BlueprintCallable, Category = "TGOR Chat", Meta = (Keywords = "C++"))
		void SendChatMessageToSystem(UTGOR_Command* Command, const FString& MessageText);

	/** Sends a message to a global channel */
	UFUNCTION(BlueprintCallable, Category = "TGOR Chat", Meta = (Keywords = "C++"))
		void SendChatMessageToChannel(UTGOR_Channel* Channel, const FString& MessageText);


	/** Send message to server */
	UFUNCTION(BlueprintCallable, Reliable, Server, WithValidation, Category = "TGOR Chat", Meta = (Keywords = "C++"))
		void SendChatMessage(FTGOR_ChatMessageInstance Message);
		virtual void SendChatMessage_Implementation(FTGOR_ChatMessageInstance Message);
		virtual bool SendChatMessage_Validate(FTGOR_ChatMessageInstance Message);


	/** Process message to player */
	UFUNCTION(BlueprintCallable, Category = "TGOR Chat", Meta = (Keywords = "C++"))
		void ProcessChatMessageToPlayer(const FTGOR_UserInstance& User, FTGOR_ChatMessageInstance Message);

	/** Process message to system */
	UFUNCTION(BlueprintCallable, Category = "TGOR Chat", Meta = (Keywords = "C++"))
		void ProcessChatMessageToSystem(const FTGOR_UserInstance& User, FTGOR_ChatMessageInstance Message);

	/** Process message to channel */
	UFUNCTION(BlueprintCallable, Category = "TGOR Chat", Meta = (Keywords = "C++"))
		void ProcessChatMessageToChannel(const FTGOR_UserInstance& User, FTGOR_ChatMessageInstance Message);

	/** Sends an error message to a player */
	UFUNCTION(BlueprintCallable, Category = "TGOR Chat", Meta = (Keywords = "C++"))
		void SendErrorMessage(const FText& MessageText);

	/** Sends an error message to this player locally */
	UFUNCTION(BlueprintCallable, Category = "TGOR Chat", Meta = (Keywords = "C++"))
		void SendErrorMessageLocally(FString Message);

	/** Receive message from server */
	UFUNCTION(BlueprintCallable, Reliable, Client, Category = "TGOR Chat", Meta = (Keywords = "C++"))
		void ReceiveChatMessage(FTGOR_ChatMessageInstance Message);
		virtual void ReceiveChatMessage_Implementation(FTGOR_ChatMessageInstance Message);

		////////////////////////////////////////////////////////////////////////////////////////////////////
protected:

	/** Last tracker update (Update at most once per second) */
	UPROPERTY(BlueprintReadOnly, Category = "TGOR Chat", Meta = (Keywords = "C++"))
		FTGOR_Time LastTrackerUpdate;

	/** Least time in between list updates */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TGOR Chat", Meta = (Keywords = "C++"))
		float TrackerUpdateTime;

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Update client with currently active avatar */
	UFUNCTION(BlueprintCallable, Category = "TGOR Chat", Meta = (Keywords = "C++"))
		void UpdateTrackerlist(bool Force = false);


	////////////////////////////////////////////////////////////////////////////////////////////////////
private:

	/** Amount of seconds in between timestamp syncs */
	UPROPERTY(EditAnywhere, Category = "TGOR Online")
		float TimestampSyncTime;

	/** Measured ping */
	UPROPERTY()
		float DuplexTime;

	/** Timer until timestamp gets sysnced */
	UPROPERTY()
		float TimestampAdapt;

	/** Sync timestamp with client */
	UFUNCTION(Unreliable, Client)
		void SyncTimestampClient(FTGOR_Time Timestamp, FTGOR_Time Reference);
		virtual void SyncTimestampClient_Implementation(FTGOR_Time Timestamp, FTGOR_Time Reference);

	/** Request timestamp sync with server */
	UFUNCTION(Unreliable, Server, WithValidation)
		void SyncTimestampServer(FTGOR_Time Timestamp);
		virtual void SyncTimestampServer_Implementation(FTGOR_Time Timestamp);
		virtual bool SyncTimestampServer_Validate(FTGOR_Time Timestamp);

	////////////////////////////////////////////////////////////////////////////////////////////////////

	/** This controller's dimension list */
	UPROPERTY(ReplicatedUsing = RepNotifyDimension)
		FTGOR_DimensionUpdateList DimensionUpdateList;

	/** Whether this controller has a deletion pending */
	UPROPERTY(Replicated)
		bool HasPendingDeletion;

	/** Currently pending deletion with timestamp (server only) */
	UPROPERTY()
		TMap<FName, FTGOR_Time> PendingDeletions;
	
	UFUNCTION()
		void RepNotifyDimension(const FTGOR_DimensionUpdateList& Old);

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Returns a dimension of given type owned by this player and checks whether it is loaded */
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "TGOR Dimension", Meta = (ExpandEnumAsExecs = "Branches", Keywords = "C++"))
		UTGOR_DimensionData* CheckPlayerDimension(TSubclassOf<UTGOR_Dimension> Type, ETGOR_FetchEnumeration& Branches);

	/** Finds or sets player dimension to a given content type and loads it */
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "TGOR Dimension", Meta = (ExpandEnumAsExecs = "Branches", Keywords = "C++"))
		FName EnsurePlayerDimension(UTGOR_Dimension* Dimension, ETGOR_FetchEnumeration& Branches);
	
	/** Gives a dimension to this player */
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "TGOR Dimension", Meta = (Keywords = "C++"))
		void AssignPlayerDimension(const FName& Identifier);
	
	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Whether a given update is already queued */
	UFUNCTION(BlueprintPure, BlueprintAuthorityOnly, Category = "TGOR Dimension", Meta = (Keywords = "C++"))
		bool HasDimensionUpdate(const FName& Identifier) const;

	/** Adds an already existing dimension to the update list */
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "TGOR Dimension", Meta = (Keywords = "C++"))
		void AddExistingDimensionUpdate(const FName& Identifier);

	/** Adds a new dimension to the update list */
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "TGOR Dimension", Meta = (Keywords = "C++"))
		void AddDimensionUpdate(const FName& Identifier, UTGOR_Dimension* Dimension);

	/** Removes a dimension from the update list */
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "TGOR Dimension", Meta = (Keywords = "C++"))
		void RemoveDimensionUpdate(const FName& Identifier);

	/** Sets the owning dimension */
	UFUNCTION(BlueprintCallable, Category = "TGOR Dimension", Meta = (Keywords = "C++"))
		void SetDimensionOwner(const FName& Identifier);
	
	/** Copy dimension update list from host */
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "TGOR Dimension", Meta = (Keywords = "C++"))
		void CopyDimensionUpdateListFromHost();

	/** Loads and unloads dimensions */
	UFUNCTION(BlueprintCallable, Category = "TGOR Dimension", Meta = (Keywords = "C++"))
		void UpdateDimensionList();

	/** Whether any dimensions need to be loaded or unloaded */
	UFUNCTION(BlueprintCallable, Category = "TGOR Dimension", Meta = (Keywords = "C++"))
		bool IsDimensionListUpToDate() const;

	/** Switches dimensions to owner */
	UFUNCTION(BlueprintCallable, Category = "TGOR Dimension", Meta = (Keywords = "C++"))
		void UpdateDimensionOwner();

	/** Whether the dimension needs to be switched */
	UFUNCTION(BlueprintCallable, Category = "TGOR Dimension", Meta = (Keywords = "C++"))
		bool IsDimensionOwnerUpToDate() const;
	
	/** Request initial map setup (usually contains the hub and current reality) */
	UFUNCTION(Reliable, Server, WithValidation)
		void RequestInitialDimensionUpdate();
		virtual void RequestInitialDimensionUpdate_Implementation();
		virtual bool RequestInitialDimensionUpdate_Validate();

	/** Notify server about a dimension having finished unloading */
	UFUNCTION(Reliable, Server, WithValidation)
		void NotifyDimensionUnload(FName Identifier);
		virtual void NotifyDimensionUnload_Implementation(FName Identifier);
		virtual bool NotifyDimensionUnload_Validate(FName Identifier);

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Generates random name */
	UFUNCTION(BlueprintPure, Category = "TGOR Online", Meta = (Keywords = "C++"))
		FString GenerateUsername();

	/** Currently loaded user key */
	UFUNCTION(BlueprintPure, BlueprintAuthorityOnly, Category = "TGOR Online", Meta = (Keywords = "C++"))
		int32 GetActiveUserKey();

	////////////////////////////////////////////////////////////////////////////////////////////////////
protected:

	/** Body this controller is waiting on to possess */
	UPROPERTY(BlueprintReadOnly)
		FTGOR_DimensionIdentifier PossessionQueue;

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Possession queue */
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "TGOR Dimension", Meta = (Keywords = "C++"))
		void SetPossessionQueue(const FTGOR_DimensionIdentifier& Identifier);

	/** Resets possession queue */
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "TGOR Dimension", Meta = (Keywords = "C++"))
		void ClearPossessionQueue();

	/** Whether queue is currently filled */
	UFUNCTION(BlueprintPure, BlueprintAuthorityOnly, Category = "TGOR Dimension", Meta = (Keywords = "C++"))
		bool HasPossessionQueue() const;

	/** Attempts possession, returns whether successful */
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "TGOR Dimension", Meta = (Keywords = "C++"))
		bool ProgressPossessionQueue();

	////////////////////////////////////////////////////////////////////////////////////////////////////
protected:

	/** Max distance for which camera tries to switch between characters smoothly */
	UPROPERTY(EditDefaultsOnly, Category = "TGOR Dimension", Meta = (Keywords = "C++"))
		float MaxCameraSwitchDistance;

	/** Available bodies for this user */
	UPROPERTY(BlueprintReadOnly, Replicated)
		TArray<FTGOR_UserBodyDisplay> BodyDisplay;

	/** Camera this controller is currently possessing if available */
	UPROPERTY(EditDefaultsOnly, Category = "TGOR Dimension", Meta = (Keywords = "C++"))
		UTGOR_CameraComponent* PawnCamera;

	/** Blend time when camera transitions */
	UPROPERTY(EditDefaultsOnly, Category = "TGOR Dimension", Meta = (Keywords = "C++"))
		float CameraBlendTime;


	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Grabs available bodies from user database */
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "TGOR Online", Meta = (Keywords = "C++"))
		void UpdateBodyDisplay();

	/** Request switching bodies */
	UFUNCTION(Reliable, Server, WithValidation, BlueprintCallable, Category = "TGOR Online", Meta = (Keywords = "C++"))
		void RequestBodySwitch(int32 Identifier);
	virtual void RequestBodySwitch_Implementation(int32 Identifier);
	virtual bool RequestBodySwitch_Validate(int32 Identifier);


	////////////////////////////////////////////////////////////////////////////////////////////////////
public:


	/** Update unlocks for a given pawn */
	UFUNCTION(BlueprintCallable, Category = "TGOR Online", Meta = (Keywords = "C++"))
		void UnlockUpdate(AActor* Actor);
	
	/** Request switching bodies */
	UFUNCTION(Reliable, Server, WithValidation, BlueprintCallable, Category = "TGOR Online", Meta = (Keywords = "C++"))
		void RequestPin(FTGOR_KnowledgePinRequest Request);
	virtual void RequestPin_Implementation(FTGOR_KnowledgePinRequest Request);
	virtual bool RequestPin_Validate(FTGOR_KnowledgePinRequest Request);


	////////////////////////////////////////////////////////////////////////////////////////////////////
protected:

	/** Index of the currently active user (on server and client) */
	UPROPERTY(EditAnywhere, Category = "TGOR Online", Meta = (Keywords = "C++"))
		int32 ActiveUserKey;

	////////////////////////////////////////////////////////////////////////////////////////////////////

	/** Player not found error message */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TGOR Chat", Meta = (Keywords = "C++"))
		FText PlayerNotFound;

	/** Command not found error message */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TGOR Chat", Meta = (Keywords = "C++"))
		FText CommandNotFound;

	/** Default error message for commands */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TGOR Chat", Meta = (Keywords = "C++"))
		FText CommandDefaultError;

	/** Channel not found error message */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TGOR Chat", Meta = (Keywords = "C++"))
		FText ChannelNotFound;

	/** Insufficient privileges error */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TGOR Chat", Meta = (Keywords = "C++"))
		FText InsufficientPrivileges;


	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Starts listen server on a given port */
	UFUNCTION(BlueprintCallable, Category = "TGOR Online", Meta = (Keywords = "C++"))
		bool StartListenServer(FString Address, int32 Port);

	/** Stops listen server */
	UFUNCTION(BlueprintCallable, Category = "TGOR Online", Meta = (Keywords = "C++"))
		bool StopListenServer();

	/** Connect to server */
	UFUNCTION(BlueprintCallable, Category = "TGOR Online", Meta = (Keywords = "C++"))
		bool StartConnectingToServer(FString Address, int32 Port);

	/////////////////////////////////////////////// INTERNAL ///////////////////////////////////////////

protected:

private:
	
};
