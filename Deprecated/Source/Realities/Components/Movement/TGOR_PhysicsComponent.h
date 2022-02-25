// The Gateway of Realities: Planes of Existence.

#pragma once

#include <functional>
#include "CoreMinimal.h"

#include "Realities/Components/Movement/TGOR_PilotComponent.h"
#include "TGOR_PhysicsComponent.generated.h"


UENUM(BlueprintType)
enum class ETGOR_MovementAuthorityMode : uint8
{
	SlaveClient		UMETA(DisplayName = "Slave client, Master server"),
	SlaveServer		UMETA(DisplayName = "Slave server, Master client"),
	SlaveSimulated	UMETA(DisplayName = "Slave simulated, Master server"),
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOutOfLevelDelegate);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FChangedVolumeDelegate, ATGOR_PhysicsVolume*, From, ATGOR_PhysicsVolume*, To);

/**
 * UTGOR_PhysicsComponent Keeps track of physics surroundings.
 */
UCLASS(ClassGroup = (Custom), Blueprintable, meta = (BlueprintSpawnableComponent))
class REALITIES_API UTGOR_PhysicsComponent : public UTGOR_PilotComponent
{
	GENERATED_BODY()
	
public:
	UTGOR_PhysicsComponent();

	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void OnPositionChange(const FTGOR_MovementPosition& Position) override;
	virtual void OnReparent(const FTGOR_MovementDynamic& Previous) override;

	/** Called when we left the level on surroundings update */
	UPROPERTY(BlueprintAssignable, Category = "TGOR Pipeline")
		FOutOfLevelDelegate OnOutOfLevel;

	/** Called when we left a volume (not called when entering a subvolume) */
	UPROPERTY(BlueprintAssignable, Category = "TGOR Pipeline")
		FChangedVolumeDelegate OnVolumeChanged;

	////////////////////////////////////////////////////////////////////////////////////////////////////
protected:

	/** Current capture */
	UPROPERTY(EditAnywhere, ReplicatedUsing = RepNotifyCapture, Category = "TGOR Movement")
		FTGOR_MovementCapture Capture;

	UFUNCTION()
		virtual void RepNotifyCapture(const FTGOR_MovementCapture& Old);

	/** Update surroundings in capture from current state */
	UFUNCTION(BlueprintCallable, Category = "TGOR Movement", Meta = (Keywords = "C++"))
		void UpdateSurroundings(const FVector& Location);

	/** Called if out of level */
	UFUNCTION(BlueprintCallable, Category = "TGOR Movement", Meta = (Keywords = "C++"))
		virtual void OutOfLevel();

	////////////////////////////////////////////////////////////////////////////////////////////////////
protected:

	/** Time not simulated last time */
	UPROPERTY()
		float PendingTime;

	/** Max timestep per simulation tick, or last Timestep length when StratisfyTimestep is false */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TGOR Movement")
		float SimulationTimestep;

	/** Simulate this object over a given amount of time over the current state */
	UFUNCTION(BlueprintCallable, Category = "TGOR Movement", Meta = (Keywords = "C++"))
		virtual float Simulate(float Time, bool Replay);

	/** Called before component triggered simulation tick (simulation can be ticked from outside and might need some cleanup using this function) */
	UFUNCTION(BlueprintCallable, Category = "TGOR Movement", Meta = (Keywords = "C++"))
		virtual void TickReset(float Time);

	/** Called when movement updated */
	UFUNCTION(BlueprintCallable, Category = "TGOR Movement", Meta = (Keywords = "C++"))
		virtual void MovementUpdate();

	/** Current master role */
	UFUNCTION(BlueprintPure, Category = "TGOR Movement", Meta = (Keywords = "C++"))
		ENetRole GetMasterRole() const;

	/** Current slave role */
	UFUNCTION(BlueprintPure, Category = "TGOR Movement", Meta = (Keywords = "C++"))
		ENetRole GetSlaveRole() const;

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Get current UpVector (Use External if available instead for better performance) */
	UFUNCTION(BlueprintPure, Category = "TGOR Movement", Meta = (Keywords = "C++"))
		FVector ComputePhysicsUpVector() const;

	/** Get current bouyancy ratio (1.0 for displaced mass being equal to own mass) */
	UFUNCTION(BlueprintPure, Category = "TGOR Movement", Meta = (Keywords = "C++"))
		float GetBouyancyRatio() const;

