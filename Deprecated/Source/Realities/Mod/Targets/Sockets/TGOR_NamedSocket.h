// The Gateway of Realities: Planes of Existence.

#pragma once

#include "CoreMinimal.h"
#include "Realities/Mod/Targets/Sockets/TGOR_Socket.h"
#include "TGOR_NamedSocket.generated.h"

/**
 * 
 */
UCLASS(Blueprintable)
class REALITIES_API UTGOR_NamedSocket : public UTGOR_Socket
{
	GENERATED_BODY()

public:
	UTGOR_NamedSocket();
	virtual void BuildStorage(UTGOR_SocketStorage* Storage) override;

	////////////////////////////////////////////////////////////////////////////////////////////////////

	virtual UTGOR_MobilityComponent* QuerySocket(UTGOR_InteractableComponent* Component, FTGOR_MovementPosition& Position) const override;
	virtual UTGOR_MobilityComponent* QuerySocketWith(USceneComponent* Component, FTGOR_MovementPosition& Position) const override;
	virtual bool AttachToSocket(UTGOR_MobilityComponent* Parent, UTGOR_PilotComponent* Component) override;
	virtual bool CanAttachToSocket(UTGOR_MobilityComponent* Parent, UTGOR_PilotComponent* Component) const override;
	virtual bool CanFitToSocket(UTGOR_MobilityComponent* Parent, const FTGOR_MovementShape& Shape) const override;

	////////////////////////////////////////////////////////////////////////////////////////////////////

	virtual void Tick(UTGOR_DimensionComponent* Owner, UTGOR_SocketStorage* Storage, float DeltaSeconds) override;
	virtual void Attach(UTGOR_SocketStorage* Storage, USceneComponent* Attachee) override;
	virtual UTGOR_MobilityComponent* GetParent(UTGOR_SocketStorage* Storage) const override;
	virtual FTGOR_MovementDynamic GetTransform(UTGOR_MobilityComponent* Owner, UTGOR_SocketStorage* Storage) const override;

	////////////////////////////////////////////////////////////////////////////////////////////////////

	/** Checks whether Component fits socket */
	UFUNCTION(BlueprintPure, Category = "TGOR Dimension", Meta = (Keywords = "C++"))
		bool CheckDimensions(const UTGOR_MobilityComponent* Parent, const FTGOR_MovementShape& Shape) const;

	/** Name of the socket virtual bone */
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		FName SocketName;

	/** Radius allowed to enter this socket (relative to parent radius) */
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		float Radius;

	/** Height allowed to enter this socket (relative to parent height) */
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		float Height;

};