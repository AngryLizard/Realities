// The Gateway of Realities: Planes of Existence.

#pragma once

#include "CoreMinimal.h"
#include "Realities/Utility/Error/TGOR_Error.h"
#include "Realities/Utility/TGOR_CustomEnumerations.h"

#include "Realities/UI/TGOR_Widget.h"
#include "TGOR_BodypartListWidget.generated.h"

class UTGOR_ContentQuery;

class UTGOR_BodypartSelectionWidget;
class UTGOR_BodypartCustomisationWidget;

/**
 * 
 */
UCLASS()
class REALITIES_API UTGOR_BodypartListWidget : public UTGOR_Widget
{
	GENERATED_BODY()
	
public:
	UTGOR_BodypartListWidget(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());


	////////////////////////////////////////////////////////////////////////////////////////////////////
protected:

	UPROPERTY(BlueprintReadOnly, Category = "TGOR Bodypart")
		TArray<UTGOR_BodypartSelectionWidget*> SelectionWidgets;

	UPROPERTY(BlueprintReadOnly, Category = "TGOR Bodypart")
		TArray<UTGOR_BodypartCustomisationWidget*> MaterialWidgets;
	

	UPROPERTY(BlueprintReadOnly, Category = "TGOR Bodypart")
		UTGOR_BodypartNode* BodypartNode;

	/** Whether the content list selection is currently opened or closed. */
	UPROPERTY(BlueprintReadOnly, Category = "TGOR Bodypart")
		bool ContentListOpened;


	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Set up  widget from a bodypart node */
	UFUNCTION(BlueprintCallable, Category = "TGOR Bodypart", Meta = (Keywords = "C++"))
		void SetupFromBodypart(UTGOR_BodypartNode* Node);
	
	/** Go to parent node, returns false if root */
	UFUNCTION(BlueprintCallable, Category = "TGOR Bodypart", Meta = (Keywords = "C++"))
		bool GoToParent();

	////////////////////////////////////////////////////////////////////////////////////////////////////
	
	/** Clear material widgets */
	UFUNCTION(BlueprintImplementableEvent, Category = "TGOR Bodypart", Meta = (Keywords = "C++"))
		void ClearCustomisationBodypartWidgets();

	/** Creates widget for bodypart selection */
	UFUNCTION(BlueprintImplementableEvent, Category = "TGOR Bodypart", Meta = (Keywords = "C++"))
		UTGOR_BodypartSelectionWidget* CreateBodypartSelectionWidget();

	/** Adds customisation widget */
	UFUNCTION(BlueprintImplementableEvent, Category = "TGOR Bodypart", Meta = (Keywords = "C++"))
		void AddBodypartWidget(UTGOR_BodypartWidget* Widget);


	/** Sets interface according to current position in the tree */
	UFUNCTION(BlueprintImplementableEvent, Category = "TGOR Bodypart", Meta = (Keywords = "C++"))
		void SetInterface(bool IsRoot);

	////////////////////////////////////////////////////////////////////////////////////////////////////
	
	/** Opens menu to select Content */
	UFUNCTION(BlueprintCallable, Category = "TGOR Bodypart", Meta = (Keywords = "C++"))
		void OpenContentList(UTGOR_ContentQuery* Query);

	/** Closes select content menu */
	UFUNCTION(BlueprintCallable, Category = "TGOR Bodypart", Meta = (Keywords = "C++"))
		void CloseContentList();

	/** Triggers when content selection list needs to be opened, functionality has to be implemented in UMG */
	UFUNCTION(BlueprintImplementableEvent, Category = "TGOR Bodypart", Meta = (Keywords = "C++"))
		void OnOpenContentList(UTGOR_ContentQuery* Query);

	/** Triggers when content selection list needs to be closed, functionality has to be implemented in UMG */
	UFUNCTION(BlueprintImplementableEvent, Category = "TGOR Bodypart", Meta = (Keywords = "C++"))
		void OnCloseContentList();

	////////////////////////////////////////////////////////////////////////////////////////////////////

	/** Called when a bodypart has been updated */
	UFUNCTION(BlueprintImplementableEvent, Category = "TGOR Bodypart", Meta = (Keywords = "C++"))
		void BodypartUpdate(UTGOR_BodypartNode* Node);

	/** Called when a bodypart has been opened */
	UFUNCTION(BlueprintImplementableEvent, Category = "TGOR Bodypart", Meta = (Keywords = "C++"))
		void BodypartOpen(UTGOR_BodypartNode* Node);
};
