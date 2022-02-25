// The Gateway of Realities: Planes of Existence.

#pragma once

#include "CoreMinimal.h"
#include "Realities/Base/Instances/Inventory/TGOR_ItemInstance.h"
#include "Realities/Base/Instances/System/TGOR_IndexInstance.h"
#include "Realities/UI/Queries/Inventory/TGOR_TabQuery.h"
#include "TGOR_TerminalTabQuery.generated.h"

class UTGOR_ActionComponent;
class UTGOR_InventoryTabMenu;
class UTGOR_TerminalComponent;

/**
 * 
 */
UCLASS()
class REALITIES_API UTGOR_TerminalTabQuery : public UTGOR_TabQuery
{
	GENERATED_BODY()
	
public:
	UTGOR_TerminalTabQuery();
	virtual bool FilterItem(UTGOR_ItemStorage* Storage) const override;
	virtual void AssignTab(UTGOR_ActionComponent* ActionComponent, TSubclassOf<UTGOR_InventoryTabMenu> TabMenu) override;


	/** Assigned terminal */
	UPROPERTY(BlueprintReadWrite, Category = "TGOR Inventory")
		TWeakObjectPtr<UTGOR_TerminalComponent> Terminal;

};
