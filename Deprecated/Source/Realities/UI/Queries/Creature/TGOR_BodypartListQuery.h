// The Gateway of Realities: Planes of Existence.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Realities/Base/Content/TGOR_Content.h"

#include "Realities/UI/Queries/TGOR_ContentQuery.h"
#include "TGOR_BodypartListQuery.generated.h"

class UTGOR_Bodypart;
class UTGOR_BodypartListWidget;

/**
 * 
 */
UCLASS()
class REALITIES_API UTGOR_BodypartListQuery : public UTGOR_ContentQuery
{
	GENERATED_BODY()
	
public:
	UTGOR_BodypartListQuery();

	TArray<UTGOR_Content*> QueryContent() const override;
	virtual void CallContent(int32 Index) override;
	virtual void CallNone() override;
	virtual void Commit() override;

	////////////////////////////////////////////////////////////////////////////////////////////////////
protected:

	UPROPERTY(BlueprintReadOnly, Category = "TGOR Bodypart")
		UTGOR_BodypartSelectionWidget* CallbackWidget;

	UPROPERTY(BlueprintReadOnly, Category = "TGOR Bodypart")
		TArray<UTGOR_Bodypart*> PossibleBodyparts;

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Sets the initial selection to the given bodypart */
	UFUNCTION(BlueprintCallable, Category = "TGOR Bodypart", Meta = (Keywords = "C++"))
		void InitialiseBodypartSelection(UTGOR_Bodypart* Bodypart);

	/** Sets display to possible bodyparts */
	UFUNCTION(BlueprintCallable, Category = "TGOR Bodypart", Meta = (Keywords = "C++"))
		void SetPossibleBodyparts(UTGOR_BodypartSelectionWidget* BodypartSelectionWidget, UTGOR_Bodypart* Bodypart, TSubclassOf<UTGOR_Bodypart> Class, bool IsRequired);
};
