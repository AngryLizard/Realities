// The Gateway of Realities: Planes of Existence.

#pragma once

#include "CoreMinimal.h"
#include "Realities/UI/Queries/TGOR_ContentQuery.h"
#include "TGOR_BodypartSkinQuery.generated.h"

class UTGOR_Skin;
class UTGOR_BodypartCustomisationWidget;

/**
 * 
 */
UCLASS()
class REALITIES_API UTGOR_BodypartSkinQuery : public UTGOR_ContentQuery
{
	GENERATED_BODY()
	
public:
	UTGOR_BodypartSkinQuery();

	TArray<UTGOR_Content*> QueryContent() const override;
	virtual void CallContent(int32 Index) override;
	virtual void Commit() override;


	////////////////////////////////////////////////////////////////////////////////////////////////////
protected:

	UPROPERTY(BlueprintReadOnly, Category = "TGOR Skin")
		UTGOR_BodypartCustomisationWidget* CallbackWidget;

	UPROPERTY(BlueprintReadOnly, Category = "TGOR Skin")
		TArray<UTGOR_Skin*> PossibleSkins;

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Sets the initial selection to the given skin */
	UFUNCTION(BlueprintCallable, Category = "TGOR Skin", Meta = (Keywords = "C++"))
		void InitialiseSkinSelection(UTGOR_Skin* Skin);

	/** Sets display to possible skins */
	UFUNCTION(BlueprintCallable, Category = "TGOR Skin", Meta = (Keywords = "C++"))
		void SetPossibleSkins(UTGOR_BodypartCustomisationWidget* CustomisationWidget, UTGOR_Bodypart* Bodypart);
};
