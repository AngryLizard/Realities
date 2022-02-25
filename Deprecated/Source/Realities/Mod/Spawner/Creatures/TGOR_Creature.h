// The Gateway of Realities: Planes of Existence.

#pragma once
#include "Realities/Actors/Pawns/TGOR_Avatar.h"

#include "Realities/Mod/Spawner/TGOR_Spawner.h"
#include "TGOR_Creature.generated.h"

class UTGOR_Bodypart;

UCLASS(Blueprintable)
class REALITIES_API UTGOR_Creature : public UTGOR_Spawner
{
	GENERATED_BODY()

public:
	UTGOR_Creature();

	virtual TSubclassOf<AActor> GetSpawnClass_Implementation() const override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "TGOR Creature")
		TSubclassOf<UTGOR_Bodypart> Bodypart;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "TGOR Creature")
		TSubclassOf<ATGOR_Avatar> Avatar;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "TGOR Creature")
		float Weight;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "TGOR Creature")
		FVector SurfaceArea;
	
private:
};

