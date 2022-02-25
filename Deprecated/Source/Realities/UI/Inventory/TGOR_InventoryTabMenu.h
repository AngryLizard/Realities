// The Gateway of Realities: Planes of Existence.

#pragma once

#include "Realities/UI/TGOR_Menu.h"
#include "TGOR_InventoryTabMenu.generated.h"

class UTGOR_ItemStorage;
class UTGOR_TabQuery;

/**
 * 
 */
UCLASS()
class REALITIES_API UTGOR_InventoryTabMenu : public UTGOR_Menu
{
	GENERATED_BODY()
	
public:

	////////////////////////////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////// ENGINE ////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////

	UTGOR_InventoryTabMenu();
	virtual void OpenMenu(UTGOR_Query* Query) override;

	////////////////////////////////////////////////////////////////////////////////////////////////////

	/** Visualise places where given storage can be placed in */
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "TGOR Inventory", Meta = (Keywords = "C++"))
		void Showcase(UTGOR_ItemStorage* Storage);
	
	/** Query assigned to this tab (same that was used for ItemRow) */
	UPROPERTY(BlueprintReadOnly, Category = "TGOR Inventory")
		UTGOR_TabQuery* TabQuery;


	/** Whether this tab is to be displayed without item row */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TGOR Inventory")
		bool HasItemRow;

	/////////////////////////////////////////////// INTERNAL ///////////////////////////////////////////

protected:
private:

};
