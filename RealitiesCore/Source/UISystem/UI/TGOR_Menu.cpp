// The Gateway of Realities: Planes of Existence.


#include "TGOR_Menu.h"

#include "Animation/WidgetAnimation.h"

#include "UISystem/Gameplay/TGOR_HUD.h"
#include "CoreSystem/TGOR_Singleton.h"

UTGOR_Menu::UTGOR_Menu(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	IgnoreFocus = false;
	IsOpen = false;

	MenuLayer = -1;
	ClosePending = false;
	OpenPending = false;
}

void UTGOR_Menu::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (ClosePending)
	{
		ETGOR_BoolEnumeration State = ETGOR_BoolEnumeration::IsNot;
		IsPlayingTransition(State);
		if (State == ETGOR_BoolEnumeration::IsNot)
		{
			FinishCloseMenu();
		}
	}

	if (OpenPending)
	{
		ETGOR_BoolEnumeration State = ETGOR_BoolEnumeration::IsNot;
		IsPlayingTransition(State);
		if (State == ETGOR_BoolEnumeration::IsNot)
		{
			FinishOpenMenu();
		}
	}
}

void UTGOR_Menu::OnWidgetRebuilt()
{
	Super::OnWidgetRebuilt();
}

////////////////////////////////////////////////////////////////////////////////////////////////////

bool UTGOR_Menu::IgnoresFocus() const
{
	return IgnoreFocus;
}

int32 UTGOR_Menu::GetMenuLayer() const
{
	return MenuLayer;
}

bool UTGOR_Menu::CanClose()
{
	return(IsOpen && Type != ETGOR_MenuTypeEnumeration::Permanent);
}

bool UTGOR_Menu::IsMenuOpen()
{
	return(IsOpen);
}


void UTGOR_Menu::InitialiseMenu(int32 Layer)
{
	MenuLayer = Layer;
}

void UTGOR_Menu::OpenMenu(UTGOR_Query* Query)
{
	if (IsOpen)
	{
		// Only call blueprint updates if already open in case query changed
		ApplyQuery(Query);
		PrepareOpen();
		return;
	}

	StartOpenMenu(Query);
}

void UTGOR_Menu::StartOpenMenu(UTGOR_Query* Query)
{
	// Add to viewport if possible and desired
	if (Type == ETGOR_MenuTypeEnumeration::Viewport &&
		!IsInViewport() && GetParent() == nullptr)
	{
		AddToViewport(MenuLayer);
	}

	SetVisibility(ESlateVisibility::SelfHitTestInvisible);

	IsOpen = true;
	ClosePending = false;
	OpenPending = true;

	// Notify Blueprint
	ApplyQuery(Query);
	PrepareOpen();

	// Nofity Hud
	APlayerController* Controller = GetOwningPlayer();
	if (IsValid(Controller))
	{
		ATGOR_HUD* Hud = Cast<ATGOR_HUD>(Controller->GetHUD());
		if (IsValid(Hud))
		{
			Hud->FocusChanged();
		}
	}
}

void UTGOR_Menu::FinishOpenMenu()
{
	OpenPending = false;
	
	Activate();
}


void UTGOR_Menu::CloseMenu()
{
	if (!IsOpen) return;

	StartCloseMenu();
}

void UTGOR_Menu::StartCloseMenu()
{
	IsOpen = false;
	ClosePending = true;
	OpenPending = false;

	PrepareClose();
}

void UTGOR_Menu::FinishCloseMenu()
{
	ClosePending = false;

	SetVisibility(ESlateVisibility::Collapsed);

	if (Type == ETGOR_MenuTypeEnumeration::Viewport)
	{
		RemoveFromParent();
	}

	Cleanup();

	// Nofity Hud
	APlayerController* Controller = GetOwningPlayer();
	if (IsValid(Controller))
	{
		ATGOR_HUD* Hud = Cast<ATGOR_HUD>(Controller->GetHUD());
		if (IsValid(Hud))
		{
			Hud->FocusChanged();
		}
	}
}


////////////////////////////////////////////////////////////////////////////////////////////////////


void UTGOR_Menu::AnimateMenu(const FTGOR_MenuAnimation& Anim, bool& State, bool Active)
{
	if (State == Active) return;
	State = Active;

	UWidgetAnimation* Stop = Active ? Anim.Retract : Anim.Expand;
	UWidgetAnimation* Start = Active ? Anim.Expand : Anim.Retract;
	
	if (Stop != nullptr && IsAnimationPlaying(Stop))
	{
		float Current = PauseAnimation(Stop);
		float Time = Stop->GetEndTime() - Current;
		PlayAnimation(Stop, Time, 1, EUMGSequencePlayMode::Reverse);
	}
	else if (Start != nullptr && !IsAnimationPlaying(Start))
	{
		PlayAnimation(Start, 0.0f, 1, EUMGSequencePlayMode::Forward);
	}
}
