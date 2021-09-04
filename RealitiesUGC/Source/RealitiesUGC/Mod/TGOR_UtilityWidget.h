// The Gateway of Realities: Planes of Existence.

#pragma once

#include "RealitiesUGC/Mod/TGOR_Content.h"

#include "Blueprint/UserWidget.h"
#include "TGOR_UtilityWidget.generated.h"


/**
 * UTGOR_UtilityWidget provides functionality to display content options in a utility widget
 */
UCLASS()
class REALITIESUGC_API UTGOR_UtilityWidget : public UUserWidget
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