	/** Get current movement capture */
	UFUNCTION(BlueprintPure, Category = "TGOR Movement", Meta = (Keywords = "C++"))
		const FTGOR_MovementCapture& GetCapture() const;


	////////////////////////////////////////////////////////////////////////////////////////////////////
protected:

	/** Threshold when comparing current state on slave to last update to master, decides whether to send an update to the master. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TGOR Network")
		FTGOR_MovementThreshold BufferThreshold;

	/** Threshold when comparing current state on master to a received update from slave, decides whether to send an adjust to the slave. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TGOR Network")
		FTGOR_MovementThreshold AdjustThreshold;

	/** Threshold when comparing a state in the past on the slave to the received adjustment from the master, decides whether to replay all states since then. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TGOR Network")
		FTGOR_MovementThreshold UpdateThreshold;

	/** Error threshold (from UpdateThreshold) when replay is called. 0 for always and 1 for never. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TGOR Network")
		float ReplayThreshold;

	/** Authority mode */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TGOR Network")
		ETGOR_MovementAuthorityMode AuthorityMode;

	/** Whether the slave server can assume authority on grave movement changes */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TGOR Network")
		bool AuthoritiveSlave;
	
	/** Whether parent components are moved on replay */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TGOR Network")
		bool ReplayParents;

	////////////////////////////////////////////////////////////////////////////////////////////////////
protected:

	/** Store current movement state in buffer on slave */
	UFUNCTION(BlueprintCallable, Category = "TGOR Network", Meta = (Keywords = "C++"))
		void BufferUpdate();

	/** Send buffer index to slave on significant state change */
	UFUNCTION(BlueprintCallable, Category = "TGOR Network", Meta = (Keywords = "C++"))
		void BufferSpread();

	/** Send buffer index to slave */
	UFUNCTION(BlueprintCallable, Category = "TGOR Network", Meta = (Keywords = "C++"))
		void BufferForce();

	/////////////////////

	/** Store current state of given base parent to cache */
	UFUNCTION(BlueprintCallable, Category = "TGOR Network", Meta = (Keywords = "C++"))
		void CacheParentBuffer(UTGOR_MobilityComponent* Parent);

	/** Replay parent cache */
	UFUNCTION(BlueprintCallable, Category = "TGOR Network", Meta = (Keywords = "C++"))
		void SimulateParentCache();

	/** Current top buffer entry index */
	UFUNCTION(BlueprintPure, Category = "TGOR Network", Meta = (Keywords = "C++"))
		int32 CurrentBufferIndex() const;

	/** Replay simulation from index to current time */
	UFUNCTION(BlueprintCallable, Category = "TGOR Network", Meta = (Keywords = "C++"))
		void BufferReplay(int32 Index);

	/** Forward buffer index to acknowledge a timestamp */
	UFUNCTION(BlueprintCallable, Category = "TGOR Network", Meta = (Keywords = "C++"))
		void BufferForward(FTGOR_Time Timestamp);

	/////////////////////

	/** Send update to slave if threshold is reached between PrevIndex or NextIndex (force update if same index), return whether update happened. */
	UFUNCTION(BlueprintCallable, Category = "TGOR Network", Meta = (Keywords = "C++"))
		virtual bool BufferPeerAdjust(int32 PrevIndex, int32 NextIndex, const FTGOR_MovementThreshold& Threshold);

	/** Send force-update to master in case the slave has authority granted by AuthoritiveSlave (currently only really set-up for NetMode authority). */
	UFUNCTION(BlueprintCallable, Category = "TGOR Network", Meta = (Keywords = "C++"))
		virtual void BufferReplicate(int32 Index);


	/////////////////////

	/** Set current external state from buffer at index, uses only information that isn't changed through the movement mode including input and base information.
	  * Note that the current base can change to a different parent than stored in the parentBuffer, but parent buffer can still be applied and the base is probable to be nearby and influence movement anyway. */
	UFUNCTION(BlueprintCallable, Category = "TGOR Network", Meta = (Keywords = "C++"))
		virtual void LoadFromBufferExternal(int32 Index);

	/** Set current state from buffer at Index, uses all available information including transform and movement mode including LoadFromBufferExternal. */
	UFUNCTION(BlueprintCallable, Category = "TGOR Network", Meta = (Keywords = "C++"))
		virtual void LoadFromBufferWhole(int32 Index);

