// The Gateway of Realities: Planes of Existence.

#pragma once

#include "CoreMinimal.h"
#include "TGOR_Socket.h"
#include "TGOR_PipelineSocket.generated.h"

class UTGOR_PipelineComponent;

/**
 * 
 */
UCLASS(Blueprintable)
class SOCKETSYSTEM_API UTGOR_PipelineSocket : public UTGOR_Socket
{
	GENERATED_BODY()

public:
	UTGOR_PipelineSocket();

	virtual UTGOR_MobilityComponent* QuerySocket(UTGOR_InteractableComponent* Component, FTGOR_MovementPosition& Position) const override;
	virtual UTGOR_MobilityComponent* QuerySocketWith(UActorComponent* Component, FTGOR_MovementPosition& Position) const override;

	////////////////////////////////////////////////////////////////////////////////////////////////////

	/** Get (entry) distance of this socket along the spline */
	UFUNCTION(BlueprintImplementableEvent, Category = "!TGOR Socket", Meta = (Keywords = "C++"))
		float GetSplineDistance(const UTGOR_PipelineComponent* Parent) const;

	/** Whether this socket points along or against the pipeline */
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		bool Forward;
};