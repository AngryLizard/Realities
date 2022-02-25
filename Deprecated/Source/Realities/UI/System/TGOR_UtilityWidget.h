// The Gateway of Realities: Planes of Existence.

#pragma once

#include "Realities/Base/Content/TGOR_Content.h"

#include "Realities/UI/TGOR_Widget.h"
#include "TGOR_UtilityWidget.generated.h"


/**
 * UTGOR_UtilityWidget provides functionality to display content options in a utility widget
 */
UCLASS()
class REALITIES_API UTGOR_UtilityWidget : public UTGOR_Widget
{
	GENERATED_BODY()
	
public:

	/** Assigns content class to this widget */
	UFUNCTION(BlueprintCallable, Category = "TGOR Content", Meta = (Keywords = "C++"))
		void AssignContentClass(TSubclassOf<UTGOR_Content> ContentClass);

	/** Get currently loaded default object (This is a default object, do not store), also marks as dirty */
	UFUNCTION(BlueprintCallable, Category = "TGOR Content", Meta = (Keywords = "C++"))
		UTGOR_Content* ModifyCurrentContent();

protected:

	/** Assign content to this widget (This is a default object, do not store. Use "GetCurrentContent" instead) */
	UFUNCTION(BlueprintImplementableEvent)
		void AssignContent(UTGOR_Content* Content);


	/* Currently loaded content */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TGOR Content", Meta = (Keywords = "C++"))
		TSubclassOf<UTGOR_Content> CurrentContent;
};
