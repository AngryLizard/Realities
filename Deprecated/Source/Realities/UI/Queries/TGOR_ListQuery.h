// The Gateway of Realities: Planes of Existence.

#pragma once

#include "CoreMinimal.h"
#include "Realities/UI/Queries/TGOR_Query.h"
#include "TGOR_ListQuery.generated.h"

/**
 * 
 */
UCLASS()
class REALITIES_API UTGOR_ListQuery : public UTGOR_Query
{
	GENERATED_BODY()
	
public:
	UTGOR_ListQuery();

	////////////////////////////////////////////////////////////////////////////////////////////////////

	UFUNCTION(BlueprintCallable, Category = "TGOR Interface", Meta = (Keywords = "C++"))
		virtual TArray<FTGOR_Display> QueryDisplays() const;
	
	UFUNCTION(BlueprintImplementableEvent, Category = "TGOR Interface", Meta = (Keywords = "C++"))
		TArray<FTGOR_Display> OnQueryDisplays() const;

	/** Call index */
	UFUNCTION(BlueprintCallable, Category = "TGOR Interface", Meta = (Keywords = "C++"))
		virtual void CallIndex(int32 Index);

	/** Call index override */
	UFUNCTION(BlueprintImplementableEvent, Category = "TGOR Interface", Meta = (Keywords = "C++"))
		void OnCallIndex(int32 Index);
};
