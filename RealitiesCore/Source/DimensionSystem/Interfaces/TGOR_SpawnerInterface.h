// The Gateway of Realities: Planes of Existence.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "TGOR_SpawnerInterface.generated.h"

class UTGOR_Spawner;
class UTGOR_SpawnModule;

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

	/** Get module type loaded by this interface (mostly used for debugging purposes) */
	UFUNCTION(BlueprintNativeEvent, Category = "!TGOR Game", Meta = (Keywords = "C++"))
		TMap<int32, UTGOR_SpawnModule*> GetModuleType() const;
	virtual TMap<int32, UTGOR_SpawnModule*> GetModuleType_Implementation() const = 0;

	template<typename T>
	TMap<int32, UTGOR_SpawnModule*> MakeModuleList(T* Entry) const
	{
		TMap<int32, UTGOR_SpawnModule*> Out;
		if (UTGOR_SpawnModule* Module = Cast<UTGOR_SpawnModule>(Entry))
		{
			Out.Emplace(INDEX_NONE, Module);
		}
		return Out;
	}

	template<typename T>
	TMap<int32, UTGOR_SpawnModule*> CastModuleList(const TArray<T*>& List) const
	{
		TMap<int32, UTGOR_SpawnModule*> Out;
		
		const int32 Num = List.Num();
		Out.Reserve(Num);

		for (int32 Index = 0; Index < Num; Index++)
		{
			const T* Entry = List[Index];
			if (UTGOR_SpawnModule* Module = Cast<UTGOR_SpawnModule>(Entry))
			{
				Out.Emplace(Index, Module);
			}
		}
		return Out;
	}
};
