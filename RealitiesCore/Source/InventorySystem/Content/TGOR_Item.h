// The Gateway of Realities: Planes of Existence. By Salireths.

#pragma once

#include "Templates/SubclassOf.h"
#include "Components/CapsuleComponent.h"

#include "DimensionSystem/TGOR_MobilityInstance.h"
#include "InventorySystem/TGOR_ItemInstance.h"

#include "TGOR_Matter.h"
#include "DimensionSystem/Content/TGOR_Spawner.h"
#include "TGOR_Item.generated.h"

class UTGOR_ItemTask;
class UTGOR_ContainerComponent;
class ATGOR_ItemDropActor;
//class UTGOR_Matter; // for some reason this lets me not use content templates and I have to include the header in here. Why why why why why why why why. (see * in cpp)


/**
* The base class of all items in the game.
*/
UCLASS(Blueprintable, BlueprintType)
class INVENTORYSYSTEM_API UTGOR_Item : public UTGOR_Spawner
{
	GENERATED_BODY()

		friend class UTGOR_ItemTask;

public:
	UTGOR_Item();

	virtual TSubclassOf<AActor> GetSpawnClass_Implementation() const override;

	////////////////////////////////////////////////////////////////////////////////////////////////////

	/** Return whether this item can be fully constructed using given matter quantities. */
	UFUNCTION(BlueprintPure, Category = "!TGOR Inventory", Meta = (Keywords = "C++"))
		bool CanAssembleWith(const TMap<UTGOR_Matter*, int32>& Matter) const;

	/** Return whether this item can be partially constructed using given matter quantities. */
	UFUNCTION(BlueprintPure, Category = "!TGOR Inventory", Meta = (Keywords = "C++"))
		bool IsCompatibleWith(const TMap<UTGOR_Matter*, int32>& Matter) const;

	/** Return whether this item can be deconstructed using given matter types. */
	UFUNCTION(BlueprintPure, Category = "!TGOR Inventory", Meta = (Keywords = "C++"))
		bool CanDisassembleInto(const TSet<UTGOR_Matter*>& Matter) const;

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Check whether composition includes given matter. */
	UFUNCTION(BlueprintPure, Category = "!TGOR Inventory", Meta = (Keywords = "C++"))
		bool HasComposition(UTGOR_Matter* Matter) const;

	/** Get number of bits required to serialise matter quantity. */
	UFUNCTION(BlueprintPure, Category = "!TGOR Inventory", Meta = (Keywords = "C++"))
		int32 GetCapacityBits(UTGOR_Matter* Matter) const;

	////////////////////////////////////////////////////////////////////////////////////////////////////
protected:

	/** 
	* The default actor of this item to spawn it in the world. 
	* The actor must have a item component and world interactable component attached by default!
	*/
	UPROPERTY(EditAnywhere, Category = "!TGOR Inventory")
		TSubclassOf<ATGOR_ItemDropActor> DefaultItemActor;

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Task type to be spawned by this item */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TSubclassOf<UTGOR_ItemTask> TaskType;

	/** Creates an item task to be used in the component */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Inventory|Instance", Meta = (Keywords = "C++"))
		UTGOR_ItemTask* CreateItemTask(UTGOR_ItemComponent* Component, int32 SlotIdentifier);

	////////////////////////////////////////////////////////////////////////////////////////////////////
protected:

	virtual void TaskInitialise(UTGOR_ItemTask* ItemTask);

	////////////////////////////////////////////////////////////////////////////////////////////////////

	/** Called once when this task is assigned to a component. Should usually only happen once. */
	UFUNCTION(BlueprintImplementableEvent, Category = "!TGOR Inventory", Meta = (Keywords = "C++"))
		void OnTaskInitialise(UTGOR_ItemTask* ItemTask);

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:
	static const float DEFAULT_ITEM_LIFETIME;

	/**
	* Get the default life time of a item. Default is 300 seconds.
	*/
	UFUNCTION(BlueprintPure, Category = "!TGOR Inventory", Meta = (Keywords = "C++"))
		static float GetDefaultItemLifeTime();

	/** */
	UFUNCTION(BlueprintPure, Category = "!TGOR Inventory", Meta = (Keywords = "C++"))
		const TMap<TObjectPtr<UTGOR_Matter>, int32>& GetComponentCapacity() const;

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Main body of this spawner */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "!TGOR Insertion")
		TMap<TSubclassOf<UTGOR_Matter>, int32> MatterInsertions;
	DECL_INSERTION(MatterInsertions);

	virtual void MoveInsertion(UTGOR_Content* Insertion, ETGOR_InsertionActionEnumeration Action, bool& Success) override;
};
