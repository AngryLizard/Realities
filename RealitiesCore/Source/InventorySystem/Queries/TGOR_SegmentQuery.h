// The Gateway of Realities: Planes of Existence.

#pragma once

#include "CoreMinimal.h"

#include "UISystem/Queries/TGOR_ContentQuery.h"
#include "TGOR_SegmentQuery.generated.h"

class UTGOR_MatterComponent;
class UTGOR_Segment;

/**
 * UTGOR_SegmentQuery
 */
UCLASS()
class INVENTORYSYSTEM_API UTGOR_SegmentQuery : public UTGOR_ContentQuery
{
	GENERATED_BODY()
	
public:
	UTGOR_SegmentQuery();
	virtual TArray<UTGOR_CoreContent*> QueryContent() const override;

	/** Constructs Segments of a given storage container */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Inventory", Meta = (Keywords = "C++"))
		virtual void AssignComponent(UTGOR_MatterComponent* MatterComponent, TSubclassOf<UTGOR_Segment> Segment);

	/** Segments */
	UPROPERTY(BlueprintReadOnly, Category = "!TGOR Inventory")
		TArray<UTGOR_Segment*> Segments;

	/** Owning MatterComponent */
	UPROPERTY(BlueprintReadOnly, Category = "!TGOR Inventory")
		UTGOR_MatterComponent* HostComponent;
};
