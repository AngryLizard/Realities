// The Gateway of Realities: Planes of Existence.

#pragma once

#include "../TGOR_BodypartInstance.h"

#include "CoreSystem/Content/TGOR_CoreContent.h"
#include "TGOR_Section.generated.h"


/**
 * 
 */

UCLASS(Blueprintable)
class CUSTOMISATIONSYSTEM_API UTGOR_Section : public UTGOR_CoreContent
{
	GENERATED_BODY()
	
public:
	UTGOR_Section();

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Material to be used for this section (takes first available mesh material otherwise) */
	UPROPERTY(EditAnywhere, Category = "!TGOR Canvas")
		UMaterialInterface* Material;

	/** Whether we transform UVs on merge */
	UPROPERTY(EditAnywhere, Category = "!TGOR Canvas")
		bool TransformUV;

	/** Available rendertargets */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "!TGOR Insertion")
		TSet<TSubclassOf<UTGOR_Canvas>> CanvasInsertions;
	DECL_INSERTION(CanvasInsertions);


	virtual void MoveInsertion(UTGOR_Content* Insertion, ETGOR_InsertionActionEnumeration Action, bool& Success) override;
};
