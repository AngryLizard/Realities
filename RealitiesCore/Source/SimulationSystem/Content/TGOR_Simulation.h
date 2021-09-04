// The Gateway of Realities: Planes of Existence.

#pragma once

#include "DimensionSystem/Content/TGOR_SpawnModule.h"
#include "TGOR_Simulation.generated.h"

class UTGOR_Stat;
class UTGOR_Effect;

/**
 * 
 */
UCLASS(Blueprintable)
class SIMULATIONSYSTEM_API UTGOR_Simulation : public UTGOR_SpawnModule
{
	GENERATED_BODY()
	
public:
	UTGOR_Simulation();

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Stats in this simulator */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "!TGOR Insertion")
		TArray<TSubclassOf<UTGOR_Stat>> StatInsertions;
	DECL_INSERTION(StatInsertions);

	/** Effects in this simulator */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "!TGOR Insertion")
		TArray<TSubclassOf<UTGOR_Effect>> EffectInsertions;
	DECL_INSERTION(EffectInsertions);

	virtual void MoveInsertion(UTGOR_Content* Insertion, ETGOR_InsertionActionEnumeration Action, bool& Success) override;
};

