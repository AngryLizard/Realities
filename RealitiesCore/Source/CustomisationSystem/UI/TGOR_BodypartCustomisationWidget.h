// The Gateway of Realities: Planes of Existence.

#pragma once

#include "CoreMinimal.h"
#include "../TGOR_BodypartInstance.h"

#include "TGOR_BodypartWidget.h"
#include "TGOR_BodypartCustomisationWidget.generated.h"

class UTGOR_BodypartMaterialQuery;

/**
 * 
 */
UCLASS()
class CUSTOMISATIONSYSTEM_API UTGOR_BodypartCustomisationWidget : public UTGOR_BodypartWidget
{
	GENERATED_BODY()
	
public:
	UTGOR_BodypartCustomisationWidget(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());	

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Set up data for this widget */
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = "!TGOR Bodypart", Meta = (Keywords = "C++"))
		void UpdateDisplay(UTGOR_Customisation* Customisation, const FTGOR_CustomisationPayload& Payload, bool HasInstance);
	
	/** Commit data to bodypart */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Bodypart", Meta = (Keywords = "C++"))
		void Commit(UTGOR_Customisation* Customisation, const FTGOR_CustomisationPayload& Payload);

	/** Query data from bodypart */
	UFUNCTION(BlueprintPure, Category = "!TGOR Bodypart", Meta = (Keywords = "C++"))
		void Query(UTGOR_Customisation* Customisation, FTGOR_CustomisationPayload& Payload) const;

	/** Removes customisation from appearance */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Bodypart", Meta = (Keywords = "C++"))
		void Reset();

	/** Callback function for transform interfaces */
	UFUNCTION(BlueprintImplementableEvent, Category = "!TGOR Bodypart", Meta = (Keywords = "C++"))
		void TransformCallback(const FTransform& Transform);

	/** Callback function for content interfaces */
	UFUNCTION(BlueprintImplementableEvent, Category = "!TGOR Bodypart", Meta = (Keywords = "C++"))
		void ContentCallback(UTGOR_Content* Content);

	/** Callback function for selection interfaces */
	UFUNCTION(BlueprintImplementableEvent, Category = "!TGOR Bodypart", Meta = (Keywords = "C++"))
		void SelectCallback(int32 Index);

	/** Callback function for selection interfaces when none is selected */
	UFUNCTION(BlueprintImplementableEvent, Category = "!TGOR Bodypart", Meta = (Keywords = "C++"))
		void NoneCallback();
};
