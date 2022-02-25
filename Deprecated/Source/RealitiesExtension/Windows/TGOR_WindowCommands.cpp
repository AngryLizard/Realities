// The Gateway of Realities: Planes of Existence.


#include "TGOR_WindowCommands.h"

#define LOCTEXT_NAMESPACE "RealitiesExtension"

void CTGOR_WindowCommands::RegisterCommands()
{
	UI_COMMAND(OpenExtensionWindow, "Realities", "Bring up mods window", EUserInterfaceActionType::Button, FInputGesture());
}

#undef LOCTEXT_NAMESPACE