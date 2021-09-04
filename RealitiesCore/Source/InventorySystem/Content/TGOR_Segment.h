// The Gateway of Realities: Planes of Existence.

#pragma once

#include "CoreMinimal.h"

#include "CoreSystem/Content/TGOR_CoreContent.h"
#include "TGOR_Segment.generated.h"

class UTGOR_Matter;

/**
 * 
 */
UCLASS()
class INVENTORYSYSTEM_API UTGOR_Segment : public UTGOR_CoreContent
{
	GENERATED_BODY()
	
public:
	UTGOR_Segment();
	virtual void PostBuildResource() override;

	////////////////////////////////////////////////////////////////////////////////////////////////////
protected:

	/** Matter quantity per matter type this TGOR is made out of. */
	UPROPERTY(EditAnywhere, Category = "!TGOR Segment")
		TMap<TSubclassOf<UTGOR_Matter>, int32> MatterComposition;

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Matter composition capacity of this Segment */
	UPROPERTY(BlueprintReadOnly, Category = "!TGOR Segment")
		TMap<UTGOR_Matter*, int32> MatterCapacity;

	/** Serialisation rank of each matter */
	UPROPERTY(BlueprintReadOnly, Category = "!TGOR Segment")
		TMap<UTGOR_Matter*, int32> MatterRanks;
};
