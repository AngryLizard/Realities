// The Gateway of Realities: Planes of Existence.

#pragma once

#include "CoreMinimal.h"
#include "Realities/UI/Queries/Inventory/TGOR_ItemQuery.h"
#include "TGOR_SocketItemQuery.generated.h"

class UTGOR_ActionComponent;
class UTGOR_InventoryTabMenu;
class UTGOR_PilotComponent;
class UTGOR_Socket;

/**
 * 
 */
UCLASS()
class REALITIES_API UTGOR_SocketItemQuery : public UTGOR_ItemQuery
{
	GENERATED_BODY()
	
public:
	UTGOR_SocketItemQuery();
	virtual bool FilterItem(UTGOR_Item* Item) const override;
	virtual void AssignComponent(UTGOR_ActionComponent* ActionComponent, int32 ActionIdentifier, const FTGOR_ItemRestriction& Filter) override;

	/** Assigned movement parent */
	UPROPERTY(BlueprintReadWrite, Category = "!TGOR Inventory")
		TWeakObjectPtr<UTGOR_PilotComponent> Movement;

	/** Assigned socket */
	UPROPERTY(BlueprintReadWrite, Category = "!TGOR Inventory")
		UTGOR_Socket* Socket;

};
