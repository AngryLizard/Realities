// The Gateway of Realities: Planes of Existence.

#include "TGOR_TerminalTabMenu.h"

#include "Realities/UI/Queries/Inventory/TGOR_TerminalTabQuery.h"
#include "Realities/Components/Dimension/Interactable/TGOR_TerminalComponent.h"

UTGOR_TerminalTabMenu::UTGOR_TerminalTabMenu()
	: Super(),
	Terminal(nullptr)
{
}

void UTGOR_TerminalTabMenu::OpenMenu(UTGOR_Query* Query)
{
	// Only open terminal once if it changed
	UTGOR_TerminalTabQuery* TerminalTabQuery = Cast<UTGOR_TerminalTabQuery>(Query);
	if (IsValid(TerminalTabQuery) && TerminalTabQuery->Terminal != Terminal)
	{
		Terminal = TerminalTabQuery->Terminal;
		OpenTerminal();
	}

	Super::OpenMenu(Query);
}

void UTGOR_TerminalTabMenu::CloseMenu()
{
	// Prompt new call when menu got actually closed
	Terminal = nullptr;
	Super::CloseMenu();
}