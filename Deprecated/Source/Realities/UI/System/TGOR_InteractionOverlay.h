// The Gateway of Realities: Planes of Existence.

#pragma once

#include "Realities/UI/TGOR_Menu.h"
#include "TGOR_InteractionOverlay.generated.h"

class UTGOR_InteractionQuery;


/**
* TGOR_Menu provides general menu functionality (layering, focus, open, close etc)
*/
UCLASS()
class REALITIES_API UTGOR_InteractionOverlay : public UTGOR_Menu
{
	GENERATED_BODY()
	
public:
	UTGOR_InteractionOverlay(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
	virtual void OnWidgetRebuilt() override;

	////////////////////////////////////////////////////////////////////////////////////////////////////
protected:

	UPROPERTY(Instanced, BlueprintReadOnly)
		UTGOR_InteractionQuery* InteractionQuery;

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

private:

};