	/** Set buffer at Index to a linear lerp between Index and Index+1. Sets Index to new BufferBottom. */
	UFUNCTION(BlueprintCallable, Category = "TGOR Network", Meta = (Keywords = "C++"))
		virtual void LerpToBuffer(int32 Index, int32 NextIndex, float Alpha);

	/** Set buffer at Index from current state. */
	UFUNCTION(BlueprintCallable, Category = "TGOR Network", Meta = (Keywords = "C++"))
		virtual void StoreToBuffer(int32 Index);

	/** Set buffer size, buffers content may or may not be wiped. */
	UFUNCTION(BlueprintCallable, Category = "TGOR Network", Meta = (Keywords = "C++"))
		virtual void SetBufferSize(int32 Size);

	////////////////////////////////////////////////////////////////////////////////////////////////////
protected:

	using BufferCompareFunc = std::function<float(int32, const FTGOR_MovementThreshold&)>;
	using BufferOverrideFunc = std::function<void(int32)>;

	void TemplateAdjust(BufferCompareFunc Compare, BufferOverrideFunc Override);
	void TemplateUpdate(BufferOverrideFunc Override);

	////////////////////////////////////////////////////////////////////////////////////////////////////
protected:

	UFUNCTION(unreliable, Server, WithValidation)
		void ServerForceUpdate(FTGOR_MovementUpdate Update);
	void ServerForceUpdate_Implementation(FTGOR_MovementUpdate Update);
	bool ServerForceUpdate_Validate(FTGOR_MovementUpdate Update);

	UFUNCTION(unreliable, Client)
		void ClientForceUpdate(FTGOR_MovementUpdate Update);
	void ClientForceUpdate_Implementation(FTGOR_MovementUpdate Update);


	UFUNCTION(unreliable, Server, WithValidation)
		void ServerBufferAdjust(FTGOR_MovementUpdate Update);
	void ServerBufferAdjust_Implementation(FTGOR_MovementUpdate Update);
	bool ServerBufferAdjust_Validate(FTGOR_MovementUpdate Update);

	UFUNCTION(unreliable, Client)
		void ClientBufferAdjust(FTGOR_MovementUpdate Update);
	void ClientBufferAdjust_Implementation(FTGOR_MovementUpdate Update);

	UFUNCTION(unreliable, NetMulticast)
		void MulticastBufferAdjust(FTGOR_MovementUpdate Update);
	void MulticastBufferAdjust_Implementation(FTGOR_MovementUpdate Update);

	////////////////////////////////////////////////////////////////////////////////////////////////////
protected:

	/** Create movement update from index */
	UFUNCTION(BlueprintCallable, Category = "TGOR Network", Meta = (Keywords = "C++"))
		FTGOR_MovementUpdate CreateMovementUpdate(int32 Index) const;

	float BufferCompare(const FTGOR_MovementUpdate& Update, int32 Index, const FTGOR_MovementThreshold& Threshold) const;
	void BufferOverride(const FTGOR_MovementUpdate& Update, int32 Index);

	////////////////////////////////////////////////////////////////////////////////////////////////////
protected:

	/** Casted PrimitiveComponent that is piloted by this movement */
	UPROPERTY()
		TWeakObjectPtr<UPrimitiveComponent> BasePrimitiveComponent;

	////////////////////////////////////////////////////////////////////////////////////////////////////
private:

	/** Most recent buffer entry */
	UPROPERTY()
		int32 BufferTop;

	/** Least recent valid buffer entry */
	UPROPERTY()
		int32 BufferBottom;

	/** Timestamps for each buffer entry */
	UPROPERTY()
		TArray<FTGOR_Time> TimeBuffer;

	/** Base buffer record with parent information */
	UPROPERTY()
		TArray<FTGOR_MovementRecord> RecordBuffer;

	/** Frame buffer */
	UPROPERTY()
		TArray<FTGOR_MovementBody> BodyBuffer;

	/** Capture buffer */
	UPROPERTY()
		TArray<FTGOR_MovementCapture> CaptureBuffer;

	/** Cache to store parent bases */
	UPROPERTY()
		TMap<UTGOR_MobilityComponent*, FTGOR_MovementBase> ParentCache;



};
