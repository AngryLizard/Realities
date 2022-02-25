// The Gateway of Realities: Planes of Existence.

#pragma once

#include "CoreMinimal.h"
#include "Realities/UI/Queries/Inventory/TGOR_TabQuery.h"
#include "TGOR_SocketTabQuery.generated.h"

class UTGOR_ActionComponent;
class UTGOR_InventoryTabMenu;
class UTGOR_PilotComponent;
class UTGOR_Socket;

/**
 * 
 */
UCLASS()
class REALITIES_API UTGOR_SocketTabQuery : public UTGOR_TabQuery
{
	GENERATED_BODY()
	
public:
	UTGOR_SocketTabQuery();
	virtual bool FilterItem(UTGOR_ItemStorage* Storage) const override;
	virtual void AssignTab(UTGOR_ActionComponent* ActionComponent, TSubclassOf<UTGOR_InventoryTabMenu> TabMenu) override;

	/** Assigned movement parent */
	UPROPERTY(BlueprintReadWrite, Category = "TGOR Inventory")
		TWeakObjectPtr<UTGOR_PilotComponent> Movement;

	/** Assigned socket */
	UPROPERTY(BlueprintReadWrite, Category = "TGOR Inventory")
		UTGOR_Socket* Socket;

};
