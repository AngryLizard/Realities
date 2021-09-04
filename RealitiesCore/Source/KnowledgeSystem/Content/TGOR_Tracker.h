// The Gateway of Realities: Planes of Existence.

#pragma once

#include "CoreMinimal.h"
#include "CoreSystem/Content/TGOR_CoreContent.h"
#include "TGOR_Tracker.generated.h"


/**
 * 
 */
UCLASS()
class KNOWLEDGESYSTEM_API UTGOR_Tracker : public UTGOR_CoreContent
{
	GENERATED_BODY()
	
public:
	UTGOR_Tracker();

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Get keyword for content */
	UFUNCTION(BlueprintPure, Category = "!TGOR Tracker", Meta = (Keywords = "C++"))
		FText GetKeyword(UTGOR_Content* Content) const;

	/** Keywords used for content */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "!TGOR Tracker", meta = (Keywords = "C++"))
		TMap<TSubclassOf<UTGOR_Content>, FText> Keywords;

	/** Default keyword used forcontent without keywords */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "!TGOR Tracker", meta = (Keywords = "C++"))
		FText DefaultKeyword;

	/** Tracker unit */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "!TGOR Tracker", meta = (Keywords = "C++"))
		FText Unit;

	////////////////////////////////////////////////////////////////////////////////////////////////////
protected:



};
