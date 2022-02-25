// The Gateway of Realities: Planes of Existence.

#pragma once

#include "CoreMinimal.h"
#include "Realities/UI/Queries/TGOR_ContentQuery.h"
#include "TGOR_BodypartGradientQuery.generated.h"

class UTGOR_Colour;
class UTGOR_BodypartCustomisationWidget;

/**
 * 
 */
UCLASS()
class REALITIES_API UTGOR_BodypartGradientQuery : public UTGOR_ContentQuery
{
	GENERATED_BODY()
	
public:
	UTGOR_BodypartGradientQuery();

	TArray<UTGOR_Content*> QueryContent() const override;
	virtual void CallContent(int32 Index) override;
	virtual void CallNone() override;
	virtual void Commit() override;

	////////////////////////////////////////////////////////////////////////////////////////////////////
protected:

	UPROPERTY(BlueprintReadOnly, Category = "TGOR Customisation")
		UTGOR_BodypartCustomisationWidget* CallbackWidget;

	UPROPERTY(BlueprintReadOnly, Category = "TGOR Customisation")
		TArray<UTGOR_Colour*> PossibleColours;

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Sets the initial selection to the given colour */
	UFUNCTION(BlueprintCallable, Category = "TGOR Customisation", Meta = (Keywords = "C++"))
		void InitialiseColourSelection(UTGOR_Colour* Colour);

	/** Sets display to possible colours */
	UFUNCTION(BlueprintCallable, Category = "TGOR Customisation", Meta = (WorldContext = "WorldContextObject", Keywords = "C++"))
		void SetPossibleColours(UTGOR_BodypartCustomisationWidget* CustomisationWidget);
};
