#pragma once

#include "CoreMinimal.h"
#include "Base/TGOR_Object.h"

#include "Base/Inventory/TGOR_Storage.h"
#include "TGOR_ArrayStorage.generated.h"

/**
 * The abstract base class for all item storages that store arrays of things.
 */
UCLASS(Abstract, Blueprintable)
class REALITIES_API UTGOR_ArrayStorage : public UTGOR_Storage
{
	GENERATED_BODY()

public:

	UTGOR_ArrayStorage() {}

	/** Remove the element at the given position. Returns true on success. */
	UFUNCTION(BlueprintCallable, Category = "TGOR ItemStorage", Meta = (Keywords = "C++"))
		virtual bool RemoveAt(int32 Index) { unimplemented() return false; }

	/** Get the current size of the storage. */
	UFUNCTION(BlueprintPure, Category = "TGOR ItemStorage", Meta = (Keywords = "C++"))
		virtual int32 GetCurrentSize() const { unimplemented() return 0; }

	/** Get the maximum size of the storage container. */
	UFUNCTION(BlueprintPure, Category = "TGOR ItemStorage", Meta = (Keywords = "C++"))
		virtual int32 GetMaxSize() const { unimplemented() return 0; }

	/** Test if the given index is in the range of the storage container. */
	UFUNCTION(BlueprintPure, Category = "TGOR ItemStorage", Meta = (Keywords = "C++"))
		virtual bool IsIndexValid(int32 Index) const { unimplemented() return false; }

};
