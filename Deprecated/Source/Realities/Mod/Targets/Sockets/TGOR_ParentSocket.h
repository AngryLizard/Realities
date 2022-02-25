// The Gateway of Realities: Planes of Existence.

#pragma once

#include "CoreMinimal.h"
#include "Realities/Mod/Targets/Sockets/TGOR_Socket.h"
#include "TGOR_ParentSocket.generated.h"

/**
 * 
 */
UCLASS(Blueprintable)
class REALITIES_API UTGOR_ParentSocket : public UTGOR_Socket
{
	GENERATED_BODY()

public:
	UTGOR_ParentSocket();
	virtual void BuildStorage(UTGOR_SocketStorage* Storage) override;

	////////////////////////////////////////////////////////////////////////////////////////////////////

	virtual void Tick(UTGOR_DimensionComponent* Owner, UTGOR_SocketStorage* Storage, float DeltaSeconds) override;
	virtual void Attach(UTGOR_SocketStorage* Storage, USceneComponent* Attachee) override;
	virtual UTGOR_MobilityComponent* GetParent(UTGOR_SocketStorage* Storage) const override;
	virtual FTGOR_MovementDynamic GetTransform(UTGOR_MobilityComponent* Owner, UTGOR_SocketStorage* Storage) const override;
};