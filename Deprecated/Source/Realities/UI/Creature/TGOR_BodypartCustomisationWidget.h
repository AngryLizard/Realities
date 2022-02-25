// The Gateway of Realities: Planes of Existence.

#pragma once

#include "CoreMinimal.h"
#include "Realities/Base/Instances/Creature/TGOR_BodypartInstance.h"

#include "TGOR_BodypartWidget.h"
#include "TGOR_BodypartCustomisationWidget.generated.h"

class UTGOR_BodypartMaterialQuery;

/**
 * 
 */
UCLASS()
class REALITIES_API UTGOR_BodypartCustomisationWidget : public UTGOR_BodypartWidget
{
	GENERATED_BODY()
	
public:
	UTGOR_BodypartCustomisationWidget(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());	

	////////////////////////////////////////////////////////////////////////////////////////////////////

	/** Customisation content class */
	UPROPERTY(BlueprintReadOnly, Category = "TGOR Bodypart")
		UTGOR_Customisation* Customisation;

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Set up data for this widget */
	UFUNCTION(BlueprintImplementableEvent, Category = "TGOR Bodypart", Meta = (Keywords = "C++"))
		void SetData(const TArray<uint8>& Payload, bool HasInstance);
	
	/** Commit data to bodypart */
	UFUNCTION(BlueprintCallable, Category = "TGOR Bodypart", Meta = (Keywords = "C++"))
		void Commit(const TArray<uint8>& Payload);

	/** Query data from bodypart */
	UFUNCTION(BlueprintPure, Category = "TGOR Bodypart", Meta = (Keywords = "C++"))
		void Query(TArray<uint8>& Payload) const;

	/** Removes customisation from appearance */
	UFUNCTION(BlueprintCallable, Category = "TGOR Bodypart", Meta = (Keywords = "C++"))
		void Reset();

	/** Callback function for selection interfaces */
	UFUNCTION(BlueprintImplementableEvent, Category = "TGOR Bodypart", Meta = (Keywords = "C++"))
		void SelectCallback(int32 Index);

	/** Callback function for selection interfaces when none is selected */
	UFUNCTION(BlueprintImplementableEvent, Category = "TGOR Bodypart", Meta = (Keywords = "C++"))
		void NoneCallback();
};
