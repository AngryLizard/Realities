// The Gateway of Realities: Planes of Existence. By Salireths.

#pragma once

#include "Templates/SubclassOf.h"
#include "Components/CapsuleComponent.h"

#include "DimensionSystem/TGOR_MobilityInstance.h"
#include "InventorySystem/Storage/TGOR_StorageInstance.h"
#include "InventorySystem/TGOR_ItemInstance.h"

#include "TGOR_Matter.h"
#include "DimensionSystem/Content/TGOR_Spawner.h"
#include "TGOR_Item.generated.h"

class UTGOR_ItemStorage;
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

	friend class UTGOR_ItemStorage;

public:
	UTGOR_Item();

	virtual TSubclassOf<AActor> GetSpawnClass_Implementation() const override;

	/**
	* Create an empty storage instance for this item.
	* @see TGOR_Storage
	*/
	UFUNCTION(BlueprintCallable, Category = "!TGOR Inventory", Meta = (Keywords = "C++"))
		virtual UTGOR_ItemStorage* CreateStorage();

	/** Build Storage modules and set their default values */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Inventory", Meta = (Keywords = "C++"))
		virtual void BuildStorage(UTGOR_ItemStorage* Storage);

	/** Change values and module values of a created storage object */
	UFUNCTION(BlueprintImplementableEvent, Category = "!TGOR Inventory", Meta = (Keywords = "C++"))
		void InitialiseStorage(UTGOR_ItemStorage* Storage);

	////////////////////////////////////////////////////////////////////////////////////////////////////

	/** Set the capsule size to the values of this item. */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Inventory", Meta = (Keywords = "C++"))
		void SetCapsuleSize(UCapsuleComponent* Capsule);

	/** Return the radius of the with of this item. */
	UFUNCTION(BlueprintPure, Category = "!TGOR Inventory", Meta = (Keywords = "C++"))
		float GetRadius() const;

	/** Return the absoule height of this item. */
	UFUNCTION(BlueprintPure, Category = "!TGOR Inventory", Meta = (Keywords = "C++"))
		float GetHeight() const;

	/** Return the absoule shape item. */
	UFUNCTION(BlueprintPure, Category = "!TGOR Inventory", Meta = (Keywords = "C++"))
		FTGOR_MovementShape GetShape() const;

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

	/** Tick this item with given storage and owner, return action to be performed by container after tick. */
	UFUNCTION(BlueprintImplementableEvent, Category = "!TGOR Inventory", Meta = (Keywords = "C++"))
		void Tick(UTGOR_DimensionComponent* Owner, UTGOR_ItemStorage* Storage, float DeltaSeconds);

	/** Check whether composition includes given matter. */
	UFUNCTION(BlueprintPure, Category = "!TGOR Inventory", Meta = (Keywords = "C++"))
		bool HasComposition(UTGOR_Matter* Matter) const;

	/** Get number of bits required to serialise matter quantity. */
	UFUNCTION(BlueprintPure, Category = "!TGOR Inventory", Meta = (Keywords = "C++"))
		int32 GetCapacityBits(UTGOR_Matter* Matter) const;

	/** Whether this item fits a given restriction. */
	UFUNCTION(BlueprintPure, Category = "!TGOR Inventory", Meta = (Keywords = "C++"))
		bool IsSupportedBy(const FTGOR_ItemRestriction& Restriction) const;

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** The width of this item as a radius. */
	UPROPERTY(EditAnywhere, Category = "!TGOR Inventory")
		float Radius;

	/** The half height of this item (without radius). */
	UPROPERTY(EditAnywhere, Category = "!TGOR Inventory")
		float HalfHeight;

	/** Modules that are part of this item storage */
	UPROPERTY(EditAnywhere, Category = "!TGOR Inventory")
		FTGOR_StorageDeclaration Modules;

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
