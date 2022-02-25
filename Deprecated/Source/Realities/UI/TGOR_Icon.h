// The Gateway of Realities: Planes of Existence.

#pragma once

#include "Realities/Base/Content/TGOR_Content.h"

#include "Realities/UI/TGOR_Widget.h"
#include "TGOR_Icon.generated.h"

class UPaperSprite;

/**
 * TGOR_Icon provides functionality to display a FTGOR_Widget
 */
UCLASS()
class REALITIES_API UTGOR_Icon : public UTGOR_Widget
{
	GENERATED_BODY()
	
public:

	/** Clear display */
	UFUNCTION(BlueprintCallable, Category = "TGOR Interface", Meta = (Keywords = "C++"))
		virtual void ClearDisplay();

	/** Reset slot with item */
	UFUNCTION(BlueprintImplementableEvent)
		void ResetVisuals();

	/** Displays given display */
	UFUNCTION(BlueprintCallable, Category = "TGOR Interface", Meta = (Keywords = "C++"))
		virtual void Display(const FTGOR_Display& Display);

	/** Visualise slot with item */
	UFUNCTION(BlueprintImplementableEvent)
		void Visualise(const FTGOR_Display& Display);

	/** Retreive sprite size */
	UFUNCTION(BlueprintCallable, Category = "TGOR Interface", Meta = (Keywords = "C++"))
		FVector2D GetSourceSize(UPaperSprite* Sprite);
};
