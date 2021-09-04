// The Gateway of Realities: Planes of Existence.

#pragma once

#include "DimensionSystem/Content/TGOR_Spawner.h"
#include "TGOR_Npc.generated.h"

class UTGOR_Creature;
class ATGOR_NpcSpawnActor;
class ATGOR_NpcController;

UCLASS(Blueprintable)
class AISYSTEM_API UTGOR_Npc : public UTGOR_Spawner
{
	GENERATED_BODY()

public:
	UTGOR_Npc();
	virtual bool Validate_Implementation() override;

	virtual TSubclassOf<AActor> GetSpawnClass_Implementation() const override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "!TGOR Creature")
		TSubclassOf<ATGOR_NpcSpawnActor> Spawner;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "!TGOR Creature")
		TSubclassOf<ATGOR_NpcController> Controller;

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Creature to be spawned */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "!TGOR Insertion")
		TSubclassOf<UTGOR_Creature> CreatureInsertion;
	DECL_INSERTION_REQUIRED(CreatureInsertion);

	virtual void MoveInsertion(UTGOR_Content* Insertion, ETGOR_InsertionActionEnumeration Action, bool& Success) override;

private:
};

