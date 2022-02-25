// The Gateway of Realities: Planes of Existence. By Salireths.

#pragma once

#include "Realities/Base/Content/TGOR_Content.h"
#include "TGOR_Category.generated.h"

/**
 * TGOR_Category describes categories content classes belong to
 */
UCLASS(Blueprintable)
class REALITIES_API UTGOR_Category : public UTGOR_Content
{
	GENERATED_BODY()
		
public:
	UTGOR_Category();
	virtual void PostBuildResource() override;

	/** Finds all content classes that are part of this category */
	UFUNCTION(BlueprintCallable, Category = "TGOR Category", Meta = (Keywords = "C++"))
		void BuildCategoryInsertions(UTGOR_Content* Category);

	////////////////////////////////////////////////////////////////////////////////////////////////////

};
