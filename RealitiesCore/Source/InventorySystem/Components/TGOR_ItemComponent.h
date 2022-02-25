// The Gateway of Realities: Planes of Existence.

#pragma once

#include "CoreSystem/Storage/TGOR_SaveInterface.h"

#include "../TGOR_ItemInstance.h"

#include "DimensionSystem/Interfaces/TGOR_SpawnerInterface.h"
#include "CoreSystem/Components/TGOR_SceneComponent.h"
#include "TGOR_ItemComponent.generated.h"

class UTGOR_ItemTask;
class UTGOR_Modifier;

///////////////////////////////////////////// DELEGATES ///////////////////////////////////////////////////

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FItemFreedDelegate, UTGOR_ItemTask*, Task);

/**
* This component can be attached to every actor to give them the functionality to provide items.
*/
UCLASS(BlueprintType, Blueprintable, ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class INVENTORYSYSTEM_API UTGOR_ItemComponent : public UTGOR_SceneComponent, public ITGOR_DimensionInterface, public ITGOR_SaveInterface, public ITGOR_SpawnerInterface
{
	GENERATED_BODY()

public:
	UTGOR_ItemComponent();
	void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	void GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const override;

	virtual void UpdateContent_Implementation(FTGOR_SpawnerDependencies& Dependencies) override;
	virtual TMap<int32, UTGOR_SpawnModule*> GetModuleType_Implementation() const override;

	//////////////////////////////////////////// IMPLEMENTABLES ////////////////////////////////////////

	UPROPERTY(BlueprintAssignable, Category = "!TGOR Inventory")
		FItemFreedDelegate OnItemFreed;

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Item types this container spawns with. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "!TGOR Inventory")
		TArray<TSubclassOf<UTGOR_Item>> SpawnItems;

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Get all item tasks of given type */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Inventory", Meta = (DeterminesOutputType = "Type", Keywords = "C++"))
		TArray<UTGOR_ItemTask*> GetItemListOfType(TSubclassOf<UTGOR_ItemTask> Type) const;

	template<typename T> TArray<T*> GetIListOfType() const
	{
		TArray<T*> Output;
		TArray<UTGOR_ItemTask*> Items = GetItemListOfType(T::StaticClass());
		for (UTGOR_ItemTask* Item : Items)
		{
			Output.Emplace(Cast<T>(Item));
		}
		return Output;
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////
protected:

	/** Currently assigned items. */
	UPROPERTY(Replicated)
		TArray<UTGOR_ItemTask*> ItemSlots;

};
