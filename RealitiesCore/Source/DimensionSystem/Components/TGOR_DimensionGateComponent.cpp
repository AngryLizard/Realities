// The Gateway of Realities: Planes of Existence.

#include "TGOR_DimensionGateComponent.h"

#include "CoreSystem/TGOR_Singleton.h"
#include "DimensionSystem/Components/TGOR_DimensionReceiverComponent.h"



#include "Net/UnrealNetwork.h"


UTGOR_DimensionGateComponent::UTGOR_DimensionGateComponent()
:	Super(),
	CurrentState(ETGOR_GatewayState::Invalid),
	IsMaster(true)
{
	SetIsReplicatedByDefault(true);
	//ConnectionName = "Gate";
}

void UTGOR_DimensionGateComponent::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(UTGOR_DimensionGateComponent, CurrentState, COND_None);
}


bool UTGOR_DimensionGateComponent::IsReady() const
{
	return CurrentState != ETGOR_GatewayState::Transceiving && Super::IsReady();
}

void UTGOR_DimensionGateComponent::ChangeLoaderLevel(ETGOR_DimensionLoaderLevel Level)
{
	Super::ChangeLoaderLevel(Level);
	switch (Level)
	{
	case ETGOR_DimensionLoaderLevel::Loading:
		ChangeState(ETGOR_GatewayState::Dialing); 
		break;

	case ETGOR_DimensionLoaderLevel::Loaded:
		EstablishLink();
		ChangeState(ETGOR_GatewayState::Activating);
		break;

	case ETGOR_DimensionLoaderLevel::Idle:
		ChangeState(ETGOR_GatewayState::Deactivating);
		GateLink = nullptr;
		break;
	}
}

void UTGOR_DimensionGateComponent::DialPortal(const FTGOR_ConnectionSelection& Selection)
{
	// Queue loading, which is going to wait on the reality to unload fully before starting
	Super::DialPortal(Selection);

	ChangeState(ETGOR_GatewayState::Dialing);
}

void UTGOR_DimensionGateComponent::DialPortalName(const FName& Portal)
{
	Super::DialPortalName(Portal);
	EstablishLink();

	ChangeState(ETGOR_GatewayState::Active);
}

void UTGOR_DimensionGateComponent::RepNotifyState(ETGOR_GatewayState Old)
{
	OnGateStateChanged.Broadcast(Old, CurrentState);
}


void UTGOR_DimensionGateComponent::ChangeState(ETGOR_GatewayState State)
{
	if (CurrentState != State)
	{
		ETGOR_GatewayState Old = CurrentState;
		CurrentState = State;

		OnGateStateChanged.Broadcast(Old, State);
	}

	if (IsValid(GateLink) && IsMaster)
	{
		GateLink->ChangeState(State);
	}
}

void UTGOR_DimensionGateComponent::StartTeleport()
{
	IsMaster = true;

	if (IsValid(GateLink))
	{
		GateLink->IsMaster = false;
	}
	ChangeState(ETGOR_GatewayState::Preparing);
}

bool UTGOR_DimensionGateComponent::IsTeleporting() const
{
	return CurrentState == ETGOR_GatewayState::Preparing || CurrentState == ETGOR_GatewayState::SpinUp || CurrentState == ETGOR_GatewayState::Transceiving;
}

void UTGOR_DimensionGateComponent::EstablishLink()
{
	SINGLETON_CHK;
	ETGOR_ComputeEnumeration State = ETGOR_ComputeEnumeration::Failed;
	//Connect(State);
	if (State == ETGOR_ComputeEnumeration::Failed)
	{
		ERROR("Failed to link gates", Warning);
	}
	else
	{
		// Make sure we are not already linked, otherwise ChangeLoaderLevel creates an endless loop
		UTGOR_DimensionReceiverComponent* Receiver = GetReceiverComponent();
		if (IsValid(Receiver))
		{
			UTGOR_DimensionGateComponent* Link = Receiver->GetOwnerComponent<UTGOR_DimensionGateComponent>();
			if (GateLink != Link)
			{
				// Link back
				GateLink = Link;
				Link->GateLink = this;

				// Set master and slave status
				IsMaster = true;
				Link->IsMaster = false;

				// Set other to be loaded
				Link->ChangeLoaderLevel(ETGOR_DimensionLoaderLevel::Loaded);
			}
		}
	}
}