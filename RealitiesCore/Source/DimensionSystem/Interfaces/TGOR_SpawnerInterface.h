// The Gateway of Realities: Planes of Existence.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "TGOR_SpawnerInterface.generated.h"

class UTGOR_Spawner;
class UTGOR_Module;

UINTERFACE(MinimalAPI)
class UTGOR_SpawnerInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 *
 */
class DIMENSIONSYSTEM_API ITGOR_SpawnerInterface
{
	GENERATED_BODY()

public:

	/** Update with given owner content */
	UFUNCTION(BlueprintNativeEvent, Category = "!TGOR Game", Meta = (Keywords = "C++"))
		void UpdateContent(UTGOR_Spawner* Spawner);
	virtual void UpdateContent_Implementation(UTGOR_Spawner* Spawner);

};
