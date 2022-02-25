// The Gateway of Realities: Planes of Existence.

#pragma once

#include "CoreMinimal.h"
#include "Framework/Commands/Commands.h"
#include "TGOR_WindowStyle.h"

class CTGOR_WindowCommands : public TCommands<CTGOR_WindowCommands>
{
public:

	CTGOR_WindowCommands()
		: TCommands<CTGOR_WindowCommands>(TEXT("RealitiesExtension"), NSLOCTEXT("Contexts", "Realities", "RealitiesExtension"), NAME_None, CTGOR_WindowStyle::GetStyleSetName())
	{
	}

	// TCommands<> interface
	virtual void RegisterCommands() override;

public:
	TSharedPtr< FUICommandInfo > OpenExtensionWindow;
};