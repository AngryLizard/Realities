// The Gateway of Realities: Planes of Existence.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "TGOR_SpawnerInterface.generated.h"

class UTGOR_Spawner;
class UTGOR_Module;

/**
* TGOR_ActorState stores region components (in regions registered actors)
 */
USTRUCT(BlueprintType)
struct DIMENSIONSYSTEM_API FTGOR_SpawnerDependencies
{
	GENERATED_USTRUCT_BODY()

	void Process(TSubclassOf<UObject> Class);
	template<typename T>
	void Process()
	{
		Process(T::StaticClass());
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////

	/** Pointer to currently processed object */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int32 Pointer = -1;

	/** Objects that we're spawning off from */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TArray<UObject*> Objects;

	/** Associated spawner content */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UTGOR_Spawner* Spawner = nullptr;
};


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

	/** Update with given dependencies */
	UFUNCTION(BlueprintNativeEvent, Category = "!TGOR Game", Meta = (Keywords = "C++"))
		void UpdateContent(UPARAM(Ref) FTGOR_SpawnerDependencies& Dependencies);
	virtual void UpdateContent_Implementation(UPARAM(Ref) FTGOR_SpawnerDependencies& Dependencies);

};
