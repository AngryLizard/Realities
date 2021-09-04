// The Gateway of Realities: Planes of Existence.

#pragma once

#include "CoreMinimal.h"

#include "TGOR_Socket.h"
#include "TGOR_NamedSocket.generated.h"

class UTGOR_Attribute;

/**
 * 
 */
UCLASS(Blueprintable)
class SOCKETSYSTEM_API UTGOR_NamedSocket : public UTGOR_Socket
{
	GENERATED_BODY()

public:
	UTGOR_NamedSocket();

	////////////////////////////////////////////////////////////////////////////////////////////////////

	virtual UTGOR_MobilityComponent* QuerySocket(UTGOR_InteractableComponent* Component, FTGOR_MovementPosition& Position) const override;
	virtual UTGOR_MobilityComponent* QuerySocketWith(UActorComponent* Component, FTGOR_MovementPosition& Position) const override;

	////////////////////////////////////////////////////////////////////////////////////////////////////

	/** Checks whether Component fits socket */
	UFUNCTION(BlueprintPure, Category = "!TGOR Sockets", Meta = (Keywords = "C++"))
		bool CheckDimensions(const UTGOR_SocketComponent* Parent, const FTGOR_MovementShape& Shape) const;

	/** Name of the socket virtual bone */
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		FName SocketName;

	/** Radius allowed to enter this socket (relative to parent radius) */
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		float Radius;

	/** Height allowed to enter this socket (relative to parent height) */
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		float Height;

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Size multiplayer attribute */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "!TGOR Sockets")
		TSubclassOf<UTGOR_Attribute> SizeAttribute;

};