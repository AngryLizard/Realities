// The Gateway of Realities: Planes of Existence. By Salireths.

#pragma once

#include "TGOR_CoreContent.h"
#include "TGOR_Category.generated.h"

/**
 * TGOR_Category describes categories content classes belong to
 */
UCLASS(Blueprintable)
class CORESYSTEM_API UTGOR_Category : public UTGOR_CoreContent
{
	GENERATED_BODY()
		
public:
	UTGOR_Category();
	virtual void PostBuildResource() override;

	/** Finds all content classes that are part of this category */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Category", Meta = (Keywords = "C++"))
		void BuildCategoryInsertions(UTGOR_CoreContent* Category);

	////////////////////////////////////////////////////////////////////////////////////////////////////

};
