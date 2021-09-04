// TGOR (C) // CHECKED //
#pragma once

#include "CoreMinimal.h"
#include "Templates/SubclassOf.h"

#include "DimensionSystem/Content/TGOR_SpawnModule.h"
#include "TGOR_Mobile.generated.h"

class UTGOR_Movement;

/**
* TGOR_Mobile
*/
UCLASS()
class MOVEMENTSYSTEM_API UTGOR_Mobile : public UTGOR_SpawnModule
{
	GENERATED_BODY()

public:
	
	////////////////////////////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////// ENGINE ////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////

	UTGOR_Mobile();
	
	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Movement modes in this spawner */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "!TGOR Insertion")
		TArray<TSubclassOf<UTGOR_Movement>> MovementInsertions;
	DECL_INSERTION(MovementInsertions);

	virtual void MoveInsertion(UTGOR_Content* Insertion, ETGOR_InsertionActionEnumeration Action, bool& Success) override;

protected:
private:
};
