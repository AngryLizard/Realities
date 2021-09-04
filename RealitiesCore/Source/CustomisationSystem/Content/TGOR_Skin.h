// The Gateway of Realities: Planes of Existence.

#pragma once

#include "CoreMinimal.h"
#include "../TGOR_BodypartInstance.h"

#include "CoreSystem/Content/TGOR_CoreContent.h"
#include "TGOR_Skin.generated.h"

/**
 * 
 */
UCLASS(Blueprintable)
class CUSTOMISATIONSYSTEM_API UTGOR_Skin : public UTGOR_CoreContent
{
	GENERATED_BODY()

public:
	UTGOR_Skin();

	/** Normal texture */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "!TGOR Skin")
		UTexture* Normal;

	/** Surface texture */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "!TGOR Skin")
		UTexture* Surface;

	////////////////////////////////////////////////////////////////////////////////////////////////////

	/** Get skin for given bodypart if available */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Skin", Meta = (Keywords = "C++"))
		UTGOR_Skin* GetBodypartSkin(UTGOR_Bodypart* Bodypart);

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Skins that are connected to this skin for different bodyparts (only child bodypart skins required) */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "!TGOR Insertion")
		TSet<TSubclassOf<UTGOR_Skin>> CollectionInsertions;
	DECL_INSERTION(CollectionInsertions);

	virtual void MoveInsertion(UTGOR_Content* Insertion, ETGOR_InsertionActionEnumeration Action, bool& Success) override;

};
