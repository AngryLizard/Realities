// The Gateway of Realities: Planes of Existence.


#include "TGOR_Overlay.h"
#include "UISystem/UI/TGOR_Menu.h"

UTGOR_Overlay::UTGOR_Overlay()
	: Super(),
	StartOpen(false),
	Layer(0)
{
}

bool UTGOR_Overlay::Validate_Implementation()
{
	if (!*Menu)
	{
		ERRET("No menu widget type defined", Error, false);
	}

	return Super::Validate_Implementation();
}

UTGOR_Menu* UTGOR_Overlay::InitialiseMenu()
{
	if (*Menu)
	{
		UTGOR_Menu* Widget = CreateWidget<UTGOR_Menu>(GetWorld(), Menu);
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
