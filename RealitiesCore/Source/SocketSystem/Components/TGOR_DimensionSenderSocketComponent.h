// The Gateway of Realities: Planes of Existence.

#pragma once

#include "CoreMinimal.h"

#include "DimensionSystem/Components/TGOR_DimensionSenderComponent.h"
#include "TGOR_DimensionSenderSocketComponent.generated.h"

class UTGOR_NamedSocket;
class UTGOR_SocketComponent;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class SOCKETSYSTEM_API UTGOR_DimensionSenderSocketComponent : public UTGOR_DimensionSenderComponent
{
	GENERATED_BODY()

public:	
	UTGOR_DimensionSenderSocketComponent();
	virtual void BeginPlay() override;

	//////////////////////////////////////////////// IMPLEMENTABLES /////////////////////////////////////////

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:
	
	/** Teleports any mobility linked to a given socket type */
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "!TGOR Dimension", Meta = (ExpandEnumAsExecs = "Branches", Keywords = "C++"))
		virtual void TeleportSocket(TSubclassOf<UTGOR_NamedSocket> SourceSocket, ETGOR_ComputeEnumeration& Branches);

	////////////////////////////////////////////////////////////////////////////////////////////////////
protected:

	/** Socket parent */
	UPROPERTY(BlueprintReadOnly, Category = "!TGOR Movement")
		UTGOR_SocketComponent* Parent;


};
