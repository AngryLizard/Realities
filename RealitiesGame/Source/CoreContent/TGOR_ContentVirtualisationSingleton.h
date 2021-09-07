// The Gateway of Realities: Planes of Existence.

#pragma once

#include "CoreSystem/TGOR_Singleton.h"
#include "TGOR_ContentVirtualisationSingleton.generated.h"


/**
* Provides common interface with TGOR_ContentVirtualisationComponent
*/
UCLASS()
class CORECONTENT_API UTGOR_ContentVirtualisationSingleton : public UTGOR_Singleton
{
	GENERATED_BODY()

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:
	/** Create and initialise content manager */
	UFUNCTION(BlueprintCallable, Category = "!TGOR System", Meta = (Keywords = "C++"))
		void CreateContentManager();


	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

private:

};
