#pragma once

#include "CoreMinimal.h"
#include "InventorySystem/TGOR_ItemInstance.h"

#include "TGOR_Module.h"
#include "TGOR_InventoryModule.generated.h"


/**
 * The base class of an inventory. Can be inherited to create extra functionallity.
 */
UCLASS(BlueprintType, Blueprintable)
class INVENTORYSYSTEM_API UTGOR_InventoryModule : public UTGOR_Module
{
	GENERATED_BODY()


public:
	UTGOR_InventoryModule();
	virtual bool Equals(const UTGOR_Module* Other) const override;
	virtual bool Merge(const UTGOR_Module* Other, ETGOR_NetvarEnumeration Priority) override;

	virtual void Write_Implementation(FTGOR_GroupWritePackage& Package, UTGOR_Singleton* Context) const override;
	virtual bool Read_Implementation(FTGOR_GroupReadPackage& Package, UTGOR_Singleton* Context) override;
	virtual void Send_Implementation(FTGOR_NetworkWritePackage& Package, UTGOR_Singleton* Context) const override;
	virtual bool Recv_Implementation(FTGOR_NetworkReadPackage& Package, UTGOR_Singleton* Context) override;

	virtual void Clear() override;
	virtual FString Print() const override;
	virtual void Tick(UTGOR_DimensionComponent* Owner, float DeltaSeconds) override;

	virtual TMap<UTGOR_Matter*, int32> GetComposition() const override;
	virtual int32 RemoveMatter(UTGOR_Matter* Matter, int32 Quantity) override;
	virtual int32 AddMatter(UTGOR_Matter* Matter, int32 Quantity) override;
	virtual int32 CountMatter(UTGOR_Matter* Matter) const override;

	virtual bool Contains(const UTGOR_Storage* Other) const override;
	virtual bool Purge(const UTGOR_Storage* Other) override;

	////////////////////////////////////////////////////////////////////////////////////////////////////

	/** Adds the item to the item list and sorts the list by storage index. */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Storage", Meta = (Keywords = "C++"))
		virtual void AddItem(UTGOR_ItemStorage* Storage);

	/** Removes and item and destroys it. There is also an indexed version. */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Storage", Meta = (Keywords = "C++"))
		virtual bool RemoveItem(UTGOR_ItemStorage* Storage);

	/** Get the item under the given index. Returns nullptr on failure, */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Storage", Meta = (Keywords = "C++"))
		virtual UTGOR_ItemStorage* GetItemAt(int32 Index) const;

	/** Base checks for an item. Will called before every item insertation. */
	UFUNCTION(BlueprintPure, Category = "!TGOR Storage", Meta = (Keywords = "C++"))
		virtual bool IsSuitable(UTGOR_ItemStorage* Storage, bool IgnoreSize = false) const;

	/** Get a index out of the given item. Returns INDEX_NONE on failure. */
	UFUNCTION(BlueprintPure, Category = "!TGOR Storage", Meta = (Keywords = "C++"))
		virtual int32 GetItemIndex(UTGOR_ItemStorage* Storage) const;

	/** Remove the element at the given position. Returns true on success. */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Storage", Meta = (Keywords = "C++"))
		virtual bool RemoveAt(int32 Index);

	/** Get the current size of the storage. */
	UFUNCTION(BlueprintPure, Category = "!TGOR Storage", Meta = (Keywords = "C++"))
		virtual int32 GetCurrentSize() const;

	/** Get the maximum size of the storage container. */
	UFUNCTION(BlueprintPure, Category = "!TGOR Storage", Meta = (Keywords = "C++"))
		virtual int32 GetMaxSize() const;

	/** Test if the given index is in the range of the storage container. */
	UFUNCTION(BlueprintPure, Category = "!TGOR Storage", Meta = (Keywords = "C++"))
		virtual bool IsIndexValid(int32 Index) const;

	////////////////////////////////////////////////////////////////////////////////////////////////////

	/** Set number of items that can be stored in this module */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Storage", Meta = (Keywords = "C++"))
		void SetRestrictions(const FTGOR_ItemRestriction& Restrictions, int32 Capacity);

	////////////////////////////////////////////////////////////////////////////////////////////////////
private:

	/** Item storage restrictions of this module */
	UPROPERTY()
		FTGOR_ItemRestriction StorageRestrictions;

	/** Item storage capacity of this module */
	UPROPERTY()
		int32 StorageCapacity;

	////////////////////////////////////////////////////////////////////////////////////////////////////
private:
	
	/** The list of stored items. */
	UPROPERTY(VisibleAnywhere, Category = "!TGOR Storage")
		TArray<FTGOR_ItemInstance> Items;
};