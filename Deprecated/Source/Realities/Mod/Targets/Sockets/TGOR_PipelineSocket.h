// The Gateway of Realities: Planes of Existence.

#pragma once

#include "CoreMinimal.h"
#include "Realities/Mod/Targets/Sockets/TGOR_Socket.h"
#include "TGOR_PipelineSocket.generated.h"

class UTGOR_PipelineComponent;

/**
 * 
 */
UCLASS(Blueprintable)
class REALITIES_API UTGOR_PipelineSocket : public UTGOR_Socket
{
	GENERATED_BODY()

public:
	UTGOR_PipelineSocket();
	virtual void BuildStorage(UTGOR_SocketStorage* Storage) override;

	virtual UTGOR_MobilityComponent* QuerySocket(UTGOR_InteractableComponent* Component, FTGOR_MovementPosition& Position) const override;
	virtual UTGOR_MobilityComponent* QuerySocketWith(USceneComponent* Component, FTGOR_MovementPosition& Position) const override;
	virtual bool AttachToSocket(UTGOR_MobilityComponent* Parent, UTGOR_PilotComponent* Component) override;
	virtual bool CanAttachToSocket(UTGOR_MobilityComponent* Parent, UTGOR_PilotComponent* Component) const override;
	virtual bool CanFitToSocket(UTGOR_MobilityComponent* Parent, const FTGOR_MovementShape& Shape) const override;

	////////////////////////////////////////////////////////////////////////////////////////////////////

	virtual void Tick(UTGOR_DimensionComponent* Owner, UTGOR_SocketStorage* Storage, float DeltaSeconds) override;
	virtual UTGOR_MobilityComponent* GetParent(UTGOR_SocketStorage* Storage) const override;
	virtual FTGOR_MovementDynamic GetTransform(UTGOR_MobilityComponent* Owner, UTGOR_SocketStorage* Storage) const override;

	////////////////////////////////////////////////////////////////////////////////////////////////////

	/** Get (entry) distance of this socket along the spline */
	UFUNCTION(BlueprintImplementableEvent, Category = "TGOR Socket", Meta = (Keywords = "C++"))
		float GetSplineDistance(const UTGOR_PipelineComponent* Parent) const;

	/** Whether this socket points along or against the pipeline */
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		bool Forward;
};