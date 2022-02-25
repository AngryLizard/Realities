// The Gateway of Realities: Planes of Existence. By Salireths.

#pragma once

#define TWOPI (6.28318530718f)
#define PARTPI (0.17453292519f)

#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Realities/Utility/TGOR_Math.h"
#include "Realities/Utility/TGOR_CustomEnumerations.h"
#include "Realities/Base/Content/TGOR_Content.h"

#include "Blueprint/WidgetLayoutLibrary.h"
#include "Blueprint/SlateBlueprintLibrary.h"

#include "Realities/Interfaces/TGOR_SingletonInterface.h"
#include "Blueprint/UserWidget.h"
#include "TGOR_Widget.generated.h"

/**
* TGOR_Widget for general widget functionality (click/hover events, movement)
*/
UCLASS()
class REALITIES_API UTGOR_Widget : public UUserWidget, public ITGOR_SingletonInterface
{
	GENERATED_BODY()
	
public:
	UTGOR_Widget(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	////////////////////////////////////////////////////////////////////////////////////////////////////
protected:

};
