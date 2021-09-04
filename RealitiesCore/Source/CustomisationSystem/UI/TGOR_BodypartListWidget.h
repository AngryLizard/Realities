// The Gateway of Realities: Planes of Existence.

#pragma once

#include "CoreMinimal.h"

#include "UISystem/UI/TGOR_Widget.h"
#include "TGOR_BodypartListWidget.generated.h"

class UTGOR_ContentQuery;

class UTGOR_CustomisationComponent;
class UTGOR_BodypartSelectionWidget;
class UTGOR_BodypartCustomisationWidget;

/**
 * 
 */
UCLASS()
class CUSTOMISATIONSYSTEM_API UTGOR_BodypartListWidget : public UTGOR_Widget
{
	GENERATED_BODY()
	
public:
	UTGOR_BodypartListWidget(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());


	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	UPROPERTY(BlueprintReadOnly, Category = "!TGOR Bodypart")
		int32 BodypartNodeIndex;

	UPROPERTY(BlueprintReadOnly, Category = "!TGOR Bodypart")
		UTGOR_CustomisationComponent* OwnerComponent;

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Set up  widget from a bodypart node */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Bodypart", Meta = (Keywords = "C++"))
		void GoToNode(UTGOR_CustomisationComponent* Component, int32 NodeIndex);
	
	/** Go to parent node, returns false if root */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Bodypart", Meta = (Keywords = "C++"))
		bool GoToParent();

	////////////////////////////////////////////////////////////////////////////////////////////////////
	
	/** Sets interface according to current position in the tree */
	UFUNCTION(BlueprintImplementableEvent, Category = "!TGOR Bodypart", Meta = (Keywords = "C++"))
		void OnUpdateInterface();

	////////////////////////////////////////////////////////////////////////////////////////////////////
	
	/** Opens menu to select content */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Bodypart", Meta = (Keywords = "C++"))
		void OpenContentList(UTGOR_ContentQuery* Query);

	/** Triggers when content selection list needs to be opened, functionality has to be implemented in UMG */
	UFUNCTION(BlueprintImplementableEvent, Category = "!TGOR Bodypart", Meta = (Keywords = "C++"))
		void OnOpenContentList(UTGOR_ContentQuery* Query);

	/** Closes select content menu */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Bodypart", Meta = (Keywords = "C++"))
		void CloseContentList();

	/** Triggers when content selection list needs to be closed, functionality has to be implemented in UMG */
	UFUNCTION(BlueprintImplementableEvent, Category = "!TGOR Bodypart", Meta = (Keywords = "C++"))
		void OnCloseContentList();

	////////////////////////////////////////////////////////////////////////////////////////////////////

	/** Opens menu to select transform */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Bodypart", Meta = (Keywords = "C++"))
		void OpenTransform(UTGOR_Query* Query);

	/** Triggers when transform needs to be opened, functionality has to be implemented in UMG */
	UFUNCTION(BlueprintImplementableEvent, Category = "!TGOR Bodypart", Meta = (Keywords = "C++"))
		void OnOpenTransform(UTGOR_Query* Query);

	/** Closes select transform menu */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Bodypart", Meta = (Keywords = "C++"))
		void CloseTransform();

	/** Triggers when transform needs to be closed, functionality has to be implemented in UMG */
	UFUNCTION(BlueprintImplementableEvent, Category = "!TGOR Bodypart", Meta = (Keywords = "C++"))
		void OnCloseTransform();
};
