// The Gateway of Realities: Planes of Existence.

#pragma once

#include "CoreMinimal.h"

#include "UISystem/Gameplay/TGOR_HUD.h"
#include "TargetSystem/Components/TGOR_InteractableComponent.h"

#include "TGOR_OnlineController.h"
#include "TGOR_PlayerController.generated.h"


/**
* TGOR_PlayerController allows a playercontroller to interface with TGOR_Characters
*/
UCLASS()
class PLAYERSYSTEM_API ATGOR_PlayerController : public ATGOR_OnlineController
{
	GENERATED_BODY()
	
public:

	////////////////////////////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////// ENGINE ////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////

	ATGOR_PlayerController();
	virtual void BeginPlay() override;

	//////////////////////////////////////////// IMPLEMENTABLES ////////////////////////////////////////

	/** */

	////////////////////////////////////////////////////////////////////////////////////////////////////
	
	/** Calls ForceGarbageCollection with purge on */
	UFUNCTION(BlueprintCallable, Category = "!TGOR System", Meta = (Keywords = "C++"))
		void ForceGarbageCollection();

	/** Trace from screen position and return location in world space */
	UFUNCTION(BlueprintCallable, Category = "!TGOR System", Meta = (Keywords = "C++"))
		FVector HitTraceFrom(FVector2D ScreenPosition, ETraceTypeQuery Query);

	/** Get Anchor position */
	UFUNCTION(BlueprintPure, Category = "!TGOR Interface", Meta = (Keywords = "C++"))
		FVector2D GetAnchorPosition(UWidget* Widget);

	/** Lerp Widget to World space */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Interface", Meta = (Keywords = "C++"))
		float LerpWidgetTo(UWidget* Widget, FVector Location, float Range, float Lerp);

	////////////////////////////////////////////////////////////////////////////////////////////////////
protected:

	/** List of currently close interactables */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "!TGOR Interaction")
		TSet<UTGOR_InteractableComponent*> Interactables;

	/** Current Hud */
	UPROPERTY(EditAnywhere, Category = "!TGOR Interface", Meta = (Keywords = "C++"))
		ATGOR_HUD* Hud;

public:

	/** Returns cached casted hud */
	UFUNCTION(BlueprintPure, Category = "!TGOR Interface", Meta = (Keywords = "C++"))
		ATGOR_HUD* GetCachedHud() const;


	////////////////////////////////////////////////////////////////////////////////////////////////////

	/////////////////////////////////////////////// INTERNAL ///////////////////////////////////////////

protected:

private:

	bool _initial;
	float _cameraZoom;
	float _cameraPitch;
};
