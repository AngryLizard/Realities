#pragma once

#include "CoreMinimal.h"

#include "CoreSystem/Storage/TGOR_PackageGroup.h"

#include "TGOR_StorageInstance.h"

#include "CoreSystem/Storage/TGOR_SaveInterface.h"
#include "CoreSystem/TGOR_Object.h"
#include "TGOR_Container.generated.h"

class UTGOR_Matter;
class UTGOR_Storage;
class UTGOR_DimensionComponent;

UENUM(BlueprintType)
enum class ETGOR_DirtEnumeration : uint8
{
	None,
	Dirty,
	Notify
};

/**
 * The abstract base class for all container classes that can hold storage and matter.
 */
UCLASS(Abstract, Blueprintable)
class INVENTORYSYSTEM_API UTGOR_Container : public UTGOR_Object, public ITGOR_SaveInterface, public ITGOR_NetInterface
{
	GENERATED_BODY()

public:

	UTGOR_Container() {}

	CTGOR_GroupPackageCache LegacyCache;

	/** Clear this container from payload data. */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Storage", Meta = (Keywords = "C++"))
		virtual void Clear() {};

	/** Print current payload. */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Storage", Meta = (Keywords = "C++"))
		virtual FString Print() const;

	/** Tick this storage. */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Storage|Internal", Meta = (Keywords = "C++"))
		virtual void Tick(UTGOR_DimensionComponent* Owner, float DeltaSeconds) {};

	/** Post-process this storage at the end of tick. */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Storage|Internal", Meta = (Keywords = "C++"))
		virtual void Post() {};

	////////////////////////////////////////////////////////////////////////////////////////////////////

	/** Check whether two containers are compatible (can be compared/merged). */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Storage|Internal", Meta = (Keywords = "C++"))
		virtual bool Compatible(const UTGOR_Container* Other) const;

	////////////////////////////////////////////////////////////////////////////////////////////////////

	/** Combines two missing quantities by adding up matter quantities */
	UFUNCTION(BlueprintPure, Category = "!TGOR Storage", Meta = (Keywords = "C++"))
		void CombineMissingOnto(UPARAM(ref) TMap<UTGOR_Matter*, int32>& Missing) const;

	/** Gets total missing matter quantities */
	UFUNCTION(BlueprintPure, Category = "!TGOR Storage", Meta = (Keywords = "C++"))
		virtual TMap<UTGOR_Matter*, int32> GetMissing() const { return TMap<UTGOR_Matter*, int32>(); };

	/** Combines two compositions by adding up matter quantities */
	UFUNCTION(BlueprintPure, Category = "!TGOR Storage", Meta = (Keywords = "C++"))
		void CombineCompositionOnto(UPARAM(ref) TMap<UTGOR_Matter*, int32>& Composition) const;

	/** Gets total available matter quantities */
	UFUNCTION(BlueprintPure, Category = "!TGOR Storage", Meta = (Keywords = "C++"))
		virtual TMap<UTGOR_Matter*, int32> GetComposition() const { return TMap<UTGOR_Matter*, int32>(); };

	////////////////////////////////////////////////////////////////////////////////////////////////////

	/** Remove given amount of given matter from this container, returns how much actually got removed */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Storage", Meta = (Keywords = "C++"))
		virtual int32 RemoveMatter(UTGOR_Matter* Matter, int32 Quantity) { return 0; };

	/** Add given amount of given matter to this container, returns leftovers */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Storage", Meta = (Keywords = "C++"))
		virtual int32 AddMatter(UTGOR_Matter* Matter, int32 Quantity) { return Quantity; };

	/** Get currently available amount of matter from this container */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Storage", Meta = (Keywords = "C++"))
		virtual int32 CountMatter(UTGOR_Matter* Matter) const { return 0; };

	////////////////////////////////////////////////////////////////////////////////////////////////////

	// UPDATE: Moved array-specific overrides to InventoryStorage.
	// However, it seems useful to instead assume that storage can generally hold other items (aka, make the set-assumption not the list-assumption).
	// This assumes the absolute minimum about this storage while minimising casting

	/** Tests whether another storage instance is already stored in this storage (for cycle prevention). */
	UFUNCTION(BlueprintPure, Category = "!TGOR Storage", Meta = (Keywords = "C++"))
		virtual bool Contains(const UTGOR_Storage* Other) const { return false; }

	/** Removes another storage instance if stored in this storage. By default does nothing. Returns whether at least one storage got removed. */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Storage", Meta = (Keywords = "C++"))
		virtual bool Purge(const UTGOR_Storage* Other) { return false; }

	////////////////////////////////////////////////////////////////////////////////////////////////////

	/** Cleans this module, returns whether it was dirty */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Storage|Internal", Meta = (Keywords = "C++"))
		virtual bool PurgeDirt();

	/** Check whether an important change is pending (extension of dirty), usually used for replication. */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Storage|Internal", Meta = (Keywords = "C++"))
		virtual bool HasNotify();

protected:

	/** Mark this module as dirty, meaning it has changes */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Storage|Internal", Meta = (Keywords = "C++"))
		void MarkDirty(bool Notify = false);

	/** Whether this module was changed. */
	UPROPERTY()
		ETGOR_DirtEnumeration Dirty;
};
