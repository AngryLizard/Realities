// The Gateway of Realities: Planes of Existence.

#pragma once

#include "UISystem/UI/TGOR_Widget.h"
#include "TGOR_ContentVirtualisationWidget.generated.h"

class UTGOR_ContentVirtualisationComponent;

/**
* Provides common interface with TGOR_ContentVirtualisationComponent
*/
UCLASS()
class GAMESYSTEM_API UTGOR_ContentVirtualisationWidget : public UTGOR_Widget
{
	GENERATED_BODY()

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Initialise this widget for the given virtualisation component. */
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "!TGOR System", Meta = (Keywords = "C++"))
		void OnInitialiseWith(UTGOR_ContentVirtualisationComponent* VirtualisationComponent);

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

private:

};
