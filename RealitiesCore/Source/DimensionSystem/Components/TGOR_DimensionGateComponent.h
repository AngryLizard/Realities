// The Gateway of Realities: Planes of Existence.

#pragma once

#include "CoreMinimal.h"

#include "TGOR_DimensionSenderComponent.h"
#include "TGOR_DimensionGateComponent.generated.h"

class ATGOR_Gate;


UENUM(BlueprintType)
enum class ETGOR_GatewayState : uint8
{
	Invalid,
	Idle, // Idle when no reality is active, idle pulsating
	Dialing, // Dialing new reality - spinning glowing rings
	Activating, // After reality is dialed, short activation state
	Active, // Idle when reality is active and counting down, Gateway is ready to send and receive
	Preparing, // When Gateway is preparing to transceive, this state exists to give off a warning so creatures can stand clear off the Gateway.
	SpinUp, // When Gateway is spinning its rings before transmission
	Transceiving, // When Gateway is sending and receiving creatures to/from a reality, it is all done at once, exchanging the places for the groups of creatures involved.
	Deactivating // When reality has expired and gateway deactivates, going back into idle state
};


DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FGateStateChangedDelegate, ETGOR_GatewayState, Old, ETGOR_GatewayState, New);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class DIMENSIONSYSTEM_API UTGOR_DimensionGateComponent : public UTGOR_DimensionSenderComponent
{
	GENERATED_BODY()

		friend class ATGOR_GateActor;

public:	
	UTGOR_DimensionGateComponent();
	virtual void GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const override;

	virtual bool IsReady() const override;

	virtual void ChangeLoaderLevel(ETGOR_DimensionLoaderLevel Level) override;
	virtual void DialPortal(const FTGOR_ConnectionSelection& Selection) override;
	virtual void DialPortalName(const FName& Portal) override;

	//////////////////////////////////////////////// IMPLEMENTABLES /////////////////////////////////////////

	UPROPERTY(BlueprintAssignable, Category = "!TGOR Dimension")
		FGateStateChangedDelegate OnGateStateChanged;


	////////////////////////////////////////////////////////////////////////////////////////////////////
protected:

	/** Current Gateway state */
	UPROPERTY(ReplicatedUsing = RepNotifyState, BlueprintReadOnly, Category = "!TGOR Dimension", Meta = (Keywords = "C++"))
		ETGOR_GatewayState CurrentState;

	UFUNCTION()
		void RepNotifyState(ETGOR_GatewayState Old);

	/** Whether this gate has started the teleport */
	UPROPERTY(BlueprintReadOnly, Category = "!TGOR Dimension", Meta = (Keywords = "C++"))
		bool IsMaster;

	/** Currently linked Gateway */
	UPROPERTY(BlueprintReadOnly, Category = "!TGOR Dimension", Meta = (Keywords = "C++"))
		UTGOR_DimensionGateComponent* GateLink;

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Changes the current state of this gate */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Dimension", Meta = (Keywords = "C++"))
		void ChangeState(ETGOR_GatewayState State);

	/** Starts teleporting procedure */
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "!TGOR Dimension", Meta = (Keywords = "C++"))
		void StartTeleport();

	/** Whether currently in teleportation procedure */
	UFUNCTION(BlueprintPure, Category = "!TGOR Dimension", Meta = (Keywords = "C++"))
		bool IsTeleporting() const;

	////////////////////////////////////////////////////////////////////////////////////////////////////
protected:

	/** Links with gate on the other side */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Dimension", Meta = (Keywords = "C++"))
		void EstablishLink();

};
