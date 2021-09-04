// The Gateway of Realities: Planes of Existence.

#pragma once

#include "CoreMinimal.h"

#include "CoreSystem/Content/TGOR_CoreContent.h"
#include "TGOR_Response.generated.h"

class UTGOR_Energy;
class UTGOR_Segment;

/**
*
*/
USTRUCT(BlueprintType)
struct SIMULATIONSYSTEM_API FTGOR_Conversion
{
	GENERATED_USTRUCT_BODY()
		FTGOR_Conversion();

	////////////////////////////////////////////////////////////////////////////////////////////////////

	/** Quantity to convert */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "!TGOR Stats")
		int32 Quantity;

	/** Rate from quantity to stat */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "!TGOR Stats")
		float Rate;
};

/**
 * 
 */
UCLASS(Blueprintable)
class SIMULATIONSYSTEM_API UTGOR_Response : public UTGOR_CoreContent
{
	GENERATED_BODY()
	
public:
	UTGOR_Response();
	virtual void PostBuildResource() override;

	////////////////////////////////////////////////////////////////////////////////////////////////////
protected:

	/** Defined attributes to modify */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TMap<TSubclassOf<UTGOR_Energy>, FTGOR_Conversion> Conversions;

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Conversion factors */
	UPROPERTY()
		TMap<UTGOR_Energy*, FTGOR_Conversion> ConversionValues;

	/** Compute response conversion */
	UFUNCTION(BlueprintNativeEvent, Category = "!TGOR Action", Meta = (Keywords = "C++"))
		int32 Convert(UTGOR_StatComponent* Stats, UTGOR_Segment* Segment, UTGOR_Energy* Energy, int32 Quantity, float Value, float& Delta) const;
	virtual int32 Convert_Implementation(UTGOR_StatComponent* Stats, UTGOR_Segment* Segment, UTGOR_Energy* Energy, int32 Quantity, float Value, float& Delta) const;

};
