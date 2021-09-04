// The Gateway of Realities: Planes of Existence.

#pragma once
#include "CreatureSystem/Actors/TGOR_Avatar.h"

#include "DimensionSystem/Content/TGOR_Spawner.h"
#include "TGOR_Creature.generated.h"

class UTGOR_Loadout;
class UTGOR_Stat;
class UTGOR_Effect;

////////////////////////////////////////////////////////////////////////////////////////////////////


UCLASS(Blueprintable)
class CREATURESYSTEM_API UTGOR_Creature : public UTGOR_Spawner
{
	GENERATED_BODY()

public:
	UTGOR_Creature();
	virtual bool Validate_Implementation() override;

	virtual TSubclassOf<AActor> GetSpawnClass_Implementation() const override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "!TGOR Creature")
		TSubclassOf<ATGOR_Pawn> Pawn;

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Stomach in this spawner */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "!TGOR Insertion")
		TSubclassOf<UTGOR_Dimension> StomachInsertion;
	DECL_INSERTION(StomachInsertion);

	/** Trackers in this spawner */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "!TGOR Insertion")
		TArray<TSubclassOf<UTGOR_Tracker>> TrackerInsertions;
	DECL_INSERTION(TrackerInsertions);

	/** Cameras in this spawner */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "!TGOR Insertion")
		TArray<TSubclassOf<UTGOR_Camera>> CameraInsertions;
	DECL_INSERTION(CameraInsertions);

	virtual void MoveInsertion(UTGOR_Content* Insertion, ETGOR_InsertionActionEnumeration Action, bool& Success) override;

private:
};

