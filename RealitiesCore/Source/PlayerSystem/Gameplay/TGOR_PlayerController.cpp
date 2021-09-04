// The Gateway of Realities: Planes of Existence.


#include "TGOR_PlayerController.h"

#include "CoreSystem/TGOR_Singleton.h"
#include "UISystem/UI/TGOR_Widget.h"
#include "UISystem/Gameplay/TGOR_HUD.h"

//#include "CoreSystem/Storage/TGOR_Package.h"
#include "Components/CanvasPanelSlot.h"
#include "Blueprint/WidgetLayoutLibrary.h"

#include "Engine.h"
#include "Kismet/KismetSystemLibrary.h"
#include "ActionSystem/Content/TGOR_Action.h"

ATGOR_PlayerController::ATGOR_PlayerController()
	: Super()
{
	_initial = true;

	PrimaryActorTick.bCanEverTick = true;
}



void ATGOR_PlayerController::BeginPlay()
{
	SINGLETON_CHK;
	Hud = Cast<ATGOR_HUD>(GetHUD());

	Super::BeginPlay();
}

////////////////////////////////////////////////////////////////////////////////////////////////////

ATGOR_HUD* ATGOR_PlayerController::GetCachedHud() const
{
	return Hud;
}


////////////////////////////////////////////////////////////////////////////////////////////////////

void ATGOR_PlayerController::ForceGarbageCollection()
{
	GEngine->ForceGarbageCollection(true);
}


FVector ATGOR_PlayerController::HitTraceFrom(FVector2D ScreenPosition, ETraceTypeQuery Query)
{
	// Get controlled pawn
	APawn* p = GetPawn();
	TArray<AActor*> Ignore;
	Ignore.Add(p);

	FHitResult Hit;
	FVector Location;
	FVector Direction;
	// Get Location and screen direction for trace
	DeprojectScreenPositionToWorld(ScreenPosition.X, ScreenPosition.Y, Location, Direction);
	FVector End = Location + Direction * HitResultTraceDistance;
	// Trace from camera
	bool Impact = UKismetSystemLibrary::LineTraceSingle(p, Location, End, Query, false, Ignore, EDrawDebugTrace::Type::None, Hit, true);
	// Return impact location or trace end if there is no impact
	return(Impact ? Hit.Location : Hit.TraceEnd);
}

FVector2D ATGOR_PlayerController::GetAnchorPosition(UWidget* Widget)
{
	// Get Canvas slot to change position
	UCanvasPanelSlot* Slot = Cast<UCanvasPanelSlot>(Widget->Slot);
	if (Slot == nullptr) return(FVector2D());
	// Get viewport size
	int32 SizeX, SizeY;
	GetViewportSize(SizeX, SizeY);
	// Get anchor ratios
	FVector2D Anchors = Slot->GetAnchors().Minimum;
	return(FVector2D(Anchors.X * SizeX, Anchors.Y * SizeY));
}

float ATGOR_PlayerController::LerpWidgetTo(UWidget* Widget, FVector Location, float Range, float Lerp)
{
	FVector2D ScreenPosition;
	// Get Canvas slot to change position
	UCanvasPanelSlot* Slot = Cast<UCanvasPanelSlot>(Widget->Slot);
	// Get screen location of world location
	bool Impact = ProjectWorldLocationToScreen(Location, ScreenPosition);
	// Get DPI and adapt return
	float Scale = UWidgetLayoutLibrary::GetViewportScale(GetPawn());
	if (Slot == nullptr || !Impact) return(0.0f);
	FVector2D Target = ScreenPosition / Scale;
	FVector2D Position = Slot->GetPosition();
	// Lerp widget position
	Slot->SetPosition(Position + (Target - Position) * Lerp);
	float Distance = (Position - Target).SizeSquared();
	float Ratio = Distance / (Range * Range);

	return(FMath::Clamp(1.0f - Ratio, 0.0f, 1.0f));
}

