// The Gateway of Realities: Planes of Existence.

#pragma once

#include "CoreMinimal.h"

#include "UISystem/Queries/TGOR_ContentQuery.h"
#include "TGOR_BodypartContentQuery.generated.h"

class UTGOR_Skin;
class UTGOR_Palette;
class UTGOR_BodypartCustomisationWidget;

/**
 * 
 */
UCLASS()
class CUSTOMISATIONSYSTEM_API UTGOR_BodypartContentQuery : public UTGOR_ContentQuery
{
	GENERATED_BODY()
	
public:
	UTGOR_BodypartContentQuery();

	TArray<UTGOR_CoreContent*> QueryContent() const override;
	virtual void CommitNone() override;
	virtual void CommitContent(int32 Index) override;


	////////////////////////////////////////////////////////////////////////////////////////////////////
protected:

	UPROPERTY(BlueprintReadOnly, Category = "!TGOR Customisation")
		UTGOR_BodypartCustomisationWidget* CallbackWidget;

	UPROPERTY(BlueprintReadOnly, Category = "!TGOR Customisation")
		TArray<UTGOR_CoreContent*> PossibleContent;

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Sets the initial selection to the given skin */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Customisation", Meta = (Keywords = "C++"))
		void InitialiseContentSelection(UTGOR_CoreContent* Content);

	/** Sets display to possible skins */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Customisation", Meta = (Keywords = "C++"))
		void SetPossibleSkins(APlayerController* OwningPlayer, UTGOR_BodypartCustomisationWidget* CustomisationWidget, UTGOR_Bodypart* Bodypart, TSubclassOf<UTGOR_Skin> Typefilter);

	/** Sets display to possible masks */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Customisation", Meta = (Keywords = "C++"))
		void SetPossibleMasks(APlayerController* OwningPlayer, UTGOR_BodypartCustomisationWidget* CustomisationWidget, UTGOR_Bodypart* Bodypart, TSubclassOf<UTGOR_Mask> Typefilter);

	/** Sets display to possible colors */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Customisation", Meta = (Keywords = "C++"))
		void SetPossiblePalettes(APlayerController* OwningPlayer, UTGOR_BodypartCustomisationWidget* CustomisationWidget, UTGOR_Bodypart* Bodypart, TSubclassOf<UTGOR_Palette> Typefilter);
};
