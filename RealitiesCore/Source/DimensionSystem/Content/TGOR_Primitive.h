// The Gateway of Realities: Planes of Existence.

#pragma once

#include "CoreMinimal.h"
#include "DimensionSystem/Content/TGOR_SpawnModule.h"
#include "TGOR_Primitive.generated.h"

class UTGOR_Pilot;

/**
 * 
 */
UCLASS(Blueprintable)
class DIMENSIONSYSTEM_API UTGOR_Primitive : public UTGOR_SpawnModule
{
	GENERATED_BODY()

public:
	UTGOR_Primitive();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "!TGOR Creature")
		float Weight = 120.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "!TGOR Creature")
		FVector SurfaceArea = FVector(0.5f, 0.5f, 0.5f);

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Pilots in this spawner */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "!TGOR Insertion")
		TArray<TSubclassOf<UTGOR_Pilot>> PilotInsertions;
	DECL_INSERTION(PilotInsertions);


	virtual void MoveInsertion(UTGOR_Content* Insertion, ETGOR_InsertionActionEnumeration Action, bool& Success) override;
};