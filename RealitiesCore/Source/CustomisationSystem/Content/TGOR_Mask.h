// The Gateway of Realities: Planes of Existence.

#pragma once

#include "CoreMinimal.h"

#include "../TGOR_BodypartInstance.h"

#include "CoreSystem/Content/TGOR_CoreContent.h"
#include "TGOR_Mask.generated.h"

/**
 * 
 */
UCLASS(Blueprintable)
class CUSTOMISATIONSYSTEM_API UTGOR_Mask : public UTGOR_CoreContent
{
	GENERATED_BODY()

public:
	UTGOR_Mask();

	/** Mask texture for separating layers */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "!TGOR Skin")
		UTexture* Texture;

	////////////////////////////////////////////////////////////////////////////////////////////////////

	/** Get mask for given bodypart if available */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Skin", Meta = (Keywords = "C++"))
		UTGOR_Mask* GetBodypartMask(UTGOR_Bodypart* Bodypart);

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Masks that are connected to this skin for different bodyparts (only child bodypart masks required) */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "!TGOR Insertion")
		TSet<TSubclassOf<UTGOR_Mask>> CollectionInsertions;
	DECL_INSERTION(CollectionInsertions);

	virtual void MoveInsertion(UTGOR_Content* Insertion, ETGOR_InsertionActionEnumeration Action, bool& Success) override;

};
