// The Gateway of Realities: Planes of Existence.

#pragma once

#include "CoreMinimal.h"

#include "TGOR_Widget.h"
#include "TGOR_QueryWidget.generated.h"

////////////////////////////////////////////// DECL //////////////////////////////////////////////////////

class UTGOR_Query;

/**
* TGOR_QueryWidget provides functionality to apply queries
*/
UCLASS()
class UISYSTEM_API UTGOR_QueryWidget : public UTGOR_Widget
{
	GENERATED_BODY()
	
public:

	UTGOR_QueryWidget(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	//////////////////////////////////////////// IMPLEMENTABLES ////////////////////////////////////////

	/** Adapt this widget to a given query */
	UFUNCTION(BlueprintImplementableEvent, Category = "!TGOR Interface", Meta = (Keywords = "C++"))
		void OnApplyQuery(UTGOR_Query* Query);

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Adapt this widget to a given query */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Interface", Meta = (Keywords = "C++"))
		virtual void ApplyQuery(UTGOR_Query* Query);

	/////////////////////////////////////////////// INTERNAL ///////////////////////////////////////////

};
