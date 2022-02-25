#pragma once

#include "CoreMinimal.h"
#include "Realities/Utility/Storage/TGOR_PackageGroup.h"
#include "Realities/Utility/Storage/TGOR_PackageNetwork.h"

#include "Realities/Utility/Storage/TGOR_Serialisation.h"
#include "TGOR_ItemInstance.generated.h"

class UTGOR_Item;
class UTGOR_Module;
class UTGOR_ItemStorage;


/**
 * Describes what a filter for item types
 */
USTRUCT(BlueprintType, Blueprintable)
struct REALITIES_API FTGOR_ItemRestriction
{
	GENERATED_BODY()
		FTGOR_ItemRestriction();

	/** Item type we look for (no items match if none). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TGOR ItemStorage")
		TSubclassOf<UTGOR_Item> ItemType;
	
	/** Module we look for (accepts no module if none). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TGOR ItemStorage")
		TSubclassOf<UTGOR_Module> ModuleType;
};


/**
 * The instance of an item. Contains the serializable data to make the item individual.
 */
USTRUCT(BlueprintType, Blueprintable)
struct REALITIES_API FTGOR_ItemInstance
{
	GENERATED_BODY()
	SERIALISE_INIT_HEADER;

public:

	FTGOR_ItemInstance();
	FTGOR_ItemInstance(UTGOR_Item* Item);
	FTGOR_ItemInstance(UTGOR_ItemStorage* NewStorage);

	CTGOR_GroupPackageCache LegacyCache;
	void Write(FTGOR_GroupWritePackage& Package, UTGOR_Singleton* Context) const;
	void Read(FTGOR_GroupReadPackage& Package, UTGOR_Singleton* Context);
	void Send(FTGOR_NetworkWritePackage& Package, UTGOR_Singleton* Context) const;
	void Recv(FTGOR_NetworkReadPackage& Package, UTGOR_Singleton* Context);

	bool operator<(const FTGOR_ItemInstance& Other) const; // Compares held item storage index (for weak ordering)
	bool operator==(const FTGOR_ItemInstance& Other) const; // Compares item storages for equality

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, DisplayName = "ItemStorage", Category = "TGOR ItemStorage")
		UTGOR_ItemStorage* Storage;

	/** Instantiate storage from given content. */
	void SetupStorage(UTGOR_Item* NewItem);
};

template<>
struct TStructOpsTypeTraits<FTGOR_ItemInstance> : public TStructOpsTypeTraitsBase2<FTGOR_ItemInstance>
{
	enum
	{
		WithNetSerializer = true
	};
};
