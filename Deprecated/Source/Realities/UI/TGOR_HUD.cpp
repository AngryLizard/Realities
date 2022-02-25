// The Gateway of Realities: Planes of Existence.


#include "TGOR_HUD.h"

#include "Realities/UI/TGOR_Menu.h"
#include "Realities/Base/TGOR_Singleton.h"
#include "Realities/Base/Content/TGOR_ContentManager.h"
#include "Realities/UI/System/TGOR_InteractionOverlay.h"
#include "Realities/Mod/Overlays/TGOR_Overlay.h"


FTGOR_MenuPair::FTGOR_MenuPair()
: Menu(nullptr),
Overlay(nullptr)	
{
}

FTGOR_MenuPair::FTGOR_MenuPair(UTGOR_Overlay* Overlay, UTGOR_Menu* Menu)
: Menu(Menu),
Overlay(Overlay)	
{

}

////////////////////////////////////////////////////////////////////////////////////////////////////


ATGOR_HUD::ATGOR_HUD()
	: Super()
{
}

void ATGOR_HUD::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ATGOR_HUD::BeginPlay()
{
	Super::BeginPlay();

	SINGLETON_CHK;

	UTGOR_ContentManager* ContentManager = Singleton->GetContentManager();
	TArray<UTGOR_Overlay*> Overlays = ContentManager->GetTListFromType<UTGOR_Overlay>();
	for (UTGOR_Overlay* Overlay : Overlays)
	{
		UTGOR_Menu* Menu = Overlay->InitialiseMenu();
		if (IsValid(Menu))
		{
			Menus.Emplace(FTGOR_MenuPair(Overlay, Menu));
		}
	}
}

TSet<UTGOR_Content*> ATGOR_HUD::GetActiveContent_Implementation() const
{
	TSet<UTGOR_Content*> Active;
	for (const FTGOR_MenuPair& Menu : Menus)
	{
		if (IsValid(Menu.Menu) && Menu.Menu->IsMenuOpen())
		{
			Active.Emplace(Menu.Overlay);
		}
	}
	return Active;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void ATGOR_HUD::OpenMenu(TSubclassOf<UTGOR_Overlay> Type, UTGOR_Query* Query)
{
	const FTGOR_MenuPair Pair = GetMenu(Type);
	if (IsValid(Pair.Menu))
	{
		Pair.Menu->OpenMenu(Query);
	}
}

void ATGOR_HUD::OpenWorldMenu(UObject* WorldContext, TSubclassOf<UTGOR_Overlay> Type, UTGOR_Query* Query)
{
	ATGOR_HUD* Hud = GetWorldHUD(WorldContext);
	if (IsValid(Hud))
	{
		Hud->OpenMenu(Type, Query);
	}
}

void ATGOR_HUD::CloseMenu(TSubclassOf<UTGOR_Overlay> Type)
{
	const FTGOR_MenuPair Pair = GetMenu(Type);
	if (IsValid(Pair.Menu))
	{
		Pair.Menu->CloseMenu();
	}
}

void ATGOR_HUD::CloseWorldMenu(UObject* WorldContext, TSubclassOf<UTGOR_Overlay> Type)
{
	ATGOR_HUD* Hud = GetWorldHUD(WorldContext);
	if (IsValid(Hud))
	{
		Hud->CloseMenu(Type);
	}
}



FTGOR_MenuPair ATGOR_HUD::GetMenu(TSubclassOf<UTGOR_Overlay> Type)
{
	for (const FTGOR_MenuPair& Menu : Menus)
	{
		if (Menu.Overlay->IsA(Type))
		{
			return Menu;
		}
	}
	return FTGOR_MenuPair();
}

FTGOR_MenuPair ATGOR_HUD::GetWorldMenu(UObject* WorldContext, TSubclassOf<UTGOR_Overlay> Type)
{
	ATGOR_HUD* Hud = GetWorldHUD(WorldContext);
	if (IsValid(Hud))
	{
		return(Hud->GetMenu(Type));
	}
	return FTGOR_MenuPair();
}

ATGOR_HUD* ATGOR_HUD::GetWorldHUD(UObject* WorldContext)
{
	UWorld* World = WorldContext->GetWorld();
	if (IsValid(World))
	{
		APlayerController* Controller = World->GetFirstPlayerController();
		if (IsValid(Controller))
		{
			return Cast<ATGOR_HUD>(Controller->GetHUD());
		}
		else
		{
			ERRET("Invalid local controller", Error, nullptr)
		}
	}
	else
	{
		ERRET("Invalid world reference", Error, nullptr)
	}
}


////////////////////////////////////////////////////////////////////////////////////////////////////

bool ATGOR_HUD::IsInMenu(bool CloseableOnly)
{
	for (const FTGOR_MenuPair& Menu : Menus)
	{
		// Check for open menus that are closeable
		if (Menu.Menu->IsMenuOpen() &&
			!Menu.Menu->IgnoresFocus() && (!CloseableOnly || Menu.Menu->CanClose()))
		{
			return(true);
		}
	}
	return false;
}

UTGOR_Menu* ATGOR_HUD::GetTop(bool CloseableOnly)
{
	UTGOR_Menu* Top = nullptr;
	for (const FTGOR_MenuPair& Menu : Menus)
	{
		// Check for open menus that are closeable and have higher layer than current top
		if (Menu.Menu->IsMenuOpen() &&
			!Menu.Menu->IgnoresFocus() && (!CloseableOnly || Menu.Menu->CanClose()) &&
			(!Top || Menu.Menu->GetMenuLayer() >= Top->GetMenuLayer()))
		{
			Top = Menu.Menu;
		}
	}
	return Top;
}

void ATGOR_HUD::Escape(bool EscapeAll)
{
	// If there is no top open escape menu
	UTGOR_Menu* Top = GetTop(true);
	if (!IsValid(Top))
	{
		OpenMenu(EscapeMenu, nullptr);
	}
	else
	{
		// Close one/all top menus
		do
		{
			Top->CloseMenu();
			if (!EscapeAll)
			{
				return;
			}
			Top = GetTop(true);
		}
		while (IsValid(Top));
	}
}

void ATGOR_HUD::WorldEscape(UObject* WorldContext, bool EscapeAll)
{
	UWorld* World = WorldContext->GetWorld();
	if (IsValid(World))
	{
		APlayerController* Controller = World->GetFirstPlayerController();
		if (IsValid(Controller))
		{
			ATGOR_HUD* Hud = Cast<ATGOR_HUD>(Controller->GetHUD());
			if (IsValid(Hud))
			{
				Hud->Escape(EscapeAll);
			}
		}
	}
	else
	{
		ERROR("Invalid world reference", Error)
	}
}
