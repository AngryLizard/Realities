// The Gateway of Realities: Planes of Existence.

#pragma once
#include "Actor.h"
#include "Realities/Base/Instances/Dimension/TGOR_ActorInstance.h"

#include "Realities/Base/Content/TGOR_Content.h"
#include "TGOR_Spawner.generated.h"


UCLASS(Blueprintable)
class REALITIES_API UTGOR_Spawner : public UTGOR_Content
{
	GENERATED_BODY()

public:
	UTGOR_Spawner();

	/** Class this spawnable spawns as */
	UFUNCTION(BlueprintNativeEvent, BlueprintPure, Category = "TGOR Creature", Meta = (Keywords = "C++"))
		TSubclassOf<AActor> GetSpawnClass() const;
		virtual TSubclassOf<AActor> GetSpawnClass_Implementation() const;
		
};

