// The Gateway of Realities: Planes of Existence. By Salireths.

#pragma once

#include "Templates/SubclassOf.h"

#include "DimensionSystem/Content/TGOR_SpawnModule.h"
#include "TGOR_Resource.generated.h"

class UTGOR_Segment;

/**
* All objects that are made out of and hold matter
*/
UCLASS(Blueprintable, BlueprintType)
class INVENTORYSYSTEM_API UTGOR_Resource : public UTGOR_SpawnModule
{
	GENERATED_BODY()

public:
	UTGOR_Resource();

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Segments in this spawner that hold matter */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "!TGOR Insertion")
		TArray<TSubclassOf<UTGOR_Segment>> SegmentInsertions;
	DECL_INSERTION(SegmentInsertions);

	virtual void MoveInsertion(UTGOR_Content* Insertion, ETGOR_InsertionActionEnumeration Action, bool& Success) override;
};
