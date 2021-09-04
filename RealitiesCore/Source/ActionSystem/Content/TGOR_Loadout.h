// TGOR (C) // CHECKED //
#pragma once

#include "CoreMinimal.h"
#include "Templates/SubclassOf.h"

#include "DimensionSystem/Content/TGOR_SpawnModule.h"
#include "TGOR_Loadout.generated.h"


class UTGOR_Action;
class UTGOR_Movement;

////////////////////////////////////////////////////////////////////////////////////////////////////

USTRUCT(BlueprintType)
struct FTGOR_ActionSlotSpot
{
	GENERATED_USTRUCT_BODY()
		FTGOR_ActionSlotSpot();

	/** Action type */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "!TGOR Action")
		TSubclassOf<UTGOR_Action> Type;

	/** Default action type if empty (None for optional) */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "!TGOR Action")
		TSubclassOf<UTGOR_Action> Obligatory;
};


/**
* TGOR_Loadout
*/
UCLASS()
class ACTIONSYSTEM_API UTGOR_Loadout : public UTGOR_SpawnModule
{
	GENERATED_BODY()

public:
	
	////////////////////////////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////// ENGINE ////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////

	UTGOR_Loadout();
	

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "!TGOR Action")
		TArray<FTGOR_ActionSlotSpot> ActionSlots;

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Actions in this spawner */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "!TGOR Insertion")
		TArray<TSubclassOf<UTGOR_Action>> StaticActionInsertions;
	DECL_INSERTION(StaticActionInsertions);

	/** Actions in this spawner */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "!TGOR Insertion")
		TArray<TSubclassOf<UTGOR_Action>> SlotActionInsertions;
	DECL_INSERTION(SlotActionInsertions);

	virtual void MoveInsertion(UTGOR_Content* Insertion, ETGOR_InsertionActionEnumeration Action, bool& Success) override;


protected:
private:

};
