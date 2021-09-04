// The Gateway of Realities: Planes of Existence.

#pragma once

#include "CoreMinimal.h"
#include "../Components/TGOR_StatComponent.h"

#include "UISystem/Queries/TGOR_ContentQuery.h"
#include "TGOR_ConversionQuery.generated.h"

////////////////////////////////////////////// DECL //////////////////////////////////////////////////////

class UTGOR_Energy;
class UTGOR_Stat;

////////////////////////////////////////////// STRUCT //////////////////////////////////////////////////////


USTRUCT(BlueprintType)
struct FTGOR_ConversionOutput
{
	GENERATED_USTRUCT_BODY()

	/** Energy content. */
	UPROPERTY(BlueprintReadWrite, Category = "!TGOR Stats")
		UTGOR_Energy* Energy;

	/** Max quantity consumed by this response per cycle */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "!TGOR Commands")
		int32 Quantity;

	/** Min float value */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "!TGOR Commands", meta = (EditCondition = "!IsInteger"))
		float Rate;
};

/**
 * 
 */
UCLASS()
class SIMULATIONSYSTEM_API UTGOR_ConversionQuery : public UTGOR_ContentQuery
{
	GENERATED_BODY()
	
public:
	UTGOR_ConversionQuery();
	TArray<UTGOR_CoreContent*> QueryContent() const override;

	////////////////////////////////////////////////////////////////////////////////////////////////////

	/** Assigns stat and  */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Stats", Meta = (Keywords = "C++"))
		virtual void AssignComponent(UTGOR_Stat* Stat);

	UPROPERTY(BlueprintReadOnly, Category = "!TGOR Stats")
		TArray<FTGOR_ConversionOutput> Conversions;
};
