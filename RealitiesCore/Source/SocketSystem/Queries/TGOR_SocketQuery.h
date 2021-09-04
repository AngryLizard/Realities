// The Gateway of Realities: Planes of Existence.

#pragma once

#include "CoreMinimal.h"
#include "UISystem/Queries/TGOR_ContentQuery.h"
#include "TGOR_SocketQuery.generated.h"

class UTGOR_NamedSocket;
class UTGOR_PilotComponent;
class UTGOR_SocketComponent;

/**
 * UTGOR_SocketQuery
 */
UCLASS()
class SOCKETSYSTEM_API UTGOR_SocketQuery : public UTGOR_ContentQuery
{
	GENERATED_BODY()
	
public:
	UTGOR_SocketQuery();
	virtual TArray<UTGOR_CoreContent*> QueryContent() const override;

	/** Construct list of sockets and attached pilots */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Inventory", Meta = (Keywords = "C++"))
		virtual void AssignComponent(UTGOR_SocketComponent* SocketComponent);

	/** Sockets */
	UPROPERTY(BlueprintReadOnly, Category = "!TGOR Inventory")
		TArray<UTGOR_NamedSocket*> Sockets;

	/** Pilots */
	UPROPERTY(BlueprintReadOnly, Category = "!TGOR Inventory")
		TArray<UTGOR_PilotComponent*> Attachments;

	/** Owning SocketComponent */
	UPROPERTY(BlueprintReadOnly, Category = "!TGOR Inventory")
		UTGOR_SocketComponent* HostComponent;
};
