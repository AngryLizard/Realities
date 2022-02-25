// The Gateway of Realities: Planes of Existence.

#pragma once

#include "Utility/TGOR_CustomEnumerations.h"
#include "Utility/Storage/Package/TGOR_PackTypeArrays.h"
#include "Base/Instances/Inventory/TGOR_ItemInstance_OLD.h"
#include "Base/Instances/System/TGOR_IndexInstance.h"

#include "Object.h"
#include "TGOR_InventoryInstance.generated.h"

class UTGOR_Item;

/**
* TGOR_InventoryRestrict defines what an inventory slot contains
*/
USTRUCT(BlueprintType)
struct REALITIES_API FTGOR_InventoryRestrict
{
	GENERATED_USTRUCT_BODY()

		FTGOR_InventoryRestrict();
		FTGOR_InventoryRestrict(TSubclassOf<UTGOR_Item> Item, int32 Count);

	////////////////////////////////////////////////////////////////////////////////////////////////////

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TGOR|Inventory")
		TSubclassOf<UTGOR_Item> Type;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TGOR|Inventory")
		int32 SlotCount;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TGOR|Inventory")
		int32 ExtraCount;
};

/**
* TGOR_InventorySlot stores an array of items
*/
USTRUCT(BlueprintType)
struct REALITIES_API FTGOR_InventorySlot
{
	GENERATED_USTRUCT_BODY()
		FTGOR_InventorySlot();
		FTGOR_InventorySlot(int32 Slots);

	using FilePack = CTGOR_FileTypeArray<FTGOR_ItemInstance_OLD::FilePack>;

	bool Write(UTGOR_Singleton* Singleton, FilePack& Pack) const;
	bool Read(UTGOR_Singleton* Singleton, const FilePack& Pack);

	////////////////////////////////////////////////////////////////////////////////////////////////////

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TGOR|Inventory")
		TArray<FTGOR_ItemInstance_OLD> Items;
};

/**
* TGOR_InventoryInstance stores an array of inventory slots
*/
USTRUCT(BlueprintType)
struct REALITIES_API FTGOR_InventoryInstance
{
	GENERATED_USTRUCT_BODY()
		FTGOR_InventoryInstance();

	using FilePack = CTGOR_FileTypeArray<FTGOR_InventorySlot::FilePack>;

	bool Write(UTGOR_Singleton* Singleton, FilePack& Pack) const;
	bool Read(UTGOR_Singleton* Singleton, const FilePack& Pack);

	////////////////////////////////////////////////////////////////////////////////////////////////////

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TGOR|Inventory")
		TArray<FTGOR_InventorySlot> Slots;

	////////////////////////////////////////////////////////////////////////////////////////////////////
	
	FTGOR_ItemInstance_OLD& GetItemInstance(const FTGOR_ItemIndex& Index, ETGOR_FetchEnumeration& Branches);

};
