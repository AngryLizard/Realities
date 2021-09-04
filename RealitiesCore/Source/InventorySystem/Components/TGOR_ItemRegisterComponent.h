// The Gateway of Realities: Planes of Existence.

#pragma once

#include <functional>

#include "../TGOR_ItemInstance.h"

#include "CoreSystem/Interfaces/TGOR_RegisterInterface.h"
#include "InventorySystem/Components/TGOR_ContainerComponent.h"
#include "TGOR_ItemRegisterComponent.generated.h"




////////////////////////////////////////////// DECL //////////////////////////////////////////////////////

class ATGOR_ItemDropActor;


///////////////////////////////////////////// STRUCTS ///////////////////////////////////////////////////


/*
USTRUCT(BlueprintType)
struct FTGOR_ItemContainer : public FFastArraySerializerItem
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FTGOR_SlotIndex Identifier;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FTGOR_ItemInstance Instance;
};

USTRUCT(BlueprintType)
struct FTGOR_ItemContainers : public FFastArraySerializer
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		TMap<FTGOR_SlotIndex, FTGOR_ItemInstance> Containers;

	bool NetDeltaSerialize(FNetDeltaSerializeInfo& DeltaParms);
};

template<>
struct TStructOpsTypeTraits<FTGOR_ItemContainers> : public TStructOpsTypeTraitsBase2<FTGOR_ItemContainers>
{
	enum { WithNetDeltaSerializer = true }; // WithNetSerializer / WithNetDeltaSerializer
};
*/

USTRUCT(BlueprintType)
struct FTGOR_ItemContainers
{
	GENERATED_USTRUCT_BODY()
	SERIALISE_INIT_HEADER;

	void Write(FTGOR_GroupWritePackage& Package, UTGOR_Singleton* Context) const;
	void Read(FTGOR_GroupReadPackage& Package, UTGOR_Singleton* Context);
	void Send(FTGOR_NetworkWritePackage& Package, UTGOR_Singleton* Context) const;
	void Recv(FTGOR_NetworkReadPackage& Package, UTGOR_Singleton* Context);

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		TMap<int32, FTGOR_ItemInstance> Slots;
};

template<>
struct TStructOpsTypeTraits<FTGOR_ItemContainers> : public TStructOpsTypeTraitsBase2<FTGOR_ItemContainers>
{
	enum { WithNetSerializer = true };
};

DECLARE_DYNAMIC_DELEGATE_FourParams(FSlotDelegate, int32, Identifier, UTGOR_ItemStorage*, Item, int32, Count, int32, Compare);

/**
* TGOR_ItemRegisterComponent stores a dictionary of item storages
*/
UCLASS()
class INVENTORYSYSTEM_API UTGOR_ItemRegisterComponent : public UTGOR_ContainerComponent
{
	GENERATED_BODY()
	
public:
	UTGOR_ItemRegisterComponent();

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	void GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const override;

	virtual TSet<UTGOR_CoreContent*> GetActiveContent_Implementation() const override;
	virtual bool PurgeItemStorage(UTGOR_ItemStorage* Storage) override;

	////////////////////////////////////////////////////////////////////////////////////////////////////
protected:

	/** Updates all items */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Storage|Internal", Meta = (Keywords = "C++"))
		void TickAll(float DeltaTime);

	////////////////////////////////////////////////////////////////////////////////////////////////////
private:

	/** Currently included slots. */
	UPROPERTY(ReplicatedUsing = RepNotifyItemContainers)
		FTGOR_ItemContainers ItemContainers;

	/**  */
	UFUNCTION()
		void RepNotifyItemContainers();

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Iterates through all item slots  */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Storage", Meta = (Keywords = "C++"))
		void ForEachSlot(const FSlotDelegate& Delegate, int32 Compare = -1);
	bool ForEachSlot(std::function<bool(int32 Identifier, UTGOR_ItemStorage * Item)> func);

	/** Get item storage at a given index */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Storage", Meta = (DeterminesOutputType = "Type", ExpandEnumAsExecs = "Branches", Keywords = "C++"))
		UTGOR_ItemStorage* GetItemStorageValidated(int32 Identifier, ETGOR_FetchEnumeration& Branches);

	/** Get item storage at a given index */
	UFUNCTION(BlueprintPure, Category = "!TGOR Storage", Meta = (Keywords = "C++"))
		UTGOR_ItemStorage* GetItemStorage(int32 Identifier) const;

	/** Swap an item storage with a given index. Swap with nullptr to remove an item. */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Storage", Meta = (Keywords = "C++"))
		UTGOR_ItemStorage* SwapItemStorage(int32 Identifier, UTGOR_ItemStorage* Storage);

	/** Set item storage at given place without any checks, previous item is silently discarded (mostly used for replication). */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Storage", Meta = (Keywords = "C++"))
		void OverrideItemStorage(int32 Identifier, UTGOR_ItemStorage* Storage);

	////////////////////////////////////////////////////////////////////////////////////////////////////
protected:

	/** Whether a given storage can be put at given index. This is only checked on SwapItemStorage. */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Action", Meta = (Keywords = "C++"))
		virtual bool CanStoreItemAt(int32 Identifier, UTGOR_ItemStorage* Storage) const;

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Only authority and autonomous actually store inventory. */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Storage", Meta = (Keywords = "C++"))
		bool HasAccess() const;

	/** Only authority and autonomous actually store inventory. */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Actions", Meta = (ExpandEnumAsExecs = "Branches", Keywords = "C++"))
		void AccessBranch(ETGOR_BoolEnumeration& Branches);

	////////////////////////////////////////////////////////////////////////////////////////////////////

protected:
private:
};
