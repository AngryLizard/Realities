// The Gateway of Realities: Planes of Existence.

#pragma once

#include "Realities/UI/Inventory/TGOR_InventoryTabMenu.h"
#include "TGOR_TerminalTabMenu.generated.h"

class UTGOR_TerminalComponent;

/**
 * 
 */
UCLASS()
class REALITIES_API UTGOR_TerminalTabMenu : public UTGOR_InventoryTabMenu
{
	GENERATED_BODY()
	
public:

	////////////////////////////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////// ENGINE ////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////

	UTGOR_TerminalTabMenu();

	virtual void OpenMenu(UTGOR_Query* Query) override;
	virtual void CloseMenu() override;


	/** Paired terminal component */
	UPROPERTY(BlueprintReadOnly, Category = "TGOR Action")
		TWeakObjectPtr<UTGOR_TerminalComponent> Terminal;

	/** Called when a terminal was opened by this menu (forward from OpenMenu) */
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = "TGOR Interface", Meta = (Keywords = "C++"))
		void OpenTerminal();

	/** Called when the terminal was approved by server to be supported by owning pawn */
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = "TGOR Interface", Meta = (Keywords = "C++"))
		void SupportTerminal(const FTGOR_SlotIndex& SlotIndex);

	/////////////////////////////////////////////// INTERNAL ///////////////////////////////////////////

protected:
private:

};
