// The Gateway of Realities: Planes of Existence.

#pragma once

#include "DimensionSystem/Content/TGOR_SpawnModule.h"
#include "TGOR_Poser.generated.h"

class UTGOR_Archetype;

/**
 * 
 */
UCLASS()
class ANIMATIONSYSTEM_API UTGOR_Poser : public UTGOR_SpawnModule
{
	GENERATED_BODY()

public:
	UTGOR_Poser();

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Main body of this spawner */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "!TGOR Insertion")
		TArray<TSubclassOf<UTGOR_Archetype>> ArchetypeInsertions;
	DECL_INSERTION_REQUIRED(ArchetypeInsertions);

	virtual void MoveInsertion(UTGOR_Content* Insertion, ETGOR_InsertionActionEnumeration Action, bool& Success) override;

private:
};
