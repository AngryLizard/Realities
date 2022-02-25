#pragma once

#include "CoreMinimal.h"
#include "CoreSystem/Storage/TGOR_PackageGroup.h"
#include "CoreSystem/Storage/TGOR_PackageNetwork.h"
#include "CoreSystem/Storage/TGOR_Serialisation.h"
#include "TGOR_ItemInstance.generated.h"

class UTGOR_Item;
class UTGOR_Module;
class UTGOR_ItemStorage;
class UTGOR_DimensionComponent;


/**
 * Describes what a filter for item types
 */
USTRUCT(BlueprintType, Blueprintable)
struct INVENTORYSYSTEM_API FTGOR_ItemRestriction
{
	GENERATED_BODY()
		FTGOR_ItemRestriction();

	/** Item type we look for (no items match if none). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "!TGOR ItemStorage")
		TSubclassOf<UTGOR_Item> ItemType;
	
	/** Module we look for (accepts no module if none). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "!TGOR ItemStorage")
		TSubclassOf<UTGOR_Module> ModuleType;
};


/**
 * The instance of an item. Contains the serializable data to make the item individual.
 */
USTRUCT(BlueprintType, Blueprintable)
struct INVENTORYSYSTEM_API FTGOR_ItemInstance
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
	bool operator==(const FTGOR_ItemInstance& Other) const; // Compares storage pointer and serial
	bool Update(UTGOR_DimensionComponent* Owner, float DeltaSeconds); // Updates storage (tick and serial increment for replication), return true if something changed
	void Post(); // Post-process storage, done after update and side-effects are handled

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, DisplayName = "ItemStorage", Category = "!TGOR ItemStorage")
		UTGOR_ItemStorage* Storage;

	/** Serial to promote replication. TODO: Replace with custom NetDelta implementation */
	UPROPERTY()
		uint16 Serial;

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
