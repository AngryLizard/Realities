// The Gateway of Realities: Planes of Existence.


#include "TGOR_Overlay.h"
#include "Realities/UI/TGOR_Menu.h"

UTGOR_Overlay::UTGOR_Overlay()
	: Super(),
	StartOpen(false),
	Layer(0)
{
}


UTGOR_Menu* UTGOR_Overlay::InitialiseMenu()
{
	if (*Menu)
	{
		const FName Name = FName(*GetDefaultName());
		UTGOR_Menu* Widget = CreateWidget<UTGOR_Menu>(GetWorld(), Menu, Name);
		if (IsValid(Widget))
		{
			Widget->InitialiseMenu(Layer);
			if (StartOpen)
			{
				Widget->OpenMenu(nullptr);
			}
			else
			{
				Widget->CloseMenu();
			}
		}
		return Widget;
	}

	ERRET(FString::Printf(TEXT("%s menu has invalid class"), *GetDefaultName()), Error, nullptr);
}
