// The Gateway of Realities: Planes of Existence.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"

#include "Realities/Utility/Error/TGOR_Error.h"
#include "Realities/Utility/TGOR_CustomEnumerations.h"

#include "TGOR_BodypartWidget.h"
#include "TGOR_BodypartSelectionWidget.generated.h"

class UTGOR_Bodypart;
class UTGOR_BodypartListQuery;

/**
 * 
 */
UCLASS()
class REALITIES_API UTGOR_BodypartSelectionWidget : public UTGOR_BodypartWidget
{
	GENERATED_BODY()
	
public:
	UTGOR_BodypartSelectionWidget(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());


	////////////////////////////////////////////////////////////////////////////////////////////////////
protected:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Instanced)
		UTGOR_BodypartListQuery* BodypartListQuery;

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Entry type */
	UPROPERTY(BlueprintReadOnly, Category = "TGOR Bodypart")
		TSubclassOf<UTGOR_Bodypart> EntryType;

	UPROPERTY(BlueprintReadOnly, Category = "TGOR Bodypart")
		UTGOR_Bodypart* BodypartType;


	/** Set up bodypart query */
	UFUNCTION(BlueprintCallable, Category = "TGOR Bodypart", Meta = (Keywords = "C++"))
		void SetQuery(bool Required);
	
	/** Set up data for this widget */
	UFUNCTION(BlueprintImplementableEvent, Category = "TGOR Bodypart", Meta = (Keywords = "C++"))
		void SetData(bool Required, bool Active);

	/** Get currently selected child node */
	UFUNCTION(BlueprintPure, Category = "TGOR Bodypart", Meta = (Keywords = "C++"))
		UTGOR_BodypartNode* GetChildNode();

	/** Remove bodypart from node */
	UFUNCTION(BlueprintCallable, Category = "TGOR Bodypart", Meta = (Keywords = "C++"))
		void RemoveBodypart();

	/** Opens bodypart in list */
	UFUNCTION(BlueprintCallable, Category = "TGOR Bodypart", Meta = (Keywords = "C++"))
		void OpenBodypart();

	/** Select bodypart from list */
	UFUNCTION(BlueprintCallable, Category = "TGOR Bodypart", Meta = (Keywords = "C++"))
		void SelectBodypart();

	/** Replace bodypart from node */
	UFUNCTION(BlueprintCallable, Category = "TGOR Bodypart", Meta = (Keywords = "C++"))
		void ReplaceBodypart(UTGOR_Bodypart* Content);

};
